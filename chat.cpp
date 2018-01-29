//g++ -Wall -g  -c chat.cpp -o chat.o
#include <arpa/inet.h>	//sockaddr_in et htons
#include <termios.h> 	//oldt, termios, ICANON, TCSANOW, tcgetattr, ECHO
#include <unistd.h> 	//read et STDIN_FILENO
#include <stdlib.h>		//system et atoi
#include <iostream>		//cout et cin
#include <stdio.h> 		//getchar et printf
#include <cstring>		//memset
#include <sstream> 		//stringstream
#include <cerrno>		//errno et EINTR
#include <map> 			//element map
//#include <sys/socket.h>
//#include <sys/types.h>
//#include <sys/time.h>
//#include <stdio.h>
//#include <string>
//#include <ctime>

void my_sleep(unsigned msec) {
	struct timespec req, rem;
	int err;
	req.tv_sec = msec / 1000;
	req.tv_nsec = (msec % 1000) * 1000000;
	while ((req.tv_sec != 0) || (req.tv_nsec != 0)) {
		if (nanosleep(&req, &rem) == 0)
			break;
		err = errno;
		// Interrupted; continue
		if (err == EINTR) {
			req.tv_sec = rem.tv_sec;
			req.tv_nsec = rem.tv_nsec;
		}
		// Unhandleable error (EFAULT (bad pointer), EINVAL (bad timeval in tv_nsec), or ENOSYS (function not supported))
		break;
	}
}
void changemode(int dir){
  static struct termios oldt, newt;
 
  if ( dir == 1 )
  {
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
  }
  else
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
}
int kbhit (void){
  struct timeval tv;
  fd_set rdfs;
 
  tv.tv_sec = 0;
  tv.tv_usec = 0;
 
  FD_ZERO(&rdfs);
  FD_SET (STDIN_FILENO, &rdfs);
 
  select(STDIN_FILENO+1, &rdfs, NULL, NULL, &tv);
  return FD_ISSET(STDIN_FILENO, &rdfs);
}
bool checkSocket(unsigned int *sock){
    int errorCount=0;
  	while(true){
		if ((*sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		{
			printf("Socket creation error \n");
			errorCount++;
			if(errorCount > 100){
				printf("Too much try.\n");
				return false;
			}
		}else{
			return true;
		}
	}
}
bool checkServer(std::string *ip,struct sockaddr_in *serv_addr,unsigned int *port,unsigned int sock,bool ipvalid){
    int errorCount=0;
  	while(true){
  		if(ipvalid){
			printf("entrer votre ip:");
			std::cin >> (*ip);
		}
		memset(&*serv_addr, '0', sizeof(*serv_addr));

		(*serv_addr).sin_family = AF_INET;
		(*serv_addr).sin_port = htons(*port);
		//connection server
		if(inet_pton(AF_INET, (*ip).c_str(), &(*serv_addr).sin_addr)<=0)
		{
			printf("Invalid address(%s:%d)/ Address not supported \n",(*ip).c_str() ,*port);
			errorCount++;
		}else{
			if (connect(sock, (struct sockaddr *)&*serv_addr, sizeof(*serv_addr)) < 0)
			{
				printf("Connection Failed \n");
				errorCount++;
			}else{
				return true;
			}
		}
		if(errorCount > 100){
			printf("Too much try.\n");
			return false;
		}
    }
}
bool checkDispPseudo(std::string *pseudo,unsigned  int sock){
    signed int valRead;
    unsigned int minSizepseudo=5;
    char buffer[1024] = {0};
    int errorCount=0;
  	while(true){
		std::string tmp;
	  	while((*pseudo).length()<minSizepseudo){
			printf("Entrer votre pseudo:\n");
			std::cin >> (*pseudo);
	  		if((*pseudo).length()<minSizepseudo){
				printf("Votre pseudo est trop cour (%d caractère minimum)\n",minSizepseudo);
	  		}
			std::stringstream ss;
			ss << "70" << (*pseudo);
			std::string tmp = ss.str();
			if(!(*pseudo).length()<minSizepseudo){
    			send(sock , tmp.c_str() , tmp.length() , 0 );
			}
	  	}
    	valRead = read( sock , buffer,1337);
    	if(valRead != 1337){
    		if(buffer[0]!='1'){
    			errorCount++;
    			if(errorCount%100==0){
	    			printf("pseudo reffusé\n");
	    			tmp="";
	    			if(errorCount>10000){
	    				return false;
	    			}
    			}
    		}else{
    			printf("pseudo ajouté\n");
    			return true;
    		}
    	}
    }
}
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
void printMenu(int roomNumber,std::map <int, int> roomList,std::string ip){
	printf("\t\t\tServeur %s\n",ip.c_str());
	printf("0 . Quit\n");
	for (int i = 0; i < roomNumber; ++i){
		printf("%d . %d\t",i+1,roomList[i]);
		if(i!=0&&i%4==0)
			printf("\n");
	}
}
void clearIn(){while ( getchar() != '\n' );}
int roomGest(std::string menuInfo,std::map <int, int> *roomList){
	char numstr[4];
	int prevNum=1;
	int prev=0;
	for (int i = 1; i < menuInfo.length(); i++){
		if((menuInfo.c_str())[i]=='&'||i+1== menuInfo.length()){
			(*roomList)[prev] = atoi(numstr);
			prevNum=i+1;
			prev++;
		}else{
			numstr[i-prevNum]=(menuInfo.c_str())[i];
		}
	}
	return prev;
}

int main(int argc, char const *argv[]){
    struct sockaddr_in serv_addr;
	std::map <int, int> roomList;
	std::string previousHist="";
	std::string menuInfo="08001&8002&8003&8004";
    std::string pseudo="";
    std::string msg="Hello from client";
	std::string Hist="//si startport!= port\n/\n/\n/\n/\n/\n/\n/\n//|attendr\n/\n//|attendr\n/\n//|attendr\n/\n//|attendr\n/\n//|attendr\n/\n//|attendr\n/\n//|attendr\n/\n//|attendr\n/\n//|attendre q//|attendre q//|attendre q//|attendre q//|attendre que le pseudo soit ajouté	attendre une requette type 1&p|attendre que le pseudo soit ajouté	attendre une requette type 1&pseudo\n//|affichage serv /info/input			faire une requette 0 pour la liste et 1 pour les info du serv reponse type 2&... sauvegarde des room dans un tableau\n//finsi						\n//|entrer room	\n// |ecrire message(si press enter arreter affichage et faire un cin)\n// |changer room(press echap)\n";
    std::string comInfo="71";
    std::string comMess="50";
    std::string ip="";
    unsigned int minSizepseudo=5;
    unsigned int port = 8001;
    unsigned int oriPort = 8001;
    unsigned int sock = 0;
    bool checkMenuQuit = false;
    bool checkServ = false;
    bool checkSock = false;
    bool checkQuit = false;
    bool checksend = false;
    int errorCount=0;
	int roomNumber=0;
  	int input=-1;
    if(!checkSocket(&sock)){
    	return -1;
    }
    if(!checkServer(&ip,&serv_addr,&port,sock,true)){
    	return -1;
    }
	if(!checkDispPseudo(&pseudo,sock)){
    	return -1;
    }
	while(true){
		checkQuit = false;
    	checkMenuQuit = false;
		send(sock , comInfo.c_str() , comInfo.length() , 0 );
		menuInfo=readToString(sock);
		roomNumber = roomGest(menuInfo,&roomList);
		msg="";
		while(!checkMenuQuit){
			system("clear");
			printMenu(roomNumber,roomList,ip);
			printf("\nEntrer votre choix:\n");
			std::cin >> msg;
			if(msg.compare("0")==0){
				return -1;
			}
			for (int i = 0; i < roomNumber; ++i){
				std::stringstream ss;
				ss << i+1;
				std::string test = ss.str();
				if(msg.compare(test)==0){
					port=roomList[i+1];
					checkMenuQuit=true;
					break;
				}
			}
			msg="";
			clearIn();
		}
	    if(!checkServer(&ip,&serv_addr,&port,sock,false)){
	    	return -1;
	    }
		while(!checkQuit){
			changemode(1);
			while(!kbhit()){
				if (Hist.compare(previousHist)!=0||checksend==true){
					system("clear");
					std::cout << Hist;
					previousHist = Hist;
					printf("press :\techat(quit)\tenter(whrite)\n");
					checksend=false;
				}
			    send(sock , comMess.c_str() , comMess.length() , 0 );
			    Hist = readToString(sock);
				my_sleep(400);
			}
			int ch;
			ch=getchar();
			changemode(0);
			if(10==ch){//enter
				checksend=true;
				printf("Entrer votre text:\n");
				std::cin >> msg;
				std::stringstream ss;
				ss << "1" << pseudo << "&" << msg.c_str();
				msg = ss.str();
				send(sock , msg.c_str() , msg.length() , 0 );
			}else if(27==ch){//esc
				printf("Press enter to exit\n");
				checkQuit=true;
			}
				clearIn();
		}
		port=oriPort;
	    if(!checkServer(&ip,&serv_addr,&port,sock,true)){
	    	return -1;
	    }
	}
    return 0;
}
