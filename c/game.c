#include <stdio.h>

int BOARD_SIZE = 8;
int LAST_ROW_INDEX = 8 - 1;
char WHITE = '0';
char BLACK = '1';
char EMPTY = '_';

void initializeBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
    for(int x = 0; x < BOARD_SIZE; ++x)
    {
        for(int y = 0; y < BOARD_SIZE; ++y)
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
    for(int x = 0; x < BOARD_SIZE; ++x)
    {
        for(int y = 0; y < BOARD_SIZE; ++y)
        {
            printf("%c", board[x][y]);
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
