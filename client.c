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
	struct sockaddr_in cli;
	puts("I am Client...");
/*creating socket*/
	server_fd=socket(AF_INET,SOCK_STREAM,0);
	puts("Socket Created Successfully...");
/*socket address structure*/
	cli.sin_family=AF_INET;
	cli.sin_addr.s_addr=inet_addr("127.0.0.1");
	cli.sin_port=htons(5000);
/*connecting to server*/
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
}