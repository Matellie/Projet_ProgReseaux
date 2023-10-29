#ifndef AWALE_H
#define AWALE_H

#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_AWALE_MOVES 1000
#define MAX_GAMES 100

#define INVALID_SLOT_ERROR -1
#define EMPTY_SLOT_ERROR -2
#define FAMINE_MOVE_ERROR -3
#define MOVE_LIMIT_REACHED_ERROR -4

#define BUF_SIZE2    1024

typedef struct
{
    char* listeObservers[BUF_SIZE2];
    int actual;
}ListeObserver;

typedef struct 
{
    ListeObserver observers;
    char player1[BUF_SIZE2];
    char player2[BUF_SIZE2];
    int player1Score;
    int player2Score;
    int currentPlayer;
    int board[12]; /* Player 1 occupies slots 0-5 and 2 occupies slots 6-11*/ 
    int nextMoveInSequence;
    int moveSequence[1000];
    bool isFinished;
}AwaleGame;

typedef struct 
{
    AwaleGame* listeAwales[MAX_GAMES];
    int actual;
}ListeAwale;

int createGame(AwaleGame* newGame);
int jouer(AwaleGame* game, int slot, char* result);
int gameToString(AwaleGame* game, char* result);
int registerMove(AwaleGame* game, int slot);
int playAwale(AwaleGame* game, int slot);
int endGame(AwaleGame* game);
int showWinner(int winner, char* result);
char* showError(int error);
int replayGame(AwaleGame* game, char* result);

#endif
