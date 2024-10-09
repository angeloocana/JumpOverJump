#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h> // TODO: Do I really need this?
#include <libmemcached/memcached.h>
#include <sys/stat.h> // TODO: Do I really need this?
#include <errno.h> // TODO: Do I really need this?
#include <pcg_variants.h>

/////////////////////////////////////////////////////////////////////////////////
// Core
/////////////////////////////////////////////////////////////////////////////////

typedef enum {
    WHITE = 'O',
    BLACK = 'X',
    EMPTY = '_'
} PositionValue;

typedef enum {
    SUCCESS = 1,
    ERROR = 0
} Result;

const char BOARD_SIZE = 8;
const char TOTAL_PIECES_PER_COLOR = BOARD_SIZE;
const char TOTAL_PIECES = BOARD_SIZE + BOARD_SIZE;
const char LAST_ROW_INDEX = BOARD_SIZE - 1;

// Max possible moves
// Positions:
// 8*8 = 64    BOARD_SIZE*BOARD_SIZE
// 64/2 = 32   Piece can't change color when jumping
// 32-1 = 31   Piece can't move to current position
const char MAX_POSSIBLE_MOVES = 22;
char maxPossibleMovesFound = 0;

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


const char MAX_MOVES = 55; // It should be 45 or less to win most of humans
 
typedef Move Moves[MAX_MOVES];

typedef struct Game {
    Moves moves;
    char moveIndex;
    Board board;
    char turn;
    char winner;
} Game;

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

typedef Position ColorPositions[TOTAL_PIECES_PER_COLOR];

typedef struct PiecesPositions {
    ColorPositions whitePositions;
    ColorPositions blackPositions;

    // Yes 1, No 0
    char isGameCloseToEnd;
    char turn;
} PiecesPositions;

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
typedef char GameHash[TOTAL_PIECES + 2];

const char BOARD_HISTORY_DB_PATH[] = "/Users/angeloocanamartins/dev/JumpOverJump_db/";

// +16 chars for pieces
// +15 for folders
// +50 for BOARD_HISTORY_DB_PATH
// +1 for null terminator
typedef char BoardFileNameFullPath[100]; // Add some extra space to avoid issues

// We can only use valid characters in file names
// We will sum the position index with this value to get a valid character
const char FILE_NAME_CHAR_START = 48;

typedef struct HistoryMoveTo {
    Position to;
    unsigned int win_count;
    unsigned int game_count;
} HistoryMoveTo;

typedef struct HistoryMovesForPiece {
    // Number of actual possible moves in the "tos" array
    char moveCount;
    Position from;
    HistoryMoveTo tos[MAX_POSSIBLE_MOVES];
} HistoryMovesForPiece;

typedef HistoryMovesForPiece HistoryMovesForColor[TOTAL_PIECES_PER_COLOR];

typedef struct LocalCacheNode {
    struct LocalCacheNode *pNext[MAX_POSITIONS];

    // Only leaf nodes will have history moves for color
    HistoryMovesForColor *pHistoryMovesForWhite;
    HistoryMovesForColor *pHistoryMovesForBlack;
} LocalCacheNode;

const int AI_VS_AI_MAX_GAMES = 100;
const char MAX_EXPLORE_DEPTH = 5;
const char MAX_EXPLORE_NEXT_POSITIONS = 2;

const unsigned int MAX_SCORE = 100000000;
const unsigned int MAX_SCORE_WIN_RATIO_MULTIPLIER = 1000;

// ~ pow((MAX_SCORE / TOTAL_PIECES_PER_COLOR), 1 / (LAST_ROW_INDEX * LAST_ROW_INDEX))
const float N_ADVANCED_ROWS_SCORE_POW = 1.3958;

typedef unsigned int Score;

typedef struct MoveScore {
    Move move;
    Score score;
} MoveScore;

// Core functions

void copyMove(Move *pSource, Move *pDestination);
void setPosition(char x, char y, char value, Board *pBoard);
void setCoordinates(Coordinates *pCoordinates, char value, Board *pBoard);
char getPositionValue(char x, char y, const Board *pBoard);
char getCoordinatesValue(Coordinates *pCoordinates, const Board *pBoard);
char getInitialPositionValue(char y);
void initializeBoard(Board *pBoard);
void initializeGame(Game *pGame);
char getWinner(const Board *pBoard);
Result addPositionToPossibleMoves(char x, char y, PossibleMovesForPosition *pPossibleMovesForPosition);
int isValidIndex(char i);
int isValidPosition(char x, char y);
void addJumpsToPossibleMoves(char x, char addX, char y, char addY, const Board *pBoard, PossibleMovesForPosition *pPossibleMovesForPosition);
void addPositionToPossibleMovesIfValid(char fromX, char addX, char fromY, char addY, const Board *pBoard, PossibleMovesForPosition *pPossibleMovesForPosition);
void getPossibleMovesForPosition(const Board *pBoard, PossibleMovesForPosition *pPossibleMovesForPosition);
void initializePossibleMovesForPosition(char x, char y, PossibleMovesForPosition *pPossibleMovesForPosition);
void getPossibleMovesForColor(const Board *pBoard, char color, PossibleMoves *pPossibleMoves);
Result isValidCoordinatesMove(char fromX, char fromY, char toX, char toY, const Board *pBoard);
Result isValidMove(const Move *pMove, const Board *pBoard);
void applyMoveCoordinatesToBoard(char fromX, char fromY, char toX, char toY, Board *pBoard);
void applyMoveToBoard(Move *pMove, Board *pBoard);
void applyMoveToGame(Move *pMove, Game *pGame);
Position getPositionFromXY(char x, char y);
Position getPositionFromCoordinates(Coordinates *pCoordinates);
void getCoordinatesFromPosition(Position *pPosition, Coordinates *pCoordinates);
void getPiecesPositions(Game *pGame, PiecesPositions *pPiecesPositions);
void getGameHash(PiecesPositions *pPiecesPositions, GameHash *pHash);
void initializeHistoryMoveTo(Position to, HistoryMoveTo *pHistoryMoveTo);
void initializeHistoryMoveForPiece(PossibleMovesForPosition *pPossibleMovesForPosition, HistoryMovesForPiece *pHistoryMovesForPiece);
void initializeHistoryMovesForColor(Game *pGame, HistoryMovesForColor *pHistory);

// Local cache

unsigned int writeHistoryToLocalCacheCount = 0;
unsigned int localCacheKeyNotFoundCount = 0;
unsigned int localCacheKeyFoundCount = 0;
unsigned int allocatedLocalCacheNodeCount = 0;
LocalCacheNode *pLocalCacheHead = NULL;
LocalCacheNode *initializeLocalCacheNode();
LocalCacheNode *getTailLocalCacheNode(LocalCacheNode *pHeadNode, ColorPositions *pPositions, char initializeIfNotFound);
void freeLocalCacheNode(LocalCacheNode *pLocalCacheNode);
Result getHistoryFromLocalCache(PiecesPositions *pPiecesPositions, HistoryMovesForColor *pHistory);
void writeHistoryToLocalCache(PiecesPositions *pPiecesPositions, HistoryMovesForColor *pHistoryMovesForColor);

// Memcached

unsigned int memcachedKeyNotFoundCount = 0;
unsigned int memcachedKeyFoundCount = 0;

memcached_return rc;
memcached_st *memc;

void setupMemcached();
Result writeHistoryToMemcached(char *key, HistoryMovesForColor *pHistory);
Result getHistoryFromMemcached(char *key, HistoryMovesForColor *pHistory);

// Disk

unsigned int filesNotFoundCount = 0;
unsigned int filesFoundCount = 0;

const char pathSeparator =
#ifdef _WIN32
  '\\';
#else
  '/';
#endif

void createFilePathDirs(char *file_path);
void getBoardFileNameFullPath(GameHash *hash, BoardFileNameFullPath *fileNameFullPath);
void getHistoryMovesForPieceFromDisk(HistoryMovesForPiece *historyMovesForPiece, FILE *pFile);
Result getHistoryFromDisk(GameHash *hash, HistoryMovesForColor *pHistory);
void writeHistoryMovesForPieceToDisk(HistoryMovesForPiece *pHistoryMovesForPiece, FILE *pFile);
void writeHistoryToDisk(GameHash *hash, HistoryMovesForColor *pHistory);

void getHistory(Game *pGame, HistoryMovesForColor *pHistory);
void writeHistory(Game *pGame, HistoryMovesForColor *pHistory);

void copyBoard(Board *pSource, Board *pDestination);
static inline char getNextColor(char color);

// AI functions

char isMovingForward(char fromX, char fromY, char toX, char toY, char color);
char getNAdvancedRows(char color, char y);
void exploreNextMovesForPosition(MoveHistory *pCurrent, PossibleMovesForPosition *pPossibleMovesForPosition, int depth, char nextMovesCount, char nextColor);
char exploreNextMovesForColor(MoveHistory *pCurrent, int depth);
void initializeMoveScore(Board *pBoard, char color, Position *pFrom, HistoryMoveTo *pHistoryMoveTo, MoveScore *pMoveScore, Score previousAdvancedRowsScore);
Score generateRandomScore(Score maxScore);
void guessBestMove(Game *pGame, Move *pMove);
void backPropagateHistory(Game *pGame);
Result aiVsAi();
void aiVsAiForNGames();

// Print functions

void printGame(Game *pGame);
void printBoard(const Board *pBoard);
void printMoves(Game *pGame);
void printMove(Move *pMove);
void printPossibleMovesForPiece(PossibleMovesForPosition *pPossibleMovesForPosition);
void printPossibleMoves(PossibleMoves *pPossibleMoves);
void printHistory(HistoryMovesForColor *pHistory);

// User interactive functions

void helpWithPossibleMoves(Game *pGame);
char askForMoveI(char i[]);
Result askForMove(Game *pGame);

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
char getPositionValue(char x, char y, const Board *pBoard) {
    return (*pBoard)[x][y];
}

char getCoordinatesValue(Coordinates *pCoordinates, const Board *pBoard) {
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

void initializeGame(Game *pGame) {
    initializeBoard(&pGame->board);
    pGame->moveIndex = 0;
    pGame->winner = EMPTY;
    pGame->turn = WHITE;
}

char getWinner(const Board *pBoard) {
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
    // printf("\naddPositionToPossibleMoves (%d,%d)\n", x, y);
    char i = POSITION_LENGHT; // skip origin position x,y

    while ((*pPossibleMovesForPosition)[i] != EMPTY) {
        if(
            x == (*pPossibleMovesForPosition)[i]
            && y == (*pPossibleMovesForPosition)[i + 1]
        ) {
            // printf("\naddPositionToPossibleMoves position already added\n");
            return ERROR; // Aborting position already added
        }

        i += POSITION_LENGHT;

        if(i >= MAX_POSSIBLE_MOVES_ARRAY_LENGTH) {
            // TODO: Add count logging to identify how often this happens
            printf("\nAborting add position (%d,%d) to possible moves, array is full\n", x, y);
            printPossibleMovesForPiece(pPossibleMovesForPosition);
            abort();
            return ERROR; // Aborting array is full
        }
    }

    (*pPossibleMovesForPosition)[i] = x;
    (*pPossibleMovesForPosition)[i + 1] = y;

    char nPossibleMoves = (i - 1) / 2;
    if(nPossibleMoves > maxPossibleMovesFound) {
        maxPossibleMovesFound = nPossibleMoves;
    }

    // printf("\naddPositionToPossibleMoves position added\n");
    return SUCCESS;
}

int isValidIndex(char i) {
    return i >= 0 && i < BOARD_SIZE ? 1 : 0;
}

int isValidPosition(char x, char y) {
    return isValidIndex(x) && isValidIndex(y) ? 1 : 0;
}

void addJumpsToPossibleMoves(char fromX, char addX, char fromY, char addY, const Board *pBoard, PossibleMovesForPosition *pPossibleMovesForPosition) {
    // printf("\naddJumpsToPossibleMoves from (%d,%d) add (%d,%d)\n", fromX, fromY, addX, addY);
    char overX = fromX + addX;
    char overY = fromY + addY;
    
    if(!isValidPosition(overX, overY) || getPositionValue(overX, overY, pBoard) == EMPTY) {
        // printf("\naddJumpsToPossibleMoves over (%d,%d) is not valid or empty\n", overX, overY);
        return;
    }

    char toX = overX + addX;
    char toY = overY + addY;
    if(!isValidPosition(toX, toY) || getPositionValue(toX, toY, pBoard) != EMPTY) {
        // printf("\naddJumpsToPossibleMoves to (%d,%d) is not valid or not empty\n", toX, toY);
        return;
    }

    if(addPositionToPossibleMoves(toX, toY, pPossibleMovesForPosition)){
        for(char i = 0; i < MAX_NEXT_POSITIONS; i++) {
            char toAddX = addForNexPositions[i][0];
            char toAddY = addForNexPositions[i][1];
            addJumpsToPossibleMoves(toX, toAddX, toY, toAddY, pBoard, pPossibleMovesForPosition);
        }
    }
}

void addPositionToPossibleMovesIfValid(char fromX, char addX, char fromY, char addY, const Board *pBoard, PossibleMovesForPosition *pPossibleMovesForPosition) {
    // printf("\naddPositionToPossibleMovesIfValid from (%d,%d) add (%d,%d)\n", fromX, fromY, addX, addY);
    char x = fromX + addX;
    char y = fromY + addY;

    if (!isValidPosition(x, y)) {
        // printf("\naddPositionToPossibleMovesIfValid position is not valid\n");
        return;
    }

    if(getPositionValue(x, y, pBoard) == EMPTY) {
        // printf("\naddPositionToPossibleMovesIfValid position is empty (%d,%d)\n", x, y);
        addPositionToPossibleMoves(x, y, pPossibleMovesForPosition);
    } else {
        // printf("\naddPositionToPossibleMovesIfValid position is not empty (%d,%d)\n", x, y);
        addJumpsToPossibleMoves(fromX, addX, fromY, addY, pBoard, pPossibleMovesForPosition);
    }
}

// Example:
// possibleMovesForPosition = [0, 0, 0, 1, 1, 1] 
// (x:0,y:0) can go to (x:0,y:1) (x:1,y:1) 
void getPossibleMovesForPosition(const Board *pBoard, PossibleMovesForPosition *pPossibleMovesForPosition) {
    // printf("\ngetPossibleMovesForPosition\n");
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

void getPossibleMovesForColor(const Board *pBoard, char color, PossibleMoves *pPossibleMoves) {
    // printf("\ngetPossibleMovesForColor %c\n", color);
    // printBoard(pBoard);
    char pieceCount = 0;
    for(char x = 0; x < BOARD_SIZE; ++x)
    {
        for(char y = 0; y < BOARD_SIZE; ++y)
        {
            if (getPositionValue(x, y, pBoard) == color) {
                // printf("\n-------------------\n");
                // printf("\ngetPossibleMovesForColor position (%d,%d) pieceCount %d\n", x, y, pieceCount);
                initializePossibleMovesForPosition(x, y, &((*pPossibleMoves)[pieceCount]));
                getPossibleMovesForPosition(pBoard, &((*pPossibleMoves)[pieceCount]));
                // printf("\n-------------------\n\n");
                pieceCount++;
            }
        }
    }
}

Result isValidCoordinatesMove(char fromX, char fromY, char toX, char toY, const Board *pBoard) {
    PossibleMovesForPosition possibleMovesForPosition;
    initializePossibleMovesForPosition(fromX, fromY, &possibleMovesForPosition);
    getPossibleMovesForPosition(pBoard, &possibleMovesForPosition);

    printPossibleMovesForPiece(&possibleMovesForPosition);
    char i = POSITION_LENGHT; // Skip index for origin possition
    while(i < MAX_POSSIBLE_MOVES_ARRAY_LENGTH && possibleMovesForPosition[i] != EMPTY) {
        char x = possibleMovesForPosition[i];
        char y = possibleMovesForPosition[i + 1];
        
        if(x == toX && y == toY) {
            return SUCCESS;
        }

        i += POSITION_LENGHT;
    }

    return ERROR;
}

Result isValidMove(const Move *pMove, const Board *pBoard) {
    return isValidCoordinatesMove(pMove->from.x, pMove->from.y, pMove->to.x, pMove->to.y, pBoard);
}

// TODO: Remove later
void applyMoveCoordinatesToBoard(char fromX, char fromY, char toX, char toY, Board *pBoard) {
    setPosition(toX, toY, getPositionValue(fromX, fromY, pBoard), pBoard);
    setPosition(fromX, fromY, EMPTY, pBoard);
}

void applyMoveToBoard(Move *pMove, Board *pBoard) {
    setCoordinates(&(pMove->to), getCoordinatesValue(&(pMove->from), pBoard), pBoard);
    setCoordinates(&(pMove->from), EMPTY, pBoard);
}

void applyMoveToGame(Move *pMove, Game *pGame) {
    applyMoveToBoard(pMove, &pGame->board);
    pGame->moves[pGame->moveIndex] = *pMove;
    pGame->moveIndex++;
    pGame->turn = getNextColor(pGame->turn);
    pGame->winner = getWinner(&pGame->board);
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

void getPiecesPositions(Game *pGame, PiecesPositions *pPiecesPositions) {
    char positionIndex = 0;
    char whiteCount = 0;
    char blackCount = 0;
    char lastWhiteY = LAST_ROW_INDEX;
    char lastBlackY = 0;
    pPiecesPositions->turn = pGame->turn;
    
    for(char x = 0; x < BOARD_SIZE; ++x)
    {
        for(char y = 0; y < BOARD_SIZE; ++y)
        {
            char piece = getPositionValue(x, y, &(pGame->board));
            switch(piece) {
                case WHITE:
                    pPiecesPositions->whitePositions[whiteCount] = positionIndex;
                    whiteCount++;
                    if(y < lastWhiteY) {
                        lastWhiteY = y;
                    }
                    break;
                case BLACK:
                    pPiecesPositions->blackPositions[blackCount] = positionIndex;
                    blackCount++;
                    if(y > lastBlackY) {
                        lastBlackY = y;
                    }
                    break;
            }
            positionIndex++;
        }
    }

    pPiecesPositions->isGameCloseToEnd = lastWhiteY - lastBlackY > 2 ? 1 : 0;
}

void getGameHash(PiecesPositions *pPiecesPositions, GameHash *pHash) {
    if(pPiecesPositions->isGameCloseToEnd) {
        // different colored pieces can't interact with each other anymore so we don't care about the other player pieces.
        for(char i = 0; i < TOTAL_PIECES_PER_COLOR; i++) {
            (*pHash)[i] = (pPiecesPositions->turn == BLACK ? pPiecesPositions->blackPositions[i] : pPiecesPositions->whitePositions[i]) + FILE_NAME_CHAR_START;
        }

        (*pHash)[TOTAL_PIECES_PER_COLOR] = pPiecesPositions->turn;
        (*pHash)[TOTAL_PIECES_PER_COLOR + 1] = '\0';
    } else {
        for(char i = 0; i < TOTAL_PIECES_PER_COLOR; i++) {
            (*pHash)[i] = pPiecesPositions->whitePositions[i] + FILE_NAME_CHAR_START;
            (*pHash)[i + TOTAL_PIECES_PER_COLOR] = pPiecesPositions->blackPositions[i] + FILE_NAME_CHAR_START;
        }

        char colorIndex = TOTAL_PIECES_PER_COLOR * 2;
        (*pHash)[colorIndex] = pPiecesPositions->turn;
        (*pHash)[colorIndex + 1] = '\0';
    }
}

void initializeHistoryMoveTo(Position to, HistoryMoveTo *pHistoryMoveTo) {
    pHistoryMoveTo->to = to;
    pHistoryMoveTo->win_count = 0;
    pHistoryMoveTo->game_count = 0;
}

void initializeHistoryMoveForPiece(PossibleMovesForPosition *pPossibleMovesForPosition, HistoryMovesForPiece *pHistoryMovesForPiece) {
    // printf("\nInitializing board history move for piece\n");
    // printPossibleMovesForPiece(pPossibleMovesForPosition);
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

        initializeHistoryMoveTo(
            getPositionFromXY(toX, toY),
            &(pHistoryMovesForPiece->tos[pHistoryMovesForPiece->moveCount])
        );

        // printf("\tBoard history move: from %d to %d\n", pHistoryMovesForPiece->from, pHistoryMovesForPiece->tos[pHistoryMovesForPiece->moveCount].to);
        pHistoryMovesForPiece->moveCount++;
    }
}

void initializeHistoryMovesForColor(Game *pGame, HistoryMovesForColor *pHistory) {
    // printf("\nInitializing board history moves for color %c\n", color);

    PossibleMoves possibleMoves;
    getPossibleMovesForColor(&(pGame->board), pGame->turn, &possibleMoves);
    // printGame(pGame);

    for(char pieceIndex = 0; pieceIndex < TOTAL_PIECES_PER_COLOR; ++pieceIndex)
    {
        initializeHistoryMoveForPiece(&possibleMoves[pieceIndex], &((*pHistory)[pieceIndex]));

        // printf("\nMove count: %d\n", (*pHistory)[pieceIndex].moveCount);
        // printf("\n -------------------\n");
    }
    // printf("\nBoard history moves for color initialized\n");
}

/////////////////////////////////////////////////////////////////////////////////
// Local cache
/////////////////////////////////////////////////////////////////////////////////

LocalCacheNode *initializeLocalCacheNode() {
    LocalCacheNode *pLocalCacheNode = (LocalCacheNode*)malloc(sizeof(LocalCacheNode));
    if (pLocalCacheNode == NULL) {
        fprintf(stderr, "Memory allocation failed for LocalCacheNode\n");
        abort();
    }
    allocatedLocalCacheNodeCount++;
            
    // printf("\nInitializing local cache node allocatedLocalCacheNodeCount: %d\n", allocatedLocalCacheNodeCount);
    for(int i = 0; i < MAX_POSITIONS; i++) {
        pLocalCacheNode->pNext[i] = NULL;
    }

    pLocalCacheNode->pHistoryMovesForWhite = NULL;
    pLocalCacheNode->pHistoryMovesForBlack = NULL;

    // printf("\nLocal cache node initialized allocatedLocalCacheNodeCount: %d\n", allocatedLocalCacheNodeCount);
    return pLocalCacheNode;
}

LocalCacheNode *getTailLocalCacheNode(LocalCacheNode *pHeadNode, ColorPositions *pPositions, char initializeIfNotFound) {
    LocalCacheNode *pNode = pHeadNode;

    if(pNode == NULL) {
        return NULL;
    }

    for(int i = 0; i < TOTAL_PIECES_PER_COLOR; i++) {
        if(pNode->pNext[(*pPositions)[i]] == NULL) {
            if(initializeIfNotFound) {
                // printf("\nInitializing local cache node\n");
                pNode->pNext[(*pPositions)[i]] = initializeLocalCacheNode();
            } else {
                // printf("\nLocal cache node not found\n");
                return NULL;
            }
        }

        // printf("\nGetting tail local cache node pNode %p\n", pNode);
        pNode = pNode->pNext[(*pPositions)[i]];
        // printf("\nGot tail local cache node i %d pNode %p\n", i, pNode);
    }

    return pNode;
}

void freeLocalCacheNode(LocalCacheNode *pLocalCacheNode) {
    if (pLocalCacheNode == NULL) {
        return;
    }

    for (int i = 0; i < MAX_POSITIONS; i++) {
        freeLocalCacheNode(pLocalCacheNode->pNext[i]);
    }

    free(pLocalCacheNode->pHistoryMovesForWhite);
    free(pLocalCacheNode->pHistoryMovesForBlack);
    free(pLocalCacheNode);
}

Result getHistoryFromLocalCache(PiecesPositions *pPiecesPositions, HistoryMovesForColor *pHistory) {
    // printf("\nGetting board history from local cache\n");
    LocalCacheNode *pNode = pLocalCacheHead;

    if(pPiecesPositions->isGameCloseToEnd) {
        ColorPositions *positions = pPiecesPositions->turn == WHITE ? &(pPiecesPositions->whitePositions) : &(pPiecesPositions->blackPositions);
        // Pieces can't interfere with each other anymore, so we only care about the current turn pieces
        pNode = getTailLocalCacheNode(pNode, positions, 0);
    } else {
        pNode = getTailLocalCacheNode(pNode, &(pPiecesPositions->whitePositions), 0);
        pNode = getTailLocalCacheNode(pNode, &(pPiecesPositions->blackPositions), 0);
    }

    if(pNode == NULL) {
        localCacheKeyNotFoundCount++;
        // printf("\nBoard history not found in local cache\n");
        return ERROR;
    }

    // printf("\nCopying board history from local cache\n");
    if(pPiecesPositions->turn == WHITE) {
        if(pNode->pHistoryMovesForWhite == NULL) {
            // printf("\nLocal cache node pHistoryMovesForWhite is NULL\n");
            return ERROR;
        }

        // printf("\nCopying board history for white from local cache %p\n", pNode->pHistoryMovesForWhite);
        memcpy(pHistory, pNode->pHistoryMovesForWhite, sizeof(HistoryMovesForColor));
    } else {
        if(pNode->pHistoryMovesForBlack == NULL) {
            // printf("\nLocal cache node pHistoryMovesForBlack is NULL\n");
            return ERROR;
        }

        // printf("\nCopying board history for black from local cache %p\n", pNode->pHistoryMovesForBlack);
        memcpy(pHistory, pNode->pHistoryMovesForBlack, sizeof(HistoryMovesForColor));
    }
    localCacheKeyFoundCount++;
    // printf("\nBoard history found in local cache\n");
    return SUCCESS;
}

void writeHistoryToLocalCache(PiecesPositions *pPiecesPositions, HistoryMovesForColor *pHistoryMovesForColor) {
    // printf("\nWriting board history to local cache\n");
    LocalCacheNode *pNode = pLocalCacheHead;

    if(pPiecesPositions->isGameCloseToEnd) {
        ColorPositions *positions = pPiecesPositions->turn == WHITE ? &(pPiecesPositions->whitePositions) : &(pPiecesPositions->blackPositions);
        pNode = getTailLocalCacheNode(pNode, positions, 1);
    } else {
        pNode = getTailLocalCacheNode(pNode, &(pPiecesPositions->whitePositions), 1);
        pNode = getTailLocalCacheNode(pNode, &(pPiecesPositions->blackPositions), 1);
    }

    writeHistoryToLocalCacheCount++;
    if(pPiecesPositions->turn == WHITE) {
        // printHistory(pHistoryMovesForColor);
        // printf("\nwriteHistoryToLocalCacheCount %d\n", writeHistoryToLocalCacheCount);
        // printf("\nmemcpy WHITE pNode->pHistoryMovesForWhite\n");
        if(pNode->pHistoryMovesForWhite == NULL) {
            // printf("\nmalloc pHistoryMovesForWhite 22222\n");
            pNode->pHistoryMovesForWhite = malloc(sizeof(HistoryMovesForColor));
        }
        memcpy(pNode->pHistoryMovesForWhite, (HistoryMovesForColor*)pHistoryMovesForColor, sizeof(HistoryMovesForColor));
        // printf("\nmemcpyED WHITE pNode->pHistoryMovesForWhite\n");
        // printHistory(pNode->pHistoryMovesForWhite);
    } else {
        // printf("\nmemcpy BLACK pNode->pHistoryMovesForBlack\n");
        // printf("\nwriteHistoryToLocalCacheCount %d\n", writeHistoryToLocalCacheCount);
        if(pNode->pHistoryMovesForBlack == NULL) {
            // printf("\nmalloc pHistoryMovesForBlack 22222\n");
            pNode->pHistoryMovesForBlack = malloc(sizeof(HistoryMovesForColor));
        }
        memcpy(pNode->pHistoryMovesForBlack, (HistoryMovesForColor*)pHistoryMovesForColor, sizeof(HistoryMovesForColor));
        // printf("\nmemcpyED BLACK pNode->pHistoryMovesForBlack\n");
        // printHistory(pNode->pHistoryMovesForBlack);
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

void freeMemcached() {
    memcached_free(memc);
}

Result writeHistoryToMemcached(char *key, HistoryMovesForColor *pHistory) {
    // printf("\nWriting board history to cache\n");
    rc = memcached_set(memc, key, strlen(key), (const char*)pHistory, sizeof(HistoryMovesForColor), (time_t)0, MEMCACHED_BEHAVIOR_BINARY_PROTOCOL);

    if (rc == MEMCACHED_SUCCESS) {
        // fprintf(stderr, "Key stored successfully\n");
        return SUCCESS;
    }
    
    // fprintf(stderr, "Couldn't store key: %s\n", memcached_strerror(memc, rc));
    return ERROR;
}

Result getHistoryFromMemcached(char *key, HistoryMovesForColor *pHistory) {
    // printf("\nGetting board history from cache\n");
    size_t value_length;
    uint32_t flags;

    char* retrieved_value = memcached_get(memc, key, strlen(key), &value_length, &flags, &rc);

    if (rc == MEMCACHED_SUCCESS) {
        // fprintf(stderr, "Key retrieved successfully\n");
        // printf("The key '%s' returned value '%s'.\n", key, pHistory);
        // TODO: Write function to print HistoryMovesForColor
        memcpy(pHistory, (HistoryMovesForColor*)retrieved_value, sizeof(HistoryMovesForColor));
        free(retrieved_value);
        memcachedKeyFoundCount++;
        // printHistory(pHistory);
        return SUCCESS;
    }
    
    memcachedKeyNotFoundCount++;
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

void getBoardFileNameFullPath(GameHash *hash, BoardFileNameFullPath *fileNameFullPath) {
    // printf("\nGetting board file name full path\n");
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
    
    // printf("\nfileNameFullPath: %s\n", *fileNameFullPath);
}

void getHistoryMovesForPieceFromDisk(HistoryMovesForPiece *historyMovesForPiece, FILE *pFile) {
    historyMovesForPiece->moveCount = fgetc(pFile);
    // printf("\nRead moveCount %d\n", historyMovesForPiece->moveCount);

    historyMovesForPiece->from = fgetc(pFile);
    // printf("\nRead historyMovesForPiece->from %d\n", historyMovesForPiece->from);
    
    fread(historyMovesForPiece->tos, sizeof(HistoryMoveTo), historyMovesForPiece->moveCount, pFile);
    // printf("\nRead historyMovesForPiece->tos\n");
}

Result getHistoryFromDisk(GameHash *hash, HistoryMovesForColor *pHistory) {
    // printf("\nGetting board history from disk\n");
    BoardFileNameFullPath fileNameFullPath;
    getBoardFileNameFullPath(hash, &fileNameFullPath);

    FILE *pFile = fopen(fileNameFullPath, "r");
    if (pFile == NULL) {
        // fprintf(stderr, "Error opening file %s: %s\n", fileNameFullPath, strerror(errno));
        filesNotFoundCount++;
        return ERROR;
    }
    filesFoundCount++;
    // printf("\nReading board history from file\n");

    for(char pieceIndex = 0; pieceIndex < TOTAL_PIECES_PER_COLOR; ++pieceIndex)
    {
        // printf("\nReading pieceIndex %d\n", pieceIndex);
        getHistoryMovesForPieceFromDisk(&((*pHistory)[pieceIndex]), pFile);
    }

    // printf("\nBoard history read from file\n");
    fclose(pFile);
    // printf("\nBoard history file closed\n");
    return SUCCESS;
}

void writeHistoryMovesForPieceToDisk(HistoryMovesForPiece *pHistoryMovesForPiece, FILE *pFile) {
    // printf("\nWriting moveCount %d\n", pHistoryMovesForPiece->moveCount);
    fputc(pHistoryMovesForPiece->moveCount, pFile);

    // printf("\nWriting pHistoryMovesForPiece->from %d\n", pHistoryMovesForPiece->from);
    fputc(pHistoryMovesForPiece->from, pFile);

    // printf("\nWriting pHistoryMovesForPiece->tos\n");
    fwrite(pHistoryMovesForPiece->tos, sizeof(HistoryMoveTo), pHistoryMovesForPiece->moveCount, pFile);
}

void writeHistoryToDisk(GameHash *hash, HistoryMovesForColor *pHistory) {
    // printf("\nWriting board history to disk\n");
    BoardFileNameFullPath fileNameFullPath;
    getBoardFileNameFullPath(hash, &fileNameFullPath);

    createFilePathDirs(fileNameFullPath);

    FILE *pFile = fopen(fileNameFullPath, "w");
    if (pFile == NULL) {
        printf("Error opening board history file for write!\n");
        return;
    }
    // printf("\nWriting board history to disk\n");

    for(char pieceIndex = 0; pieceIndex < TOTAL_PIECES_PER_COLOR; ++pieceIndex)
    {
        // printf("\nWriting pieceIndex %d\n", pieceIndex);
        writeHistoryMovesForPieceToDisk(&((*pHistory)[pieceIndex]), pFile);
    }

    // printf("\nBoard history written to file\n");
    fclose(pFile);
    // printf("\nBoard history file closed\n");
}

void getHistory(Game *pGame, HistoryMovesForColor *pHistory) {
    // printf("\nGetting board history\n");
    PiecesPositions piecesPositions;
    getPiecesPositions(pGame, &piecesPositions);

    Result result = getHistoryFromLocalCache(&piecesPositions, pHistory);
    if(result == SUCCESS) {
        return;
    }

    GameHash hash;
    getGameHash(&piecesPositions, &hash);

    if(getHistoryFromMemcached(&(hash[0]), pHistory) == SUCCESS) {
        return;
    }

    if(getHistoryFromDisk(&hash, pHistory) == ERROR) {
        initializeHistoryMovesForColor(pGame, pHistory);
    }
}

void writeHistory(Game *pGame, HistoryMovesForColor *pHistory) {
    // printf("\nWriting board history\n");
    PiecesPositions piecesPositions;
    getPiecesPositions(pGame, &piecesPositions);

    writeHistoryToLocalCache(&piecesPositions, pHistory);

    GameHash hash;
    getGameHash(&piecesPositions, &hash);

    writeHistoryToMemcached(&(hash[0]), pHistory);
    writeHistoryToDisk(&hash, pHistory);
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
            // printf("\nNot moving forward from %dx,%dy to %dx,%dy\n", x, y, toX, toY);
            continue;
        }

        Board boardAfterMove;
        copyBoard(pCurrent->pBoardAfterMove, &boardAfterMove);
        applyMoveCoordinatesToBoard(x, y, toX, toY, &boardAfterMove);

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

void initializeMoveScore(Board *pBoard, char color, Position *pFrom, HistoryMoveTo *pHistoryMoveTo, MoveScore *pMoveScore, Score previousAdvancedRowsScore) {
    getCoordinatesFromPosition(pFrom, &(pMoveScore->move.from));

    getCoordinatesFromPosition(&(pHistoryMoveTo->to), &(pMoveScore->move.to));

    Board boardAfterMove;
    copyBoard(pBoard, &boardAfterMove);
    applyMoveToBoard(&(pMoveScore->move), &boardAfterMove);

    char winner = getWinner(&boardAfterMove);
    if(winner == color) {
        pMoveScore->score = MAX_SCORE;
    } else {
        // TODO: Proper calculate MCTS score
        float winRatio = 0;
        if(pHistoryMoveTo->game_count > 0) {
            winRatio = (float)pHistoryMoveTo->win_count / pHistoryMoveTo->game_count;
        }
        // printMove(&(pMoveScore->move));
        Score advancedRowsScore;
        getAdvancedRowsScore(&boardAfterMove, color, &advancedRowsScore);

        Score diffAdvancedRowsScore = previousAdvancedRowsScore > advancedRowsScore ? 0 : advancedRowsScore - previousAdvancedRowsScore;
        Score score = (winRatio*MAX_SCORE_WIN_RATIO_MULTIPLIER) + diffAdvancedRowsScore;
        pMoveScore->score = score > MAX_SCORE ? MAX_SCORE : score;
        // printf("Score: %d | ", pMoveScore->score);
        // printf("Win ratio: %f | ", winRatio);
        // printf("Games: %d | ", pHistoryMoveTo->game_count);
        // printf("Wins: %d | ", pHistoryMoveTo->win_count);
        // printf("Diff: %d | ", diffAdvancedRowsScore);
        // printf("N advanced rows score: %d | ", advancedRowsScore);
        // printf("Previous advanced rows score: %d \n", previousAdvancedRowsScore);
    }
}

// Declare a global PCG random number generator
static pcg32_random_t rng;

// Initialize the RNG (call this once at the start of your program)
void initRNG() {
    pcg32_srandom_r(&rng, time(NULL), (intptr_t)&rng);
}

Score generateRandomScore(Score max_score) {
    return pcg32_boundedrand_r(&rng, max_score + 1);
}

void guessBestMove(Game *pGame, Move *pMove) {
    // printf("\nGuessing best move for color %c\n", color);
    HistoryMovesForColor boardHistory;
    getHistory(pGame, &boardHistory);
    // printf("\nguessBestMove Board history:\n");
    // printHistory(&boardHistory);

    char movesCount = 0;
    Score totalScore = 0;
    MoveScore movesScore[TOTAL_PIECES_PER_COLOR];

    Score advancedRowsScore;
    getAdvancedRowsScore(&(pGame->board), pGame->turn, &advancedRowsScore);
    // printf("\nAdvanced rows score: %d\n", advancedRowsScore);

    for(char pieceIndex = 0; pieceIndex < TOTAL_PIECES_PER_COLOR; ++pieceIndex)
    {
        for(char moveIndex = 0; moveIndex < boardHistory[pieceIndex].moveCount; ++moveIndex){
            initializeMoveScore(
                &(pGame->board),
                pGame->turn,
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
    // printf("\nTotal score: %d |", totalScore);
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

void backPropagateHistory(Game *pGame) {
    // printf("\nBack propagating board history\n");
    Game gameCopy;
    initializeGame(&gameCopy);
   
    for(char i = 0; i <= pGame->moveIndex; ++i) {
        // printf("\nBack propagating move %d\n", i);
        // printGame(pGame);
        HistoryMovesForColor boardHistory;
        getHistory(&gameCopy, &boardHistory);
        // printf("\nBack propagating board history:\n");
        // printHistory(&boardHistory);

        Position from = getPositionFromCoordinates(&((pGame->moves)[i].from));
        Position to = getPositionFromCoordinates(&((pGame->moves)[i].to));

        // printMove(&((*pGame)->moves[i]));
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
                if(pGame->winner == gameCopy.turn) {
                    boardHistory[pieceIndex].tos[moveIndex].win_count++;
                    // printf("\nIncrement win count moveIndex %d pieceIndex %d Win count: %d\n", moveIndex, pieceIndex, boardHistory[pieceIndex].tos[moveIndex].win_count);
                }
                break;
            }
            break;
        }

        writeHistory(&gameCopy, &boardHistory);
        applyMoveToGame(&((pGame->moves)[i]), &gameCopy);
    }

    // printf("\nBoard history back propagated\n");
}

// Success means a winner was found
Result aiVsAi() {
    Game game;
    initializeGame(&game);

    while(game.winner == EMPTY && game.moveIndex < MAX_MOVES) {
        Move move;
        guessBestMove(&game, &move);
        applyMoveToGame(&move, &game);
    }

    printGame(&game);
    backPropagateHistory(&game);
    
    return game.winner != EMPTY ? SUCCESS : ERROR;
}

void aiVsAiForNGames() {
    memcachedKeyNotFoundCount = 0;
    memcachedKeyFoundCount = 0;
    filesNotFoundCount = 0;
    filesFoundCount = 0;

    int winnerCount = 0;
    for(int i = 0; i < AI_VS_AI_MAX_GAMES; ++i) {
        if(aiVsAi()) {
            winnerCount++;
        }
        printf("\nMax possible moves found: %d\n", maxPossibleMovesFound);
        printf("\nGame %d: | Winner Count: %d | Winner ratio: %f", i, winnerCount, (float)winnerCount / (i + 1));

        // struct timespec remaining, request = { 0, 10000000 }; 
        // nanosleep(&request, &remaining);
        // sleep(1);
    }

    printf("\nAllocated local cache nodes: %d\n", allocatedLocalCacheNodeCount);
    printf("\nLocal cache keys %d not found | %d found | Percentage %.2f", localCacheKeyNotFoundCount, localCacheKeyFoundCount, (float)localCacheKeyFoundCount / (localCacheKeyFoundCount + localCacheKeyNotFoundCount));
    printf("\nMemcached keys %d not found | %d found | Percentage %.2f", memcachedKeyNotFoundCount, memcachedKeyFoundCount, (float)memcachedKeyFoundCount / (memcachedKeyFoundCount + memcachedKeyNotFoundCount));
    printf("\nFiles %d not found | %d found | Percentage %.2f\n", filesNotFoundCount, filesFoundCount, (float)filesFoundCount / (filesFoundCount + filesNotFoundCount));
}

/////////////////////////////////////////////////////////////////////////////////
// Terminal
/////////////////////////////////////////////////////////////////////////////////

void printGame(Game *pGame) {
    PiecesPositions piecesPositions;
    getPiecesPositions(pGame, &piecesPositions);
    
    GameHash hash;
    getGameHash(&piecesPositions, &hash);
    
    printf("\n-----------------------------------\n");
    printf("Hash: %s | ", hash);
    printf("Turn: %c | ", pGame->turn);
    printf("Move index: %d | ", pGame->moveIndex);
    printf("Winner: %c \n", pGame->winner);
    printBoard(&(pGame->board));
    printMoves(pGame);
    printf("\n-----------------------------------\n");
}

void printBoard(const Board *pBoard) {
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

void printMoves(Game *pGame) {
    printf("\nMoves: ");
    for(char i = 0; i < pGame->moveIndex; ++i) {
        printf("(x%d,y%d to x%d,y%d) ", (pGame->moves)[i].from.x, (pGame->moves)[i].from.y, (pGame->moves)[i].to.x, (pGame->moves)[i].to.y);
    }
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

void printHistory(HistoryMovesForColor *pHistory) {
    printf("\nHistoryMovesForColor:\n");
    for(char pieceIndex = 0; pieceIndex < TOTAL_PIECES_PER_COLOR; ++pieceIndex) {
        printf("\nPiece %d:\n", pieceIndex);
        for(char moveIndex = 0; moveIndex < (*pHistory)[pieceIndex].moveCount; ++moveIndex) {
            printf("Move %d: %d to %d | Win count: %d | Game count: %d\n", moveIndex, (*pHistory)[pieceIndex].from, (*pHistory)[pieceIndex].tos[moveIndex].to, (*pHistory)[pieceIndex].tos[moveIndex].win_count, (*pHistory)[pieceIndex].tos[moveIndex].game_count);
        }
    }
}

void helpWithPossibleMoves(Game *pGame) {
    PossibleMoves possibleMoves;
    getPossibleMovesForColor(&(pGame->board), pGame->turn, &possibleMoves);
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

Result askForMove(Game *pGame) {
    Move move;
    move.from.x = askForMoveI("from x"); 
    move.from.y = askForMoveI("from y");

    if(getCoordinatesValue(&(move.from), &(pGame->board)) != pGame->turn) {
        printf("\n Error: Not your piece at %dx,%dy\n", move.from.x, move.from.y);
        return ERROR;
    }

    move.to.x = askForMoveI("to x"); 
    move.to.y = askForMoveI("to y");
    
    if(getCoordinatesValue(&(move.to), &(pGame->board)) != EMPTY) {
        printf("\n Error: To position not empty at %dx,%dy\n", move.to.x, move.to.y);
        return ERROR;
    }

    if(!isValidMove(&move, &(pGame->board))) {
        printf("\n Error: Invalid move: ");
        printMove(&move);
        return ERROR;
    }

    applyMoveToGame(&move, pGame);
    printGame(pGame);
    return SUCCESS;
}

void init() {
    printf("Welcome to Jump Over Jump!\n\n");
    pLocalCacheHead = initializeLocalCacheNode();
    setupMemcached();

    // Init random number generators
    // srand(time(NULL)); // Only needed for rand but I'm using pcg now
    initRNG();
}

void end() {
    freeLocalCacheNode(pLocalCacheHead);
    freeMemcached();
    printf("\nGoodbye!\n");
}

/////////////////////////////////////////////////////////////////////////////////
// Main
/////////////////////////////////////////////////////////////////////////////////

int main() {
    init();

    Game game;
    initializeGame(&game);
    printGame(&game);

    Move move;
    
    char exitGame = 0;

    while(exitGame == 0) {
        if(game.winner != EMPTY || game.moveIndex >= MAX_MOVES) {
            printf("\nGame finished! Winner: %c\n", game.winner);
            backPropagateHistory(&game);
            break;
        }

        printf("\nWhat would you like to do? Enter:\n");
        printf("'a' ai vs ai\n");
        printf("'b' board\n");
        printf("'h' help with possible moves\n");
        printf("'m' move\n");
        printf("'g' guess best move\n");
        printf("'p' play best move\n");
        printf("'e' exit\n");

        char answer = getchar();
        getchar(); // Removes new line
        
        switch (answer)
        {
            case 'a':
                aiVsAiForNGames();
                break;

            case 'b':
                printGame(&game);
                break;

            case 'h':
                helpWithPossibleMoves(&game);
                break;

            case 'm':
                askForMove(&game);
                break;

            case 'g':
                guessBestMove(&game, &move);
                printMove(&move);
                break;

            case 'p':
                guessBestMove(&game, &move);
                printMove(&move);
                applyMoveToGame(&move, &game);
                printGame(&game);
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

    end();
    return 0;
}