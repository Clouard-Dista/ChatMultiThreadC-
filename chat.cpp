#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <unistd.h>
int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    std::string ip="";
    int port = 8001;
    std::string pseudo="";
    int minSizepseudo=5;
    bool checkServ = false;
    bool checkSock = false;
    int errorCount=0;
  	while(!checkSock){
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			printf("Socket creation error \n");
			errorCount++;
			if(errorCount > 100){
				printf("Too much try.\n");
				return -1;
			}
		}else{
			checkSock = true;
		}
	}
	errorCount=0;
  	while(!checkServ){
		printf("Entrer l'ip du serveur:\n");
		std::cin >> ip;
		memset(&serv_addr, '0', sizeof(serv_addr));

		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port = htons(port);
		// Convert IPv4 and IPv6 addresses from text to binary form
		if(inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr)<=0)
		{
			printf("Invalid address(%s:%d)/ Address not supported \n",ip,port);
			errorCount++;
		}else{
			//connection server
			if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
			{
				printf("Connection Failed \n");
				errorCount++;
			}else{
				checkServ = true;
			}
		}
		if(errorCount > 100){
			printf("Too much try.\n");
			return -1;
		}
    }
  	while(pseudo.length()<minSizepseudo){
		printf("Entrer votre pseudo:\n");
		std::cin >> pseudo;
  		if(pseudo.length()<minSizepseudo){
			printf("Votre pseudo est trop cour (%d caractère minimum)\n",minSizepseudo);
  		}
  	}

//|attendre que le pseudo soit ajouté	attendre une requette type 1&pseudo
//|affichage serv /info/input			faire une requette 0 pour la liste et 1 pour les info du serv reponse type 2&... sauvegarde des room dans un tableau
//|entrer room							
// |ecrire message(si press enter arreter affichage et faire un cin)
// |changer room(press echap)

    send(sock , hello , strlen(hello) , 0 );
    printf("Hello message sent\n");
    valread = read( sock , buffer, 1024);
    printf("%s\n",buffer );
    return 0;
}
