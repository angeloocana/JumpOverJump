#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h> // TODO: Do I really need this?
#include <libmemcached/memcached.h>
#include <sys/stat.h> // TODO: Do I really need this?
#include <errno.h> // TODO: Do I really need this?

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

// +8 chars for white pieces
// +8 chars for black pieces
// +1 char for color
// +1 char for null terminator
typedef char BoardHash[TOTAL_PIECES + 2];

const char BOARD_HISTORY_DB_PATH[] = "/Users/angeloocanamartins/dev/JumpOverJump_db/";

// +16 chars for pieces
// +15 for folders
// +50 for BOARD_HISTORY_DB_PATH
// +1 for null terminator
typedef char BoardFileNameFullPath[100]; // Add some extra space to avoid issues

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
const unsigned int MAX_SCORE_WIN_RATIO_MULTIPLIER = 1000;

// ~ pow((MAX_SCORE / TOTAL_PIECES_PER_COLOR), 1 / (LAST_ROW_INDEX * LAST_ROW_INDEX))
const float N_ADVANCED_ROWS_SCORE_POW = 1.3958;

typedef unsigned int Score;

typedef struct MoveScore {
    Move move;
    Score score;
} MoveScore;

const char AI_VS_AI_MAX_MOVES = 100;
const int AI_VS_AI_GAMES_COUNT = 1000;

// Core functions

void copyMove(Move *pSource, Move *pDestination);
void setPosition(char x, char y, char value, Board *pBoard);
void setCoordinates(Coordinates *pCoordinates, char value, Board *pBoard);
char getPositionValue(char x, char y, Board *pBoard);
char getCoordinatesValue(Coordinates *pCoordinates, Board *pBoard);
char getInitialPositionValue(char y);
void initializeBoard(Board *pBoard);
char getWinner(Board *pBoard);
char addPositionToPossibleMoves(char x, char y, PossibleMovesForPosition *pPossibleMovesForPosition);
int isValidIndex(char i);
int isValidPosition(char x, char y);
void addJumpsToPossibleMoves(char x, char addX, char y, char addY, Board *pBoard, PossibleMovesForPosition *pPossibleMovesForPosition);
void addPositionToPossibleMovesIfValid(char fromX, char addX, char fromY, char addY, Board *pBoard, PossibleMovesForPosition *pPossibleMovesForPosition);
void getPossibleMovesForPosition(Board *pBoard, PossibleMovesForPosition *pPossibleMovesForPosition);
void initializePossibleMovesForPosition(char x, char y, PossibleMovesForPosition *pPossibleMovesForPosition);
void getPossibleMovesForColor(Board *pBoard, char color, PossibleMoves *pPossibleMoves);
int isValidMove(char fromX, char fromY, char toX, char toY, Board *pBoard);
void applyMoveToBoard(char fromX, char fromY, char toX, char toY, Board *pBoard);
void applyMove(Move *pMove, Board *pBoard);
Position getPositionFromXY(char x, char y);
Position getPositionFromCoordinates(Coordinates *pCoordinates);
void getCoordinatesFromPosition(Position *pPosition, Coordinates *pCoordinates);
void getBoardHash(Board *pBoard, char color, BoardHash *pHash);
void initializeBoardHistoryMoveTo(Position to, BoardHistoryMoveTo *pBoardHistoryMoveTo);
void initializeBoardHistoryMoveForPiece(PossibleMovesForPosition *pPossibleMovesForPosition, BoardHistoryMovesForPiece *pHistoryMovesForPiece);
void initializeBoardHistoryMovesForColor(Board *pBoard, char color, BoardHistoryMovesForColor *pBoardHistory);

// Cache

int cacheKeyNotFoundCount = 0;
int cacheKeyFoundCount = 0;

memcached_return rc;
memcached_st *memc;

void setupMemcached();
Result writeBoardHistoryToCache(char *key, BoardHistoryMovesForColor *pBoardHistory);
Result getBoardHistoryFromCache(char *key, BoardHistoryMovesForColor *pBoardHistory);

// Disk

int filesNotFoundCount = 0;
int filesFoundCount = 0;

const char pathSeparator =
#ifdef _WIN32
  '\\';
#else
  '/';
#endif

void createFilePathDirs(char *file_path);
void getBoardFileNameFullPath(BoardHash *hash, BoardFileNameFullPath *fileNameFullPath);
void getBoardHistoryMovesForPieceFromDisk(BoardHistoryMovesForPiece *historyMovesForPiece, FILE *pFile);
Result getBoardHistoryFromDisk(BoardHash *hash, BoardHistoryMovesForColor *pBoardHistory);
void writeBoardHistoryMovesForPieceToDisk(BoardHistoryMovesForPiece *pHistoryMovesForPiece, FILE *pFile);
void writeBoardHistoryToDisk(BoardHash *hash, BoardHistoryMovesForColor *pBoardHistory);

void getBoardHistory(Board *pBoard, char color, BoardHistoryMovesForColor *pBoardHistory);
void writeBoardHistory(Board *pBoard, char color, BoardHistoryMovesForColor *pBoardHistory);

void copyBoard(Board *pSource, Board *pDestination);
char getNextColor(char color);

// AI functions

char isMovingForward(char fromX, char fromY, char toX, char toY, char color);
char getNAdvancedRows(char color, char y);
void exploreNextMovesForPosition(MoveHistory *pCurrent, PossibleMovesForPosition *pPossibleMovesForPosition, int depth, char nextMovesCount, char nextColor);
char exploreNextMovesForColor(MoveHistory *pCurrent, int depth);
void initializeMoveScore(Board *pBoard, char color, Position *pFrom, BoardHistoryMoveTo *pBoardHistoryMoveTo, MoveScore *pMoveScore, Score previousAdvancedRowsScore);
Score generateRandomScore(Score maxScore);
void guessBestMove(Board *pBoard, char color, Move *pMove);
void backPropagateBoardHistory(Moves *pMoves, char totalMoves, char winner);
Result aiVsAi();
void aiVsAiForNGames();

// Print functions

void printBoard(Board *pBoard);
void printMove(Move *pMove);
void printPossibleMovesForPiece(PossibleMovesForPosition *pPossibleMovesForPosition);
void printPossibleMoves(PossibleMoves *pPossibleMoves);
void printBoardHistory(BoardHistoryMovesForColor *pBoardHistory);

// User interactive functions

void helpWithPossibleMoves(Board *pBoard, char color);
char askForMoveI(char i[]);
int askForMove(Board *pBoard, char color);

// Code

void copyMove(Move *pSource, Move *pDestination) {
    pDestination->from.x = pSource->from.x;
    pDestination->from.y = pSource->from.y;
    pDestination->to.x = pSource->to.x;
    pDestination->to.y = pSource->to.y;
}

// TODO: Remove later
void setPosition(char x, char y, char value, Board *pBoard) {
    (*pBoard)[x][y] = value;
}

void setCoordinates(Coordinates *pCoordinates, char value, Board *pBoard) {
    (*pBoard)[pCoordinates->x][pCoordinates->y] = value;
}

// TODO: Remove later
char getPositionValue(char x, char y, Board *pBoard) {
    return (*pBoard)[x][y];
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

void initializeBoard(Board *pBoard) {
    for(char x = 0; x < BOARD_SIZE; ++x)
    {
        for(char y = 0; y < BOARD_SIZE; ++y)
        {
            setPosition(x, y, getInitialPositionValue(y), pBoard);
        }
    }
}

char getWinner(Board *pBoard) {
    char whiteCount = 0;
    char blackCount = 0;

    for(char x = 0; x < TOTAL_PIECES_PER_COLOR; ++x)
    {
        if(getPositionValue(x, 0, pBoard) == BLACK) {
            blackCount++;
        } 
        
        if(getPositionValue(x, LAST_ROW_INDEX, pBoard) == WHITE) {
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

Result addPositionToPossibleMoves(char x, char y, PossibleMovesForPosition *pPossibleMovesForPosition) {
    char i = POSITION_LENGHT; // skip origin position x,y

    while ((*pPossibleMovesForPosition)[i] != EMPTY) {
        if(
            x == (*pPossibleMovesForPosition)[i]
            && y == (*pPossibleMovesForPosition)[i + 1]
        ) {
            return ERROR; // Aborting position already added
        }

        i += POSITION_LENGHT;
    }

    (*pPossibleMovesForPosition)[i] = x;
    (*pPossibleMovesForPosition)[i + 1] = y;

    return SUCCESS;
}

int isValidIndex(char i) {
    return i >= 0 && i < BOARD_SIZE ? 1 : 0;
}

int isValidPosition(char x, char y) {
    return isValidIndex(x) && isValidIndex(y) ? 1 : 0;
}

void addJumpsToPossibleMoves(char x, char addX, char y, char addY, Board *pBoard, PossibleMovesForPosition *pPossibleMovesForPosition) {
    char nextX = x + addX;
    char nextY = y + addY;

    if(isValidPosition(nextX, nextY) && getPositionValue(nextX, nextY, pBoard) != EMPTY) {
        nextX = nextX + addX;
        nextY = nextY + addY;
        if(isValidPosition(nextX, nextY) && getPositionValue(nextX, nextY, pBoard) == EMPTY) {
            if(addPositionToPossibleMoves(nextX, nextY, pPossibleMovesForPosition)){
                for(char i = 0; i < MAX_NEXT_POSITIONS; i++) {
                    char nextAddX = addForNexPositions[i][0];
                    char nextAddY = addForNexPositions[i][1];
                    addJumpsToPossibleMoves(nextX, nextAddX, nextY, nextAddY, pBoard, pPossibleMovesForPosition);
                }
            }
        }
    }
}

void addPositionToPossibleMovesIfValid(char fromX, char addX, char fromY, char addY, Board *pBoard, PossibleMovesForPosition *pPossibleMovesForPosition) {
    char x = fromX + addX;
    char y = fromY + addY;

    if (!isValidPosition(x, y)) {
        return;
    }

    if(getPositionValue(x, y, pBoard) == EMPTY) {
        addPositionToPossibleMoves(x, y, pPossibleMovesForPosition);
    } else {
        addJumpsToPossibleMoves(fromX, addX, fromY, addY, pBoard, pPossibleMovesForPosition);
    }
}

// Example:
// possibleMovesForPosition = [0, 0, 0, 1, 1, 1] 
// (x:0,y:0) can go to (x:0,y:1) (x:1,y:1) 
void getPossibleMovesForPosition(Board *pBoard, PossibleMovesForPosition *pPossibleMovesForPosition) {
    char x = (*pPossibleMovesForPosition)[0];
    char y = (*pPossibleMovesForPosition)[1];

    for(char i = 0; i < MAX_NEXT_POSITIONS; i++) {
        char addX = addForNexPositions[i][0];
        char addY = addForNexPositions[i][1];
        addPositionToPossibleMovesIfValid(x, addX, y, addY, pBoard, pPossibleMovesForPosition);
    }
}

void initializePossibleMovesForPosition(char x, char y, PossibleMovesForPosition *pPossibleMovesForPosition) {
    (*pPossibleMovesForPosition)[0] = x;
    (*pPossibleMovesForPosition)[1] = y;

    // Set remaining array positions to EMPTY to avoid unexpected old memory values
    for(char i = POSITION_LENGHT; i < MAX_POSSIBLE_MOVES_ARRAY_LENGTH; i++) {
        (*pPossibleMovesForPosition)[i] = EMPTY;
    }
    // or use memset if #include <string.h> included because of other reasons
    // memset(possibleMoves[pieceCount] + 2, EMPTY, MAX_POSSIBLE_MOVES_ARRAY_LENGTH - 2);
}

void getPossibleMovesForColor(Board *pBoard, char color, PossibleMoves *pPossibleMoves) {
    char pieceCount = 0;
    for(char x = 0; x < BOARD_SIZE; ++x)
    {
        for(char y = 0; y < BOARD_SIZE; ++y)
        {
            if (getPositionValue(x, y, pBoard) == color) {
                initializePossibleMovesForPosition(x, y, &((*pPossibleMoves)[pieceCount]));
                getPossibleMovesForPosition(pBoard, &((*pPossibleMoves)[pieceCount]));
                pieceCount++;
            }
        }
    }
}

int isValidMove(char fromX, char fromY, char toX, char toY, Board *pBoard) {
    PossibleMovesForPosition possibleMovesForPosition;
    initializePossibleMovesForPosition(fromX, fromY, &possibleMovesForPosition);
    getPossibleMovesForPosition(pBoard, &possibleMovesForPosition);

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
void applyMoveToBoard(char fromX, char fromY, char toX, char toY, Board *pBoard) {
    setPosition(toX, toY, getPositionValue(fromX, fromY, pBoard), pBoard);
    setPosition(fromX, fromY, EMPTY, pBoard);
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

void getBoardHash(Board *pBoard, char color, BoardHash *pHash) {
    char positionIndex = 0;
    char whiteCount = 0;
    char blackCount = 0;
    char lastWhiteY = LAST_ROW_INDEX;
    char lastBlackY = 0;
    
    for(char x = 0; x < BOARD_SIZE; ++x)
    {
        for(char y = 0; y < BOARD_SIZE; ++y)
        {
            char piece = getPositionValue(x, y, pBoard);
            switch(piece) {
                case WHITE:
                    (*pHash)[whiteCount] = positionIndex + FILE_NAME_CHAR_START;
                    whiteCount++;
                    if(y < lastWhiteY) {
                        lastWhiteY = y;
                    }
                    break;
                case BLACK:
                    (*pHash)[blackCount + TOTAL_PIECES_PER_COLOR] = positionIndex + FILE_NAME_CHAR_START;
                    blackCount++;
                    if(y > lastBlackY) {
                        lastBlackY = y;
                    }
                    break;
            }
            positionIndex++;
        }
    }

    // printf("\nLast white y: %d | Last black y: %d\n", lastWhiteY, lastBlackY);
    // printf("\nwhole file name: %s\n", *pFileName);
    // Check if game is close to the end
    // different colored pieces can't interact with each other anymore so we don't care about the other player pieces.
    if(lastWhiteY - lastBlackY > 2) {
        if (color == BLACK) {
            memmove(*pHash, *pHash + TOTAL_PIECES_PER_COLOR, TOTAL_PIECES_PER_COLOR);
            // printf("\nmemove name: %s\n", *pFileName);
        }

        (*pHash)[TOTAL_PIECES_PER_COLOR] = color;
        (*pHash)[TOTAL_PIECES_PER_COLOR + 1] = '\0';
    } else {
        char colorIndex = TOTAL_PIECES_PER_COLOR * 2;
        (*pHash)[colorIndex] = color;
        (*pHash)[colorIndex + 1] = '\0';
    }
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
    getPossibleMovesForColor(pBoard, color, &possibleMoves);

    for(char pieceIndex = 0; pieceIndex < TOTAL_PIECES_PER_COLOR; ++pieceIndex)
    {
        initializeBoardHistoryMoveForPiece(&possibleMoves[pieceIndex], &((*pBoardHistory)[pieceIndex]));

        // printf("\nMove count: %d\n", (*pBoardHistory)[pieceIndex].moveCount);
        // printf("\n -------------------\n");
    }
}

/////////////////////////////////////////////////////////////////////////////////
// Memcached
/////////////////////////////////////////////////////////////////////////////////

void setupMemcached() {
    memcached_server_st *servers = NULL;

    // TODO: Please note, when you write new application use memcached() over memcached_create().
    // https://awesomized.github.io/libmemcached/libmemcached/memcached_create.html#description
    memc = memcached_create(NULL);
    servers = memcached_server_list_append(servers, "localhost", 11211, &rc);
    rc = memcached_server_push(memc, servers);

    if (rc == MEMCACHED_SUCCESS)
        fprintf(stderr, "Memcached server added successfully\n");
    else
        fprintf(stderr, "Couldn't add memcached server: %s\n", memcached_strerror(memc, rc));
}

Result writeBoardHistoryToCache(char *key, BoardHistoryMovesForColor *pBoardHistory) {
    rc = memcached_set(memc, key, strlen(key), (const char*)pBoardHistory, sizeof(BoardHistoryMovesForColor), (time_t)0, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL);

    if (rc == MEMCACHED_SUCCESS) {
        // fprintf(stderr, "Key stored successfully\n");
        return SUCCESS;
    }
    
    fprintf(stderr, "Couldn't store key: %s\n", memcached_strerror(memc, rc));
    return ERROR;
}

Result getBoardHistoryFromCache(char *key, BoardHistoryMovesForColor *pBoardHistory) {
    size_t value_length;
    uint32_t flags;

    char* retrieved_value = memcached_get(memc, key, strlen(key), &value_length, &flags, &rc);

    if (rc == MEMCACHED_SUCCESS) {
        // fprintf(stderr, "Key retrieved successfully\n");
        // printf("The key '%s' returned value '%s'.\n", key, pBoardHistory);
        // TODO: Write function to print BoardHistoryMovesForColor
        memcpy(pBoardHistory, (BoardHistoryMovesForColor*)retrieved_value, sizeof(BoardHistoryMovesForColor));
        free(retrieved_value);
        cacheKeyFoundCount++;
        // printBoardHistory(pBoardHistory);
        return SUCCESS;
    }
    
    cacheKeyNotFoundCount++;
    // fprintf(stderr, "Couldn't retrieve key: %s\n", memcached_strerror(memc, rc));
    return ERROR;
}

/////////////////////////////////////////////////////////////////////////////////
// Disk
/////////////////////////////////////////////////////////////////////////////////

// Given a file path, create all constituent directories if missing
void createFilePathDirs(char *file_path) {
  char *dir_path = (char *) malloc(strlen(file_path) + 1);
  char *next_sep = strchr(file_path, pathSeparator);
  while (next_sep != NULL) {
    int dir_path_len = next_sep - file_path;
    memcpy(dir_path, file_path, dir_path_len);
    dir_path[dir_path_len] = '\0';
    mkdir(dir_path, S_IRWXU|S_IRWXG|S_IROTH);
    next_sep = strchr(next_sep + 1, pathSeparator);
  }
  free(dir_path);
}

void getBoardFileNameFullPath(BoardHash *hash, BoardFileNameFullPath *fileNameFullPath) {
    // Write "../db/" to the start of fileNameFullPath
    strcpy(*fileNameFullPath, BOARD_HISTORY_DB_PATH);
    
    // Creates a directory for each position, the goal is to reduce the number of files per directory
    char iHash = 0;
    char iFileNameFullPath = strlen(*fileNameFullPath);
    do {
        (*fileNameFullPath)[iFileNameFullPath] = (*hash)[iHash];
        if ((*hash)[iHash + 1] != '\0') {
            (*fileNameFullPath)[iFileNameFullPath + 1] = '/';
            iFileNameFullPath += 2;
        } else {
            iFileNameFullPath++;
        }
        iHash++;
    } while((*hash)[iHash] != '\0');

    (*fileNameFullPath)[iFileNameFullPath] = '\0';
    
    // printf("\nOpen: %s\n", *fileNameFullPath);
}

void getBoardHistoryMovesForPieceFromDisk(BoardHistoryMovesForPiece *historyMovesForPiece, FILE *pFile) {
    historyMovesForPiece->moveCount = fgetc(pFile);
    // printf("\nRead moveCount %d\n", historyMovesForPiece->moveCount);

    historyMovesForPiece->from = fgetc(pFile);
    // printf("\nRead historyMovesForPiece->from %d\n", historyMovesForPiece->from);
    
    fread(historyMovesForPiece->tos, sizeof(BoardHistoryMoveTo), historyMovesForPiece->moveCount, pFile);
    // printf("\nRead historyMovesForPiece->tos\n");
}

Result getBoardHistoryFromDisk(BoardHash *hash, BoardHistoryMovesForColor *pBoardHistory) {
    BoardFileNameFullPath fileNameFullPath;
    getBoardFileNameFullPath(hash, &fileNameFullPath);

    FILE *pFile = fopen(fileNameFullPath, "r");
    if (pFile == NULL) {
        // printf("Error opening board history file for read!\n");
        filesNotFoundCount++;
        return ERROR;
    }
    filesFoundCount++;
    // printf("\nReading board history from file\n");

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

void writeBoardHistoryToDisk(BoardHash *hash, BoardHistoryMovesForColor *pBoardHistory) {
    BoardFileNameFullPath fileNameFullPath;
    getBoardFileNameFullPath(hash, &fileNameFullPath);

    createFilePathDirs(fileNameFullPath);

    FILE *pFile = fopen(fileNameFullPath, "w");
    if (pFile == NULL) {
        // printf("Error opening board history file for write!\n");
        return;
    }
    // printf("\nWriting board history to disk\n");

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
    BoardHash hash;
    getBoardHash(pBoard, color, &hash);

    if(getBoardHistoryFromCache(&(hash[0]), pBoardHistory) == SUCCESS) {
        return;
    }

    if(getBoardHistoryFromDisk(&hash, pBoardHistory) == ERROR) {
        initializeBoardHistoryMovesForColor(pBoard, color, pBoardHistory);
    }
}

void writeBoardHistory(Board *pBoard, char color, BoardHistoryMovesForColor *pBoardHistory) {
    // printf("\nWriting board history\n");
    BoardHash hash;
    getBoardHash(pBoard, color, &hash);

    writeBoardHistoryToCache(&(hash[0]), pBoardHistory);
    writeBoardHistoryToDisk(&hash, pBoardHistory);
}

void copyBoard(Board *pSource, Board *pDestination) {
    memcpy(*pDestination, *pSource, sizeof(Board));
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

void exploreNextMovesForPosition(MoveHistory *pCurrent, PossibleMovesForPosition *pPossibleMovesForPosition, int depth, char nextMovesCount, char nextColor) {
    char x = (*pPossibleMovesForPosition)[0];
    char y = (*pPossibleMovesForPosition)[1];

    char i = POSITION_LENGHT; // Skip index for origin possition
    while(i < MAX_POSSIBLE_MOVES_ARRAY_LENGTH && (*pPossibleMovesForPosition)[i] != EMPTY && nextMovesCount < MAX_EXPLORE_NEXT_POSITIONS) {
        char toX = (*pPossibleMovesForPosition)[i];
        char toY = (*pPossibleMovesForPosition)[i + 1];
        i += POSITION_LENGHT;

        if(!isMovingForward(x, y, toX, toY, pCurrent->color)) {
            printf("\nNot moving forward from %dx,%dy to %dx,%dy\n", x, y, toX, toY);
            continue;
        }

        Board boardAfterMove;
        copyBoard(pCurrent->pBoardAfterMove, &boardAfterMove);
        applyMoveToBoard(x, y, toX, toY, &boardAfterMove);

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

        pCurrent->pNextMoves[nextMovesCount] = &nextMoveHistory;
        nextMovesCount++;

        exploreNextMovesForColor(&nextMoveHistory, depth + 1);
    }
}

// Returns winner
char exploreNextMovesForColor(MoveHistory *pCurrent, int depth) {
    char winner = getWinner(pCurrent->pBoardAfterMove);

    printf("\nExploring depth: %d\n", depth);
    printf("\nCurrent color: %c\n", pCurrent->color);
    printf("\nWinner: %c\n", winner);
    printBoard(pCurrent->pBoardAfterMove);

    if(winner != EMPTY) {
        if(winner == pCurrent->color) {
            pCurrent->win_count++;
        }
        pCurrent->game_count++;

        printf("\ngame finished\n");
        return winner;
    } 

    if(depth >= MAX_EXPLORE_DEPTH) {
        return EMPTY; // No winner found
    }

    char nextColor = getNextColor(pCurrent->color);
    PossibleMoves possibleMoves;
    getPossibleMovesForColor(pCurrent->pBoardAfterMove, nextColor, &possibleMoves);

    char nextMovesCount = 0;
    for(char pieceIndex = 0; pieceIndex < TOTAL_PIECES_PER_COLOR; ++pieceIndex)
    {
        exploreNextMovesForPosition(pCurrent, &possibleMoves[pieceIndex], depth, nextMovesCount, nextColor);
    }

    printf("\n<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<\n");

    return EMPTY;
}

void getAdvancedRowsScore(Board *pBoard, char color, Score *pAdvancedRowsScore) {
    PossibleMoves possibleMoves;
    getPossibleMovesForColor(pBoard, color, &possibleMoves);

    *pAdvancedRowsScore = 0;
    for(char i = 0; i < TOTAL_PIECES_PER_COLOR; ++i) {
        char y = possibleMoves[i][1];
        char nAdvancedRows = getNAdvancedRows(color, y);
        if(nAdvancedRows > 0) {
            *pAdvancedRowsScore += pow(N_ADVANCED_ROWS_SCORE_POW, nAdvancedRows * LAST_ROW_INDEX);
            // printf("N: %d ", nAdvancedRows);
            // printf("Score: %d | ", *pAdvancedRowsScore);
        }
    }
    // printf("\n");
}

void initializeMoveScore(Board *pBoard, char color, Position *pFrom, BoardHistoryMoveTo *pBoardHistoryMoveTo, MoveScore *pMoveScore, Score previousAdvancedRowsScore) {
    getCoordinatesFromPosition(pFrom, &(pMoveScore->move.from));

    getCoordinatesFromPosition(&(pBoardHistoryMoveTo->to), &(pMoveScore->move.to));

    Board boardAfterMove;
    copyBoard(pBoard, &boardAfterMove);
    applyMove(&(pMoveScore->move), &boardAfterMove);

    char winner = getWinner(&boardAfterMove);
    if(winner == color) {
        pMoveScore->score = MAX_SCORE;
    } else {
        // TODO: Proper calculate MCTS score
        float winRatio = 0;
        if(pBoardHistoryMoveTo->game_count > 0) {
            winRatio = (float)pBoardHistoryMoveTo->win_count / pBoardHistoryMoveTo->game_count;
        }
        // printMove(&(pMoveScore->move));
        Score advancedRowsScore;
        getAdvancedRowsScore(&boardAfterMove, color, &advancedRowsScore);

        Score diffAdvancedRowsScore = previousAdvancedRowsScore > advancedRowsScore ? 0 : advancedRowsScore - previousAdvancedRowsScore;
        Score score = (winRatio*MAX_SCORE_WIN_RATIO_MULTIPLIER) + diffAdvancedRowsScore;
        pMoveScore->score = score > MAX_SCORE ? MAX_SCORE : score;
        // printf("Score: %d | ", pMoveScore->score);
        // printf("Win ratio: %f | ", winRatio);
        // printf("Games: %d | ", pBoardHistoryMoveTo->game_count);
        // printf("Wins: %d | ", pBoardHistoryMoveTo->win_count);
        // printf("Diff: %d | ", diffAdvancedRowsScore);
        // printf("N advanced rows score: %d | ", advancedRowsScore);
        // printf("Previous advanced rows score: %d \n", previousAdvancedRowsScore);
    }
}

Score generateRandomScore(Score maxScore) {
    return rand() % (maxScore + 1);
}

void guessBestMove(Board *pBoard, char color, Move *pMove) {
    // printf("\nGuessing best move for color %c\n", color);
    BoardHistoryMovesForColor boardHistory;
    getBoardHistory(pBoard, color, &boardHistory);

    char movesCount = 0;
    Score totalScore = 0;
    MoveScore movesScore[TOTAL_PIECES_PER_COLOR];

    Score advancedRowsScore;
    getAdvancedRowsScore(pBoard, color, &advancedRowsScore);
    // printf("\nAdvanced rows score: %d\n", advancedRowsScore);

    for(char pieceIndex = 0; pieceIndex < TOTAL_PIECES_PER_COLOR; ++pieceIndex)
    {
        for(char moveIndex = 0; moveIndex < boardHistory[pieceIndex].moveCount; ++moveIndex){
            initializeMoveScore(
                pBoard,
                color,
                &(boardHistory[pieceIndex].from), 
                &(boardHistory[pieceIndex].tos[moveIndex]), 
                &(movesScore[movesCount]),
                advancedRowsScore
            );

            // printMove(&(movesScore[movesCount].move));
            // printf("\nMove score: %d\n\n", movesScore[movesCount].score);
            totalScore += movesScore[movesCount].score;
            movesCount++;
        }
    }

    // Generate a random number between 0 and totalScore
    Score randomScore = generateRandomScore(totalScore);
    // printf("Total score: %d |", totalScore);
    // printf("Random score: %d\n", randomScore);
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
    // printf("\nBack propagating board history\n");
    Board board;
    initializeBoard(&board);
    char turn = WHITE;

    for(char i = 0; i < totalMoves; ++i) {
        // printf("\nBack propagating move %d\n", i);
        // printBoard(&board);
        BoardHistoryMovesForColor boardHistory;
        getBoardHistory(&board, turn, &boardHistory);

        Position from = getPositionFromCoordinates(&((*pMoves)[i].from));
        Position to = getPositionFromCoordinates(&((*pMoves)[i].to));

        // printMove(&((*pMoves)[i]));
        // printf("\nFrom: %d, To: %d\n", from, to);
        
        for(char pieceIndex = 0; pieceIndex < TOTAL_PIECES_PER_COLOR; ++pieceIndex) {
            if(boardHistory[pieceIndex].from != from) {
                continue;
            }

            for(char moveIndex = 0; moveIndex < boardHistory[pieceIndex].moveCount; ++moveIndex) {
                if(boardHistory[pieceIndex].tos[moveIndex].to != to) {
                    continue;
                }

                boardHistory[pieceIndex].tos[moveIndex].game_count++;
                // printf("\nIncrement game count moveIndex %d pieceIndex %d Game count: %d\n", moveIndex, pieceIndex, boardHistory[pieceIndex].tos[moveIndex].game_count);
                if(winner == turn) {
                    boardHistory[pieceIndex].tos[moveIndex].win_count++;
                    // printf("\nIncrement win count moveIndex %d pieceIndex %d Win count: %d\n", moveIndex, pieceIndex, boardHistory[pieceIndex].tos[moveIndex].win_count);
                }
                break;
            }
            break;
        }

        writeBoardHistory(&board, turn, &boardHistory);
        applyMove(&((*pMoves)[i]), &board);
        turn = getNextColor(turn);
    }

    // printf("\nBoard history back propagated\n");
}

// Success means a winner was found
Result aiVsAi() {
    Board board;
    initializeBoard(&board);

    // Game starts with white but we change the turn before playing in the while loop
    char turn = BLACK;
    char moveIndex = 0;
    char winner = EMPTY;
    Moves moves;
    while(winner == EMPTY && moveIndex < AI_VS_AI_MAX_MOVES) {
        turn = getNextColor(turn);

        guessBestMove(&board, turn, &moves[moveIndex]);
        // printBoard(&board);
        // printf("\nMove %d:\n", moveIndex);
        // printMove(&moves[moveIndex]);

        applyMove(&(moves[moveIndex]), &board);
        // printBoard(&board);

        winner = getWinner(&board);
        moveIndex++;
    }

    // printBoard(&board);
    // printf("\nWinner: %c | ", winner);
    // printf("Move index: %d | ", moveIndex);
    backPropagateBoardHistory(&moves, moveIndex, winner);
    return winner != EMPTY ? SUCCESS : ERROR;
}

void aiVsAiForNGames() {
    cacheKeyNotFoundCount = 0;
    cacheKeyFoundCount = 0;
    filesNotFoundCount = 0;
    filesFoundCount = 0;

    int winnerCount = 0;
    for(int i = 0; i < AI_VS_AI_GAMES_COUNT; ++i) {
        if(aiVsAi()) {
            winnerCount++;
        }
        printf("\nGame %d: | Winner Count: %d | Winner ratio: %f", i, winnerCount, (float)winnerCount / (i + 1));

        // struct timespec remaining, request = { 0, 1000 }; 
        // nanosleep(&request, &remaining);
    }

    printf("\nCache keys %d not found | %d found | Percentage %.2f", cacheKeyNotFoundCount, cacheKeyFoundCount, (float)cacheKeyFoundCount / (cacheKeyFoundCount + cacheKeyNotFoundCount));
    printf("\nFiles %d not found | %d found | Percentage %.2f\n", filesNotFoundCount, filesFoundCount, (float)filesFoundCount / (filesFoundCount + filesNotFoundCount));
}

/////////////////////////////////////////////////////////////////////////////////
// Terminal
/////////////////////////////////////////////////////////////////////////////////

void printBoard(Board *pBoard) {
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
            printf(" %c ", getPositionValue(x, y, pBoard));
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

void printPossibleMovesForPiece(PossibleMovesForPosition *pPossibleMovesForPosition) {
    char x = (*pPossibleMovesForPosition)[0];
    char y = (*pPossibleMovesForPosition)[1];
    printf("(x%d,y%d) =>", x, y);

    char i = POSITION_LENGHT; // Skip index for origin possition
    while(i < MAX_POSSIBLE_MOVES_ARRAY_LENGTH && (*pPossibleMovesForPosition)[i] != EMPTY) {
        char toX = (*pPossibleMovesForPosition)[i];
        char toY = (*pPossibleMovesForPosition)[i + 1];
        printf(" (x%d,y%d)", toX, toY);
        i += POSITION_LENGHT;
    }

    printf("\n");
}

void printPossibleMoves(PossibleMoves *pPossibleMoves) {
    printf("\nPossible moves:\n");

    for(char i = 0; i < TOTAL_PIECES_PER_COLOR; ++i)
    {
        printPossibleMovesForPiece(&((*pPossibleMoves)[i]));
    }
}

void printBoardHistory(BoardHistoryMovesForColor *pBoardHistory) {
    printf("\nBoard history:\n");
    for(char pieceIndex = 0; pieceIndex < TOTAL_PIECES_PER_COLOR; ++pieceIndex) {
        printf("\nPiece %d:\n", pieceIndex);
        for(char moveIndex = 0; moveIndex < (*pBoardHistory)[pieceIndex].moveCount; ++moveIndex) {
            printf("Move %d: %d to %d | Win count: %d | Game count: %d\n", moveIndex, (*pBoardHistory)[pieceIndex].from, (*pBoardHistory)[pieceIndex].tos[moveIndex].to, (*pBoardHistory)[pieceIndex].tos[moveIndex].win_count, (*pBoardHistory)[pieceIndex].tos[moveIndex].game_count);
        }
    }
}

void helpWithPossibleMoves(Board *pBoard, char color) {
    PossibleMoves possibleMoves;
    getPossibleMovesForColor(pBoard, color, &possibleMoves);
    printPossibleMoves(&possibleMoves);
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

int askForMove(Board *pBoard, char color) {
    char fromX = askForMoveI("from x"); 
    char fromY = askForMoveI("from y");

    if(getPositionValue(fromX, fromY, pBoard) != color) {
        printf("\n Error: Not your piece at %dx,%dy\n", fromX, fromY);
        return 0;
    }

    char toX = askForMoveI("to x"); 
    char toY = askForMoveI("to y");
    
    if(getPositionValue(toX, toY, pBoard) != EMPTY) {
        printf("\n Error: To position not empty at %dx,%dy\n", toX, toY);
        return 0;
    }

    if(!isValidMove(fromX, fromY, toX, toY, pBoard)) {
        printf("\n Error: Invalid move from %dx,%dy to %dx,%dy\n", fromX, fromY, toX, toY);
        return 0;
    }

    applyMoveToBoard(fromX, fromY, toX, toY, pBoard);
    printBoard(pBoard);
    return 1;
}

/////////////////////////////////////////////////////////////////////////////////
// Main
/////////////////////////////////////////////////////////////////////////////////

int main() {
    printf("Welcome to Jump Over Jump!\n\n");

    setupMemcached();

    char turn = WHITE;
    Board board;
    initializeBoard(&board);
    printBoard(&board);
    Move move;

    srand(time(NULL));

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
                aiVsAiForNGames();
                break;

            case 'b':
                printBoard(&board);
                break;

            case 'h':
                helpWithPossibleMoves(&board, turn);
                break;

            case 'm':
                if (askForMove(&board, turn)) {
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