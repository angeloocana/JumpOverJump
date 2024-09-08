#include <stdio.h>

int BOARD_SIZE = 8;
int LAST_ROW_INDEX = 8 - 1;
char WHITE = 'O';
char BLACK = 'X';
char EMPTY = '_';

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
        printf(" %d", x);
    }

    printf("\n");

    for(char x = 0; x < BOARD_SIZE; ++x)
    {
        printf(" %d", x);

        for(char y = 0; y < BOARD_SIZE; ++y)
        {
            printf(" %c", board[x][y]);
        }

        printf("\n");
    }
}

int main() {
    char board[BOARD_SIZE][BOARD_SIZE];
    initializeBoard(board);
    printBoard(board);
    return 0;
}
