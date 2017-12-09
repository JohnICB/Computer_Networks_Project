#include <arpa/inet.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/mman.h>

#define BUFF_SIZE 1024
#define PORT 5000
//extern int errno;

int nr_clients = -1;

typedef struct pipes pipes;

struct pipes
{
    int pipe_desc[2];
    char buffer[BUFF_SIZE];
    int pipe_number;
    int client_desc;

    int isInUse;
};

int str_echo(int clt_desc, pipes *pipe_handler);
void child_exiting_handler(pipes *p, int serv_desc, int clt_desc, bool isError)
{
    if (isError == true)
    {
        perror("Error writing to pipe from child \n");
    }
    if (serv_desc != 0)
    {
        close(serv_desc);
    }
    close(clt_desc);
    close(p->pipe_desc[1]);
    close(p->pipe_desc[0]);

}

static int *nr;
int main()
{
    int server_descriptor,client_descriptor,len;
    struct sockaddr_in serv,cli;
    struct pipes pipe_handler[100];
    pid_t pid;
    int number_of_clients = 0;
    int optval=1; //opt for setsocketopt()
    nr = mmap(NULL, sizeof *nr, PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    *nr = 0;
    //creating socket
    if ((server_descriptor = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror ("Error creating socket()\n");
        return errno;
    }

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
    printf("Server is up and running. Waiting for connections...\n");

    if(listen(server_descriptor, 5) == -1)
    {
        perror ("Error listening()\n");
        return errno;
    }
        int pipeA[2]; 

        pipe(pipeA);

    for(; ;)
    {
        len=sizeof(cli);
        //accepting client connection
        client_descriptor=accept(server_descriptor, (struct sockaddr*)&cli, (socklen_t *) &len);
        if (client_descriptor < 0) {
            perror("Error accepting()\n");
            continue;
        }
        printf("Connected to Client w/ desc = %d\n", client_descriptor);

        nr_clients++;
        *nr = *nr + 1;
        printf("parent nr ++ %d -> %d\n", nr , *nr);
        if(nr_clients % 2 == 0)
        {
            pipe(pipe_handler[nr_clients].pipe_desc); //open pipe for current client and next client, so they can communicate to each other
            pipe(pipe_handler[nr_clients+1].pipe_desc);
            *nr = *nr + 4;
        }
        pipe_handler[nr_clients].pipe_number = nr_clients;
        pipe_handler[nr_clients].client_desc = client_descriptor;
        pipe_handler[nr_clients].isInUse = 1;


        //number_of_clients++;

        pid = fork();
        if (pid < 0)
        {
            perror("FORK error\n");
            return errno;
        }

        if(pid == 0)
        {
  
            if (nr_clients % 2 == 0)
            {
            	char msg[BUFF_SIZE] = " nada here yet \n"; 
                if ( read(pipe_handler[nr_clients + 1].pipe_desc[0], msg, BUFF_SIZE) < 0)
            		{
            		    child_exiting_handler(&pipe_handler[nr_clients], server_descriptor, client_descriptor, true);
            		    printf("Child process killed by error from pipe with PID = %d\n", getpid());
            		    exit(0);
            		}

            		printf("my hommie said : %s\n", msg);

            } else{
            	if (nr_clients % 2 == 1)
            	{	
            		char msg[BUFF_SIZE] = "wazzup homie\n"; 
                	printf("%d I'm looking for partner.. \n", pipe_handler[nr_clients].pipe_number);

            		if ( write(pipe_handler[nr_clients].pipe_desc[1], msg, BUFF_SIZE) < 0)
            		{
            		    child_exiting_handler(&pipe_handler[nr_clients], server_descriptor, client_descriptor, true);
            		    printf("Child process killed by error from pipe with PID = %d\n", getpid());
            		    exit(0);
            		}
                
            	}

            }
            str_echo(client_descriptor, &pipe_handler[nr_clients]);

            child_exiting_handler(&pipe_handler[nr_clients], server_descriptor, client_descriptor, false);
            *nr = *nr - 3;
            printf("child nr -5 %d -> %d\n", nr , *nr);
           // printf("Exiting child %d\n", getpid());

            exit(0);
        }
        else //is parent
        {
            int status;
            waitpid(-1, &status, WNOHANG); //nu lasam copii atarnati, orfani, zombi etc
            /* TODO: Check which file descriptors are aviable to determinate which client has disconnected*/

        }
            printf("nr = %d\n", *nr);
    }
    close(client_descriptor);
    close(server_descriptor);
    return 0;
}

int str_echo(int clt_desc, pipes *pipe_handler)
{
    int bytes;			/* numarul de octeti cititi/scrisi */
    unsigned char msg[BUFF_SIZE];		//mesajul primit de la client
    //char msgrasp[BUFF_SIZE]=" ";        //mesaj de raspuns pentru client

   // printf("Talking with client : %d\n", clt_desc);
    fflush (stdout);

    bzero(msg, BUFF_SIZE);

    do{

        bytes = (int) read (clt_desc, msg, BUFF_SIZE);
        if (bytes < 0)
        {
            perror ("Error reading from client()\n");
            return errno;
        }

        if (bytes == 0 )
        {
            printf("[SYSTEM] Client[%d] has disconnected.\n", clt_desc);
            return bytes;
        }

        printf ("Client[%d]: %s\n", clt_desc, msg);
        fflush (stdout);

        if (write (clt_desc, msg, (size_t) bytes) < 0)
        {
            perror ("Error writing to client()\n");
            return errno;
        }


    }while(bytes > 0);
    printf("Bytes = %d\n", bytes);

    child_exiting_handler(pipe_handler, 0, clt_desc, false);
    bzero(&pipe_handler, sizeof(pipe_handler));
    nr_clients--;
    return bytes;
}
