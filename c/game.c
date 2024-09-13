#include <stdio.h>

/////////////////////////////////////////////////////////////////////////////////
// Core
/////////////////////////////////////////////////////////////////////////////////

const char BOARD_SIZE = 8;
const char LAST_ROW_INDEX = BOARD_SIZE - 1;
const char WHITE = 'O';
const char BLACK = 'X';
const char EMPTY = '_';

// Max possible moves
// Positions:
// 8*8 = 64    BOARD_SIZE*BOARD_SIZE
// 64/2 = 32   Piece can't change color when jumping
// 32-1 = 31   Piece can't move to current position
// TODO: Confirm right number. I'm guessing the max is 16 moves, 
// with some quick tests the maximum I got were 15 moves.
const char MAX_POSSIBLE_MOVES = 16;

const char POSITION_LENGHT = 2;
const char MAX_POSSIBLE_MOVES_ARRAY_LENGTH = POSITION_LENGHT + (MAX_POSSIBLE_MOVES * POSITION_LENGHT);

typedef char Board[BOARD_SIZE][BOARD_SIZE];

void setPosition(char x, char y, char value, Board board) {
    board[x][y] = value;
}

char getPosition(char x, char y, Board board) {
    return board[x][y];
}

char getInitialPositionValue(char y) {
    switch (y)
    {
        case 0:
            return WHITE;
        case LAST_ROW_INDEX:
            return BLACK;
        default:
            return EMPTY;
    }
}

void initializeBoard(Board board) {
    for(char x = 0; x < BOARD_SIZE; ++x)
    {
        for(char y = 0; y < BOARD_SIZE; ++y)
        {
            setPosition(x, y, getInitialPositionValue(y), board);
        }
    }
}


typedef char PossibleMovesForPosition[MAX_POSSIBLE_MOVES_ARRAY_LENGTH];

void addPositionToPossibleMoves(char x, char y, PossibleMovesForPosition possibleMovesForPosition) {
    char i = POSITION_LENGHT; // skip origin position x,y

    while (possibleMovesForPosition[i] != EMPTY) {
        i += POSITION_LENGHT;
    }

    possibleMovesForPosition[i] = x;
    possibleMovesForPosition[i + 1] = y;
}

int isValidIndex(char i) {
    return i >= 0 && i < BOARD_SIZE ? 1 : 0;
}

int isValidPosition(char x, char y) {
    return isValidIndex(x) && isValidIndex(y) ? 1 : 0;
}

void addPositionToPossibleMovesIfValid(char x, char y, Board board, PossibleMovesForPosition possibleMovesForPosition) {
    if (!isValidPosition(x, y) || getPosition(x, y, board) != EMPTY) {
        return;
    }

    addPositionToPossibleMoves(x, y, possibleMovesForPosition);
}

// Example:
// possibleMovesForPosition = [0, 0, 0, 1, 1, 1] 
// (x:0,y:0) can go to (x:0,y:1) (x:1,y:1) 
void getPossibleMovesForPosition(Board board, PossibleMovesForPosition possibleMovesForPosition) {
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

void initializePossibleMovesForPosition(char x, char y, PossibleMovesForPosition possibleMovesForPosition) {
    possibleMovesForPosition[0] = x;
    possibleMovesForPosition[1] = y;

    // Set remaining array positions to EMPTY to avoid unexpected old memory values
    for(char i = POSITION_LENGHT; i < MAX_POSSIBLE_MOVES_ARRAY_LENGTH; i++) {
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
typedef char PossibleMoves[BOARD_SIZE][MAX_POSSIBLE_MOVES_ARRAY_LENGTH];

void getPossibleMovesForColor(Board board, char color, PossibleMoves possibleMoves) {
    char pieceCount = 0;
    for(char x = 0; x < BOARD_SIZE; ++x)
    {
        for(char y = 0; y < BOARD_SIZE; ++y)
        {
            if (getPosition(x, y, board) == color) {
                initializePossibleMovesForPosition(x, y, possibleMoves[pieceCount]);
                getPossibleMovesForPosition(board, possibleMoves[pieceCount]);
                pieceCount++;
            }
        }
    }
}

void move(char fromX, char fromY, char toX, char toY, Board board) {
    setPosition(toX, toY, getPosition(fromX, fromY, board), board);
    setPosition(fromX, fromY, EMPTY, board);
}

/////////////////////////////////////////////////////////////////////////////////
// Terminal
/////////////////////////////////////////////////////////////////////////////////

void printBoard(Board board) {
    printf("\n  ");

    for(char x = 0; x < BOARD_SIZE; ++x)
    {
        printf(" x%d", x);
    }

    printf("\n");

    for(char y = BOARD_SIZE - 1; y >= 0; --y)
    {
        printf(" y%d", y);

        for(char x = 0; x < BOARD_SIZE; ++x)
        {
            printf(" %c ", getPosition(x, y, board));
        }

        printf("y%d \n", y);
    }

    printf("  ");

    for(char x = 0; x < BOARD_SIZE; ++x)
    {
        printf(" x%d", x);
    }
    printf("\n");
}

void printPossibleMovesForPiece(PossibleMovesForPosition possibleMoves) {
    char x = possibleMoves[0];
    char y = possibleMoves[1];
    printf("(x%d,y%d) =>", x, y);

    char i = POSITION_LENGHT; // Skip index for origin possition
    while(i < MAX_POSSIBLE_MOVES_ARRAY_LENGTH && possibleMoves[i] != EMPTY) {
        char toX = possibleMoves[i];
        char toY = possibleMoves[i + 1];
        printf(" (x%d,y%d)", toX, toY);
        i += POSITION_LENGHT;
    }

    printf("\n");
}

void printPossibleMoves(PossibleMoves possibleMoves) {
    printf("\nPossible moves:\n");

    for(char i = 0; i < BOARD_SIZE; ++i)
    {
        printPossibleMovesForPiece(possibleMoves[i]);
    }
}

void helpWithPossibleMoves(Board board, char color) {
    PossibleMoves possibleMoves;
    getPossibleMovesForColor(board, color, possibleMoves);
    printPossibleMoves(possibleMoves);
}

char askForMoveI(char i[]) {
    char answer = -1;

    do {
        printf("\nEnter %s: ", i);
        answer = getchar();
        getchar(); // Removes new line
        answer = answer - 48; // Converts char number to char index

        if (!isValidIndex(answer)) {
            answer = -1;
            printf("\nInvalid index. Please try again.\n");
        }
    } while(answer == -1);

    return answer;
}

int askForMove(Board board, char color) {
    char fromX = askForMoveI("from x"); 
    char fromY = askForMoveI("from y");

    if(getPosition(fromX, fromY, board) != color) {
        printf("\n Error: Not your piece at %dx,%dy\n", fromX, fromY);
        return 0;
    }

    char toX = askForMoveI("to x"); 
    char toY = askForMoveI("to y");
    
    if(getPosition(toX, toY, board) != EMPTY) {
        printf("\n Error: To position not empty at %dx,%dy |%d|%c|\n", toX, toY, getPosition(toX, toY, board), getPosition(toX, toY, board));
        return 0;
    }

    move(fromX, fromY, toX, toY, board);
    printBoard(board);
    return 1;
}

int main() {
    printf("Welcome to Jump Over Jump!\n\n");

    char turn = WHITE;
    Board board;
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
                if (askForMove(board, turn)) {
                    turn = turn == WHITE ? BLACK : WHITE;
                }
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
