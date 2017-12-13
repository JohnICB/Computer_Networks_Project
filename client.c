
//client.c

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define BUFF_SIZE 1024


extern int errno;

typedef struct pipes pipes;

struct pipes
{
    char buffer[100];
    int client_desc;
    int client_id;

    bool isInUse;
};

int str_echo(int server_fd); //takes care of the communication

int main()
{
	int server_fd;
	struct sockaddr_in server;
	int answer = 0;
/*creating socket*/
while (answer == 0)
{
	if ( (server_fd = socket(AF_INET,SOCK_STREAM,0)) == -1)
		{
		perror ("Error creating socket()\n");
      	return errno;
		}


/*socket address structure*/
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=inet_addr("127.0.0.1");
	server.sin_port=htons(5000);

/*connecting to server*/
	if (connect (server_fd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
      perror ("Error connecting()\n");
      return errno;
    }

	answer = str_echo(server_fd); //communication takes place here
	close(server_fd);
}
	return 0;
}
int str_echo(int server_fd)
{
	char buf[BUFF_SIZE];//,buf1[BUFF_SIZE];
	ssize_t bytes = 0;
	int sz = 1; //bytes size
	//printf("Reading from Server...\n");
	bzero(&buf, BUFF_SIZE);
	int cl_number;
	pipes pip;

	if ((bytes = read(server_fd, &pip, BUFF_SIZE)) < 0)
	{
		perror ("Error reading from stdin\n");
		return errno;
	}
	//TODO: Insert name
	//printf("Server: %d\n", pip.client_desc);
	//printf("Server: %s\n", pip.buffer);
	if (pip.client_desc % 2 == 0)
	{
		printf("You are client A!\n");

		if ((bytes = read(server_fd, buf, BUFF_SIZE)) < 0)
			{
				perror ("Error reading from stdin\n");
				return errno;
			}
		printf("The other client has connected! You start!\n");

		do{
			
			bzero(buf, BUFF_SIZE);
			printf("You: ");
			fflush(stdout);
			if ((bytes = read(0, buf, BUFF_SIZE)) < 0)
			{
				perror ("Error reading from stdin\n");
				return errno;
			}
			//if (strlen(buf) == 1)
			//{
			//	printf("Type something..\n");
			//}

			buf[strlen(buf) - 1] = '\0'; //remove '\n'
			//printf("cmp: %d\n",strcmp(buf, "/quit"));
			if (strcmp(buf, "/quit") == 0)
			{
				printf("Thanks for playing! Exiting now..\n");
				return 1;
			}
			//printf("You said: %s",buf);
			//fflush(stdout);

			if (write (server_fd, buf, BUFF_SIZE) < 0)
		    {
		      perror ("Error while writing to the Server.\n");
		      return errno;
		  	}

		  	bzero(buf, BUFF_SIZE);
			if ((bytes = read(server_fd, buf, BUFF_SIZE)) < 0)
			{
				perror ("Error reading from stdin\n");
				return errno;
			}

		  	if (strcmp(buf, "DISCONNECTED") == 0)
			{
				printf("Opponent has dissconnected..\nReconnecting..\n");
				return 0;
			}
			printf("Client B: %s\n",buf);
			fflush(stdout);

		}while(1);
	}
	else
	{
		printf("You are Client B You are second!\n" );


	do{
			bzero(buf, BUFF_SIZE);
			if ((bytes = read(server_fd, buf, BUFF_SIZE)) < 0)
			{
				perror ("Error reading from stdin\n");
				return errno;
			}
			if (bytes == 0)
			{
				printf("Lost connection to the server...\nWant to try to reconnect ? Y/N..\n");
				///TODO: handle answer
				return 0;
			}
		  	if (strcmp(buf, "DISCONNECTED") == 0)
			{
				printf("Opponent has dissconnected..\n");
				return 0;
			}

			printf("Client A: %s\n",buf);
			fflush(stdout);

			bzero(buf, BUFF_SIZE);
			printf("You: ");
			fflush(stdout);
			if ((bytes = read(0, buf, BUFF_SIZE)) < 0)
			{
				perror ("Error reading from stdin\n");
				return errno;
			}
			//if (strlen(buf) == 1)
			//{
			//	printf("Type something..\n");
			//}

			buf[strlen(buf) - 1] = '\0'; //remove '\n'
			//printf("cmp: %d\n",strcmp(buf, "/quit"));
			if (strcmp(buf, "/quit") == 0)
			{
				printf("Thanks for playing! Exiting now..\n");
				return 1;
			}

			if (write (server_fd, buf, BUFF_SIZE) < 0)
		    {
		      perror ("Error while writing to the Server.\n");
		      return errno;
		  	}

		
	
		}while(bytes > 0);
 
	}
}

		//printf("Enter the Message...\n");
		//fflush (stdout);
		
		//if ((bytes = read(0, buf, BUFF_SIZE)) < 0)
		//{
		//	perror ("Error reading from stdin\n");
		//	return errno;
		//}

		//if (strlen(buf) == 1)
		//{
		//	printf("Type something..\n");
		//}

		//buf[strlen(buf) - 1] = '\0'; //remove '\n'
//
//		//if (strcmp(buf, "/quit") == 0)
//		//{
//		//	printf("Thanks for playing! Exiting now..\n");
//		//	return 0;
		//}
		
		//if(strlen(buf) > 0)
		//{
		//printf("StrlenMSG: \"%s\" =  %d\n",buf, strlen(buf) );

			//receiving data from server
			//sz = read(server_fd, buf, BUFF_SIZE);
			//if (sz < 0)
			//{
			//	perror ("Error reading from stdin\n");
			//	return errno;
			//}
			//printf("Enter the Message...\n");
