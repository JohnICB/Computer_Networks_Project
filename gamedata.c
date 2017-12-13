//
// Created by john on 13.12.2017.
//

#include "gamedata.h"

void update_map( gameData *game_data, short pos, bool isPlayerA)
{
    if (game_data == NULL)
    {
        perror("Invalid pointer to game data! \n");
        return;
    }

    if (game_data->map[0][pos] != 0)
    {
        perror("Column already full\n");
        return;
    }

    for (int i = -1; i < HEIGHT; ++i) {
        if (game_data->map[i+1][pos] != 0)
        {
            if (isPlayerA)
            {
                game_data->map[i][pos] = game_data->symbol_A;
            } else
                game_data->map[i][pos] = game_data->symbol_B;
            break;
        }
    }

}

int play_game(gameData *playerList, int numb)
{
   // int numb = *nr_clients;
    gameData cltA = playerList[numb -1];
    gameData cltB = playerList[numb];

    unsigned char msg[BUFF_SIZE] = "";

    if ((write(cltA.client_desc, msg, BUFF_SIZE)) < 0)
    {
        perror("Error writing to clt a desc\n");
        return errno;
    }
    ssize_t bytes;

    while(1)
    {
        bzero(&msg, BUFF_SIZE);

        if ((bytes = read(cltA.client_desc, msg, BUFF_SIZE)) < 0)
        {
            perror("Error reading from clt a desc\n");
            return errno;
        }

        if (bytes == 0)
        {
            printf("Client A has disconnected!\n");
            bzero(&msg, sizeof(msg));
            strcpy((char *) msg, "DISCONNECTED");
            if ((write(cltB.client_desc, msg, BUFF_SIZE)) < 0)
            {
                perror("Error writing to clt b desc\n");
                return errno;
            }
            return 0;
        }

        if ((write(cltB.client_desc, msg, BUFF_SIZE)) < 0)
        {
            perror("Error writing to clt b desc\n");
            return errno;
        }

        if ((bytes = read(cltB.client_desc, msg, BUFF_SIZE)) < 0)
        {
            perror("Error reading from clt b desc\n");
            return errno;
        }

        if (bytes == 0)
        {
            printf("Client B has disconnected!\n");
            bzero(&msg, sizeof(msg));
            strcpy((char *) msg, "DISCONNECTED");
            if ((write(cltA.client_desc, msg, BUFF_SIZE)) < 0)
            {
                perror("Error writing to clt b desc\n");
                return errno;
            }
            return 0;
        }


        if ((write(cltA.client_desc, msg, BUFF_SIZE)) < 0)
        {
            perror("Error writing to clt a desc\n");
            return errno;
        }

    }


}
