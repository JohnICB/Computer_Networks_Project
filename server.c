#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

#define MAP_LINES 10
#define MAP_COLUMNS 10

//struct game_data{
//	int fd_cl_A = -1; //file descriptor client 1
//	int fd_cl_B = -1; //file descriptor client 2
//
//	char map[MAP_LINES][MAP_COLUMNS]; //harta jocului
//	unsigned char playerTurn = 1; //de la cine fac read, adica cui ii e randul
//	int score_player_A = 0; //scorul jucatorilor
//	int score_player_B = 0; //scorul jucatorilor
//};

void str_echo(int s);
//void play_game(struct gameData);


int main()
{
	int ls,cs,len;
	struct sockaddr_in serv,cli;
	pid_t pid;
	int number_of_clients = 0;

	puts("I am Server...");
//creating socket
	ls=socket(AF_INET,SOCK_STREAM,0);
	puts("Socket Created Successfully...");
//socket address structure
	serv.sin_family=AF_INET;
	serv.sin_addr.s_addr=INADDR_ANY;
	serv.sin_port=htons(5000);


	bind(ls,(struct sockaddr*)&serv,sizeof(serv));
	puts("Binding Done...");
	listen(ls,3);
	puts("Listening for Client...");
	for(; ;)
	{
		len=sizeof(cli);
//accepting client connection
		cs=accept(ls,(struct sockaddr*)&cli,&len);
		puts("Connected to Client...");
		number_of_clients++;
//creating child process

		int pipe_desc[2]; //0 = read 1 = write
		pipe(pipe_desc);


		if((pid=fork()) == 0)
		{
			char b;
			b = itoa(cs);
			write(pipe_desc[1], b, 10);	

			puts("Child process created...");
			close(ls);
			str_echo(cs);
			close(cs);
			exit(0);
		}
		else //e parintele
		{
			int clt_A, clt_B;
			char b[10];

			if (number_of_clients % 2 == 1)
			{
				read(pipe_desc[0], b, 10);	
				clt_A = atoi(b);
			}
			else
			{
				read(pipe_desc[0], b, 10);	
				clt_B = atoi(b);
			}
		}

		close(cs);
		number_of_clients--;
	}
	return 0;
}

void str_echo(int s)
{
	char buf[50];
	do{
//receiving data from client
		recv(s,buf,50,0);
		puts("Message from Client...");
		fputs(buf,stdout);
		send(s,buf,50,0);
	}while(strlen(buf)>1);
}

//void play_game(struct gameData)
//{
//
//}