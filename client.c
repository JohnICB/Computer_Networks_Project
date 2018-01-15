
//client.c

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define BUFF_SIZE 1024
#define NAME_SIZE 15

#define HEIGHT 8
#define WIDTH 8

#define SELF 1
#define OPPONENT 0

#define RED   "\x1B[31m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define RESET "\x1B[0m"

extern int errno;

typedef struct playerData playerData;
typedef struct pair pair;
struct playerData
{
    char opponent_name[NAME_SIZE];
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
int SKIP_MENU = 0;

void printEmptyMap();
int str_echo(int server_fd); //takes care of the communication
int getMenuInput();
int exitHandler(int target);
void printHelp();
int main()
{
	strcpy(p_data.name, "");
	strcpy(p_data.opponent_name, "");
	p_data.client_desc = -1;

	int server_fd;
	struct sockaddr_in server;
	int answer;
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
			//server.sin_addr.s_addr=inet_addr("79.112.65.33");
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
	char answer[3];
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
	else if (itsMe == 0)
	{
		printf("%s WON!! \nInput: \n1 to PLAY AGAIN\n2 for MENU\n3 to QUIT\n", p_data.opponent_name);
	}
	else if (itsMe == 2)
	{
		printf("ITS A TIE!!\n");
	}

	do
	{	
		printf("You: ");
		fflush(stdout);
        bzero(&answer, sizeof(answer));
		//scanf("%d", &answr);
		if (( read(0,  &answer, sizeof(answer))) < 0)
		{
			perror ("Error reading from stdin\n");
			return errno;
		}
		answr = atoi(answer);

		if (answr == 2 || answr == 1 || answr == 3 )
		{
			 if (write (server_fd, &answr, sizeof(answr)) < 0)
			{
				perror ("Error while writing to the Server.\n");
				return -1;
			}

			//printf("server said %d", answr);
			//fflush(stdout);

			if (answr == 3)
			{
				return answr;
			}
			if (answr == 2)
			{
				return 0;
			}

			if (answr == 1)
			{
				printf("Waiting for opponent to answer...\n");
				fflush(stdout);
				if (( read(server_fd, &answr, sizeof(answr))) < 0)
				{
					perror ("Error reading from server\n");
					return errno;
				}

				if (answr == 1)
				{
					printEmptyMap();
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

		//printf("inp: %s/%d\n",answer, strcmp(answer, "y\n") == 0 );

		if (strcmp(answer, "\n") == 0 || strcmp(answer, "y\n") == 0 || strcmp(answer, "Y\n") == 0 || strcmp(answer, "yes\n") == 0 || strcmp(answer, "1\n") == 0)
		{
			SKIP_MENU = 1;
			return 0;
		}
		else if (strcmp(answer, "NO\n") == 0 || strcmp(answer, "n\n") == 0 || strcmp(answer, "N\n") == 0 || strcmp(answer, "nop\n") == 0 || strcmp(answer, "0\n") == 0)
		{
			return 1;
		}
		
		if (target == SELF)
		{
			printf("Lost connection to the server... \nWant to try to reconnect? [Y/N]\n");
		}
		else
		{
			printf("Opponent has disconnected..\nWant to Play some more? [Y/N]\n");
		}
	}while(1);
}
void printHelp()
{
	printf("Hi! You have to fill a matrix with pieces, The pieces fall straight down, occupying the next available space within the column.\nThe objective of the game is to be the first to form a horizontal, vertical, or diagonal line of four of one's own pieces\nTo chose the column, input it's number from 1 to 8.\nYou get 10 points for a horizontal or diagonal win and 15 for a diagonal win\nWhile playing, you can type /quit to quit the game\n/score to see the score\n");
	fflush(stdout);
}
int getMenuInput()
{

	if (SKIP_MENU == 1)
	{
		SKIP_MENU = 0;
		return 1;
	}

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
		if (strcmp(input, "/help\n") == 0)
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

	//return 0;
}
int getPlayerInput(char *input, size_t size, int server_fd, int isA)
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
       		if (isA == 1)
       		{
       			printf("Score is: \nYou: %d     Your Opponent: %d\n", score[0], score[1] );
       			fflush(stdout);
       		}
       		else
       		{
       			printf("Score is: \nYou: %d     Your Opponent: %d\n", score[1], score[0] );
       			fflush(stdout);
       		}
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
        //printf("Had readd: %d\n", isValid);
        fflush(stdout);

	}while(isValid != 0);

	return 0;
}
void printMap(char map[][HEIGHT])
{
	printf(BLU "\n┏━━━━┳━━━━┳━━━━┳━━━━┳━━━━┳━━━━┳━━━━┳━━━━┓\n");
	for (int i = 0; i < WIDTH; ++i)
    {
        for (int j = 0; j < HEIGHT; ++j)
        {
            if(map[i][j] != 0)
            {
            	if (map[i][j] == 'X')
            	{
            		//printf(BLU "┃" RED "███" RESET);
            		printf(BLU "┃" RED " ⬤  " RESET);
            	}
            	else
            		//printf(BLU "┃" YEL "███" RESET);	
            		printf(BLU "┃" YEL " ⬤  " RESET);	
                //printf(BLU "┃ %c ", map[i][j]);
            }
            else
            {
                printf(BLU "┃    ");
            }
        }
        if(i < WIDTH - 1)
        printf(BLU "┃\n┣━━━━╋━━━━╋━━━━╋━━━━╋━━━━╋━━━━╋━━━━╋━━━━┫\n" );
    }
    printf(BLU "┃\n┗━━━━┻━━━━┻━━━━┻━━━━┻━━━━┻━━━━┻━━━━┻━━━━┛\n" RESET);
    printf("   1    2    3    4    5    6    7    8\n\n");
    //printf("---------------------------------\n");
    fflush(stdout);
}
void printEmptyMap()
{
	printf(BLU "\n┏━━━━┳━━━━┳━━━━┳━━━━┳━━━━┳━━━━┳━━━━┳━━━━┓\n");
	for (int i = 0; i < WIDTH; ++i)
    {
        for (int j = 0; j < HEIGHT; ++j)
        {
            printf(BLU "┃    ");
        }
        if(i < WIDTH - 1)
        printf(BLU "┃\n┣━━━━╋━━━━╋━━━━╋━━━━╋━━━━╋━━━━╋━━━━╋━━━━┫\n" );
    }
    printf(BLU "┃\n┗━━━━┻━━━━┻━━━━┻━━━━┻━━━━┻━━━━┻━━━━┻━━━━┛\n" RESET);
    printf("   1    2    3    4    5    6    7    8\n\n");
    //printf("---------------------------------\n");
    fflush(stdout);
}

int client_A_Handler(int server_fd, char opponent_name[15])
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
	printf("You are playing agasint %s!\nYou start!\n", opponent_name);

	do{
		
		bzero(buf, BUFF_SIZE);
		printf("It's Your turn\n");
		fflush(stdout);

		if(getPlayerInput(buf, BUFF_SIZE, server_fd, 1) == 1)
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
		else if (data.result == 3)
		{
			answer = winHandler(2, server_fd, 1);
			if (answer == 1)
			{
				continue;
			}
			else return answer;

		}

		bzero(buf, BUFF_SIZE);
		printf("It's %s's turn\n", opponent_name);
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

		}else if (data.result == 3)
		{
			answer = winHandler(2, server_fd, 1);
			if (answer == 1)
			{
				continue;
			}
			else return answer;

		}
		fflush(stdout);


	}while(1);

	//return 2;
}
int client_B_Handler(int server_fd, char opponent_name[15])
{

	char buf[BUFF_SIZE];
	pair data;
	int answer;
	ssize_t bytes;
	bzero(&data, sizeof(data));
	printMap(data.map);
	printf("You are playing against %s!\nYou start second!\n", opponent_name);

	do{
		bzero(buf, BUFF_SIZE);
		bzero(&data, sizeof(data));
		printf("It's %s's turn\n", opponent_name);
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

		}else if (data.result == 3)
		{
			answer = winHandler(2, server_fd, 0);
			if (answer == 1)
			{
				continue;
			}
			else return answer;

		}
		bzero(buf, BUFF_SIZE);
		printf("It's Your turn\n");
		fflush(stdout);

		if(getPlayerInput(buf, BUFF_SIZE, server_fd, 0) == 1)
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

		}else if (data.result == 3)
		{
			answer = winHandler(2, server_fd, 0);
			if (answer == 1)
			{
				continue;
			}
			else return answer;

		}

	}while(1);

	//return 2;
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
void inputPlayerName(char *buf)
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

	}while(!(checkValidName(buf, (int) strlen(buf))));
}
int str_echo(int server_fd)
{
	char buf[BUFF_SIZE];//,buf1[BUFF_SIZE];
    bzero(&buf, BUFF_SIZE);
	int cl_number;

	if ((read(server_fd, &cl_number, BUFF_SIZE)) < 0)
	{
		perror ("Error reading from stdin\n");
		return errno;
	}
	p_data.client_desc = cl_number;

	//printf(".%s, %d \n",p_data.name, strcmp(p_data.name, "") );
	if (strcmp(p_data.name, "") == 0)
	{
		inputPlayerName(buf);
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

	if ((read(server_fd, buf, BUFF_SIZE)) < 0)
	{
		perror ("Error reading from stdin\n");
		return errno;
	}
	strcpy(p_data.opponent_name, buf);

	if (p_data.client_desc % 2 == 0)
	{
		return client_A_Handler(server_fd, p_data.opponent_name);
	}
	else
	{
		return client_B_Handler(server_fd, p_data.opponent_name);
	}
}

