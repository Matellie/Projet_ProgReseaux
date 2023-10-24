#ifndef AWALE_H
#define AWALE_H

struct AwaleGame {
    char* player1;
    char* player2;
    int player1Score;
    int player2Score;
    int currentPlayer;
    int board[12]; /* Player 1 occupies slots 0-5 and 2 occupies slots 6-11*/ 
};

struct AwaleGame* createGame();
char* jouer(struct AwaleGame* game, int slot);
char* gameToString(struct AwaleGame* game);
char* askColumn(struct AwaleGame* game);
int playAwale(struct AwaleGame* game, int slot);
int endGame(struct AwaleGame* game);
char* showWinner(int winner);
char* showError(int error);

#endif
