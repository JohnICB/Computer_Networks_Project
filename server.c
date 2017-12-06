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
<<<<<<< HEAD
#define PORT 5000
extern int errno;
typedef struct pipe_com_handler  pipe_com_handler;
=======

extern int errno;
>>>>>>> master
//struct game_data{
//	int fd_cl_A = -1; //file descriptor client 1
//	int fd_cl_B = -1; //file descriptor client 2
//
//	char map[MAP_LINES][MAP_COLUMNS]; //harta jocului
//	unsigned char playerTurn = 1; //de la cine fac read, adica cui ii e randul
//	int score_player_A = 0; //scorul jucatorilor
//	int score_player_B = 0; //scorul jucatorilor
//};

<<<<<<< HEAD

struct pipe_com_handler{ //communication handler
	int cl_A;
	int cl_B;

	char message[BUFF_SIZE];
};

int str_echo(int s);
void write_to_pipe(int pipe_desc[2]);
pipe_com_handler read_from_pipe(int pipe_desc[2]);
=======
int str_echo(int s);
>>>>>>> master
//void play_game(struct gameData);


int main()
{
<<<<<<< HEAD
	int server_descriptor,client_descriptor,len;
	struct sockaddr_in serv,cli; 
	pid_t pid;
	int number_of_clients = 0;
	int nfds = 3; //number of file descriptors
	int optval=1; //opt for setsocketopt()

	//creating socket
	if ((server_descriptor = socket (AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror ("Error creating socket()\n");
		return errno;
	}
=======
	int ls,client_descriptor,len;
	struct sockaddr_in serv,cli;
	pid_t pid;
	int number_of_clients = 0;
	int nfds = 3; //number of file descriptors
>>>>>>> master

	//socket options
	setsockopt(server_descriptor, SOL_SOCKET, SO_REUSEADDR,&optval,sizeof(optval));

	//socket address structure
	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = htonl(INADDR_ANY); //127.0.0.1
	serv.sin_port=htons(PORT); //5000



	if ((bind(server_descriptor,(struct sockaddr*)&serv,sizeof(serv))) == -1)
	{
		perror ("Error binding()\n");
		return errno;
	}
	if(listen(server_descriptor,5) == -1)
	{
		perror ("Error listening()\n");
		return errno;
	}

<<<<<<< HEAD
	for(; ;)
	{
		len=sizeof(cli);
	//accepting client connection
		client_descriptor=accept(server_descriptor,(struct sockaddr*)&cli, &len);	
		if (client_descriptor < 0)
		{
			perror ("Error accepting()\n");
			continue;
		}
		//puts("Connected to Client..."); 
		printf("Connected to Client with DESCRIPTOR = %d\n", client_descriptor);
	//creating child process

		 if (nfds < client_descriptor) /* ajusteaza valoarea maximului */
		nfds = client_descriptor;

=======
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
>>>>>>> master

		 if (nfds < client_descriptor) /* ajusteaza valoarea maximului */
            nfds = client_descriptor;


		int pipe_desc[2]; //0 = read 1 = write
		pipe(pipe_desc);

		number_of_clients++;
		//printf("Number of clients incremented, now = %d\n", number_of_clients);

		if((pid=fork()) == 0)
		{
<<<<<<< HEAD
			printf("Child proces created with PID = %d\n", getpid());

			//char b;
			//b = itoa(client_descriptor);
			if ( write(pipe_desc[1], &client_descriptor, sizeof(client_descriptor)) < 0)
			{
				perror("Error writing to pipe from child \n");
				close(server_descriptor);
				close(client_descriptor);
				close(pipe_desc[1]);
				close(pipe_desc[0]);
				printf("Child proces KILLED with PID = %d\n", getpid());
				exit(0);
			}

			str_echo(client_descriptor);
			
			//puts("Child process created...");
			close(server_descriptor);
			close(client_descriptor);
			close(pipe_desc[1]);
			close(pipe_desc[0]);
=======
			//char b;
			//b = itoa(client_descriptor);
			write(pipe_desc[1], &client_descriptor, sizeof(client_descriptor));

			//puts("Child process created...");
			printf("Child proces created with PID = %d\n", getpid());
			str_echo(client_descriptor);
			close(ls);
			close(client_descriptor);
>>>>>>> master
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
		int status;
		waitpid(-1, &status, WNOHANG); //nu lasam copii atarnati, orfani, zombi etc

		//number_of_clients--;

<<<<<<< HEAD
=======
		//number_of_clients--;
		close(client_descriptor);
>>>>>>> master
	}
	close(client_descriptor);
	return 0;
}

int str_echo(int s)
{
<<<<<<< HEAD
=======
	//char buf[50];
	//do{
//receiving data from client
		//recv(s,buf,50,0);
		//puts("Message from Client...");
		//fputs(buf,stdout);
		//send(s,buf,50,0);
	//}while(strlen(buf)>1);

>>>>>>> master
  char buffer[BUFF_SIZE];		/* mesajul */
  int bytes;			/* numarul de octeti cititi/scrisi */
  char msg[BUFF_SIZE];		//mesajul primit de la client 
  char msgrasp[BUFF_SIZE]=" ";        //mesaj de raspuns pentru client
<<<<<<< HEAD

  printf("Talking with client : %d\n", s);
  fflush (stdout);

  bzero(msg, BUFF_SIZE);

  do{

  	bytes = read (s, msg, BUFF_SIZE);
  	if (bytes < 0)
  	{
  		perror ("Error reading from client()\n");
  		return errno;
  	}
  	
  	if (bytes == 0 )
  	{
  		printf("s-a stins\n");
  		return bytes;
  	}

  	printf ("Client: %s\n", msg);
  	fflush (stdout);
  	
  	if (write (s, msg, bytes) < 0)
  	{
  		perror ("Error writing to client()\n");
  		return errno;
  	}
  	

  }while(bytes > 0);
  printf("Bytes = %d\n", bytes);

=======
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
>>>>>>> master
  return bytes;
}

//void play_game(struct gameData)
//{
//
//}