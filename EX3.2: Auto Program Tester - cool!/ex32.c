//Avner Merhavi
//203090436

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <sys/wait.h>

#define TRUE 1
#define MAX_CONFIG 500
#define MAX_PATH 1024
#define MAX_LINE 1024
#define MAX_INT_LEN 21
#define HAVE_TO_HANDLE (-1)
#define C_FILE_FOUND 1
#define C_FILE_NOT_FOUND_YET 0

/**
 * handling systemCall errors.
 * @param handleFlag represents does handle needed
 */
void handleErrors(int handleFlag)
{
    char sysCallError[] = {"Error in system call"};
    if(handleFlag == -1)
    {
        write(STDERR_FILENO, sysCallError, sizeof(sysCallError));
        exit(EXIT_FAILURE);
    }
}

/**
 * writes grade to results file.
 * @param resultFd resultFd
 * @param name  student name
 * @param score score
 * @param scoreExplanation scoreExplanation
 */
void writeResult(int resultFd, char* name, int score, char* scoreExplanation)
{
    char resultEntry[MAX_LINE];
    char scoreStr[MAX_INT_LEN];
    //convert score to string
    sprintf(scoreStr, "%d", score);

    strcpy(resultEntry, name);
    strcat(resultEntry, ",");
    strcat(resultEntry, scoreStr);
    strcat(resultEntry, ",");
    strcat(resultEntry, scoreExplanation);
    strcat(resultEntry, "\n");

    //write entry to results.csv
    handleErrors(write(resultFd, resultEntry, strlen(resultEntry) + 1));

    printf("New entry: ");
    printf("%s\n", resultEntry);

}

/**
 * compiles c file.
 * @param pathToCFile pathToCFile
 * @param compilePathAndName where to compile to and the desired name
 * @return is compilation error boolean
 */
int compile(char* pathToCFile, char* compilePathAndName)
{
    int pid, status;
    if ((pid = fork()) == 0)
    {
        handleErrors(execl("/usr/bin/gcc", "gcc", "-o", compilePathAndName, pathToCFile, NULL));
    }
    handleErrors(pid);

    //wait to compilation to complete
    waitpid(pid, &status, 0);
    //if not returned normally
    if(!WIFEXITED(status))
    {
        handleErrors(HAVE_TO_HANDLE);
    }
    if(WEXITSTATUS(status))
    {
        return 1;
    }
    return 0;
}

/**
 *
 * @param inputPath path to input file
 * @param compilePathAndName compilePathAndName
 * @return isTimeout boolean
 */
int run(char* inputPath, char* compilePathAndName)
{
    int pid, status, returnedPid;
    if ((pid = fork()) == 0)
    {
        //redirect stdout to output
        int outputFd = open("output.txt", O_RDWR |  O_CREAT | O_TRUNC ,  S_IRWXU| S_IRWXG | S_IRWXO);
        int inputFd = open(inputPath, O_RDWR);
        dup2(outputFd, STDOUT_FILENO);

        //redirect stdin to input
        dup2(inputFd, STDIN_FILENO);

        //run

        handleErrors(execl(compilePathAndName, "a.out", NULL));
    }

    handleErrors(pid);

    //wait 5 seconds for running to be finished
    sleep(5);
    //if still running
    returnedPid = waitpid(pid, &status, WNOHANG);
    //still running
    if(returnedPid == 0)
    {
        return 1;
    }
    return 0;
}

/**
 * compare student output with correct one.
 * @param studentName studentName
 * @param correctOutputPath correctOutputPath
 * @param resultsFd resultsFd
 */
void compare(char* studentName, char* correctOutputPath, int resultsFd)
{
    int pid, status, comparisonFlag;
    char* bad_output = "BAD_OUTPUT";
    char* similar_output = "SIMILAR_OUTPUT";
    char* great_job = "GREAT_JOB";

    if((pid = fork()) == 0)
    {
        handleErrors(execl("./comp.out", "comp.out", "output.txt", correctOutputPath, NULL));
    }
    //handle failed fork
    handleErrors(pid);

    //check if comparison program failed
    waitpid(pid, &status, 0);
    if(!WIFEXITED(status))
    {
        handleErrors(HAVE_TO_HANDLE);
    }


    comparisonFlag = WEXITSTATUS(status);

    switch(comparisonFlag)
    {
        case 3:
            writeResult(resultsFd, studentName, 100,  great_job);
            break;
        case 2:
            writeResult(resultsFd, studentName, 80, similar_output);
            break;
        case 1:
            writeResult(resultsFd, studentName, 60, bad_output);
        default:;
    }
}

/**
 * operates compilation, running and comparison on c file using Auxilliary methods.
 * @param dirPath  dirPath
 * @param studentName
 * @param entryName file name
 * @param inputPath
 * @param correctOutputPath
 * @param resultsFd
 * @return operation completion flag.
 */

int handleCFile(char * dirPath, char* studentName,
                char* entryName, char*inputPath, char* correctOutputPath, int resultsFd )
{
    int isTimeout, isCompilationError;
    char compilePathAndName[MAX_PATH], pathToCFile[MAX_PATH];
    char *compilation_error = "COMPILATION_ERROR";
    char* timeout = "TIMEOUT";
    compilePathAndName[0] = '\0';
    pathToCFile[0] = '\0';

    /*prepare paths*/

    //dirPath + / + fileName
    strcpy(pathToCFile, dirPath);
    strcat(pathToCFile, "/");
    strcat(pathToCFile, entryName);

    //add a.out
    strcpy(compilePathAndName, dirPath);
    strcat(compilePathAndName, "/a.out");

    // COMPILE
    isCompilationError = compile(pathToCFile, compilePathAndName);

    if(isCompilationError)
    {
        writeResult(resultsFd, studentName, 0, compilation_error);
        return C_FILE_FOUND;
    }
    //RUN
    isTimeout = run(inputPath, compilePathAndName);

    if(isTimeout)
    {
        writeResult(resultsFd, studentName, 0, timeout);
        //delete .out file
        unlink(compilePathAndName);
        return C_FILE_FOUND;
    }

    //delete .out file
    unlink(compilePathAndName);

    //COMPARE with correct output
    compare(studentName, correctOutputPath, resultsFd);
    //finish work in directory, since a .c file was found and checked
    return C_FILE_FOUND;
}

/**
 * Takes a students directory path and grading all of their programs, --recursively--
 * @param dirPath directoty path
 * @param indent
 * @param inputPath path to input for tested program
 * @param correctOutputPath path to file of expected output
 * @param resultsFd fd of results file, where scores are written
 * @param studentName studentName
 * @return recursion flag: 0 at end or if c file was not found in sub folder, 1 if found
 */
int GradeDirectory(char *dirPath, int indent, char* inputPath,
                        char* correctOutputPath, int resultsFd,  char* studentName)
{
    DIR *dir;
    struct dirent *entry;
    int recursionRetVal = 0;
    char *lastDot;
    char *no_c_file = "NO_C_FILE";

    int savedStdout = dup(STDOUT_FILENO);

    if (!(dir = opendir(dirPath)))
    {
        handleErrors(HAVE_TO_HANDLE);
    }

    while (TRUE)
    {
        // if we found c file and we are somewhere in student directory
        if(studentName != NULL && recursionRetVal == C_FILE_FOUND)
        {
            closedir(dir);

            //propagate up that already found
            return C_FILE_FOUND;
        }
        //no more items in directory
        if((entry = readdir(dir)) == NULL)
        {
            //first level
            if(studentName == NULL)
            {
                //delete output file we created
                unlink("output.txt");

                closedir(dir);
                return 0;
            }
            /*if not 1st level and it's student directory (2nd level,
              we check that by comparing the end of the path to current student name)
              and recursionRetVal is 0, so  no .c file was found
             */
            if(strcmp(dirPath + strlen(dirPath) - strlen(studentName),
                                             studentName) == 0 && recursionRetVal == 0)
            {
                writeResult(resultsFd, studentName, 0, no_c_file);
                closedir(dir);
                return 0;

            //some sub-folder of a student with no .c file
            } else{
                closedir(dir);
                return C_FILE_NOT_FOUND_YET;
            }
        }
        //if it's a folder, recurse
        if (entry->d_type == DT_DIR)
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;

            /*RECURSE*/
            //create sub-directory path (declaration here for efficiency, only fo dirs)
            char path[MAX_PATH];
            snprintf(path, sizeof(path), "%s/%s", dirPath, entry->d_name);
            printf("%*s[%s]\n", indent, "", entry->d_name);

            //if it's a folder in first level, pass student name as folder name and recurse
            if(studentName == NULL) {
                recursionRetVal = GradeDirectory(path, indent + 2, inputPath, correctOutputPath,
                                                     resultsFd, entry->d_name);
                continue;
            }
            // else, recurse with current student name (when we recurse to sub-directory )
            recursionRetVal = GradeDirectory(path, indent + 2, inputPath, correctOutputPath,
                               resultsFd, studentName);
            continue;

            //if it's a file
        } else {
            printf("%*s- %s\n", indent, "", entry->d_name);

            lastDot = strrchr(entry->d_name, '.');

            //if it's a .c file
            if (lastDot && !strcmp(lastDot, ".c"))
            {
                recursionRetVal = handleCFile(dirPath, studentName, entry->d_name, inputPath,
                                              correctOutputPath, resultsFd);
                closedir(dir);
                return recursionRetVal;
            }
        }
    }
}

/**
 * main function.
 * @param argc expected a path to a single configuration file
 * @param argv expected to be 2
 */
int main( int argc, char** argv) {

    char illegalInput[] = {"illegal Input"};
    char* pathToConfigFIle;
    char buffer[MAX_CONFIG];
    char* configLines[3];
    int configFile, resultsFd;

    if(argc != 2)
    {
        write(STDERR_FILENO, illegalInput, sizeof(illegalInput));
        exit(EXIT_FAILURE);
    }

    pathToConfigFIle = argv[1];
    //open configuration file
    handleErrors(configFile = open(pathToConfigFIle, O_RDONLY));


    //read whole config file
    handleErrors(read(configFile, buffer, sizeof(buffer)));

    /*parse buffer*/

    //students directory path
    configLines[0] = strtok(buffer, "\n");
    //input path
    configLines[1] = strtok(NULL, "\n");
    //correct output path
    configLines[2] = strtok(NULL, "\n");

    handleErrors(resultsFd = open("results.csv",
                                  O_RDWR | O_TRUNC |O_CREAT , S_IRWXU| S_IRWXG | S_IRWXO));
    //grade whole directory
    GradeDirectory(configLines[0], 0, configLines[1], configLines[2], resultsFd, NULL);

    return 0;
}