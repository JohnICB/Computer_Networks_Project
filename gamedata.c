//
// Created by john on 13.12.2017.
//

#include "gamedata.h"
int checkWin(char map[][HEIGHT])
{


    //checks horizontal win
    for(int i=0;i<WIDTH;i++)
        for(int j=0;j<HEIGHT-3;j++)
            if(map[i][j] != 0 && map[i][j]==map[i][j+1] && map[i][j]==map[i][j+2] && map[i][j]==map[i][j+3])
                return 1;

    //checks vertical win
    for(int i=0;i<WIDTH-3;i++)
        for(int j=0;j<HEIGHT;j++)
            if(map[i][j] != 0 && map[i][j]==map[i+1][j] && map[i][j]==map[i+2][j] && map[i][j]==map[i+3][j])
                return 2;

    //checks right diagonal win
    for(int i=0;i<WIDTH-3;i++)
        for(int j=0;j<HEIGHT-3;j++)
            if(map[i][j] != 0 && map[i][j]==map[i+1][j+1] && map[i][j]==map[i+2][j+2] && map[i][j]==map[i+3][j+3])
                return 3;

    //checks left diagonal win
    for(int i=0;i<WIDTH-3;i++)
        for(int j=0;j<HEIGHT-3;j++)
            if(map[i][j] != 0 && map[i][j]==map[i+1][j-1] && map[i][j]==map[i+2][j-2] && map[i][j]==map[i+3][j-3])
                return 4;

    return 0;

}
void update_map( pair *game_data, int pos, char symbol)
{
    if (game_data == NULL)
    {
        perror("Invalid pointer to game data! \n");
        return;
    }

    --pos;

    if (game_data->map[0][pos] != 0)
    {
         //printf("Column already full 1\n");
         return ;
    }


    for (int i = -1; i < HEIGHT; ++i) {
        if (game_data->map[i+1][pos] == 0 && i + 1 != HEIGHT)
        {
           // printf(" is empty %d \n", game_data->map[i][pos]);
            continue;
            //return;
        }
        else
        {
            game_data->map[i][pos] = symbol;
            break;
        }
    }

    /*
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
        printf("\n-----------------------------------------\n");
    }
     */
        


}
int checkValidInput(const char *input)
{
    int ok = 1;

    if (strlen(input) == 1)
    {
       // printf("You should only input numbers between 1 and 8!\n~Try Again!~\n");
        return 1;
    }

    if (atoi(input) < 1 || atoi(input) > 8)
    {
        //printf("You should only input numbers between 1 and 8!\n~Try Again!~\n");
        return 1;
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

            //printf("You should only input numbers between 1 and 8!\n~Try Again!~\n");
            return 1;
        }
        else if(ok == 0)
        {
            //printf("You should only input numbers between 1 and 8!\n~Try Again!~\n");
            return 1;
        }
    }

    return 0;
}
int initPlayers(playerData *playerOne, playerData *playerTwo)
{
    char msg[BUFF_SIZE] = "";
    //*
    //read from A his name
    if ((read(playerOne->client_desc, msg, BUFF_SIZE)) < 0)
    {
        perror("Error reading NAME from clt a desc\n");
        return errno;
    }
    strcpy(playerTwo->opponent_name, (const char *) msg);

    //read from B his name
    if ((read(playerTwo->client_desc, msg, BUFF_SIZE)) < 0)
    {
        perror("Error reading NAME from clt a desc\n");
        return errno;
    }
    strcpy(playerOne->opponent_name, (const char *) msg);

    printf("A: %s\nB: %s\n", playerTwo->opponent_name, playerOne->opponent_name);
// */

    //write to A name of B
    if ((write(playerOne->client_desc, playerOne->opponent_name, BUFF_SIZE)) < 0) //just a signal
    {
        perror("Error writing to clt a desc\n");
        return errno;
    }
    //write to B name of A
    if ((write(playerTwo->client_desc, playerTwo->opponent_name, BUFF_SIZE)) < 0) //just a signal
    {
        perror("Error writing to clt a desc\n");
        return errno;
    }

    if ((write(playerOne->client_desc, msg, BUFF_SIZE)) < 0) //just a signal
    {
        perror("Error writing to clt a desc\n");
        return errno;
    }
}
int getInput(playerData *playerOne, playerData *playerTwo, int scoreKeeper[2], pair *data)
{
    int isValid;
    ssize_t bytes;
    do{
        isValid = 3;
        bzero(&data->message, sizeof(data->message));
        if ((bytes = read(playerOne->client_desc, &data->message, sizeof(data->message))) < 0)
        {
            perror("Error reading from clt a desc\n");
            return errno;
        }
        if (bytes == 0)
        {
            //printf("Client A has disconnected!\n");
            bzero(&data->message, sizeof(data->message));
            strcpy((char *) data->message, "DISCONNECTED");
            if ((write(playerTwo->client_desc, &data->message, sizeof(data->message))) < 0)
            {
                perror("Error writing to clt b desc\n");
                return errno;
            }
            return 0;
        }
        //printf(" ff %s\n", data->message);
        if (strcmp(data->message, "/score\n") == 0)
        {
            if ((write(playerOne->client_desc, scoreKeeper, sizeof(scoreKeeper))) < 0)
            {
                perror("Error wr to clt a desc\n");
                return errno;
            }
            //printf("Score is: \nYou: %d     Your Opponent: %d\n", scoreKeeper[0], scoreKeeper[1] );
            fflush(stdout);
            isValid = 5;
            continue;
        }

        isValid = checkValidInput(data->message);
        //printf(" ff %d\n", data.map[0][atoi(msg)-1]);

        if (data->map[0][atoi(data->message)-1] != 0) //check if column is full
        {
            isValid = 2;
        }

        //printf("Valid from A: %s -> %d \n",data->message, isValid);
        fflush(stdout);
        //printf("isvalid : %d\n", isValid);

        if ((write(playerOne->client_desc, &isValid, sizeof(isValid))) < 0)
        {
            perror("Error wr to clt a desc\n");
            return errno;
        }

    }while(isValid != 0);

    return 0;
}
int winHandler(playerData *playerOne, playerData *playerTwo, int scoreKeeper[2], pair *data)
{
    int answr[2] = {-1,-1};
    ssize_t bytes;

    if ((write(playerTwo->client_desc, scoreKeeper, sizeof(scoreKeeper))) < 0) //HERE
    {
        perror("Error writing to clt b desc\n");
        return errno;
    }
    if ((write(playerOne->client_desc, scoreKeeper, sizeof(scoreKeeper))) < 0) //HERE
    {
        perror("Error writing to clt A desc\n");
        return errno;
    }

    //printf("Won from 1!\n");
    //read answer from both clients if they want to play again
    if ((bytes = (read(playerOne->client_desc, &answr[0], sizeof(answr[0])))) < 0) {
        perror("Error reading answer from clt a desc\n");
        return errno;
    }

    if (bytes == 0) {
        //printf("Client A has disconnected!\n");
        answr[0] = 2;
    }
    //printf("read from a ans %d\n", answr[0]);

    if ((bytes = (read(playerTwo->client_desc, &answr[1], sizeof(answr[1])))) < 0) {
        perror("Error reading answer from clt b desc\n");
        return errno;
    }
    if (bytes == 0) {
        //printf("Client B has disconnected!\n");
        answr[1] = 2;
    }

    //("read from b ans %d\n", answr[1]);

    if (answr[1] == answr[0] && answr[1] == 1) {
        bzero(data->map, sizeof(data->map));
        if ((write(playerOne->client_desc, &answr[1], sizeof(answr[1]))) < 0) {
            perror("Error writing to clt a desc\n");
            return errno;
        }
        if ((write(playerTwo->client_desc, &answr[1], sizeof(answr[1]))) < 0) {
            perror("Error writing to clt a desc\n");
            return errno;
        }
        return 1;
    }

    if (answr[1] != 1 && answr[0] != 1) {
        return 0;
    }

    if (answr[1] != 1 && answr[0] == 1) {
        //write to A that B has disconnected
        //printf("Client B has disconnected!\n");
        answr[1] = 2;
        if ((write(playerOne->client_desc, &answr[1], sizeof(answr[1]))) < 0) {
            perror("Error writing to clt a desc\n");
            return errno;
        }
        return 0;
    }

    if (answr[0] != 1 && answr[1] == 1) {
        //write to B that A has disconnected
        //printf("Client A has disconnected!\n");
        answr[1] = 2;
        if ((write(playerTwo->client_desc, &answr[1], sizeof(answr[1]))) < 0) {
            perror("Error writing to clt b desc\n");
            return errno;
        }
        return 0;
    }
}
int playGame(playerData *playerList, int numb)
{
    playerData playerOne = playerList[numb -1];
    playerData playerTwo = playerList[numb];

    int scoreKeeper[2] = {0,0};
    pair data;
    int nrOfMoves = 0;
    bzero(&data, sizeof(data));
    initPlayers(&playerOne, &playerTwo);

    while(1)
    {
        bzero(&data.message, sizeof(data.message));
        getInput(&playerOne, &playerTwo, scoreKeeper, &data);
        nrOfMoves++;
        update_map(&data, atoi(data.message), (char) 'X'); //symbol a
        data.result = checkWin( data.map);

        if (data.result != 0)
        {
            if (data.result == 1 || data.result == 2 )
            {
                scoreKeeper[0] += 10;
            }
            else if (data.result == 3 || data.result == 4)
            {
                scoreKeeper[0] += 15;
            }


            data.result = 1; //signal that client B has won
        }

        if (nrOfMoves == HEIGHT * WIDTH - 1)
        {
            data.result = 3; //egalitate
        }

        if ((write(playerTwo.client_desc, &data, sizeof(data))) < 0) //HERE
        {
            perror("Error writing to clt b desc\n");
            return errno;
        }
        if ((write(playerOne.client_desc, &data, sizeof(data))) < 0) //HERE
        {
            perror("Error writing to clt A desc\n");
            return errno;
        }


        if (data.result != 0)
        {
            if (winHandler(&playerOne, &playerTwo, scoreKeeper, &data) == 1)
            {
                continue;
            }
            else
            {
                return 0;
            }
        }

        getInput(&playerTwo, &playerOne, scoreKeeper, &data);
        nrOfMoves++;
        update_map(&data, atoi(data.message), 'O'); //symbol b
        data.result = checkWin(data.map);
        if (data.result != 0)
        {
            if (data.result == 1 || data.result == 2 )
            {
                scoreKeeper[1] += 10;
            }
            else if (data.result == 3 || data.result == 4)
            {
                scoreKeeper[1] += 15;
            }
            data.result = 2; //signal that client B has won
        }

        if (nrOfMoves == HEIGHT * WIDTH - 1)
        {
            data.result = 3; //egalitate
        }


        if ((write(playerTwo.client_desc, &data, sizeof(data))) < 0) //HERE
        {
            perror("Error writing to clt b desc\n");
            return errno;
        }
        if ((write(playerOne.client_desc, &data, sizeof(data))) < 0) //HERE
        {
            perror("Error writing to clt A desc\n");
            return errno;
        }

        if (data.result != 0) {

            if (winHandler(&playerOne, &playerTwo, scoreKeeper, &data) == 1)
            {
                continue;
            }
            else
            {
                return 0;
            }

        }

    }


}
