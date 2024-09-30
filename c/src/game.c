#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

/////////////////////////////////////////////////////////////////////////////////
// Core
/////////////////////////////////////////////////////////////////////////////////

const char SUCCESS = 1;
const char ERROR = 0;
typedef char Result;

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

typedef struct Coordinates {
    char x;
    char y;
} Coordinates;

typedef struct Move {
    Coordinates from;
    Coordinates to;
} Move;

typedef Move Moves[100]; // TODO: Check max moves

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

typedef Position BoardHash[TOTAL_PIECES];

// First 8 chars for white pieces
// Next 8 chars for black pieces
// Last char for null terminator
typedef char BoardFileName[TOTAL_PIECES + 1];
typedef char BoardFileNameFullPath[29];

// We can only use valid characters in file names
// We will sum the position index with this value to get a valid character
const char FILE_NAME_CHAR_START = 48;

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

char MAX_EXPLORE_DEPTH = 5;
char MAX_EXPLORE_NEXT_POSITIONS = 2;

const unsigned int MAX_SCORE = 100000000;

typedef unsigned int Score;

typedef struct MoveScore {
    Move move;
    Score score;
} MoveScore;

const char AI_VS_AI_MAX_MOVES = 45; // TODO: Change to 45 when AI is improved

// Core functions

void copyMove(Move *pSource, Move *pDestination);
void setPosition(char x, char y, char value, Board board);
void setCoordinates(Coordinates *pCoordinates, char value, Board *pBoard);
char getPositionValue(char x, char y, Board board);
char getCoordinatesValue(Coordinates *pCoordinates, Board *pBoard);
char getInitialPositionValue(char y);
void initializeBoard(Board board);
char getWinner(Board board);
char addPositionToPossibleMoves(char x, char y, PossibleMovesForPosition possibleMovesForPosition);
int isValidIndex(char i);
int isValidPosition(char x, char y);
void addJumpsToPossibleMoves(char x, char addX, char y, char addY, Board board, PossibleMovesForPosition possibleMovesForPosition);
void addPositionToPossibleMovesIfValid(char fromX, char addX, char fromY, char addY, Board board, PossibleMovesForPosition possibleMovesForPosition);
void getPossibleMovesForPosition(Board board, PossibleMovesForPosition possibleMovesForPosition);
void initializePossibleMovesForPosition(char x, char y, PossibleMovesForPosition possibleMovesForPosition);
void getPossibleMovesForColor(Board board, char color, PossibleMoves possibleMoves);
int isValidMove(char fromX, char fromY, char toX, char toY, Board board);
void applyMoveToBoard(char fromX, char fromY, char toX, char toY, Board board);
void applyMove(Move *pMove, Board *pBoard);
Position getPositionFromXY(char x, char y);
Position getPositionFromCoordinates(Coordinates *pCoordinates);
void getCoordinatesFromPosition(Position *pPosition, Coordinates *pCoordinates);
void getBoardFileName(Board *pBoard, BoardFileName *pFileName);
void getBoardHash(Board *pBoard, BoardHash *pHash);
void initializeBoardHistoryMoveTo(Position to, BoardHistoryMoveTo *pBoardHistoryMoveTo);
void initializeBoardHistoryMoveForPiece(PossibleMovesForPosition *pPossibleMovesForPosition, BoardHistoryMovesForPiece *pHistoryMovesForPiece);
void initializeBoardHistoryMovesForColor(Board *pBoard, char color, BoardHistoryMovesForColor *pBoardHistory);
FILE *openBoardHistoryFile(Board *pBoard, char color, const char * __restrict __mode);
void getBoardHistoryMovesForPieceFromDisk(BoardHistoryMovesForPiece *historyMovesForPiece, FILE *pFile);
Result getBoardHistoryFromDisk(Board *pBoard, char color, BoardHistoryMovesForColor *pBoardHistory);
void writeBoardHistoryMovesForPieceToDisk(BoardHistoryMovesForPiece *pHistoryMovesForPiece, FILE *pFile);
void writeBoardHistoryToDisk(Board *pBoard, char color, BoardHistoryMovesForColor *pBoardHistory);
void getBoardHistory(Board *pBoard, char color, BoardHistoryMovesForColor *pBoardHistory);
void writeBoardHistory(Board *pBoard, char color, BoardHistoryMovesForColor *pBoardHistory);
void copyBoard(Board source, Board destination);
char getNextColor(char color);

// AI functions

char isMovingForward(char fromX, char fromY, char toX, char toY, char color);
char getNAdvancedRows(char color, char y);
void exploreNextMovesForPosition(MoveHistory current, PossibleMovesForPosition possibleMovesForPosition, int depth, char nextMovesCount, char nextColor);
char exploreNextMovesForColor(MoveHistory current, int depth);
void initializeMoveScore(Board *pBoard, char color, Position *pFrom, BoardHistoryMoveTo *pBoardHistoryMoveTo, MoveScore *pMoveScore);
Score generateRandomScore(Score maxScore);
void guessBestMove(Board *pBoard, char color, Move *pMove);
void backPropagateBoardHistory(Moves *pMoves, char totalMoves, char winner);
Result aiVsAi();
void aiVsAiForNGames(char nGames);

// Print functions

void printBoard(Board board);
void printMove(Move *pMove);
void printPossibleMovesForPiece(PossibleMovesForPosition possibleMovesForPosition);
void printPossibleMoves(PossibleMoves possibleMoves);
void helpWithPossibleMoves(Board board, char color);
char askForMoveI(char i[]);
int askForMove(Board board, char color);

// Code

void copyMove(Move *pSource, Move *pDestination) {
    pDestination->from.x = pSource->from.x;
    pDestination->from.y = pSource->from.y;
    pDestination->to.x = pSource->to.x;
    pDestination->to.y = pSource->to.y;
}

// TODO: Remove later
void setPosition(char x, char y, char value, Board board) {
    board[x][y] = value;
}

void setCoordinates(Coordinates *pCoordinates, char value, Board *pBoard) {
    (*pBoard)[pCoordinates->x][pCoordinates->y] = value;
}

// TODO: Remove later
char getPositionValue(char x, char y, Board board) {
    return board[x][y];
}

char getCoordinatesValue(Coordinates *pCoordinates, Board *pBoard) {
    return (*pBoard)[pCoordinates->x][pCoordinates->y];
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

Result addPositionToPossibleMoves(char x, char y, PossibleMovesForPosition possibleMovesForPosition) {
    char i = POSITION_LENGHT; // skip origin position x,y

    while (possibleMovesForPosition[i] != EMPTY) {
        if(
            x == possibleMovesForPosition[i]
            && y == possibleMovesForPosition[i + 1]
        ) {
            return ERROR; // Aborting position already added
        }

        i += POSITION_LENGHT;
    }

    possibleMovesForPosition[i] = x;
    possibleMovesForPosition[i + 1] = y;

    return SUCCESS;
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

// TODO: Remove later
void applyMoveToBoard(char fromX, char fromY, char toX, char toY, Board board) {
    setPosition(toX, toY, getPositionValue(fromX, fromY, board), board);
    setPosition(fromX, fromY, EMPTY, board);
}

void applyMove(Move *pMove, Board *pBoard) {
    setCoordinates(&(pMove->to), getCoordinatesValue(&(pMove->from), pBoard), pBoard);
    setCoordinates(&(pMove->from), EMPTY, pBoard);
}

Position getPositionFromXY(char x, char y) {
    return (x * BOARD_SIZE) + y;
}

Position getPositionFromCoordinates(Coordinates *pCoordinates) {
    return getPositionFromXY(pCoordinates->x, pCoordinates->y);
}

void getCoordinatesFromPosition(Position *pPosition, Coordinates *pCoordinates) {
    pCoordinates->x = *pPosition / BOARD_SIZE;
    pCoordinates->y = *pPosition % BOARD_SIZE;
}

void getBoardFileName(Board *pBoard, BoardFileName *pFileName) {
    char positionIndex = 0;
    char whiteCount = 0;
    char blackCount = 0;
    for(char x = 0; x < BOARD_SIZE; ++x)
    {
        for(char y = 0; y < BOARD_SIZE; ++y)
        {
            char piece = getPositionValue(x, y, *pBoard);
            switch(piece) {
                case WHITE:
                    (*pFileName)[whiteCount] = positionIndex + FILE_NAME_CHAR_START;
                    whiteCount++;
                    break;
                case BLACK:
                    (*pFileName)[blackCount + TOTAL_PIECES_PER_COLOR] = positionIndex + FILE_NAME_CHAR_START;
                    blackCount++;
                    break;
            }
            positionIndex++;
        }
    }
    (*pFileName)[TOTAL_PIECES] = '\0';
}

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

void initializeBoardHistoryMoveTo(Position to, BoardHistoryMoveTo *pBoardHistoryMoveTo) {
    pBoardHistoryMoveTo->to = to;
    pBoardHistoryMoveTo->win_count = 0;
    pBoardHistoryMoveTo->game_count = 0;
}

void initializeBoardHistoryMoveForPiece(PossibleMovesForPosition *pPossibleMovesForPosition, BoardHistoryMovesForPiece *pHistoryMovesForPiece) {
    char x = (*pPossibleMovesForPosition)[0];
    char y = (*pPossibleMovesForPosition)[1];

    pHistoryMovesForPiece->from = getPositionFromXY(x, y);
    pHistoryMovesForPiece->moveCount = 0;

    char i = POSITION_LENGHT; // Skip index for origin possition
    while(i < MAX_POSSIBLE_MOVES_ARRAY_LENGTH && (*pPossibleMovesForPosition)[i] != EMPTY) {
        char toX = (*pPossibleMovesForPosition)[i];
        char toY = (*pPossibleMovesForPosition)[i + 1];
        i += POSITION_LENGHT;

        // printf("\tPossible move %d: (x%d,y%d)\n", pHistoryMovesForPiece->moveCount, toX, toY);

        initializeBoardHistoryMoveTo(
            getPositionFromXY(toX, toY),
            &(pHistoryMovesForPiece->tos[pHistoryMovesForPiece->moveCount])
        );

        // printf("\tBoard history move: from %d to %d\n", pHistoryMovesForPiece->from, pHistoryMovesForPiece->tos[pHistoryMovesForPiece->moveCount].to);
        pHistoryMovesForPiece->moveCount++;
    }
}

void initializeBoardHistoryMovesForColor(Board *pBoard, char color, BoardHistoryMovesForColor *pBoardHistory) {
    // printf("\nInitializing board history moves for color %c\n", color);

    PossibleMoves possibleMoves;
    getPossibleMovesForColor(*pBoard, color, possibleMoves);

    for(char pieceIndex = 0; pieceIndex < TOTAL_PIECES_PER_COLOR; ++pieceIndex)
    {
        initializeBoardHistoryMoveForPiece(&possibleMoves[pieceIndex], &((*pBoardHistory)[pieceIndex]));

        // printf("\nMove count: %d\n", (*pBoardHistory)[pieceIndex].moveCount);
        // printf("\n -------------------\n");
    }
}

FILE *openBoardHistoryFile(Board *pBoard, char color, const char * __restrict __mode) {
    BoardFileName fileName;
    getBoardFileName(pBoard, &fileName);

    // printf("\nCreating file name full path\n");
    BoardFileNameFullPath fileNameFullPath;

    sprintf(fileNameFullPath, "../db/%s_%c.bin", fileName, color);
    printf("\nOpen: %s\n", fileNameFullPath);
    return fopen(fileNameFullPath, __mode);
}

void getBoardHistoryMovesForPieceFromDisk(BoardHistoryMovesForPiece *historyMovesForPiece, FILE *pFile) {
    historyMovesForPiece->moveCount = fgetc(pFile);
    // printf("\nRead moveCount %d\n", historyMovesForPiece->moveCount);

    historyMovesForPiece->from = fgetc(pFile);
    // printf("\nRead historyMovesForPiece->from %d\n", historyMovesForPiece->from);
    
    fread(historyMovesForPiece->tos, sizeof(BoardHistoryMoveTo), historyMovesForPiece->moveCount, pFile);
    // printf("\nRead historyMovesForPiece->tos\n");
}

Result getBoardHistoryFromDisk(Board *pBoard, char color, BoardHistoryMovesForColor *pBoardHistory) {
    FILE *pFile = openBoardHistoryFile(pBoard, color, "r");
    if (pFile == NULL) {
        // printf("Error opening board history file for read!\n");
        return ERROR;
    }

    printf("\nReading board history from file\n");

    for(char pieceIndex = 0; pieceIndex < TOTAL_PIECES_PER_COLOR; ++pieceIndex)
    {
        // printf("\nReading pieceIndex %d\n", pieceIndex);
        getBoardHistoryMovesForPieceFromDisk(&((*pBoardHistory)[pieceIndex]), pFile);
    }

    // printf("\nBoard history read from file\n");
    fclose(pFile);
    // printf("\nBoard history file closed\n");
    return SUCCESS;
}

void writeBoardHistoryMovesForPieceToDisk(BoardHistoryMovesForPiece *pHistoryMovesForPiece, FILE *pFile) {
    // printf("\nWriting moveCount %d\n", pHistoryMovesForPiece->moveCount);
    fputc(pHistoryMovesForPiece->moveCount, pFile);

    // printf("\nWriting pHistoryMovesForPiece->from %d\n", pHistoryMovesForPiece->from);
    fputc(pHistoryMovesForPiece->from, pFile);

    // printf("\nWriting pHistoryMovesForPiece->tos\n");
    fwrite(pHistoryMovesForPiece->tos, sizeof(BoardHistoryMoveTo), pHistoryMovesForPiece->moveCount, pFile);
}

void writeBoardHistoryToDisk(Board *pBoard, char color, BoardHistoryMovesForColor *pBoardHistory) {
    FILE *pFile = openBoardHistoryFile(pBoard, color, "w");
    if (pFile == NULL) {
        printf("Error opening board history file for write!\n");
        return;
    }
    // printf("\nWriting board history to file\n");

    for(char pieceIndex = 0; pieceIndex < TOTAL_PIECES_PER_COLOR; ++pieceIndex)
    {
        // printf("\nWriting pieceIndex %d\n", pieceIndex);
        writeBoardHistoryMovesForPieceToDisk(&((*pBoardHistory)[pieceIndex]), pFile);
    }

    // printf("\nBoard history written to file\n");
    fclose(pFile);
    // printf("\nBoard history file closed\n");
}

void getBoardHistory(Board *pBoard, char color, BoardHistoryMovesForColor *pBoardHistory) {
    // printf("\nGetting board history\n");
    if(getBoardHistoryFromDisk(pBoard, color, pBoardHistory) == ERROR) {
        initializeBoardHistoryMovesForColor(pBoard, color, pBoardHistory);
    }
}

void writeBoardHistory(Board *pBoard, char color, BoardHistoryMovesForColor *pBoardHistory) {
    printf("\nWriting board history\n");
    writeBoardHistoryToDisk(pBoard, color, pBoardHistory);
}

void copyBoard(Board source, Board destination) {
    memcpy(destination, source, sizeof(Board));
}

char getNextColor(char color) {
    return color == WHITE ? BLACK : WHITE;
}

char isMovingForward(char fromX, char fromY, char toX, char toY, char color) {
    return color == WHITE ? toY < fromY : toY > fromY;
}

char getNAdvancedRows(char color, char y) {
    if(color == BLACK) {
        return BOARD_SIZE - 1 - y;
    }
    return y;
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
        applyMoveToBoard(x, y, toX, toY, boardAfterMove);

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

void initializeMoveScore(Board *pBoard, char color, Position *pFrom, BoardHistoryMoveTo *pBoardHistoryMoveTo, MoveScore *pMoveScore) {
    getCoordinatesFromPosition(pFrom, &(pMoveScore->move.from));

    getCoordinatesFromPosition(&(pBoardHistoryMoveTo->to), &(pMoveScore->move.to));

    Board boardAfterMove;
    copyBoard(*pBoard, boardAfterMove);
    applyMove(&(pMoveScore->move), &boardAfterMove);

    char winner = getWinner(boardAfterMove);
    if(winner == color) {
        pMoveScore->score = MAX_SCORE;
    } else {
        // TODO: Proper calculate MCTS score
        int winRation = pBoardHistoryMoveTo->win_count / pBoardHistoryMoveTo->game_count;
        char nAdvancedRows = getNAdvancedRows(color, pMoveScore->move.to.y);
        // TODO: Calculate nAdvancedRows for all pieces
        Score score = (winRation*MAX_SCORE) + (nAdvancedRows * 10);
        pMoveScore->score = score > MAX_SCORE ? MAX_SCORE : score;
        printMove(&(pMoveScore->move));
        printf("\nWin ration: %d\n", winRation);
        printf("\nN advanced rows: %d\n", nAdvancedRows);
        printf("\nScore: %d\n", pMoveScore->score);
        printf("\n------\n");
    }
}

Score generateRandomScore(Score maxScore) {
    srand(time(NULL));
    return rand() % (maxScore + 1);
}

void guessBestMove(Board *pBoard, char color, Move *pMove) {
    printf("\nGuessing best move for color %c\n", color);
    BoardHistoryMovesForColor boardHistory;
    getBoardHistory(pBoard, color, &boardHistory);

    char movesCount = 0;
    Score totalScore = 0;
    MoveScore movesScore[TOTAL_PIECES_PER_COLOR];

    for(char pieceIndex = 0; pieceIndex < TOTAL_PIECES_PER_COLOR; ++pieceIndex)
    {
        for(char moveIndex = 0; moveIndex < boardHistory[pieceIndex].moveCount; ++moveIndex){
            initializeMoveScore(
                pBoard,
                color,
                &(boardHistory[pieceIndex].from), 
                &(boardHistory[pieceIndex].tos[moveIndex]), 
                &(movesScore[movesCount])
            );

            // printMove(&(movesScore[movesCount].move));
            // printf("\nMove score: %d\n\n", movesScore[movesCount].score);
            totalScore += movesScore[movesCount].score;
            movesCount++;
        }
    }

    // Generate a random number between 0 and totalScore
    Score randomScore = generateRandomScore(totalScore);
    // printf("\nTotal score: %d\n", totalScore);
    // printf("\nRandom score: %d\n", randomScore);
    // printf("\nmovesCount: %d\n", movesCount);

    Score cumulativeScore = 0;
    char moveIndex = -1;
    do {
        moveIndex++;
        cumulativeScore += movesScore[moveIndex].score;
    } while(randomScore > cumulativeScore);

    copyMove(&(movesScore[moveIndex].move), pMove);
}

void backPropagateBoardHistory(Moves *pMoves, char totalMoves, char winner) {
    printf("\nBack propagating board history\n");
    Board board;
    initializeBoard(board);
    char turn = WHITE;

    for(char i = 0; i < totalMoves; ++i) {
        printf("\nBack propagating move %d\n", i);
        printBoard(board);
        BoardHistoryMovesForColor boardHistory;
        getBoardHistory(&board, turn, &boardHistory);

        Position from = getPositionFromCoordinates(&((*pMoves)[i].from));
        Position to = getPositionFromCoordinates(&((*pMoves)[i].to));

        printMove(&((*pMoves)[i]));
        printf("\nFrom: %d, To: %d\n", from, to);
        
        for(char pieceIndex = 0; pieceIndex < TOTAL_PIECES_PER_COLOR; ++pieceIndex) {
            if(boardHistory[pieceIndex].from != from) {
                continue;
            }

            for(char moveIndex = 0; moveIndex < boardHistory[pieceIndex].moveCount; ++moveIndex) {
                if(boardHistory[pieceIndex].tos[moveIndex].to != to) {
                    continue;
                }

                boardHistory[pieceIndex].tos[moveIndex].game_count++;
                printf("\nIncrement game count moveIndex %d pieceIndex %d Game count: %d\n", moveIndex, pieceIndex, boardHistory[pieceIndex].tos[moveIndex].game_count);
                if(winner == turn) {
                    boardHistory[pieceIndex].tos[moveIndex].win_count++;
                    printf("\nIncrement win count moveIndex %d pieceIndex %d Win count: %d\n", moveIndex, pieceIndex, boardHistory[pieceIndex].tos[moveIndex].win_count);
                }
                break;
            }
            break;
        }

        writeBoardHistory(&board, turn, &boardHistory);
        applyMove(&((*pMoves)[i]), &board);
        turn = getNextColor(turn);
    }

    printf("\nBoard history back propagated\n");
}

// Success means a winner was found
Result aiVsAi() {
    Board board;
    initializeBoard(board);

    // Game starts with white but we change the turn before playing in the while loop
    char turn = BLACK;
    char moveIndex = 0;
    char winner = EMPTY;
    Moves moves;
    while(winner == EMPTY && moveIndex < AI_VS_AI_MAX_MOVES) {
        turn = getNextColor(turn);

        guessBestMove(&board, turn, &moves[moveIndex]);
        printf("\nMove %d:\n", moveIndex);
        printMove(&moves[moveIndex]);

        applyMove(&(moves[moveIndex]), &board);
        printBoard(board);

        winner = getWinner(board);
        moveIndex++;
    }

    printf("\nWinner: %c\n", winner);
    // backPropagateBoardHistory(&moves, moveIndex, winner);
    return winner != EMPTY ? SUCCESS : ERROR;
}

void aiVsAiForNGames(char nGames) {
    int winnerCount = 0;
    for(char i = 0; i < nGames; ++i) {
        if(aiVsAi()) {
            winnerCount++;
        }
        printf("\nGame %d: Winner Count: %d\n", i, winnerCount);
    }
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

void printMove(Move *pMove) {
    printf("\nMove: x%d,y%d to x%d,y%d\n", pMove->from.x, pMove->from.y, pMove->to.x, pMove->to.y);
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

    applyMoveToBoard(fromX, fromY, toX, toY, board);
    printBoard(board);
    return 1;
}

int main() {
    printf("Welcome to Jump Over Jump!\n\n");

    char turn = WHITE;
    Board board;
    initializeBoard(board);
    printBoard(board);
    Move move;

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
                aiVsAiForNGames(1);
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
                guessBestMove(&board, turn, &move);
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