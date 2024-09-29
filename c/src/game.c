#include <stdio.h>
#include <string.h> // Add this at the top of the file with other includes

/////////////////////////////////////////////////////////////////////////////////
// Core
/////////////////////////////////////////////////////////////////////////////////

const char BOARD_SIZE = 8;
const char TOTAL_PIECES_PER_COLOR = BOARD_SIZE;
const char TOTAL_PIECES = BOARD_SIZE + BOARD_SIZE;
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
const char MAX_POSSIBLE_MOVES = 15;

const char POSITION_LENGHT = 2;
const char MAX_POSSIBLE_MOVES_ARRAY_LENGTH = POSITION_LENGHT + (MAX_POSSIBLE_MOVES * POSITION_LENGHT);

//    x0 x1 x2 x3 x4 x5 x6 x7
//  y7 X  X  X  X  X  X  X  X y7 
//  y6 _  _  _  _  _  _  _  _ y6 
//  y5 _  _  _  _  _  _  _  _ y5 
//  y4 _  _  _  _  _  _  _  _ y4 
//  y3 _  _  _  _  _  _  _  _ y3 
//  y2 _  _  _  _  _  _  _  _ y2 
//  y1 _  _  _  _  _  _  _  _ y1 
//  y0 O  O  O  O  O  O  O  O y0 
//    x0 x1 x2 x3 x4 x5 x6 x7
typedef char Board[BOARD_SIZE][BOARD_SIZE];

const char MAX_NEXT_POSITIONS = 8;

// Add to [[x, y], ...] to get to the next position
const char addForNexPositions[8][2] = {
    {-1, 1}, // Top left 
    {0, 1}, // Top
    {1, 1}, // Top right

    {-1, 0}, // Left
    {1, 0}, // Right

    {-1, -1}, // Bottom left
    {0, -1}, // Bottom
    {1, -1}, // Bottom right
};

void setPosition(char x, char y, char value, Board board) {
    board[x][y] = value;
}

char getPositionValue(char x, char y, Board board) {
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

char getWinner(Board board) {
    char whiteCount = 0;
    char blackCount = 0;

    for(char x = 0; x < TOTAL_PIECES_PER_COLOR; ++x)
    {
        if(getPositionValue(x, 0, board) == BLACK) {
            blackCount++;
        } 
        
        if(getPositionValue(x, LAST_ROW_INDEX, board) == WHITE) {
            whiteCount++;
        }
    }

    if(whiteCount == TOTAL_PIECES_PER_COLOR) {
        return WHITE;
    }

    if(blackCount == TOTAL_PIECES_PER_COLOR) {
        return BLACK;
    }

    return EMPTY;
}

typedef char PossibleMovesForPosition[MAX_POSSIBLE_MOVES_ARRAY_LENGTH];

// 0: Aborting position already added
// 1: Position addded
char addPositionToPossibleMoves(char x, char y, PossibleMovesForPosition possibleMovesForPosition) {
    char i = POSITION_LENGHT; // skip origin position x,y

    while (possibleMovesForPosition[i] != EMPTY) {
        if(
            x == possibleMovesForPosition[i]
            && y == possibleMovesForPosition[i + 1]
        ) {
            return 0; // Aborting position already added
        }

        i += POSITION_LENGHT;
    }

    possibleMovesForPosition[i] = x;
    possibleMovesForPosition[i + 1] = y;

    return 1;
}

int isValidIndex(char i) {
    return i >= 0 && i < BOARD_SIZE ? 1 : 0;
}

int isValidPosition(char x, char y) {
    return isValidIndex(x) && isValidIndex(y) ? 1 : 0;
}

void addJumpsToPossibleMoves(char x, char addX, char y, char addY, Board board, PossibleMovesForPosition possibleMovesForPosition) {
    char nextX = x + addX;
    char nextY = y + addY;

    if(isValidPosition(nextX, nextY) && getPositionValue(nextX, nextY, board) != EMPTY) {
        nextX = nextX + addX;
        nextY = nextY + addY;
        if(isValidPosition(nextX, nextY) && getPositionValue(nextX, nextY, board) == EMPTY) {
            if(addPositionToPossibleMoves(nextX, nextY, possibleMovesForPosition)){
                for(char i = 0; i < MAX_NEXT_POSITIONS; i++) {
                    char nextAddX = addForNexPositions[i][0];
                    char nextAddY = addForNexPositions[i][1];
                    addJumpsToPossibleMoves(nextX, nextAddX, nextY, nextAddY, board, possibleMovesForPosition);
                }
            }
        }
    }
}

void addPositionToPossibleMovesIfValid(char fromX, char addX, char fromY, char addY, Board board, PossibleMovesForPosition possibleMovesForPosition) {
    char x = fromX + addX;
    char y = fromY + addY;

    if (!isValidPosition(x, y)) {
        return;
    }

    if(getPositionValue(x, y, board) == EMPTY) {
        addPositionToPossibleMoves(x, y, possibleMovesForPosition);
    } else {
        addJumpsToPossibleMoves(fromX, addX, fromY, addY, board, possibleMovesForPosition);
    }
}

// Example:
// possibleMovesForPosition = [0, 0, 0, 1, 1, 1] 
// (x:0,y:0) can go to (x:0,y:1) (x:1,y:1) 
void getPossibleMovesForPosition(Board board, PossibleMovesForPosition possibleMovesForPosition) {
    char x = possibleMovesForPosition[0];
    char y = possibleMovesForPosition[1];

    for(char i = 0; i < MAX_NEXT_POSITIONS; i++) {
        char addX = addForNexPositions[i][0];
        char addY = addForNexPositions[i][1];
        addPositionToPossibleMovesIfValid(x, addX, y, addY, board, possibleMovesForPosition);
    }
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
// [0, 0, 0, 1, 1, 1], (x:0,y:0) can go to (x:0,y:1) (x:1,y:1) 
// [1, 0, 0, 1, 1, 1, 2, 1], (x:1,y:0) can go to (x:0,y:1) (x:1,y:1) (x:2,y:1) 
typedef char PossibleMovesForPosition[MAX_POSSIBLE_MOVES_ARRAY_LENGTH];

// Example:
// possibleMoves = [
//  [0, 0, 0, 1, 1, 1], (x:0,y:0) can go to (x:0,y:1) (x:1,y:1) 
//  [1, 0, 0, 1, 1, 1, 2, 1], (x:1,y:0) can go to (x:0,y:1) (x:1,y:1) (x:2,y:1) 
// }
typedef char PossibleMoves[TOTAL_PIECES_PER_COLOR][MAX_POSSIBLE_MOVES_ARRAY_LENGTH];

void getPossibleMovesForColor(Board board, char color, PossibleMoves possibleMoves) {
    char pieceCount = 0;
    for(char x = 0; x < BOARD_SIZE; ++x)
    {
        for(char y = 0; y < BOARD_SIZE; ++y)
        {
            if (getPositionValue(x, y, board) == color) {
                initializePossibleMovesForPosition(x, y, possibleMoves[pieceCount]);
                getPossibleMovesForPosition(board, possibleMoves[pieceCount]);
                pieceCount++;
            }
        }
    }
}

int isValidMove(char fromX, char fromY, char toX, char toY, Board board) {
    PossibleMovesForPosition possibleMovesForPosition;
    initializePossibleMovesForPosition(fromX, fromY, possibleMovesForPosition);
    getPossibleMovesForPosition(board, possibleMovesForPosition);

    char i = POSITION_LENGHT; // Skip index for origin possition
    while(i < MAX_POSSIBLE_MOVES_ARRAY_LENGTH && possibleMovesForPosition[i] != EMPTY) {
        char x = possibleMovesForPosition[i];
        char y = possibleMovesForPosition[i + 1];
        
        if(x == toX && y == toY) {
            return 1;
        }

        i += POSITION_LENGHT;
    }

    return 0;
}

void move(char fromX, char fromY, char toX, char toY, Board board) {
    setPosition(toX, toY, getPositionValue(fromX, fromY, board), board);
    setPosition(fromX, fromY, EMPTY, board);
}

typedef struct MoveHistory {
    char fromX;
    char fromY;
    char toX;
    char toY;
    Board *pBoardAfterMove;
    char color;
    int win_count;
    int game_count;
    struct MoveHistory *pNextMoves[MAX_NEXT_POSITIONS];
} MoveHistory;

// Board by position index with coordinates:
//    x0 x1 x2 x3 x4 x5 x6 x7
//  y7 7 15 23 31 39 47 55 63 y7 
//  y6 6 14 22 30 38 46 54 62 y6 
//  y5 5 13 21 29 37 45 53 61 y5 
//  y4 4 12 20 28 36 44 52 60 y4 
//  y3 3 11 19 27 35 43 51 59 y3 
//  y2 2 10 18 26 34 42 50 58 y2 
//  y1 1  9 17 25 33 41 49 57 y1 
//  y0 O  8 16 24 32 40 48 56 y0 
//    x0 x1 x2 x3 x4 x5 x6 x7
typedef unsigned char Position;

const unsigned char MAX_POSITIONS = 64;
const unsigned char POSITION_INCREMENT_FOR_WHITE = MAX_POSITIONS;
const unsigned char POSITION_INCREMENT_FOR_BLACK = POSITION_INCREMENT_FOR_WHITE + MAX_POSITIONS;

Position getPositionFromCoordinates(char x, char y) {
    return (x * BOARD_SIZE) + y;
}

typedef Position BoardHash[TOTAL_PIECES];

void getBoardHash(Board *pBoard, BoardHash *pHash) {
    printf("\nGetting board hash\n");
    Position position = 0;
    char pieceCount = 0;

    for(char x = 0; x < BOARD_SIZE; ++x)
    {
        for(char y = 0; y < BOARD_SIZE; ++y)
        {
            // printf("\nPosition: %d\n", position);
            char piece = getPositionValue(x, y, *pBoard);
            // printf("\nPiece: %c\n", piece);
            switch(piece) {
                case WHITE:
                    (*pHash)[pieceCount] = position + POSITION_INCREMENT_FOR_WHITE;
                    // printf("\nWHITE: %d\n", (*pHash)[pieceCount]);
                    pieceCount++;
                    break;
                case BLACK:
                    (*pHash)[pieceCount] = position + POSITION_INCREMENT_FOR_BLACK;
                    // printf("\nBLACK: %d\n", (*pHash)[pieceCount]);
                    pieceCount++;
                    break;
            }
            position++;
        }
    }
    printf("\nBoard hash: %s\n", *pHash);
}

typedef struct BoardHistoryMoveTo {
    Position to;
    unsigned int win_count;
    unsigned int game_count;
} BoardHistoryMoveTo;

typedef struct BoardHistoryMovesForPiece {
    char moveCount;
    Position from;
    BoardHistoryMoveTo tos[MAX_POSSIBLE_MOVES];
} BoardHistoryMovesForPiece;

typedef BoardHistoryMovesForPiece BoardHistoryMovesForColor[TOTAL_PIECES_PER_COLOR];

FILE *openBoardHistoryFile(BoardHash *pBoardHash, char color, const char * __restrict __mode) {
    printf("\nsizeof(BoardHistoryMovesForColor) %zu\n", sizeof(BoardHistoryMovesForColor));

    printf("\nOpening board history file\n");
    // 21 chars from fixed file name
    // 16 chars for hash
    // 1 char for color
    // 21 + 16 + 1 = 38
    char boardHistoryFileName[100];
    // sprintf(boardHistoryFileName, "../db/%s_%c.bin", *pBoardHash, color);

    sprintf(boardHistoryFileName, "../db/%c.bin", color);
    printf("\nOpening board history file: %s\n", boardHistoryFileName);
    return fopen(boardHistoryFileName, __mode);
    // FILE *f = fopen("c.bin", __mode);
    // printf("\nBoard history file opened %p \n", f);
    // return f;
}

void getBoardHistoryFromDisk(BoardHash *pBoardHash, char color, BoardHistoryMovesForColor *pBoardHistory) {
    FILE *pFile = openBoardHistoryFile(pBoardHash, color, "r");
    if (pFile == NULL) {
        printf("Error opening board history file!\n");
        return;
    }

    printf("\nReading board history from file\n");

    for(char pieceIndex = 0; pieceIndex < TOTAL_PIECES_PER_COLOR; ++pieceIndex)
    {
        printf("\nReading pieceIndex %d\n", pieceIndex);
        BoardHistoryMovesForPiece historyMovesForPiece = (*pBoardHistory)[pieceIndex];

        historyMovesForPiece.moveCount = fgetc(pFile);
        printf("\nRead moveCount %d\n", historyMovesForPiece.moveCount);

        historyMovesForPiece.from = fgetc(pFile);
        printf("\nRead historyMovesForPiece.from %d\n", historyMovesForPiece.from);
        
        fread(historyMovesForPiece.tos, sizeof(BoardHistoryMoveTo), historyMovesForPiece.moveCount, pFile);
        printf("\nRead historyMovesForPiece.tos\n");
    }

    printf("\nBoard history read from file\n");
    fclose(pFile);
    printf("\nBoard history file closed\n");
}

void writeBoardHistoryToDisk(BoardHash *pBoardHash, char color, BoardHistoryMovesForColor *pBoardHistory) {
    FILE *pFile = openBoardHistoryFile(pBoardHash, color, "w");
    if (pFile == NULL) {
        printf("Error opening board history file!\n");
        return;
    }
    printf("\nWriting board history to file\n");

    for(char pieceIndex = 0; pieceIndex < TOTAL_PIECES_PER_COLOR; ++pieceIndex)
    {
        printf("\nWriting pieceIndex %d\n", pieceIndex);
        BoardHistoryMovesForPiece historyMovesForPiece = (*pBoardHistory)[pieceIndex];

        printf("\nWriting moveCount %d\n", historyMovesForPiece.moveCount);
        fputc(historyMovesForPiece.moveCount, pFile);

        printf("\nWriting historyMovesForPiece.from %d\n", historyMovesForPiece.from);
        fputc(historyMovesForPiece.from, pFile);

        printf("\nWriting historyMovesForPiece.tos\n");
        fwrite(historyMovesForPiece.tos, sizeof(BoardHistoryMoveTo), historyMovesForPiece.moveCount, pFile);
    }

    printf("\nBoard history written to file\n");
    fclose(pFile);
    printf("\nBoard history file closed\n");
}

void getBoardHistory(Board *pBoard, char color, BoardHistoryMovesForColor *pBoardHistory) {
    printf("\nGetting board history\n");
    BoardHash boardHash;
    getBoardHash(pBoard, &boardHash);
    getBoardHistoryFromDisk(&boardHash, color, pBoardHistory);
}

void writeBoardHistory(Board *pBoard, char color, BoardHistoryMovesForColor *pBoardHistory) {
    printf("\nWriting board history\n");
    BoardHash boardHash;
    getBoardHash(pBoard, &boardHash);
    writeBoardHistoryToDisk(&boardHash, color, pBoardHistory);
}

void copyBoard(Board source, Board destination) {
    memcpy(destination, source, sizeof(Board));
}

char getNextColor(char color) {
    return color == WHITE ? BLACK : WHITE;
}

char MAX_EXPLORE_DEPTH = 5;
char MAX_EXPLORE_NEXT_POSITIONS = 2;

void printBoard(Board board); // TODO: Removel after debuging

char exploreNextMovesForColor(MoveHistory current, int depth);

char isMovingForward(char fromX, char fromY, char toX, char toY, char color) {
    return color == WHITE ? toY < fromY : toY > fromY;
}

void exploreNextMovesForPosition(MoveHistory current, PossibleMovesForPosition possibleMovesForPosition, int depth, char nextMovesCount, char nextColor) {
    char x = possibleMovesForPosition[0];
    char y = possibleMovesForPosition[1];

    char i = POSITION_LENGHT; // Skip index for origin possition
    while(i < MAX_POSSIBLE_MOVES_ARRAY_LENGTH && possibleMovesForPosition[i] != EMPTY && nextMovesCount < MAX_EXPLORE_NEXT_POSITIONS) {
        char toX = possibleMovesForPosition[i];
        char toY = possibleMovesForPosition[i + 1];
        i += POSITION_LENGHT;

        if(!isMovingForward(x, y, toX, toY, current.color)) {
            printf("\nNot moving forward from %dx,%dy to %dx,%dy\n", x, y, toX, toY);
            continue;
        }

        Board boardAfterMove;
        copyBoard(*current.pBoardAfterMove, boardAfterMove);
        move(x, y, toX, toY, boardAfterMove);

        MoveHistory nextMoveHistory = {
            .fromX = x,
            .fromY = y,
            .toX = toX,
            .toY = toY,
            .pBoardAfterMove = &boardAfterMove,
            .color = nextColor,
            .win_count = 0,
            .game_count = 0,
        };

        current.pNextMoves[nextMovesCount] = &nextMoveHistory;
        nextMovesCount++;

        exploreNextMovesForColor(nextMoveHistory, depth + 1);
    }
}

// Returns winner
char exploreNextMovesForColor(MoveHistory current, int depth) {
    char winner = getWinner(*current.pBoardAfterMove);

    printf("\n>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    printf("\nExploring depth: %d\n", depth);
    printf("\nCurrent color: %c\n", current.color);
    printf("\nWinner: %c\n", winner);
    printBoard(*current.pBoardAfterMove);

    if(winner != EMPTY) {
        if(winner == current.color) {
            current.win_count++;
        }
        current.game_count++;

        printf("\ngame finished\n");
        return winner;
    } 

    if(depth >= MAX_EXPLORE_DEPTH) {
        return EMPTY; // No winner found
    }

    char nextColor = getNextColor(current.color);
    PossibleMoves possibleMoves;
    getPossibleMovesForColor(*current.pBoardAfterMove, nextColor, possibleMoves);

    char nextMovesCount = 0;
    for(char pieceIndex = 0; pieceIndex < TOTAL_PIECES_PER_COLOR; ++pieceIndex)
    {
        exploreNextMovesForPosition(current, possibleMoves[pieceIndex], depth, nextMovesCount, nextColor);
    }

    printf("\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");

    return EMPTY;
}

void guessBestMove(Board *pBoard, char color) {
    printf("\nGuessing best move for color %c\n", color);
    printBoard(*pBoard);
    BoardHistoryMovesForColor boardHistory;

    PossibleMoves possibleMoves;
    getPossibleMovesForColor(*pBoard, color, possibleMoves);

    for(char pieceIndex = 0; pieceIndex < TOTAL_PIECES_PER_COLOR; ++pieceIndex)
    {
        char x = possibleMoves[pieceIndex][0];
        char y = possibleMoves[pieceIndex][1];

        printf("\nPiece %d: (x%d,y%d)\n", pieceIndex, x, y);

        BoardHistoryMovesForPiece historyMovesForPiece = {
            .from = getPositionFromCoordinates(x, y),
            .moveCount = 0,
        };

        char i = POSITION_LENGHT; // Skip index for origin possition
        while(i < MAX_POSSIBLE_MOVES_ARRAY_LENGTH && possibleMoves[pieceIndex][i] != EMPTY) {
            char toX = possibleMoves[pieceIndex][i];
            char toY = possibleMoves[pieceIndex][i + 1];
            i += POSITION_LENGHT;

            printf("\tPossible move %d: (x%d,y%d)\n", historyMovesForPiece.moveCount, toX, toY);

            BoardHistoryMoveTo historyMoveTo = {
                .to = getPositionFromCoordinates(toX, toY),
                .win_count = 0,
                .game_count = 0,
            };

            printf("\tBoard history move: from %d to %d\n", historyMovesForPiece.from, historyMoveTo.to);

            historyMovesForPiece.tos[historyMovesForPiece.moveCount] = historyMoveTo;
            historyMovesForPiece.moveCount++;
        }

        printf("\nMove count: %d\n", historyMovesForPiece.moveCount);
        boardHistory[pieceIndex] = historyMovesForPiece;
        printf("\n -------------------\n");
    }

    printf("\nWriting board history\n");
    writeBoardHistory(pBoard, color, &boardHistory);
    printf("\nBoard history written\n");
    printf("\nReading board history\n");
    getBoardHistory(pBoard, color, &boardHistory);
    printf("\nBoard history read\n");
    // MoveHistory initialMoveHistory = {
    //     .pBoardAfterMove = pBoard,
    //     .color = color,
    //     .win_count = 0,
    //     .game_count = 0,
    // };

    // exploreNextMovesForColor(initialMoveHistory, 0);

    // TODO: Find bext next move
}

void aiVsAi() {
    Board board;
    initializeBoard(board);
    printBoard(board);

    char turn = WHITE;
    // while(getWinner(board) == EMPTY) {
        guessBestMove(&board, turn);
    //    turn = getNextColor(turn);
    //}
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
            printf(" %c ", getPositionValue(x, y, board));
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

    for(char i = 0; i < TOTAL_PIECES_PER_COLOR; ++i)
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

    if(getPositionValue(fromX, fromY, board) != color) {
        printf("\n Error: Not your piece at %dx,%dy\n", fromX, fromY);
        return 0;
    }

    char toX = askForMoveI("to x"); 
    char toY = askForMoveI("to y");
    
    if(getPositionValue(toX, toY, board) != EMPTY) {
        printf("\n Error: To position not empty at %dx,%dy\n", toX, toY);
        return 0;
    }

    if(!isValidMove(fromX, fromY, toX, toY, board)) {
        printf("\n Error: Invalid move from %dx,%dy to %dx,%dy\n", fromX, fromY, toX, toY);
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
        printf("'g' guess best move\n");
        printf("'a' ai vs ai\n");
        printf("'b' board\n");
        printf("'e' exit\n");

        char answer = getchar();
        getchar(); // Removes new line
        
        switch (answer)
        {
            case 'a':
                aiVsAi();
                break;

            case 'b':
                printBoard(board);
                break;

            case 'h':
                helpWithPossibleMoves(board, turn);
                break;

            case 'm':
                if (askForMove(board, turn)) {
                    turn = getNextColor(turn);
                }
                break;

            case 'g':
                guessBestMove(&board, turn);
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