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


static int *nr;
static int *nr_clients;

typedef struct pipes pipes;

struct pipes
{
    int pipe_desc[2];
    char buffer[BUFF_SIZE];
    int pipe_number;
    int client_desc;
    int client_id;

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

    bzero(p, sizeof(*p));

}
int str_echo(int clt_desc, pipes *pipe_handler)
{
    int bytes;			/* numarul de octeti cititi/scrisi */
    unsigned char msg[BUFF_SIZE];		//mesajul primit de la client
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
            printf("[SYSTEM] Client[%d] has disconnected.\n", pipe_handler->client_id);
            return bytes;
        }

        printf ("Client[%d]: %s\n", pipe_handler->client_id, msg);
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
    return bytes;
}
int client_A_handler(pipes *players)
{
    pipes cltA = players[*nr_clients];
    int *pip = players[*nr_clients + 1].pipe_desc;
    ssize_t bytes = -1;
    char msg[BUFF_SIZE] = "Client A\n";

    printf("A: p0 p1 %d %d\n", pip[0], pip[1]);

    if ((read(pip[0], &msg, BUFF_SIZE)) < 0)
    {
        perror("Error reading from pipe\n");
        return errno;
    }
    printf("clt a: read from pipe %s\n", msg);


    do{
        //bzero(&msg, BUFF_SIZE);
        if ((write(cltA.client_desc, msg, BUFF_SIZE)) < 0)
        {
            perror("Error writing to clt a desc\n");
            return errno;
        }
        if ((read(cltA.client_desc, msg, BUFF_SIZE)) < 0)
        {
            perror("Error reading from clt a desc\n");
            return errno;
        }


        printf("clt a said: %s", msg);
        fflush(stdout);

        if ((write(pip[1], msg, BUFF_SIZE)) < 0)
        {
            perror("Error writing to pipe\n");
            return errno;
        }

        if ((bytes = read(pip[0], msg, BUFF_SIZE)) < 0)
        {
            perror("Error reading from pipe\n");
            return errno;
        }




} while (bytes);
    return 0;

}
int client_B_handler(pipes *players)
{
    pipes cltB = players[*nr_clients];
    int *pip = players[*nr_clients].pipe_desc;
    ssize_t bytes = -1;

    char msg[BUFF_SIZE] = "Insert message: \n";
    printf("B: p0 p1 %d %d\n", pip[0], pip[1]);
    if ((write(cltB.client_desc, msg, BUFF_SIZE)) < 0)
    {
        perror("Error writing to clt b desc\n");
        return errno;
    }

    do{
       // bzero(&msg, BUFF_SIZE);
        if ((read(cltB.client_desc, msg, BUFF_SIZE)) < 0)
        {
            perror("Error reading from clt b desc\n");
            return errno;
        }
        if ((write(pip[1], msg, BUFF_SIZE)) < 0)
        {
            perror("Error writing to pipe\n");
            return errno;
        }

       // sleep(3);
        if ((bytes = read(pip[0], msg, BUFF_SIZE)) < 0)
        {
            perror("Error reading from pipe\n");
            return errno;
        }
        printf("b: msg dupa read pipe 0 : %s\n" , msg);
        if ((write(cltB.client_desc, msg, BUFF_SIZE)) < 0)
        {
            perror("Error writing to clt b desc\n");
            return errno;
        }
        printf("clt b  answered : %s", msg);
        fflush(stdout);

        //printf("cl b = %s ", msg);



    } while (bytes);
    return 0;
}

int simulate_clientAB(pipes *playerList)
{
    int numb = *nr_clients;
    pipes cltA = playerList[numb -1];

   // while(*nr_clients % 2 == 0) {}

    pipes cltB = playerList[numb];

    unsigned char msg[BUFF_SIZE] = "Welcome to Connect4\nYou start!\n";

    if ((write(cltA.client_desc, msg, BUFF_SIZE)) < 0)
    {
        perror("Error writing to clt a desc\n");
        return errno;
    }

    while(1)
    {
        bzero(&msg, BUFF_SIZE);

        if ((read(cltA.client_desc, msg, BUFF_SIZE)) < 0)
        {
            perror("Error reading from clt a desc\n");
            return errno;
        }

        //printf("msg from cltA: %s\n", msg);
        //fflush(stdout);
        if ((write(cltB.client_desc, msg, BUFF_SIZE)) < 0)
        {
            perror("Error writing to clt b desc\n");
            return errno;
        }

        if ((read(cltB.client_desc, msg, BUFF_SIZE)) < 0)
        {
            perror("Error reading from clt b desc\n");
            return errno;
        }

        //printf("msg from cltB: %s\n", msg);

        if ((write(cltA.client_desc, msg, BUFF_SIZE)) < 0)
        {
            perror("Error writing to clt a desc\n");
            return errno;
        }

    }


}


int communicate_with_partner(pipes *pipe_handler)
{
    pipes client_2;
    char msg[BUFF_SIZE] = " ... \n";
    int bytes = -1;
    int *pip;
    if (*nr_clients % 2 == 0)
    {
        pip = pipe_handler[*nr_clients + 1].pipe_desc;
        if ( read(pip[0], &client_2, BUFF_SIZE) < 0)
        {
            child_exiting_handler(&pipe_handler[*nr_clients], 0, 0, true);
            printf("Child process killed by error from pipe with PID = %d\n", getpid());
            exit(0);
        }

        printf("connected \n");
    } else{
        if (*nr_clients % 2 == 1)
        {
            pip = pipe_handler[*nr_clients].pipe_desc;
            //printf("%d I'm looking for partner.. \n", pipe_handler[*nr_clients].pipe_number);

            if ( write(pip[1], pipe_handler, BUFF_SIZE) < 0)
            {
                child_exiting_handler(&pipe_handler[*nr_clients], 0, 0, true);
                printf("Child process killed by error from pipe with PID = %d\n", getpid());
                exit(0);
            }

        }

    }

    do{
        bzero(&msg, BUFF_SIZE);

    }while(bytes > 0);
    printf("Bytes = %d\n", bytes);

    //child_exiting_handler(pipe_handler, 0, clt_desc, false);
    bzero(&pipe_handler, sizeof(pipe_handler));
    return bytes;
}

int main()
{
    int server_descriptor,client_descriptor,len;
    struct sockaddr_in serv,cli;
    struct pipes pipe_handler[100];
    pid_t pid;
    int optval=1; //opt for setsocketopt()
    nr = mmap(NULL, sizeof *nr * BUFF_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    nr_clients = mmap(NULL, sizeof *nr_clients, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    *nr_clients = -1;
    nr[0] = 0;
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

    for(; ;) {
        len = sizeof(cli);
        //accepting client connection
        client_descriptor = accept(server_descriptor, (struct sockaddr *) &cli, (socklen_t *) &len);
        if (client_descriptor < 0) {
            perror("Error accepting()\n");
            continue;
        }
        printf("Connected to Client w/ desc = %d\n", client_descriptor);

        *nr_clients = *nr_clients + 1;
        printf("nr clts %d\n", *nr_clients);
        nr[0] = nr[0] + 1;

        if (*nr_clients % 2 == 0) {
            //pipe(pipe_handler[*nr_clients].pipe_desc); //open pipe for current client and next client, so they can communicate to each other
            //pipe(pipe_handler[*nr_clients + 1].pipe_desc);
            nr[0] = nr[0] + 4;
        }
       // pipe_handler[*nr_clients].pipe_number = *nr_clients;
        pipe_handler[*nr_clients].client_desc = client_descriptor;
        pipe_handler[*nr_clients].isInUse = 1;
        pipe_handler[*nr_clients].client_id = (*nr_clients);


        if ((write(client_descriptor, &pipe_handler[*nr_clients].client_id, BUFF_SIZE)) < 0)
        {
        perror("Error writing to clt a desc\n");
        return errno;
        }

        if (*nr_clients % 2 == 1)
        {
            pid = fork();
            if (pid < 0)
        {
            perror("FORK error\n");
            return errno;
        }

            if(pid == 0)
        {
                simulate_clientAB(pipe_handler);

           // str_echo(client_descriptor, &pipe_handler[*nr_clients]);

            child_exiting_handler(&pipe_handler[*nr_clients], server_descriptor, client_descriptor, false);

            //nr[0] = nr[0] - 3; //nr of used files descriptors;
            *nr_clients = *nr_clients - 1;
            exit(0);
        }
        else //is parent
        {
            int status;
            waitpid(-1, &status, WNOHANG); //nu lasam copii atarnati, orfani, zombi etc

        }
        printf("nr = %d\n", nr[0]);
        }
    }
    close(client_descriptor);
    close(server_descriptor);
    return 0;
}


/* TODO: Check which file descriptors are aviable to determinate which client has disconnected
Use shared memory
Add semaphores or something like that
Maybe share all the array of structs so when a client disconnects updates the file descriptors and nr of clients
 Schimba *nr of clients pentru ca poate sa fuck up pipeurile
 trebuie ceva ceva cu shared memory sa inchida descriptorii si parintele poate un vector in care punem descriptrii ce se inchid alocat dinamic
 si marimea lui


 ERROR CHECK cand se inchide vreun cleint
 schimbarea nr client
 inchidera clientilor in server main ?
 Comenzi la client
 Inceperea jocului
 */
