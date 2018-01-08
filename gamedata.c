//
// Created by john on 13.12.2017.
//

#include "gamedata.h"

void update_map( pair *game_data, int pos, char symbol)
{
    if (game_data == NULL)
    {
        perror("Invalid pointer to game data! \n");
        return;
    }

    if (game_data->map[0][pos] != 0)
    {
         printf("Column already full\n");
         return ;
    }


    for (int i = -1; i < HEIGHT; ++i) {
        if (game_data->map[i+1][pos] == 0 && i + 1 != HEIGHT)
        {
            printf(" is empty %d \n", game_data->map[i][pos]);
            continue;
            //return;
        }
        else
        {
            game_data->map[i][pos] = symbol;
            break;
        }
    }

    for (int i = 0; i < HEIGHT; ++i)
    {
        for (int j = 0; j < WIDTH; ++j)
        {
            if(game_data->map[i][j] != 0)
            {
                printf("| %c |", game_data->map[i][j]);
            }
            else
            {
                printf("|   |");
            }
        }
        printf("\n-------------------------------\n");
    }
        


}
int checkValidInput(const char *input)
{

    //printf("%d - %s\n", (int) strlen(input), input);
    int ok = 1;
    //printf("%s is valid \n", input);
    fflush(stdout);

    if (atoi(input) < 0 || atoi(input) > 8)
    {
        printf("You should only input numbers between 1 and 8!\n~Try Again!~\n");
        return false;
    }

    for (int i = 0; i < strlen(input) - 1; ++i)
    {
        if (input[i] < '0' || input[i] > '9')
        {
            if(input[i] == ' ')
            {
                ok = 0;
                continue;
            }

            printf("You should only input numbers between 1 and 8!\n~Try Again!~\n");
            return false;
        }
        else if(ok == 0)
        {
            printf("You should only input numbers between 1 and 8!\n~Try Again!~\n");
            return 0;
        }
    }



    return 1;
}
int playGame(playerData *playerList, int numb)
{
    playerData playerOne = playerList[numb -1];
    playerData playerTwo = playerList[numb];

    pair data;
    bzero(&data, sizeof(data));

    ssize_t bytes;

    char msg[BUFF_SIZE] = "";
    //*
    //read from A his name
    if ((read(playerOne.client_desc, msg, BUFF_SIZE)) < 0)
    {
        perror("Error reading NAME from clt a desc\n");
        return errno;
    }
    strcpy(playerTwo.oponnent_name, (const char *) msg);

    //read from B his name
    if ((read(playerTwo.client_desc, msg, BUFF_SIZE)) < 0)
    {
        perror("Error reading NAME from clt a desc\n");
        return errno;
    }
    strcpy(playerOne.oponnent_name, (const char *) msg);

    printf("A: %s\nB: %s\n", playerTwo.oponnent_name, playerOne.oponnent_name);
// */

    //write to A name of B
    if ((write(playerOne.client_desc, playerOne.oponnent_name, BUFF_SIZE)) < 0) //just a signal
    {
        perror("Error writing to clt a desc\n");
        return errno;
    }
    //write to B name of A
    if ((write(playerTwo.client_desc, playerTwo.oponnent_name, BUFF_SIZE)) < 0) //just a signal
    {
        perror("Error writing to clt a desc\n");
        return errno;
    }

    if ((write(playerOne.client_desc, msg, BUFF_SIZE)) < 0) //just a signal
    {
        perror("Error writing to clt a desc\n");
        return errno;
    }

    while(1)
    {
        int isValid = 0;
        bzero(&msg, BUFF_SIZE);

        do{

        if ((bytes = read(playerOne.client_desc, msg, BUFF_SIZE)) < 0)
        {
            perror("Error reading from clt a desc\n");
            return errno;
        }
        if (bytes == 0)
        {
            printf("Client A has disconnected!\n");
            bzero(&msg, sizeof(msg));
            strcpy((char *) msg, "DISCONNECTED");
            if ((write(playerTwo.client_desc, msg, BUFF_SIZE)) < 0)
            {
                perror("Error writing to clt b desc\n");
                return errno;
            }
            return 0;
        }

        isValid = checkValidInput(msg);
            if (isValid)
            {
                if (data.map[0][atoi(msg)] != 0)
                {
                    isValid = 0;
                }
            }

            printf("Valid from A: %s -> %d \n",msg, isValid);
            fflush(stdout);
            //printf("isvalid : %d\n", isValid);

        if ((write(playerOne.client_desc, &isValid, sizeof(isValid))) < 0)
        {
            perror("Error wr to clt a desc\n");
            return errno;
        }

        }while(isValid == 0);

        update_map(&data, atoi(msg), 'X'); //symbol a
        bzero(data.message, sizeof(data.message));
        strcpy(data.message, msg);

        if ((write(playerTwo.client_desc, &data, BUFF_SIZE)) < 0) //HERE
        {
            perror("Error writing to clt b desc\n");
            return errno;
        }
        bzero(&msg, BUFF_SIZE);

        do{
        bzero(&msg, sizeof(msg));
        if ((bytes = read(playerTwo.client_desc, msg, BUFF_SIZE)) < 0)
        {
            perror("Error reading from clt b desc\n");
            return errno;
        }

            if (bytes == 0)
            {
                printf("Client A has disconnected!\n");
                bzero(&msg, sizeof(msg));
                strcpy((char *) msg, "DISCONNECTED");
                if ((write(playerTwo.client_desc, msg, BUFF_SIZE)) < 0)
                {
                    perror("Error writing to clt b desc\n");
                    return errno;
                }
                return 0;
            }

            isValid = checkValidInput(msg);
            if (isValid)
            {
                if (data.map[0][atoi(msg)] != 0)
                {
                    isValid = false;
                }
            }

            printf("Valid from B: %s -> %d \n",msg, isValid);
            fflush(stdout);
            if ((write(playerTwo.client_desc, &isValid, sizeof(isValid))) < 0)
            {
                perror("Error wr to clt a desc\n");
                return errno;
            }

        }while(isValid == 0);


        update_map(&data, atoi(msg), 'O'); //symbol b

        bzero(data.message, sizeof(data.message));
        strcpy(data.message, msg);

        if ((write(playerOne.client_desc, &data, BUFF_SIZE)) < 0)
        {
            perror("Error writing to clt a desc\n");
            return errno;
        }

    }


}
