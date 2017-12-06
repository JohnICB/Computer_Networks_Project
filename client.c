//client.c

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>

#define BUFF_SIZE 1024


extern int errno;

int str_echo(int server_fd); //takes care of the communication

int main()
{
	int server_fd;
<<<<<<< HEAD
	struct sockaddr_in server;
/*creating socket*/
	if ( (server_fd = socket(AF_INET,SOCK_STREAM,0)) == -1)
		{
		perror ("Error creating socket()\n");
      	return errno;
		}


=======
	struct sockaddr_in cli;
	puts("I am Client...");
/*creating socket*/
	server_fd=socket(AF_INET,SOCK_STREAM,0);
	puts("Socket Created Successfully...");
>>>>>>> master
/*socket address structure*/
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=inet_addr("127.0.0.1");
	server.sin_port=htons(5000);

/*connecting to server*/
<<<<<<< HEAD
	if (connect (server_fd, (struct sockaddr *) &server,sizeof (struct sockaddr)) == -1)
    {
      perror ("Error connecting()\n");
      return errno;
    }

	str_echo(server_fd); //communication takes place here

	close(server_fd);
	return 0;
}
int str_echo(int server_fd)
{
	char buf[BUFF_SIZE];//,buf1[BUFF_SIZE];
	int bytes = 0;
	int sz = 1; //bytes size
	do{
		bzero(buf, BUFF_SIZE);
		printf("Enter the Message...\n");
		fflush (stdout);
		
		if ((bytes = read(0, buf, BUFF_SIZE)) < 0)
		{
			perror ("Error reading from stdin\n");
			return errno;
		}

		buf[strlen(buf) - 1] = '\0'; //remove '\n'
		
		if(strlen(buf) > 0)
		{
		//printf("StrlenMSG: \"%s\" =  %d\n",buf, strlen(buf) );

			if (write (server_fd, buf, BUFF_SIZE) < 0)
		    {
		      perror ("Error while writing to the Server.\n");
		      return errno;
		  	}


			//receiving data from server
			sz = read(server_fd, buf, BUFF_SIZE);
			if (sz < 0)
			{
				perror ("Error reading from stdin\n");
				return errno;
			}
			printf("Server: %s\n",buf);
			fflush (stdout);
		}
		else
		{
			
		}
		
	
	}while(bytes > 0);
 

=======
	connect(server_fd,(struct sockaddr*)&cli,sizeof(cli));
	puts("Connected with Server...");
	//printf("%d returned \n",str_echo(server_fd));
	str_echo(server_fd);
	close(server_fd);
	return 0;
}

int str_echo(int server_fd)
{
	char buf[BUFF_SIZE];//,buf1[BUFF_SIZE];
	do{
	bzero(buf, BUFF_SIZE);
	//puts("Enter the Message...");
	//fgets(buf,BUFF_SIZE,stdin);
	//send(server_fd,buf,BUFF_SIZE,0);
	//recv(server_fd,buf1,BUFF_SIZE,0);
	//puts("Message from Server...");
	//fputs(buf1,stdout);
	
	printf("Enter the Message...\n");
	
	if (read(0, buf, BUFF_SIZE) < 0)
	{
		perror ("Error reading from stdin\n");
		return errno;
	}

	//printf("%s was lalala \n", buf);
	//fputs(buf, BUFF_SIZE);
	//sending data to server
	if (strlen(buf) > 2)
	{
		/* code */
	}
	buf[strlen(buf) - 1] = '\0';
	//write(0, buf, BUFF_SIZE);

	if (write (server_fd, buf, BUFF_SIZE) < 0)
    {
      perror ("Error while writing to the Server.\n");
      return errno;
  	}

  	int sz;
	//receiving data from server
	sz = read(server_fd, buf, BUFF_SIZE);
	if (sz < 0)
	{
		perror ("Error reading from stdin\n");
		return errno;
	}

	printf("Message from Server...  %s\n",buf);
	//snprintf(stdout, sz,"Message fromm Server... %s\n", buf);

	}while(strlen(buf)>1);
>>>>>>> master
}