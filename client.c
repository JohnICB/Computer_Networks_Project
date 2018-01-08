
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
#define NAME_SIZE 15
#define MAP_WIDTH 8
#define MAP_HEIGHT 8


#define HEIGHT 8
#define WIDTH 8

extern int errno;

typedef struct playerData playerData;
typedef struct pair pair;
struct playerData
{
    char oponnent_name[NAME_SIZE];
    int client_desc;
};

struct pair
{
    char message[BUFF_SIZE];
    char map[HEIGHT][WIDTH];
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
void menu();

void getPlayerInput(char *input, size_t size, int server_fd)
{
	int isValid = 0;
	do{
		bzero(input,  size);
		isValid = 0;
		if ((read(0, input, size)) < 0)
        {
            perror ("Error reading from stdin\n");
            return;
        }

        if (write (server_fd, input, BUFF_SIZE) < 0)
		{
			perror ("Error while writing to the Server.\n");
			return;
		}
		 printf("Before read: %d\n", isValid);
		if ((read(server_fd, &isValid, sizeof(int))) < 0)
        {
            perror ("Error reading from server\n");
            return;
        }

        printf("Had raed: %d\n", isValid);
        fflush(stdout);

	}while(isValid == 0);
}

int client_A_Handler(int server_fd, char oponnent_name[15])
{
	fflush(stdout);
	pair data;
	char buf[BUFF_SIZE];
	ssize_t bytes;
	if ((bytes = read(server_fd, buf, BUFF_SIZE)) < 0) //signal that the other client has connected
	{
		perror ("Error reading from stdin\n");
		return errno;
	}
	printf("You are playing agasint %s!\nYou start!\n", oponnent_name);

	do{
		
		bzero(buf, BUFF_SIZE);
		int input = -1;
		printf("It's Your turn\nYou: ");
		fflush(stdout);

		getPlayerInput(buf, BUFF_SIZE, server_fd);
		//put this in a fcuntiion
		//checkSpecialInput(buf, BUFF_SIZE);
		//if (strcmp(buf, "/quit") == 0)
		//{
		//	printf("Thanks for playing! Exiting now..\n");
		//	return 1;
		//}
		//TODO: add more commands
		//if (write (server_fd, (buf), BUFF_SIZE) < 0)
		//{
		//	perror ("Error while writing to the Server.\n");
		//	return errno;
		//}

		//read bool if  correct

		bzero(buf, BUFF_SIZE);
		printf("It's %s's turn\n", oponnent_name);
		fflush(stdout);
		bzero(&data, sizeof(data));
		if ((bytes = read(server_fd, &data, BUFF_SIZE)) < 0)
		{
			perror ("Error reading from stdin\n");
			return errno;
		}

		//put this in a function
		//if (strcmp(buf, "DISCONNECTED") == 0)
		//{
		//	printf("Opponent has dissconnected..\nReconnecting..\n");
		//	return 0;
		//}
		//printf("%s: %s\n",oponnent_name, buf);
		//printMap(map);
		//bzero(&map, sizeof(map));
		fflush(stdout);


	}while(1);
}
int client_B_Handler(int server_fd, char oponnent_name[15])
{

	char buf[BUFF_SIZE];
	pair data;
	ssize_t bytes;

	printf("You are playing against %s!\n You start second!\n", oponnent_name);
	fflush(stdout);

	do{
		bzero(buf, BUFF_SIZE);
		bzero(&data, sizeof(data));
		printf("It's %s's turn\n", oponnent_name);
		fflush(stdout);

		if ((bytes = read(server_fd, &data, BUFF_SIZE)) < 0)
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
		if (strcmp(data.message, "DISCONNECTED") == 0)
		{
			printf("Opponent has dissconnected..\n");
			return 0;
		}

		//printf("%s: %s\n",oponnent_name, buf);
		//printMap(map);
		fflush(stdout);

		bzero(buf, BUFF_SIZE);
		printf("It's Your turn\nYou: ");
		fflush(stdout);

		getPlayerInput(buf, BUFF_SIZE, server_fd);
		//printf("cmp: %d\n",strcmp(buf, "/quit"));
		//if (strcmp(buf, "/quit") == 0)
		//{
		//	printf("Thanks for playing! Exiting now..\n");
		//	return 1;
		//}

		//if (write (server_fd, buf, BUFF_SIZE) < 0)
		//{
		//	perror ("Error while writing to the Server.\n");
		//	return errno;
		//}



		}while(bytes > 0);
}	
bool checkValidName(char *name, int size)
{
	if (size < 2 || size > 15)
	{
		printf("%s is an invalid name\n", name);
		printf("Your name should be at least 2 letters long, but not longer than 15\n~Try another name!~\n");
		fflush(stdout);
		return false; 
	}

	for (int i = 0; i < size; ++i) {
		if (!((name[i] >= 'a' && name[i] <= 'z') || (name[i] >= 'A' && name[i] <= 'Z') || (name[i] == ' ')))
		{ 
			printf("%s is an invalid name\n", name);
			printf("Your name should be made only by Letters!\n~Try another name!~\n");
			fflush(stdout);
			return false; }
		}

	return true;
}
void inputPlayerName(char *buf, int size)
{
	do{
	bzero(buf, BUFF_SIZE);
	printf("Insert your name: ");
	fflush(stdout);
	if ((read(0, buf, BUFF_SIZE)) < 0)
		{
			perror ("Error reading from stdin\n");
			return;
		}
	buf[strlen(buf) - 1] = '\0';

	}while(!(checkValidName(buf, strlen(buf))));
}
int str_echo(int server_fd)
{
	char buf[BUFF_SIZE];//,buf1[BUFF_SIZE];
	ssize_t bytes = 0;
	int sz = 1; //bytes size
	bzero(&buf, BUFF_SIZE);
	int cl_number;
	playerData data;

	printf("Welcome to Connect 4!\n");
	fflush(stdout);
	
	if ((bytes = read(server_fd, &cl_number, BUFF_SIZE)) < 0)
	{
		perror ("Error reading from stdin\n");
		return errno;
	}
	data.client_desc = cl_number;

	inputPlayerName(buf, strlen(buf));

	printf("Waiting for other player..\n");
	fflush(stdout);

	//printf("playing versus %s\n", data.oponnent_name);
	//TODO: Insert name
	if ((write(server_fd, buf, BUFF_SIZE)) < 0)
	{
		perror ("Error reading from stdin\n");
		return errno;
	}

	if ((bytes = read(server_fd, buf, BUFF_SIZE)) < 0)
	{
		perror ("Error reading from stdin\n");
		return errno;
	}

	strcpy(data.oponnent_name, buf);

	printf("Playing against %s!!\n", data.oponnent_name);

	if (data.client_desc % 2 == 0)
	{
		client_A_Handler(server_fd, data.oponnent_name);
	}
	else
	{
		client_B_Handler(server_fd, data.oponnent_name);
	}
}




   // do{
   //     bzero(input,  size);
   //     if ((read(0, input, size)) < 0)
   //     {
   //         perror ("Error reading from stdin\n");
   //         return;
   //     }
//
//   //     input[strlen(input) - 1] = '\0'; //remove '\n'
//   //     //printf("succes\n");
   // }while(!checValidInput(input, (int) strlen(input)));

//void printMap(char *map)
//{
//	for (int i = 0; i < MAP_HEIGHT; ++i)
//	{
//		for (int j = 0; j < MAP_WIDTH; ++j)
//		{
//			printf("| %c |", map[i * j + j]);
//		}
//		printf("\n------------------\n");
//	}
//}

//bool checValidInput(char *input, int size)
//{
//    bool ok = true;
//    //printf("%s is valid \n", input);
//    fflush(stdout);
//    for (int i = 0; i < size; ++i)
//    {
//        if (input[i] < '0' || input[i] > '9')
//        {
//            if(input[i] == ' ')
//            {
//                ok = false;
//                continue;
//            }
//
//            printf("You should only input numbers between 1 and 8!\n~Try Again!~\n");
//            return false;
//        }
//        else if(ok == false)
//        {
//            printf("You should only input numbers between 1 and 8!\n~Try Again!~\n");
//            return false;
//        }
//    }
//
//    if (atoi(input) < 0 || atoi(input) > 8)
//    {
//        printf("You should only input numbers between 1 and 8!\n~Try Again!~\n");
//        return false;
//    }
//    return true;
//}
//bool checkValidPlacement(const char *map, short pos)
//{
//    if(map[pos] == ' ')
//    {
//        return true;
//    }
//    else {
//        printf("Column is full!\n~Try Again~!\n");
//        return false;
//    }
//}
//bool checkSpecialInput(char *input)
//{
//    if (strcmp(input, "/quit") == 0)
//    {
//        printf("Thanks for playing! Exiting now..\n");
//        return true;
//    }
//    return false;
//}