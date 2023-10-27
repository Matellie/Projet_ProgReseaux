#include "awale.h"

// int main(int argc, char **argv){
//     AwaleGame* game = malloc(sizeof(AwaleGame));
//     createGame(game);
//     int slot;
//     char* text = malloc(sizeof(char)*1000);
//     askColumn(game,text);
//     printf("%s",text);
//     free(text);
//     while(game->isFinished == false){
//         scanf("%d", &slot);
//         text = malloc(sizeof(char)*1000);
//         jouer(game, slot, text);
//         printf("%s", text);
//         free(text);
//     }
//     printf("\nReplay of the game ? (1(yes)/0(no)) : ");
//     scanf("%d", &slot);
//     if (slot){
//         text = malloc(sizeof(char)*1000*game->nextMoveInSequence);
//         replayGame(game,text);
//         printf("\n%s\n",text);
//         free(text);
//     }
//     free(game);
//     printf("GoodBye !\n");
//     return 0;
// }

int jouer(AwaleGame* game, int slot, char* result){
    int error = playAwale(game, slot);
    char* boardMsg = malloc(sizeof(char)*500);
    gameToString(game, boardMsg);
    strcpy(result,boardMsg);
    free(boardMsg);
    if (error < 0){ /* If move was not accepted display the error */
        char* errorMsg = showError(error);
        strcat(result,errorMsg);
    } else { /* If move is accepted then register it*/
        registerMove(game, slot);
    }
    int winner = endGame(game);
    if (winner == 0){
        char* askMsg = malloc(sizeof(char)*200);
        askColumn(game, askMsg);
        strcat(result, askMsg);
        free(askMsg);
    } else {
        char* winnerMsg = malloc(sizeof(char)*60);
        showWinner(winner, winnerMsg);
        strcat(result,winnerMsg);
        free(winnerMsg);
    }
    return 0;
}

int gameToString(AwaleGame* game, char* result){
    char separator[] = "+--+--+--+--+--+--+\n"; 
    char endOfLine[] = "|\n";
    strcpy(result,separator);
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
    char player1[50];
    char player2[50];
    sprintf(player1, "player 1 : %d\n", game->player1Score);
    sprintf(player2, "player 2 : %d\n\n", game->player2Score);
    char tourJoueur[BUF_SIZE2];
    sprintf(tourJoueur, "C'est au tour de %s\n", (game->currentPlayer==1 ? game->player1 : game->player2));
    strcat(result, tourJoueur);
    strcat(result,scores);
    strcat(result,player1);
    strcat(result,player2);
    return 0;
}

int askColumn(AwaleGame* game, char* result){
    char temp[200];
    // sprintf(temp, "Current player : %d\n", game->currentPlayer);
    // strcpy(result,temp);
    sprintf(temp, "Choose a slot (1-6) : ");
    strcpy(result, temp);
    return 0;
}

int createGame(AwaleGame* newGame){
    newGame->player1Score = 0;
    newGame->player2Score = 0;
    for (int i=0; i<12; i++){
        newGame->board[i] = 4;
    }
    newGame->currentPlayer = 1;
    newGame->isFinished = false;
    newGame->nextMoveInSequence = 0;
    return 0;
}

int registerMove(AwaleGame* game, int slot){
    if (game->nextMoveInSequence >= MAX_AWALE_MOVES ) return MOVE_LIMIT_REACHED_ERROR; /*Error, move limit has been reached */

    game->moveSequence[game->nextMoveInSequence++] = slot;
    return 0;
}

int playAwale(AwaleGame* game, int slot){
    slot -= 1; // To use 0-based index.
    if (slot < 0 || slot > 5) return INVALID_SLOT_ERROR;

    int i;
    if (game->currentPlayer == 2)
        slot = 11 - slot;

    /* Create a fake array to simulate the next turn */
    int previewArray[12];
    for (i=0; i<12; i++)
        previewArray[i] = game->board[i];

    
    int nbBeads =  previewArray[slot];
    if (nbBeads == 0) return EMPTY_SLOT_ERROR; /* Case when player choose and empty slot */
    
    /* Take the beads out of the selected slot*/
    previewArray[slot] = 0;

    /* Put the beads in the next slots */
    int nextSlot;
    for (i=1; i<=nbBeads; ++i){
        nextSlot = (slot+i)%12;
        if (nextSlot != slot) /* Must ignore the slot that was emptied */
            previewArray[nextSlot] += 1;
    }

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
    if (famine) return FAMINE_MOVE_ERROR;

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

int endGame(AwaleGame* game){
    int i;
    int winner = (game->player1Score > game->player2Score ? 1 : 2);

    if (game->player1Score > 24 || game->player2Score > 24 || game->nextMoveInSequence >= MAX_AWALE_MOVES){
        game->isFinished = true;
        return winner;    
    }

    /* Checking if player 1's side is empty*/
    bool emptySide = true;
    for (i=0; i<5; i++){
        if (game->board[i] > 0){
            emptySide = false;
            break;
        }
    }
    if (emptySide){
        game->isFinished = true;
        return winner;
    } 

    /* Checking if player 2's side is empty*/
    emptySide = true;
    for (i=6; i<12; i++){
        if (game->board[i] > 0){
            emptySide = false;
            break;
        }
    }
    if (emptySide){
        game->isFinished = true;
        return winner;
    } 

    return 0;
}

int showWinner(int winner, char* result){
    sprintf(result, "\nPlayer %d won !\nPlayer %d is the beans master !\n", winner, winner);
    return 0;
}

char* showError(int error){
    switch (error){
        case INVALID_SLOT_ERROR :
            return "Invalid input : Move must be between 1 and 6 (included) !\n";
            break;
        case EMPTY_SLOT_ERROR :
            return "Invalid move : The selected move was an empty slot !\n";
            break;
        case FAMINE_MOVE_ERROR :
            return "Invalid move : The selected move generated a famine !\n";
            break;
        case MOVE_LIMIT_REACHED_ERROR :
            return "Error : Move has been player when move limit was already reached ! \n";
            break;
    }
    return "Unhandled Error\n";
}

int replayGame(AwaleGame* game, char* result){
    AwaleGame* duplicateGame = malloc(sizeof(AwaleGame));
    createGame(duplicateGame);
    char replayMsg[] = "Replay of the game :\n\n";
    strcpy(result,replayMsg);
    for (int i=0; i<game->nextMoveInSequence; ++i){
        char* moveMsg = malloc(sizeof(char)*1000);
        jouer(duplicateGame, game->moveSequence[i], moveMsg);
        strcat(result, moveMsg);
        free(moveMsg);
    }
    free(duplicateGame);
    return 0;
}
