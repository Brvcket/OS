#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_INPUT_SIZE 256

void execute_command(char *command, char *args[]) {
    pid_t pid = fork();
    
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        if (execvp(command, args) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }
}

int main() {
    char input[MAX_INPUT_SIZE];
    signal(SIGINT, SIG_IGN);  // Ignore Ctrl+C in the parent process
    
    while (1) {
        printf("MyShell> ");
        fgets(input, MAX_INPUT_SIZE, stdin);
        
        // Remove trailing newline character
        input[strcspn(input, "\n")] = '\0';
        
        if (strcmp(input, "exit") == 0) {
            printf("Exiting shell...\n");
            break;
        }
        
        // Tokenize the input to separate the command and arguments
        char *command = strtok(input, " ");
        char *args[MAX_INPUT_SIZE / 2];
        int arg_count = 0;
        
        while (command != NULL) {
            args[arg_count] = command;
            arg_count++;
            command = strtok(NULL, " ");
        }
        
        args[arg_count] = NULL;
        
        if (arg_count > 0) {
            execute_command(args[0], args);
        }
    }
    
    return 0;
}
