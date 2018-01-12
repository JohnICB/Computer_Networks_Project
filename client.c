
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

#define SELF 1
#define OPPONENT 0
#define MENU_STATE 2

extern int errno;

typedef struct playerData playerData;
typedef struct pair pair;
struct playerData
{
    char oponnent_name[NAME_SIZE];
    char name[NAME_SIZE];
    int client_desc;
};

struct pair
{
    char message[BUFF_SIZE];
    char map[HEIGHT][WIDTH];
    int result;
};

playerData p_data;


int str_echo(int server_fd); //takes care of the communication
int getMenuInput();
int exitHandler(int target);
void printHelp();
int main()
{
	strcpy(p_data.name, "");
	strcpy(p_data.oponnent_name, "");
	p_data.client_desc = -1;

	int server_fd;
	struct sockaddr_in server;
	int answer = MENU_STATE;
	/*creating socket*/

	printf("Welcome to Connect 4!\n");
	fflush(stdout);

	do{

		if (getMenuInput() == 1)
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
		}
		else
		{
			close(server_fd);
			return 0;
		}

		close(server_fd);
	}while (answer == 0);
	return 0;
}
int winHandler(int itsMe, int server_fd, int isA)
{
	char answer[15];
	int answr;
	int score[2];
	bzero(&answer, sizeof(answer));

	if (( read(server_fd, &score, sizeof(score))) < 0)
	{
		perror ("Error reading from server\n");
		return errno;
	}

	if (isA == 1)
	{
		printf("Score is: \nYou: %d     Your Opponent: %d\n", score[0], score[1] );
	}
	else
	{
		printf("Score is: \nYou: %d     Your Opponent: %d\n", score[1], score[0] );
	}

	if (itsMe == 1)
	{
		printf("YOU WON!! \nInput: \n1 to PLAY AGAIN\n2 to go to the MENU\n3 to QUIT\n");
	}
	else
	{
		printf("%s WON!! \nInput: \n1 to PLAY AGAIN\n2 for MENU\n3 to QUIT\n", p_data.oponnent_name);
	}

	do
	{	
		printf("You: ");
		fflush(stdout);
		answr = -1;
		bzero(&answer, sizeof(answer));
		if (( read(0, answer, sizeof(answer))) < 0)
		{
			perror ("Error reading from stdin\n");
			return errno;
		}

		
		if (strcmp(answer, "1\n") == 0)
		{
			answr = 1;
		}
		else if (strcmp(answer, "2\n") == 0)
		{
			answr = 0;
		}
		else if (strcmp(answer, "3\n") == 0)
		{
			answr = 2;
		}
		
		if (answr == 0 || answr == 1 || answr == 3 )
		{
			 if (write (server_fd, &answr, sizeof(answr)) < 0)
			{
				perror ("Error while writing to the Server.\n");
				return -1;
			}

			//printf("server said %d", answr);
			fflush(stdout);

			if (answr == 1)
			{
				printf("Waiting for opponent to answer...\n");
				fflush(stdout);
				if (( read(server_fd, &answr, sizeof(answr))) < 0)
				{
					perror ("Error reading from server\n");
					return errno;
				}

				if (answr != 1)
				{
					answr = exitHandler(OPPONENT );

					if (answr == 1)
						return 2;
				}
			}
			return answr;
		}

		printf("1 to PLAY AGAIN\n2 to go to the MENU\n3 to QUIT\n");	
		fflush(stdout);
	}while(1);
}
int exitHandler(int target)
{
	char answer[10];
	if (target == SELF)
	{
		printf("Lost connection to the server... \nWant to try to reconnect? [Y/N]\n");
	}
	else
	{
		printf("Opponent has dissconnected..\nWant to Play some more? [Y/N]\n");
	}
	
	do{
		printf("You: ");
		fflush(stdout);

		bzero(&answer, sizeof(answer));

		if (( read(0, &answer, sizeof(answer))) < 0)
		{
		perror ("Error reading from stdin\n");
		return errno;
		}

		printf("inp: %s/%d\n",answer, strcmp(answer, "y\n") == 0 );

		if (strcmp(answer, "\n") == 0 || strcmp(answer, "y\n") == 0 || strcmp(answer, "Y\n") == 0 || strcmp(answer, "yes\n") == 0 || strcmp(answer, "1\n") == 0)
		{
			return 0;
		}
		else if (strcmp(answer, "NO\n") == 0 || strcmp(answer, "n\n") == 0 || strcmp(answer, "N\n") == 0 || strcmp(answer, "nop\n") == 0 || strcmp(answer, "0\n") == 0)
		{
			return 1;
		}
		
		printf("Want to try to reconnect? [Y/N]\n");	
		fflush(stdout);
	}while(1);
}
void printHelp()
{
	printf("Hi! You have to fill a matrix with pices, The pieces fall straight down, occupying the next available space within the column.\nThe objective of the game is to be the first to form a horizontal, vertical, or diagonal line of four of one's own pices\nTo chose the column, input it's number from 1 to 8.\nYou get 10 points for a horizontal or diagonal win and 15 for a diagonal win\nWhile playing, you can type /quit to quit the game\n/score to see the score\n");
	fflush(stdout);
}
int getMenuInput()
{
	char input[BUFF_SIZE];
	do{
		bzero(input,  sizeof(input));
		printf("Insert:\n1 To Play\n2 To Quit\n/help for help\n");
		printf("You: ");
		fflush(stdout);
		if ((read(0, input, sizeof(input))) < 0)
        {
            perror ("Error reading from stdin\n");
            return -1;
        }

        if (strcmp(input, "1\n") == 0)
		{
			return 1;
		}
        else if (strcmp(input, "2\n") == 0)
		{
			printf("Thanks for playing! Exiting now..\n");
			fflush(stdout);
			return 2;
		}
		else if (strcmp(input, "/help\n") == 0)
		{
			printHelp();
			printf("Try again\n");
			fflush(stdout);
		}
		else 
		{
			printf("Invalid input, /help for help and rules\n");
			fflush(stdout);
		}


	}while(1);

	return 0;
}
int getPlayerInput(char *input, size_t size, int server_fd)
{
	int isValid = 0;
	int score[2];
	do{
		bzero(input,  size);
		isValid = 0;
		printf("You: ");
		fflush(stdout);
		if ((read(0, input, size)) < 0)
        {
            perror ("Error reading from stdin\n");
            return -1;
        }
        if (strcmp(input, "/quit\n") == 0)
		{
			printf("Thanks for playing! Exiting now..\n");
			return 1;
		}

        if (write (server_fd, input, BUFF_SIZE) < 0)
		{
			perror ("Error while writing to the Server.\n");
			return -1;
		}

		 if (strcmp(input, "/score\n") == 0)
		{
			if ((read(server_fd, &score, sizeof(score))) < 0)
        	{
          	  perror ("Error reading from server\n");
          	  return -1;
       		}

       		printf("Score is: \nYou: %d     Your Opponent: %d\n", score[0], score[1] );
       		fflush(stdout);
       		isValid = 5;
       		continue;
		}


		if ((read(server_fd, &isValid, sizeof(int))) < 0)
        {
            perror ("Error reading from server\n");
            return -1;
        }

        if (isValid == 1)
        {
        	printf("You should only input numbers between 1 and 8!\n~Try Again!~\n");
        }
        else if (isValid == 2)
        {
        	printf("The column is already full!\n~Try Again!~\n");
        }
        else if (isValid != 0)
        {
        	printf("Invalid Input. /help for help and rules!\n~Try Again!~\n");
        }
        //printf("Had raed: %d\n", isValid);
        fflush(stdout);

	}while(isValid != 0);

	return 0;
}
void printMap(char map[][HEIGHT])
{
	printf("\n---------------------------------\n");
	for (int i = 0; i < WIDTH; ++i)
    {
        for (int j = 0; j < HEIGHT; ++j)
        {
            if(map[i][j] != 0)
            {
                printf("| %c ", map[i][j]);
            }
            else
            {
                printf("|   ");
            }
              //printf("| %d |", map[i][j]);
        }
        printf("|\n---------------------------------\n");
    }
    fflush(stdout);
}
int client_A_Handler(int server_fd, char oponnent_name[15])
{
	fflush(stdout);
	pair data;
	char buf[BUFF_SIZE];
	ssize_t bytes;
	int answer;

	if ((bytes = read(server_fd, buf, BUFF_SIZE)) < 0) //signal that the other client has connected
	{
		perror ("Error reading from stdin\n");
		return errno;
	}
	if (bytes == 0)
	{
		return exitHandler(SELF);
	}

	bzero(&data, sizeof(data));
	printMap(data.map);
	printf("You are playing agasint %s!\nYou start!\n", oponnent_name);

	do{
		
		bzero(buf, BUFF_SIZE);
		int input = -1;
		printf("It's Your turn\n");
		fflush(stdout);

		if(getPlayerInput(buf, BUFF_SIZE, server_fd) == 1)
		{
			return 1;
		}

		if ((bytes = read(server_fd, &data, sizeof(data))) < 0)
		{
			perror ("Error reading from server data\n");
			return errno;
		}
		if (bytes == 0)
		{
			return exitHandler(SELF);
		}
		if (strcmp(data.message, "DISCONNECTED") == 0)
		{
			return exitHandler(OPPONENT);
		}

		printMap(data.map);

		if (data.result == 1)
		{
			answer = winHandler(1, server_fd, 1);

			if (answer == 1)
			{
				continue;
			}
			else return answer;

		}
		else if (data.result == 2)
		{
			answer = winHandler(0,server_fd, 1);
			if (answer == 1)
			{
				continue;
			}
			else return answer;

		}

		bzero(buf, BUFF_SIZE);
		printf("It's %s's turn\n", oponnent_name);
		fflush(stdout);
		bzero(&data, sizeof(data));
		fflush(stdout);
		if ((bytes = read(server_fd, &data, sizeof(data))) < 0)
		{
			perror ("Error reading from stdin\n");
			return errno;
		}
		if (bytes == 0)
		{
			return exitHandler(SELF);
		}
		if (strcmp(data.message, "DISCONNECTED") == 0)
		{
			return exitHandler(OPPONENT);
		}

		printMap(data.map);

		if (data.result == 1)
		{
			answer = winHandler(1, server_fd,1 );

			if (answer == 1)
			{
				continue;
			}
			else return answer;

		}
		else if (data.result == 2)
		{
			answer = winHandler(0, server_fd, 1);
			if (answer == 1)
			{
				continue;
			}
			else return answer;

		}
		fflush(stdout);


	}while(1);
}
int client_B_Handler(int server_fd, char oponnent_name[15])
{

	char buf[BUFF_SIZE];
	pair data;
	int answer;
	ssize_t bytes;
	bzero(&data, sizeof(data));
	printMap(data.map);
	printf("You are playing against %s!\nYou start second!\n", oponnent_name);

	do{
		bzero(buf, BUFF_SIZE);
		bzero(&data, sizeof(data));
		printf("It's %s's turn\n", oponnent_name);
		fflush(stdout);

		if ((bytes = read(server_fd, &data, sizeof(data))) < 0)
		{
			perror ("Error reading from stdin\n");
			return errno;
		}

		if (bytes == 0)
		{
			return exitHandler(SELF);
		}
		if (strcmp(data.message, "DISCONNECTED") == 0)
		{
			return exitHandler(OPPONENT);
		}

		printMap(data.map);
		if (data.result == 1)
		{
			answer = winHandler(0, server_fd, 0);

			if (answer == 1)
			{
				continue;
			}
			else return answer;

		}
		else if (data.result == 2)
		{
			answer = winHandler(1, server_fd, 0);
			if (answer == 1)
			{
				continue;
			}
			else return answer;

		}
		bzero(buf, BUFF_SIZE);
		printf("It's Your turn\n");
		fflush(stdout);

		if(getPlayerInput(buf, BUFF_SIZE, server_fd) == 1)
		{
			return 1;
		}

		if ((bytes = read(server_fd, &data, sizeof(data))) < 0)
		{
			perror ("Error reading from stdin\n");
			return errno;
		}

		if (bytes == 0)
		{
			return exitHandler(SELF);
		}
		if (strcmp(data.message, "DISCONNECTED") == 0)
		{
			return exitHandler(OPPONENT);
		}

		printMap(data.map);


		if (data.result == 1)
		{
			answer = winHandler(0, server_fd,0);

			if (answer == 1)
			{
				continue;
			}
			else return answer;

		}
		else if (data.result == 2)
		{
			answer = winHandler(1, server_fd, 0);
			if (answer == 1)
			{
				continue;
			}
			else return answer;

		}

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

	if ((bytes = read(server_fd, &cl_number, BUFF_SIZE)) < 0)
	{
		perror ("Error reading from stdin\n");
		return errno;
	}
	p_data.client_desc = cl_number;

	//printf(".%s, %d \n",p_data.name, strcmp(p_data.name, "") );
	if (strcmp(p_data.name, "") == 0)
	{
		inputPlayerName(buf, strlen(buf));
		strcpy(p_data.name, buf);
	}


	printf("Looking for other player..\n");
	fflush(stdout);

	if ((write(server_fd, p_data.name, BUFF_SIZE)) < 0)
	{
		perror ("Error reading from stdin\n");
		return errno;
	}

	bzero(&buf, BUFF_SIZE);

	if ((bytes = read(server_fd, buf, BUFF_SIZE)) < 0)
	{
		perror ("Error reading from stdin\n");
		return errno;
	}
	strcpy(p_data.oponnent_name, buf);

	if (p_data.client_desc % 2 == 0)
	{
		return client_A_Handler(server_fd, p_data.oponnent_name);
	}
	else
	{
		return client_B_Handler(server_fd, p_data.oponnent_name);
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