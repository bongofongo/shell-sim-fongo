#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <ctype.h>
#include <signal.h>

void myPrint(char *msg)
{
    write(STDOUT_FILENO, msg, strlen(msg));
}

void printErr() {
    char error_message[30] = "An error has occurred\n";
    write(STDOUT_FILENO, error_message, strlen(error_message));
}

void msgSep(char *input, char **args, int *words) {
    int i = 0;
    char* token = strtok(input, " \t\n");

    while (token != NULL && i < 100) {
        args[i++] = token;
        token = strtok(NULL, " \n\t");
    }
    args[i] = NULL;
    *words = i;

}
int isSpace(char* str) {
    int i;
    int len = strlen(str);
    for (i = 0; i < len; i++) {
        if (!isspace(str[i])) return 1;
    }
    return 0;
}

// Works as intended.
void pwd() {
    char path[514];
    getcwd(path, 514);
    myPrint(path);
    myPrint("\n");
}

void execute(char **args, int num_args) {

    pid_t pid = fork();

    if (pid < 0) {
        printErr(); 
        return;
    } else if (pid == 0) {
        if (execvp(args[0], args) < 0) {
            printErr();
            exit(0);
        }
    } else  {
        waitpid(pid, NULL, 0);
    }
}

int main(int argc, char *argv[]) 
{
    char cmd_buff[100];
    char *pinput;
    int num_args;
    char* args[514];
    int j;
    int stop = 1;
    // Batch Mode
    if (argc > 2) {
        printErr();
        exit(1);
    } else if (argc == 2) {
        char file_line[1500];
        FILE* tp = fopen(argv[1], "r");
        while(fgets(file_line, sizeof(file_line), tp) && stop) {
            if (strlen(file_line) > 512) {
                myPrint(file_line);
                printErr();
                continue;
            }
            if (!isSpace(file_line)) {
                continue;
            }
            myPrint(file_line);
            char* input[514];
            int total_commands = 0;
            char *token = strtok(file_line, ";\n");
            
            while (token != NULL) {
                if (isSpace(token)) {
                    input[total_commands++] = token;
                }
                token = strtok(NULL, ";\n");
            }
            input[total_commands] = NULL;
            // creating args array.
            j = 0;
            while (input[j] != NULL && j < total_commands) {

                num_args=0;
                msgSep(input[j], args, &num_args);

                if (!strcmp(args[0], "cd")) {
                    if (num_args == 1) {
                        chdir(getenv("HOME"));
                    } else if (num_args > 2) {
                        printErr();
                    } else {
                        if (chdir(args[1]) != 0) {
                            printErr();
                        }
                    }
                } else if (!strcmp(args[0], "pwd")) {
                    if (num_args == 1)
                        pwd();
                    else
                        printErr();
                } else if (!strcmp(args[0], "exit")) {
                    if (num_args == 1) {
                        return 0;
                    } else {
                        printErr();
                    }
                } else {
                    execute(args, num_args);
                }
                j++;
                //myPrint(cmd_buff);
            }
        }
        exit(0);
    }
    else {
        while (1) {
            myPrint("myshell> ");
            pinput = fgets(cmd_buff, 1500, stdin);
            
            if (strlen(pinput) > 512) {
                myPrint(pinput);
                printErr();
                continue;
            }

            if (!isSpace(pinput)) {
                continue;
            }

            if (!pinput) {
                exit(0);
            }
            char* input[514];
            int total_commands = 0;
            char *token = strtok(pinput, ";");
            while (token != NULL) {
                if (!isSpace(token)) {
                    printErr();
                } else {
                    input[total_commands++] = token;
                }
                token = strtok(NULL, ";");
            }
            input[total_commands] = NULL;

            // creating args array.
            j = 0;
            while (input[j] != NULL && j < total_commands) {
                msgSep(input[j], args, &num_args);

                if (!strcmp(args[0], "cd")) {

                    if (num_args == 1) {
                        chdir(getenv("HOME"));
                    } else if (num_args > 2) {
                        printErr();
                    } else {
                        if (chdir(args[1]) != 0) {
                            printErr();
                        }
                    }
                } else if (!strcmp(args[0], "pwd")) {
                    if (num_args == 1)
                        pwd();
                    else 
                        printErr();
                } else if (!strcmp(args[0], "exit")) {
                    if (num_args > 1)
                        printErr();
                    else 
                        exit(0);
                } else {

                    execute(args, num_args);
                }
                j++;
                //myPrint(cmd_buff);
            }
        }
    }
}
