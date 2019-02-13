//Avner Merhavi
//203090436

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "MyLinkedList.h"
#define TRUE 1
#define MISTAKEN_CMD 10
#define MAX 1024
/*
 * updates jobs list and displays it.
 */
void jobs(LinkedList* jobsList) {
    int j;
    int status;
    Node* node = jobsList->head;

    // check which jobs are still active
    pid_t result;
    for(j = 0; j < jobsList->size; j++) {
        result = waitpid((pid_t)node->pid, &status, WNOHANG);
        if (result == -1 || result == node->pid) {
            // remove from list
            removeNode(jobsList, node);
        }
        node = node->next;
    }
    //print list
    jobsList->display(jobsList);
}
/*
 * cd command.
 */
void cd (char** args, char* previousPath, char* currentPath) {

    // If no path given or ~ flag
    if ((args[1] == NULL) || (strcmp(args[1], "~") == 0)) {
        char* newPath = getenv("HOME");
        chdir(newPath);
        strcpy(previousPath, currentPath);
        strcpy(currentPath, newPath);
    }

    else if(strcmp(args[1], "..") == 0) {
        chdir("..");
        strcpy(previousPath, currentPath);
        getcwd(currentPath, 1024);
    }
        // flag - : previous path
    else if(strcmp(args[1], "-") == 0) {
        chdir(previousPath);
    }
    else if(strcmp(args[1], ".") == 0) {
        ;
    }
        // if a specific path is given
    else{

        if (chdir(args[1]) == -1) {
            printf(" %s: no such directory\n", args[1]);
        }
    }

}



/*
 * main function.
 */
int main() {
    pid_t pid;
    char buffer[MAX];
    char buffBackup[MAX];
    LinkedList* jobsList = createLinkedList();
    int shouldWait;
    int status;
    int returned;
    //for the "cd -" command
    char previousPath[MAX];
    char currentPath[MAX];
    //assign current path
    getcwd(previousPath, MAX);
    getcwd(currentPath, MAX);

    while(TRUE) {

        shouldWait = 1;
        printf("prompt>");
        memset(buffer, '\0', MAX);

        fgets(buffer, MAX, stdin);

        //if it's only a newline - continue
        if(strcmp(buffer, "\n") == 0)
            continue;

        //replace \n with \0
        char* pos;
        pos = strchr(buffer, '\n');
        *pos = '\0';

        // check if & exists at the end
        if (*(--pos) == '&'){
            //set flg to false and remove '&'
            shouldWait = 0;
            *(--pos) = '\0';
        }
        strcpy(buffBackup, buffer);

        // exit if requested
        if(strcmp(buffer, "exit") == 0) {
            //free jobs list

            deleteList(&jobsList);
            return 0;
        }

        char* arg;
        char* args[MAX];
        memset(args, '\0', 50);

        args[0] = strtok(buffer, " ");
        int i = 1;
        while ( (arg = strtok(NULL, " ")) != NULL) {
            args[i] = arg;
            i++;
        }
        if(strcmp(args[0], "jobs") == 0) {
            jobs(jobsList);
            continue;
        }
        if(strcmp(args[0], "cd") == 0) {
            cd(args, previousPath, currentPath);
            continue;
        }


        if ((pid = fork()) == 0) {
            //child
            if (execvp(args[0], args) == -1) {
                printf("mistaken command\n");
                return MISTAKEN_CMD;
            }
        }
        else if(pid < 0) {
                printf("error in creating process\n");
                continue;
        }

        else {
            //father
            Node* newNode = jobsList->addLast(jobsList, pid, buffBackup);


            if(shouldWait) {
                printf("%d\n", pid);
                wait(&status);
                if (WIFEXITED(status)){
                    returned = WEXITSTATUS(status);
                    //check if command was mistaken and remove it from jobs
                    if(returned == MISTAKEN_CMD) {
                        removeNode(jobsList, newNode);
                    }
                }
            } else {
                printf("%d\n", getpid());
            }
        }
    }
}
