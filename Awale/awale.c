#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "awale.h"

int main(int argc, char **argv)
{
    struct AwaleGame* game = createGame();
    int winner = 0;
    int slot;
    while(endGame(game) ==  0){
        slot = -1;
        while (slot <1 || slot > 6){
            printf("Current player : %d\n", game->currentPlayer);
            printf("Choose a slot (1-6) : ");
            scanf("%d", &slot);
        }
        printf("\n");
        slot -= 1;
        playAwale(game, slot);
        printf("%s\n", gameToString(game));
    }
}

static char* gameToString(struct AwaleGame* game){
    char* result = malloc(sizeof(char)*500);
    char separator[] = "+--+--+--+--+--+--+\n"; 
    char endOfLine[] = "|\n";
    strcat(result,separator);
    for (int i=0; i<6; ++i){
        char slot[10];
        if (game->board[i] < 10)
            sprintf(slot, "| %d", game->board[i]);
        else
            sprintf(slot, "|%d", game->board[i]);
        strcat(result,slot);
    }
    strcat(result,endOfLine);
    strcat(result,separator);
    for (int i=11; i>5; --i){
        char slot[10];
        if (game->board[i] < 10)
            sprintf(slot, "| %d", game->board[i]);
        else
            sprintf(slot, "|%d", game->board[i]);
        strcat(result,slot);
    }
    strcat(result,endOfLine);
    strcat(result,separator);
    char scores[] = "Scores : \n";
    char player1[40];
    char player2[40];
    sprintf(player1, "player 1 : %d\n", game->player1Score);
    sprintf(player2, "player 2 : %d\n\n", game->player2Score);
    strcat(result,scores);
    strcat(result,player1);
    strcat(result,player2);
    return result;
}

struct AwaleGame* createGame(){
    struct AwaleGame* newGame = malloc(sizeof(struct AwaleGame));
    newGame->player1Score = 0;
    newGame->player2Score = 0;
    for (int i=0; i<12; i++){
        newGame->board[i] = 4;
    }
    newGame->currentPlayer = 1;
    return newGame;
}

static int playAwale(struct AwaleGame* game, int slot){
    int i;
    if (game->currentPlayer == 2)
        slot = 11 - slot;

    /* Create a fake array to simulate the next turn */
    int previewArray[12];
    for (i=0; i<12; i++)
        previewArray[i] = game->board[i];

    
    int nbBeads =  previewArray[slot];
    if (nbBeads == 0) return -1; /* Case when player choose and empty slot */

    printf("nbBeads : %d\n", nbBeads);
    
    /* Take the beads out of the selected slot*/
    previewArray[slot] = 0;

    /* Put the beads in the next slots */
    int nextSlot;
    for (i=1; i<=nbBeads; ++i){
        nextSlot = (slot+i)%12;
        if (nextSlot != slot) /* Must ignore the slot that was emptied */
            previewArray[nextSlot] += 1;
    }

    printf("PreviewArray : ");
    for(i=0; i<12; ++i)
        printf("%d|",previewArray[i]);
    printf("\n");

    int scoreToAdd = 0;
    int slotToEmpty = (slot+nbBeads)%12;

    /* Perform captures */
    if (game->currentPlayer == 1 && slotToEmpty > 5){ /* player1 finished in player2's side*/
        while (slotToEmpty > 5){
            if (previewArray[slotToEmpty] > 1 && previewArray[slotToEmpty] < 4){
                scoreToAdd += previewArray[slotToEmpty];
                previewArray[slotToEmpty] = 0;
                slotToEmpty = (slotToEmpty+11)%12;
            } else {
                break;
            }
        }
    } else if (game->currentPlayer == 2 && (slot+nbBeads)%12 < 6){ /* player2 finished in player1's side*/
        while (slotToEmpty < 6){
            if (previewArray[slotToEmpty] > 1 && previewArray[slotToEmpty] < 4){
                scoreToAdd += previewArray[slotToEmpty];
                previewArray[slotToEmpty] = 0;
                slotToEmpty = (slotToEmpty+11)%12;
            } else {
                break;
            }
        }
    }

    /* Check that the play didn't cause a famine */
    bool famine = true;
    if (game->currentPlayer == 1) {
        for (i=6; i<12; i++){
            if (previewArray[i] > 0){
                famine = false;
                break;
            }
        }
    } else {
        for (i=0; i<5; i++){
            if (previewArray[i] > 0){
                famine = false;
                break;
            }
        }
    }

    /* Return if a famine was generated */
    if (famine) return -1;

    /* Else update the game*/

    /* Update score */
    if (game->currentPlayer == 1){
        game->player1Score += scoreToAdd;
    } else {
        game->player2Score += scoreToAdd;
    }

    /* Update board */
    for (i=0; i<12; ++i){
        game->board[i] = previewArray[i];
    }

    /* Update current player */
    game->currentPlayer = (game->currentPlayer == 1 ? 2 : 1);
    return 0;
}

static int endGame(struct AwaleGame* game){
    int i;
    int winner = (game->player1Score > game->player2Score ? 1 : 2);
    if (game->player1Score > 24 || game->player2Score >24)
        return winner;
    bool emptySide = true;
    if (game->currentPlayer == 2) {
        for (i=6; i<12; i++){
            if (game->board[i] > 0){
                emptySide = false;
                break;
            }
        }
    } else {
        for (i=0; i<5; i++){
            if (game->board[i] > 0){
                emptySide = false;
                break;
            }
        }
    }

    if (emptySide){
        free(game);
        return winner;
    }

    return 0;
}