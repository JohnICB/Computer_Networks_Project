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
#define HEIGHT 8
#define WIDTH 8

typedef struct pipes pipes;
typedef struct gameData gameData;

struct pipes
{
    char buffer[100];
    int client_desc;
    int client_id;

    bool isInUse;
};

struct gameData
{
    int client_desc;

    char map[HEIGHT][WIDTH];
    int score_A;
    int score_B;
    char symbol_A;
    char symbol_B;
};

void update_map( gameData *game_data, short pos, bool isPlayerA);


#endif //UNTITLED_GAMEDATA_H
