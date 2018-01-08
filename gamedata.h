//
// Created by john on 13.12.2017.
//

#ifndef UNTITLED_GAMEDATA_H
#define UNTITLED_GAMEDATA_H

#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFF_SIZE 1024
#define PORT 5000
#define NAME_SIZE 15
#define HEIGHT 8
#define WIDTH 8


typedef struct gameData gameData;
typedef struct playerData playerData;
typedef struct pair pair;

struct playerData
{
    char oponnent_name[BUFF_SIZE];
    int client_desc;
};

struct pair
{
    char message[BUFF_SIZE];
    char map[HEIGHT][WIDTH];
};

void update_map( pair *game_data, int pos, char symbol);
int playGame(playerData *playerList, int numb);

#endif //UNTITLED_GAMEDATA_H
