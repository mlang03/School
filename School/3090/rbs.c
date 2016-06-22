/* rbs - Red-Blue Simulation
 * By:Michael Lang 
 * mlang03@mail.uoguelph.ca
 * Oct 23rd 2014
 * 0733368
 */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

/*
#ifndef _WIN32 // if we're not on Windows
#define _POSIX_C_SOURCE 200112L // get barriers on Linux
#endif
*/

#define WHITE 0
#define RED 1
#define BLUE 2

int initBoard(char *** initBoardRead, char *** initBoardWrite);
int printBoard(char ** print_board);
void writeBoard(char ** dest, char ** source);
void moveRedRow(int rowNum);
void moveBlueColumn(int colNum);
void divideWork(void);
void printWork(void);
int getDensity(int threadNum, int tileNum);
void * work_func(void * q);


int num_threads;
int board_size;
int tile_size;
int max_density;
int iteration_limit;
int random_seed = 0;
int interactive_mode = 0;
char ** board_read;
char ** board_write;
int iterations = 1;
int densityReached = 0;

pthread_barrier_t barrier;


typedef struct {
    int row;
    int col;
    int size;
} tile;

typedef struct {
    int * cols;
    int numCols;
    int * rows;
    int numRows;
    tile * tiles;
    int numTiles;
} work;


work * thread_work;


int main(int argc, char ** argv){
    int i,j;
    pthread_t threads[num_threads];
    if (argc >= 6){ //No options
        for (i = 1; i < argc; i++){
            if (argv[i][0] == 'p'){
                num_threads = atoi(argv[i]+1);
            }
            if (argv[i][0] == 'b'){
                board_size = atoi(argv[i]+1);
            }
            if (argv[i][0] == 't'){
                tile_size = atoi(argv[i]+1);
            }
            if (argv[i][0] == 'c'){
                max_density = atoi(argv[i]+1);
            }
            if (argv[i][0] == 'm'){
                iteration_limit = atoi(argv[i]+1);
            }
            if (argv[i][0] == 's'){
                    random_seed = atoi(argv[i]+1);
            }
            if (argv[i][0] == 'i'){
                    interactive_mode = 1;
            }
        }
        if (num_threads < 1) { printf("Error p < 1.\n"); exit(0); }
        if (board_size < 2) { printf("Error b < 2.\n"); exit(0); }
        if (board_size % tile_size != 0) { printf("Error tile size does not divide board evenly.\n"); exit(0); }
        if (max_density < 0 || max_density > 100) { printf("Density out of range.\n"); exit(0); }
    }
    else {
        printf("Insufficient Arguments\n");
        exit(0);
    }
    initBoard(&board_read, &board_write);
    divideWork();
    printWork();
    printBoard(board_read);
    
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);
    pthread_barrier_init(&barrier, NULL, num_threads);
    
    
    for (i = 0; i < num_threads; i++){
        pthread_create(&(threads[i]), &attr, work_func, (void*) i);
    }
    
    for (i = 0; i < num_threads; i++){
        pthread_join(threads[i], NULL);
    }
    
    writeBoard(board_read, board_write);
    printBoard(board_read);
    
    //printWork();
    //printf("%d\n", getDensity(0,0));
//     //printBoard(board_write);
//     writeBoard(board_write, board_read);
//     printBoard(board_write);
//     moveRedRow(0);
//     printBoard(board_write);
//     writeBoard(board_read, board_write);
//     moveBlueColumn(0);
//     printBoard(board_write);
    return 0;
}

void * work_func(void * q){
    int i,j;
    int threadNum = (int) q;
    
    while (1){
        
        //Perform red movements
        for (i = 0; i < thread_work[threadNum].numRows; i++){
            moveRedRow(thread_work[threadNum].rows[i]);
        }
        
        pthread_barrier_wait(&barrier);
        
        //Sync the double buffer
        if (threadNum == 0){
            writeBoard(board_read, board_write);
        }
        pthread_barrier_wait(&barrier);
        
        //Perform blue movements
        for (i = 0; i < thread_work[threadNum].numCols; i++){
            moveBlueColumn(thread_work[threadNum].cols[i]);
        }
        
        pthread_barrier_wait(&barrier);
        
        //Sync the double buffer
        if (threadNum == 0){
            writeBoard(board_read, board_write);
        }
        pthread_barrier_wait(&barrier);
        
        //Check densitys
        for (i = 0; i < thread_work[threadNum].numTiles; i++){
            printf("$$%d\n",getDensity(threadNum,1));
            if (getDensity(threadNum,i) >= max_density){
                densityReached = 1;
            }
        } 
        
         if (threadNum == 0){
            printf("Iteration %d\n",iterations);
            iterations++;
        }
        
        pthread_barrier_wait(&barrier);
        
        if (densityReached == 1){
            printf("Density stopping condition reached\n");
            return 0;
        }
        else if (iterations > iteration_limit){
            printf("Iteration limit reached\n");
            return 0;
        }
        
        pthread_barrier_wait(&barrier);
    }
    
    
    return (void*) 0;
}

int getDensity(int threadNum, int tileNum){
    int i,j;
    int numRed = 0, numBlue = 0;
    int size = tile_size * tile_size;
    int startRow = thread_work[threadNum].tiles[tileNum].row;
    int startCol = thread_work[threadNum].tiles[tileNum].col;
    for (i = startRow; i < startRow + tile_size; i++){
        for (j = startCol; j < startCol + tile_size; j++){
            if (board_read[i][j] == RED) numRed++;
            if (board_read[i][j] == BLUE) numBlue++;
        }
    }
    return numRed >= numBlue ? (numRed*100 / size) : (numBlue*100 / size);
}

void printWork(void){
    int i,j;
    for (i = 0; i < num_threads; i++){
        printf("rows:\n");
        for (j = 0; j < thread_work[i].numRows; j++){
            printf("\t%d\n", thread_work[i].rows[j]);
        }
        printf("cols:\n");
        for (j = 0; j < thread_work[i].numCols; j++){
            printf("\t%d\n", thread_work[i].cols[j]);
        }
        printf("tiles:\n");
        for (j = 0; j < thread_work[i].numTiles; j++){
            printf("\trow: %d col: %d\n", thread_work[i].tiles[j].row ,thread_work[i].tiles[j].col);
        }
        printf("==============\n");
    }
}

void divideWork(void){
    int i,j;
    
    //Allocate memory to the array of work structures.
    thread_work = calloc(num_threads,sizeof(work));
    
    //Finding number of rows per thread and remainder.
    int rowsPerThread = board_size / num_threads;
    int rowsRemainder = board_size % num_threads;
    
    
    //Finding number of tiles per thread and remainder.
    int numTiles = board_size / tile_size;
    numTiles = numTiles * numTiles;
    int numTilesPerThread = numTiles / num_threads;
    int tilesRemainder = numTiles % num_threads;
    
    int curRow = 0;
    int curTileX = 0;
    int curTileY = 0;
    for (i = 0; i < num_threads; i++){
        
        //Allocate memory for row/column work
        thread_work[i].numRows = rowsPerThread;
        thread_work[i].numCols = rowsPerThread;
        thread_work[i].rows = calloc(rowsPerThread, sizeof(int));
        thread_work[i].cols = calloc(rowsPerThread, sizeof(int));
        
        //Allocate memory for tile work
        thread_work[i].numTiles = numTilesPerThread;
        thread_work[i].tiles = calloc(numTilesPerThread, sizeof(tile));
        
        //Assign row/column work to each thread EXCLUDING remainder
        for (j = 0; j < rowsPerThread; j++){
            thread_work[i].rows[j] = curRow;
            thread_work[i].cols[j] = curRow;
            curRow++;
        }
        
        //Assign an additional row/column if we have remainders
        if (rowsRemainder > 0){
            //Assign remainder row to current thread (low-to-high)
            thread_work[i].numRows++;
            thread_work[i].rows = realloc(thread_work[i].rows,sizeof(int)*(thread_work[i].numRows));
            thread_work[i].rows[thread_work[i].numRows - 1] = curRow;
            
            //Assign remainder column to thread high-to-low
            thread_work[i].numCols++;
            thread_work[i].cols = realloc(thread_work[i].cols,sizeof(int)*(thread_work[i].numCols));
            thread_work[i].cols[thread_work[i].numCols - 1] = curRow;
            
            curRow++;
            rowsRemainder--;
        }
        
        //Assign tile work to each thread EXCLUDING remainder 
        for (j = 0; j < numTilesPerThread; j++){
            thread_work[i].tiles[j].row = curTileY;
            thread_work[i].tiles[j].col = curTileX;
            thread_work[i].tiles[j].size = tile_size;
            curTileX += 2;
            if (curTileX == board_size){
                curTileX = 0;
                curTileY += 2;
            }
        }
        
        if (tilesRemainder > 0){
            thread_work[i].numTiles++;
            thread_work[i].tiles = realloc(thread_work[i].tiles, sizeof(tile)*thread_work[i].numTiles);
            thread_work[i].tiles[thread_work[i].numTiles - 1].row = curTileY;
            thread_work[i].tiles[thread_work[i].numTiles - 1].col = curTileX;
            thread_work[i].tiles[thread_work[i].numTiles - 1].size = tile_size;
            curTileX += 2;
            if (curTileX == board_size){
                curTileX = 0;
                curTileY += 2;
            }
            tilesRemainder--;
        }
    }
}
    
int initBoard(char *** initBoardRead, char *** initBoardWrite){
    int i,j;
    srand(random_seed);// Seed random generator
    
    *initBoardRead = calloc(board_size, sizeof(char*)); //Allocate memory for the board
    *initBoardWrite = calloc(board_size, sizeof(char*));
    for (i = 0; i < board_size; i++){
        (*initBoardRead)[i] = calloc(board_size, sizeof(char));
        (*initBoardWrite)[i] = calloc(board_size, sizeof(char));
    }
    
    for (i = 0; i < board_size; i++){
        for (j = 0; j < board_size; j++){
            (*initBoardRead)[i][j] = rand() % 3;
            (*initBoardWrite)[i][j] = (*initBoardRead)[i][j];
        }
    }
    return 0;
}

int printBoard(char ** print_board){
    int i,j;
    for (i = 0; i < board_size; i++){
        for (j = 0; j < board_size; j++){
            printf("%d ", print_board[i][j]);
        }
        printf("\n");
    }
    printf("---------------------------\n");
}

void writeBoard(char ** dest, char ** source){
    int i,j;
    for (i = 0; i < board_size; i++){
        for (j = 0; j < board_size; j++){
            dest[i][j] = source[i][j];
        }
    }
}

void moveRedRow(int rowNum){
    int i;
    for (i = 0; i < board_size - 1; i++){
        if (board_read[rowNum][i] == RED && board_read[rowNum][i+1] == WHITE){
            board_write[rowNum][i] = WHITE;
            board_write[rowNum][i+1] = RED;
        }
    }
    if (board_read[rowNum][board_size - 1] == RED && board_read[rowNum][0] == WHITE){
        board_write[rowNum][board_size - 1] = WHITE;
        board_write[rowNum][0] = RED;
    }
}

void moveBlueColumn(int colNum){
    int i;
    for (i = 0; i < board_size - 1; i++){
        if (board_read[i][colNum] == BLUE && board_read[i+1][colNum] == WHITE){
            board_write[i][colNum] = WHITE;
            board_write[i+1][colNum] = BLUE;
        }
    }
    if (board_read[board_size - 1][colNum] == BLUE && board_read[0][colNum] == WHITE){
        board_write[board_size - 1][colNum] = WHITE;
        board_write[0][colNum] = BLUE;
    }
}