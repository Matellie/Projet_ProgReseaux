#ifndef AWALE_H
#define AWALE_H

struct AwaleGame {
    int player1;
    int player2;
    int player1Score;
    int player2Score;
    int currentPlayer;
    int board[12]; /* Player 1 occupies slots 0-5 and 2 occupies slots 6-11*/ 
};

struct AwaleGame* createGame();
static int playAwale(struct AwaleGame* game, int slot);
static char* gameToString(struct AwaleGame* game);
static int endGame(struct AwaleGame* game);

#endif
