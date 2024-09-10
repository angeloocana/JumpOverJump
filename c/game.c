#include <stdio.h>

/////////////////////////////////////////////////////////////////////////////////
// Core
/////////////////////////////////////////////////////////////////////////////////

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

void addPositionToPossibleMoves(char x, char y, char possibleMovesForPosition[MAX_POSSIBLE_MOVES_ARRAY_LENGTH]) {
    char i = 2; // skip origin position x,y

    while (possibleMovesForPosition[i] != EMPTY) {
        i += 2;
    }

    possibleMovesForPosition[i] = x;
    possibleMovesForPosition[i + 1] = y;
}

void addPositionToPossibleMovesIfValid(char x, char y, char board[BOARD_SIZE][BOARD_SIZE], char possibleMovesForPosition[MAX_POSSIBLE_MOVES_ARRAY_LENGTH]) {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
        return; // Position doesn't exist
    } else if (board[x][y] != EMPTY) {
        return; // Position not empty
    }

    addPositionToPossibleMoves(x, y, possibleMovesForPosition);
}

// Example:
// possibleMovesForPosition = [0, 0, 0, 1, 1, 1] 
// (x:0,y:0) can go to (x:0,y:1) (x:1,y:1) 
void getPossibleMovesForPosition(char board[BOARD_SIZE][BOARD_SIZE], char possibleMovesForPosition[MAX_POSSIBLE_MOVES_ARRAY_LENGTH]) {
    char x = possibleMovesForPosition[0];
    char y = possibleMovesForPosition[1];

    addPositionToPossibleMovesIfValid(x - 1, y - 1, board, possibleMovesForPosition); // Top left
    addPositionToPossibleMovesIfValid(x, y - 1, board, possibleMovesForPosition); // Top
    addPositionToPossibleMovesIfValid(x + 1, y - 1, board, possibleMovesForPosition); // Top right

    addPositionToPossibleMovesIfValid(x - 1, y, board, possibleMovesForPosition); // Left
    addPositionToPossibleMovesIfValid(x + 1, y, board, possibleMovesForPosition); // Right

    addPositionToPossibleMovesIfValid(x - 1, y + 1, board, possibleMovesForPosition); // Bottom left
    addPositionToPossibleMovesIfValid(x, y + 1, board, possibleMovesForPosition); // Bottom
    addPositionToPossibleMovesIfValid(x + 1, y + 1, board, possibleMovesForPosition); // Bottom right
}

void initializePossibleMovesForPosition(char x, char y, char possibleMovesForPosition[MAX_POSSIBLE_MOVES_ARRAY_LENGTH]) {
    possibleMovesForPosition[0] = x;
    possibleMovesForPosition[1] = y;

    // Set remaining array positions to EMPTY to avoid unexpected old memory values
    for(char i = 2; i < MAX_POSSIBLE_MOVES_ARRAY_LENGTH; i++) {
        possibleMovesForPosition[i] = EMPTY;
    }
    // or use memset if #include <string.h> included because of other reasons
    // memset(possibleMoves[pieceCount] + 2, EMPTY, MAX_POSSIBLE_MOVES_ARRAY_LENGTH - 2);
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
                initializePossibleMovesForPosition(x, y, possibleMoves[pieceCount]);
                getPossibleMovesForPosition(board, possibleMoves[pieceCount]);
                pieceCount++;
            }
        }
    }
}


/////////////////////////////////////////////////////////////////////////////////
// Terminal
/////////////////////////////////////////////////////////////////////////////////

void printBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
    printf("\n  ");

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

void printPossibleMovesForPiece(char possibleMoves[MAX_POSSIBLE_MOVES_ARRAY_LENGTH]) {
    char x = possibleMoves[0];
    char y = possibleMoves[1];
    char testX = 2;
    char testY = 2;
    printf("(x%d,y%d) =>", x, y);

    char i = 2; // Skip index for origin possition
    while(i <= MAX_POSSIBLE_MOVES_ARRAY_LENGTH && possibleMoves[i] != EMPTY) {
        char toX = possibleMoves[i];
        char toY = possibleMoves[i + 1];
        printf(" (x%d,y%d)", toX, toY);
        i += 2;
    }

    printf("\n");
}

void printPossibleMoves(char possibleMoves[BOARD_SIZE][MAX_POSSIBLE_MOVES_ARRAY_LENGTH]) {
    printf("\nPossible moves:\n");

    for(char i = 0; i < BOARD_SIZE; ++i)
    {
        printPossibleMovesForPiece(possibleMoves[i]);
    }
}

void helpWithPossibleMoves(char board[BOARD_SIZE][BOARD_SIZE], char color) {
    char possibleMoves[BOARD_SIZE][MAX_POSSIBLE_MOVES_ARRAY_LENGTH];
    getPossibleMovesForColor(board, color, possibleMoves);
    printPossibleMoves(possibleMoves);
}

char askForMoveI(char i) {
    char answer = -1;

    while(answer == -1) {
        printf("\nEnter %c: ", i);
        answer = getchar();
        getchar(); // Removes new line
        answer = answer - 48; // Converts char number to char index

        if (answer < 0 || answer >= BOARD_SIZE) {
            answer = -1;
            printf("\nInvalid index. Please try again.\n");
        }
    }

    return answer;
}

void askForMove(char board[BOARD_SIZE][BOARD_SIZE], char color) {
    char x = askForMoveI('x'); 
    char y = askForMoveI('y');

    printf("\nWill move to x:%d,y:%d another day when I get back to this.\n", x, y);
}

int main() {
    printf("Welcome to Jump Over Jump!\n\n");

    char turn = WHITE;
    char board[BOARD_SIZE][BOARD_SIZE];
    initializeBoard(board);
    printBoard(board);

    char exitGame = 0;

    while(exitGame == 0) {
        printf("\nWhat would you like to do? Enter:\n");
        printf("'m' move\n");
        printf("'h' help with possible moves\n");
        printf("'b' board\n");
        printf("'e' exit\n");

        char answer = getchar();
        getchar(); // Removes new line
        
        switch (answer)
        {
            case 'b':
                printBoard(board);
                break;

            case 'h':
                helpWithPossibleMoves(board, turn);
                break;

            case 'm':
                askForMove(board, turn);
                break;
            
            case 'e':
                printf("Goodbye!\n");
                exitGame = 1;
                break;
            
            default:
                printf("Invalid option. Please try again!\n");
                break;
        }
    }

    return 0;
}
