#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#define SIZE 20
#define TRUE 1
typedef enum orientation {VERTICAL, HORIZONTAL} Orientation;

// updated by the sigusr2 signal
int isSig = 0;

/**
 * handling systemCall errors.
 * @param shouldHandle int "boolean" represents does handle needed
 */
void handleErrors(int shouldHandle)
{
    if(shouldHandle)
    {
        char sysCallError[] = {"Error in system call"};

        write(STDERR_FILENO, sysCallError, sizeof(sysCallError));
        exit(EXIT_FAILURE);
    }
}

/**
 * handler for SIGUSR2.
 */
void on_sigusr2(int num)
{
    handleErrors(signal(SIGUSR2, on_sigusr2) == SIG_ERR);
    isSig = 1;
}
/**
 * handler for SIGALRM
 */
void on_alarm(int num)
{
    handleErrors(signal(SIGALRM, on_alarm) == SIG_ERR);
}

/**
 * prints board.
 * @param board assuming 20*20 board
 * @return 1 if some cells in the bottom row are filled, 0 otherwise
 */
int printBoard(char board[][SIZE])
{
    int i;
    int j;
    for(i = 0; i < SIZE; i++)
    {
        for(j = 0; j < SIZE; j++)
        {
            if(board[i][j] == 0)
            {
                printf(" ");
                continue;
            }
            printf("%c", board[i][j]);
        }
        printf("\n");
    }
}
/**
 * Move down.
 * @param board board matrix
 * @param r row index of center
 * @param c column index of center
 * @param orientation vertical/horizontal
 * @return 1 if already reached bottom before call. 0 otherwise.
 */
int moveDown(char board[][20], int *r, int *c, Orientation *orientation)
{
    if(*orientation == HORIZONTAL)
    {
        if(*r + 1 > SIZE - 2) {return 1;}
        //delete current position
        board[*r][*c - 1] = board [*r][*c] = board [*r][*c + 1] = 0;

        //put it a row down
        *r = *r + 1;
        board[*r][*c - 1] = board [*r][*c] = board [*r][*c + 1] = '-';
        return  0;
    }
    // (if vertical)
    // if exceeds bottom
    if(*r + 2 > SIZE - 2) {return 1;}

    //push down
    board[*r - 1][*c] = 0;
    board[*r + 2][*c] = '-';
    *r = *r + 1;
    return 0;
}

/**
 * Move left.
 * @param board board matrix
 * @param r row index of center
 * @param c column index of center
 * @param orientation vertical/horizontal
 */
void moveLeft(char board[][20], int* r, int* c, Orientation *orientation)
{
    if(*orientation == HORIZONTAL)
    {
        if(*c - 1 > 1)
        {
            board[*r][*c + 1] = 0;
            board[*r][*c - 2] = '-';
            *c = *c - 1;
        }
        return;
    }
    //(if vertical)
    if(*c > 1)
    {
        //delete
        board[*r - 1][*c] = board[*r][*c] = board[*r + 1][*c] = 0;
        //update
        *c = *c - 1;
        board[*r - 1][*c] = board[*r][*c] = board[*r + 1][*c] = '-';
    }
}

/**
 * Move right.
 * @param board board matrix
 * @param r row index of center
 * @param c column index of center
 * @param orientation vertical/horizontal
 */
void moveRight(char board[][20], int* r, int* c, Orientation *orientation)
{
    if(*orientation == HORIZONTAL)
    {
        if(*c + 1 < SIZE - 2)
        {
            board[*r][*c - 1] = 0;
            board[*r][*c + 2] = '-';
            *c = *c + 1;
        }
        return;
    }
    //(if vertical)
    if(*c  < SIZE - 2)
    {
        //delete
        board[*r - 1][*c] = board[*r][*c] = board[*r + 1][*c] = 0;
        //update
        *c = *c + 1;
        board[*r - 1][*c] = board[*r][*c] = board[*r + 1][*c] = '-';
    }
}
/**
 * cleans current shape and repositioning it at the top.
 * @param board board matrix
 * @param r row index of center
 * @param c column index of center
 * @param orientation vertical/horizontal
 * @return
 */
void reInitializeBoard(char board[][20], int *r, int *c, Orientation *orientation)
{
    //clean bottom row
    int j;
    for(j = 1; j < SIZE - 1; j++)
    {
        board[SIZE - 2][j] = 0;
    }

    // if orientation is vertical, clean additional 2 rows above bottom
    if(*orientation == VERTICAL)
    {
        for(j = 1; j < SIZE - 1; j++)
        {
            board[SIZE - 3][j] = 0;
        }
        for(j = 1; j < SIZE - 1; j++)
        {
            board[SIZE - 4][j] = 0;
        }
    }
    //reposition the shape at the top as horizontal (default)
    *r = 0;
    *c = round(SIZE/2.0);
    board[*r][*c - 1] = board[*r][*c] = board[*r][*c + 1] = '-';
    *orientation  = HORIZONTAL;
}

/**
 * reorient shape. vertical <--> horizontal
 * @param board board matrix
 * @param r row index of center
 * @param c column index of center
 * @param orientation vertical/horizontal
 * @return 1 if reorient caused shape to reach bottom. 0 otherwise
 */
int reorient(char board[][20], int *r, int *c, Orientation *orientation)
{
    int isReachedBottom = 0;
    // if: - - -
    if(*orientation == HORIZONTAL)
    {
        //if turning exceeds bottom
        if(*r - 1 < 0 || *r + 1 > SIZE - 2) {return isReachedBottom;}
        if(*r + 1 == SIZE - 2) {isReachedBottom = 1;}
        //delete sides
        board[*r][*c - 1] = board[*r][*c + 1] = 0;

        // add up and beneath, update orientation
        board[*r - 1][*c] = board[*r + 1][*c] = '-';
        *orientation = VERTICAL;
        return isReachedBottom;
    }
    // (if vertical)
    //if turning exceeds sides
    if( *c - 1 < 1 || *c + 1 > SIZE - 2) { return isReachedBottom;}

    //delete top and bottom
    board[*r - 1][*c] = board[*r + 1][*c] = 0;

    //add in sides, update orientation
    board[*r][*c - 1] = board[*r][*c + 1] = '-';
    *orientation = HORIZONTAL;
    return isReachedBottom;
}

int main() {

    char board[SIZE][SIZE];
    int isReachedBottom = 0;

    /*initialize board*/
    int i;
    int j;
    for(i = 0; i < SIZE; i++)
    {
        for(j = 0; j < SIZE; j++)
        {
            if(j == 0 || j == SIZE -1 || i == SIZE -1)
            {
                board[i][j] = '*';
                continue;
            }
            board[i][j] = 0;
        }
    }

    // the initial center position and orientation.
    int r = 0;
    int c = round(SIZE/2.0);
    board[r][c - 1] = board[r][c] = board[r][c + 1] = '-';
    Orientation orientation = HORIZONTAL;
    //define the signals handlers
    handleErrors(signal(SIGALRM, on_alarm) == SIG_ERR);
    handleErrors(signal(SIGUSR2, on_sigusr2) == SIG_ERR);

    char input;
    while(TRUE)
    {
        if(isReachedBottom) { reInitializeBoard(board, &r, &c, &orientation);}
        system("clear");

        printBoard(board);

        alarm(1);
        pause();

        /*if got isusr2, move accordingly*/

        if(isSig)
        {
            handleErrors(read(STDIN_FILENO, &input, sizeof(input)) == -1);
            switch(input)
            {
                case 'a':
                    moveLeft(board, &r, &c, &orientation);
                    break;
                case 'd':
                    moveRight(board, &r, &c, &orientation);
                    break;
                case 's':
                   moveDown(board, &r, &c, &orientation);
                    break;
                case 'w':
                    reorient(board, &r, &c, &orientation);
                    break;
                case 'q':
                    return 0;
                default:;
            }
            isSig = 0;
        }
        /* for any signal of the both, move down.*/
        isReachedBottom = moveDown(board, &r, &c, &orientation);
    }
}