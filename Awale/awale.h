#ifndef AWALE_H
#define AWALE_H

#define MAX_AWALE_MOVES 1000

struct AwaleGame {
    char* player1;
    char* player2;
    int player1Score;
    int player2Score;
    int currentPlayer;
    int board[12]; /* Player 1 occupies slots 0-5 and 2 occupies slots 6-11*/ 
    int nextMoveInSequence;
    int moveSequence[1000];
    bool isFinished;
};

int createGame(struct AwaleGame* newGame);
int jouer(struct AwaleGame* game, int slot, char* result);
int gameToString(struct AwaleGame* game, char* result);
int askColumn(struct AwaleGame* game, char* result);
int registerMove(struct AwaleGame* game, int slot);
int playAwale(struct AwaleGame* game, int slot);
int endGame(struct AwaleGame* game);
int showWinner(int winner, char* result);
char* showError(int error);
int replayGame(struct AwaleGame* game, char* result);

#endif
