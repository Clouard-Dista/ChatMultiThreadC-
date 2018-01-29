// Server side C/C++ program to demonstrate Socket programming
#include <sstream>      //stringstream
#include <map>          //element map
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <thread>
#include <iostream>
#include <vector>

int maxRoom=5;
std::map <int, int> roomList;

std::string readToString(unsigned  int sock){
    char buffer[1024] = {0};
    signed int valRead;
    std::string tot;
    while(true){
        valRead = read( sock , buffer,1024);
        if(valRead!=1024){
            buffer[valRead]='\0';
        }
        std::stringstream ss;
        ss << tot << buffer;
        tot = ss.str();
        if(valRead!=1024){
            return tot;
        }
    }
}

#define PORT 8001
void home(){
    std::map <std::string, std::string> pseudoList;
    std::vector <std::thread> roomVal;
    std::string text="";
    int opt = 1;
    char buffer[1024] = {0};
    std::string comPseu="70";
    std::string comInfo="71";
    std::string comMess="50";
    std::string msghello = "Hello from server";
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    int server_fd, new_socket, valread;
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 
    if (bind(server_fd, (struct sockaddr *)&address,sizeof(address))<0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("en attente de conection\n");
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0){
        perror("accept");
        exit(EXIT_FAILURE);
    }
    printf("nouvelle conection\n");
    while(true){
        printf("en attente de message\n");
        std::string msgRead = readToString(new_socket);
        printf("|%s|\n",msgRead.c_str());

        if(msgRead.compare(0,2,comPseu)==0){
            printf("pseudo 1\n");
            std::string msgReturn ="1";
            printf("pseudo 2\n");
            msgRead = msgRead.substr(2,msgRead.length()-1);
            printf("pseudo 3\n");
            pseudoList[msgRead]=msgRead;


            send(new_socket , msgReturn.c_str() , msgReturn.length() , 0 );
            printf("pseudo %s validé\n",(msgRead.c_str()) );
        }
        if(msgRead.compare(0,2,comInfo)==0){
            printf("info 1\n");
            std::string msgInfo ="0";
            for (int i = 0; i < maxRoom; ++i){
                std::stringstream ss;
                ss << roomList[i];
                if(i+1 < maxRoom)
                    ss << "&";
                msgInfo = ss.str();
            }
            printf("info 2\n");
            send(new_socket , msgInfo.c_str() , msgInfo.length() , 0 );
            printf("info 3\n");
        }
    }
}

void addroom(int portRoom){
    std::map <std::string, std::string> pseudoList;
    std::string text="";
    int opt = 1;
    char buffer[1024] = {0};
    std::string comPseu="70";
    std::string comInfo="71";
    std::string comMess="50";
    std::string msghello = "Hello from server";
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( portRoom );

    int server_fd, new_socket, valread;
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 
    if (bind(server_fd, (struct sockaddr *)&address,sizeof(address))<0){
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("r%d|en attente de conection\n",portRoom);
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0){
        perror("accept");
        exit(EXIT_FAILURE);
    }
    printf("r%d|nouvelle conection\n",portRoom);
    while(true){
        printf("r%d|en attente de message\n",portRoom);
        std::string msgRead = readToString(new_socket);
        printf("|%s|\n",msgRead.c_str());
    }
}

int main(int argc, char const *argv[]){
    for (int i = 0; i < maxRoom; ++i){
        roomList[i]=8002+i;
    }
    std::thread th([]() {
        home();
    });
    std::thread t1([]() {
        addroom(8002);
    });
    std::thread t2([]() {
        addroom(8003);
    });
    std::thread t3([]() {
        addroom(8004);
    });
    th.join();
    t1.join();
    t2.join();
    t3.join();
    return 0;
}

