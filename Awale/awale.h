#ifndef AWALE_H
#define AWALE_H

#define MAX_AWALE_MOVES 1000

typedef struct {
    char* player1;
    char* player2;
    int player1Score;
    int player2Score;
    int currentPlayer;
    int board[12]; /* Player 1 occupies slots 0-5 and 2 occupies slots 6-11*/ 
    int nextMoveInSequence;
    int moveSequence[1000];
    bool isFinished;
} AwaleGame;

int createGame(AwaleGame* newGame);
int jouer(AwaleGame* game, int slot, char* result);
int gameToString(AwaleGame* game, char* result);
int askColumn(AwaleGame* game, char* result);
int registerMove(AwaleGame* game, int slot);
int playAwale(AwaleGame* game, int slot);
int endGame(AwaleGame* game);
int showWinner(int winner, char* result);
char* showError(int error);
int replayGame(AwaleGame* game, char* result);

#endif
