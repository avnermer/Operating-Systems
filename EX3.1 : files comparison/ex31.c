// Avner Merhavi
// 203090436

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define IDENTICAL 3
#define ALIKE 2
#define DIFFERENT 1

#define TRUE 1


/**
 * check if char is a joker (newline or space)
 * @param c char
 * @return 1 if joker 0 if not
 */
int isJoker(char c) {
    if(c == '\n' || c == ' ')
        return 1;
    else
        return 0;
}

/**
 * returns the first non joker char.
 * @param file1 file to be checked till it's end
 * @return char
 */
char getFirstNoneJoker(int file1) {
    char c;
    int stat;
    const char error[] = "Error in system call";


    while(TRUE)
    {
        if((stat = read(file1, &c, sizeof(char))) == -1)
        {
            write(STDERR_FILENO, error, sizeof(error));
            return EXIT_FAILURE;
        }
        //if reached EOF
        if(!stat)
            return c;

        else if (isJoker(c))
            continue;
        //reached first non joker
        else
            return c;
    }
}

/**
 * main.
 * program compares 2 files.
 * joker means newline or space
 * @param argc argc
 * @param argv 2 paths of files.
 * @return 1 : different
 *         2: alike (changes in upper\ lower case, jokers)
 *         3: identical
 */
int main(int argc, char** argv) {

    int file1, file2, fileToCheck, stat1, stat2;
    // comparison flag
    int compFlag = IDENTICAL;
    const char error[] = "Error in system call";
    char char1, char2, charR, noneJoker;

    //open 2 given files, and write errors to stderr if something went wrong
    if((file1 = open(argv[1], O_RDONLY)) == -1) {
        write(STDERR_FILENO, error, sizeof(error));
    }
    if((file2 = open(argv[2], O_RDONLY)) == -1) {
        write(STDERR_FILENO, error, sizeof(error));
    }

    //compare 2 files char by char

    while(TRUE) {

        if((stat1 = read(file1, &char1, sizeof(char))) == -1) {
            write(STDERR_FILENO, error, sizeof(error));
            return EXIT_FAILURE;
        }
        if((stat2 = read(file2, &char2, sizeof(char))) == -1) {
            write(STDERR_FILENO, error, sizeof(error));
            return EXIT_FAILURE;
        }

        /*if both ended the same time, their similarity status remains the same.
         * just close and break.
         */
        if (!stat1 && !stat2)
        {
            close(file1);
            close(file2);
            return compFlag;
        }
        /*if one ends before the other:
         * if the remaining char is joker, comparison status remains same.
         * if another char exist in the rest of file, change comparison to different
         * */
        if (!stat1 || !stat2)
        {
            compFlag = ALIKE;
            fileToCheck = (!stat1) ? file2 : file1;
            charR = (!stat1) ? char2 : char1;
            //check if charR not a joker, means files are different
            if (!isJoker(charR))
            {
                return DIFFERENT;
            }

            /*check the rest*/

            // if all the rest are jokers
            if(isJoker(getFirstNoneJoker(fileToCheck)))
                return compFlag;

            return DIFFERENT;
        }
        //if both didn't end yet
        else
        {
            // if identical
            if(char1 == char2)
                continue;
            // if same letter but one is uppercase
            if((97 <=char1 <= 122) && (97 <=char2 <= 122) && (abs(char1 - char2) == 32))
                compFlag = ALIKE;
            //different, but both jokers
            else if(isJoker(char1) && isJoker(char2))
            {
                compFlag = ALIKE;
                continue;
            }
            //if only one is a joker
            else if(isJoker(char1) || isJoker(char2))
            {
                compFlag = ALIKE;
                fileToCheck = (isJoker(char1)) ? file1 : file2;
                noneJoker = (isJoker(char1)) ? char2 : char1;
                if(noneJoker == getFirstNoneJoker(fileToCheck))
                    continue;
                else
                    return DIFFERENT;

            }
            else
                return DIFFERENT;
        }
    }
}