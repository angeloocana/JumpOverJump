#include <stdio.h>

char BOARD_SIZE = 8;
char LAST_ROW_INDEX = 8 - 1;
char WHITE = 'O';
char BLACK = 'X';
char EMPTY = '_';

// Max possible moves
// Positions:
// 8*8 = 64    BOARD_SIZE*BOARD_SIZE
// 64/2 = 32   Piece can't change color when jumping
// 32-1 = 31   Piece can't move to current position
// TODO: Confirm right number. I'm guessing the max is 16 moves, 
// with some quick tests the maximum I got were 15 moves.
char MAX_POSSIBLE_MOVES = 16;

// currentPosition + (MAX_POSSIBLE_MOVES * position)
// 2 + (16 * 2)
char MAX_POSSIBLE_MOVES_ARRAY_LENGTH = 34;

void initializeBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
    for(char x = 0; x < BOARD_SIZE; ++x)
    {
        for(char y = 0; y < BOARD_SIZE; ++y)
        {
            if (x == 0) {
                board[x][y] = WHITE;
            } else {
                board[x][y] = x == LAST_ROW_INDEX ? BLACK : EMPTY;
            }
        }
    }
}

void printBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
    printf("  ");

    for(char x = 0; x < BOARD_SIZE; ++x)
    {
        printf(" x%d", x);
    }

    printf("\n");

    for(char x = 0; x < BOARD_SIZE; ++x)
    {
        printf(" y%d", x);

        for(char y = 0; y < BOARD_SIZE; ++y)
        {
            printf(" %c ", board[x][y]);
        }

        printf("y%d \n", x);
    }

    printf("  ");

    for(char x = 0; x < BOARD_SIZE; ++x)
    {
        printf(" x%d", x);
    }
    printf("\n");
}

// Example:
// possibleMoves = [
//  [0, 0, 0, 1, 1, 1], (x:0,y:0) can go to (x:0,y:1) (x:1,y:1) 
//  [1, 0, 0, 1, 1, 1, 2, 1], (x:1,y:0) can go to (x:0,y:1) (x:1,y:1) (x:2,y:1) 
// }
void getPossibleMovesForColor(char board[BOARD_SIZE][BOARD_SIZE], char color, char possibleMoves[BOARD_SIZE][MAX_POSSIBLE_MOVES_ARRAY_LENGTH]) {
    char pieceCount = 0;
    for(char x = 0; x < BOARD_SIZE; ++x)
    {
        for(char y = 0; y < BOARD_SIZE; ++y)
        {
            if (board[x][y] == color) {
                possibleMoves[pieceCount][0] = x;
                possibleMoves[pieceCount][1] = y;
                pieceCount++;
            }
        }
    }
}

void printPossibleMoves(char possibleMoves[BOARD_SIZE][MAX_POSSIBLE_MOVES_ARRAY_LENGTH]) {
    printf("\nPossible moves:\n");

    for(char i = 0; i < BOARD_SIZE; ++i)
    {
        char x = possibleMoves[i][0];
        char y = possibleMoves[i][1];
        printf("(x%d,y%d) => ", x, y);
        
        printf("\n");
    }
}

int main() {
    char board[BOARD_SIZE][BOARD_SIZE];
    initializeBoard(board);
    printBoard(board);

    char possibleMoves[BOARD_SIZE][MAX_POSSIBLE_MOVES_ARRAY_LENGTH];
    getPossibleMovesForColor(board, WHITE, possibleMoves);
    printPossibleMoves(possibleMoves);

    return 0;
}
