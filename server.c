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
#define BUFF_SIZE 1024

extern int errno;
//struct game_data{
//	int fd_cl_A = -1; //file descriptor client 1
//	int fd_cl_B = -1; //file descriptor client 2
//
//	char map[MAP_LINES][MAP_COLUMNS]; //harta jocului
//	unsigned char playerTurn = 1; //de la cine fac read, adica cui ii e randul
//	int score_player_A = 0; //scorul jucatorilor
//	int score_player_B = 0; //scorul jucatorilor
//};

int str_echo(int s);
//void play_game(struct gameData);


int main()
{
	int ls,client_descriptor,len;
	struct sockaddr_in serv,cli;
	pid_t pid;
	int number_of_clients = 0;
	int nfds = 3; //number of file descriptors

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
	listen(ls,5);
	puts("Listening for Client...");
	for(; ;)
	{
		len=sizeof(cli);
//accepting client connection
		client_descriptor=accept(ls,(struct sockaddr*)&cli,&len);
		//puts("Connected to Client..."); 
		printf("Connected to Client with DESCRIPTOR = %d\n", client_descriptor);
//creating child process

		 if (nfds < client_descriptor) /* ajusteaza valoarea maximului */
            nfds = client_descriptor;


		int pipe_desc[2]; //0 = read 1 = write
		pipe(pipe_desc);

		number_of_clients++;
		//printf("Number of clients incremented, now = %d\n", number_of_clients);

		if((pid=fork()) == 0)
		{
			//char b;
			//b = itoa(client_descriptor);
			write(pipe_desc[1], &client_descriptor, sizeof(client_descriptor));

			//puts("Child process created...");
			printf("Child proces created with PID = %d\n", getpid());
			str_echo(client_descriptor);
			close(ls);
			close(client_descriptor);
			printf("Child proces KILLED with PID = %d\n", getpid());
			exit(0);
		}
		else //e parintele
		{
			int clt_A, clt_B;
			//char b[10];
			//printf("*This is parent with PID = %d\n", getpid());
			//printf("*Number of clients: %d\n", number_of_clients);

			if (number_of_clients % 2 == 1 )//&& number_of_clients != 1)
			{
				//printf("1\n");
				if(read(pipe_desc[0], &clt_A, sizeof(clt_A)) < 0)
				{
					perror("RIP Read from PIPE");
					return errno;
				}
				printf("%d CLT A\n", clt_A);
				//clt_A = atoi(b);
				//printf("2\n");
			}
			else if (number_of_clients % 2 == 0)
			{
				//printf("3\n");
				if(read(pipe_desc[0], &clt_B, sizeof(clt_B)) < 0)
				{
					perror("RIP Read from PIPE");
					return errno;
				}	
				printf("%d CLT B\n", clt_B);
				//printf("4\n");
				//clt_B = atoi(b);
			}
		}

		//number_of_clients--;
		close(client_descriptor);
	}
	return 0;
}

int str_echo(int s)
{
	//char buf[50];
	//do{
//receiving data from client
		//recv(s,buf,50,0);
		//puts("Message from Client...");
		//fputs(buf,stdout);
		//send(s,buf,50,0);
	//}while(strlen(buf)>1);

  char buffer[BUFF_SIZE];		/* mesajul */
  int bytes;			/* numarul de octeti cititi/scrisi */
  char msg[BUFF_SIZE];		//mesajul primit de la client 
  char msgrasp[BUFF_SIZE]=" ";        //mesaj de raspuns pentru client
printf("Talking with client : %d\n", s);
  bzero(msg, BUFF_SIZE);
do{

  bytes = read (s, msg, BUFF_SIZE);
  if (bytes < 0)
    {
      perror ("Eroare la read() de la client.\n");
      return errno;
    }
    if (bytes == 0)
    {
      //printf("s-a stins\n");
      return 0;
    }
  printf ("[server]Mesajul a fost receptionat...%s\n", msg);
  //printf("StrlenMSG 1 : \"%s\" =  %d\n",msg, strlen(msg) );
  /*pregatim mesajul de raspuns */
  //bzero(msgrasp,BUFF_SIZE);
  //strcat(msgrasp,"Hello ");
  //strcat(msgrasp,msg);
      
  //printf("[server]Trimitem mesajul inapoi...%s\n",msg);
      
  if (write (s, msg, bytes) < 0)
    {
      perror ("[server] Eroare la write() catre client.\n");
      return errno;
    }

  }while(strlen(msg) > 1);

  printf("StrlenMSG: \"%s\" =  %d\n",msg, strlen(msg) );
  return bytes;
}

//void play_game(struct gameData)
//{
//
//}