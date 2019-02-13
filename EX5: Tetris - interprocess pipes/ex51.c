#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>

#define TRUE 1
/**
 * reading a single char without waiting fot '\n'
 * @return char read
 */
char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror ("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror ("tcsetattr ~ICANON");
    return (buf);
}

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

int main()
{

    int pid;
    int fd[2];
    char c;
    handleErrors(pipe(fd) == -1);

    handleErrors((pid = fork()) < 0);
    if(pid == 0)
    {
        //child
        handleErrors(dup2(fd[0], STDIN_FILENO) == -1);
        handleErrors(close(fd[1]) == -1);
        execl("./draw.out", "draw.out", NULL);
        handleErrors(TRUE);

    }
    //father
    handleErrors(close(fd[0]) == -1);
    handleErrors(dup2(fd[1], STDOUT_FILENO) == -1);
    while(TRUE)
    {
        sleep(1);
        c = getch();
        handleErrors(kill(pid, SIGUSR2) == -1);
        handleErrors(write(STDOUT_FILENO, &c, sizeof(c)) == -1);
        if(c == 'q')
        {
            return 0;
        }
    }

}
