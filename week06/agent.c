#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

void print_file_to_console() {
    FILE *file = fopen("text.txt", "r");
    char text;

    while ((text = getc(file)) != EOF)
        putchar(text);

    fclose(file);
}

void terminate() {
    printf("Process terminating...");
    remove("agent.pid");
    exit(0);
}

int main() {
    FILE *file = fopen("agent.pid", "w");
    fprintf(file, "%d", getpid());
    fclose(file);

    print_file_to_console();

    signal(SIGUSR1, print_file_to_console);
    signal(SIGUSR2, terminate);

    while (1) sleep(1);
}
