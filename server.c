#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include "gamedata.h"

//extern int errno;


static int *nr_clients;


void child_exiting_handler(playerData *p, int serv_desc, int clt_desc, bool isError)
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

    bzero(p, sizeof(*p));

}




int main()
{
    int server_descriptor,client_descriptor,len;
    struct sockaddr_in serv,cli;
    playerData playerList[1000];
    pid_t pid;
    int optval=1; //opt for setsocketopt()
    nr_clients = mmap(NULL, sizeof *nr_clients, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    *nr_clients = -1;

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

        playerList[*nr_clients].client_desc = client_descriptor;

        if ((write(client_descriptor, &client_descriptor, BUFF_SIZE)) < 0)
        {
            perror("Error writing to clt desc\n");
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
                playGame(playerList, *nr_clients);

                child_exiting_handler(&playerList[*nr_clients], server_descriptor, client_descriptor, false);

                *nr_clients = *nr_clients - 2;
                exit(0);
            }
            else //is parent
            {
                close(playerList[*nr_clients].client_desc);
                close(playerList[*nr_clients - 1].client_desc);
            }


            int status;
            waitpid(-1, &status, WNOHANG); //nu lasam copii atarnati, orfani, zombi etc
            //   printf("nr = %d\n", nr[0]);
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
