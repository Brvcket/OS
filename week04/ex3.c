#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        perror("not correct args");
        return 1;
    }

    int n = atoi(argv[1]);
    if (n <= 0) {
        perror("n should be positive");
        return 1
    }

    for (int i = 0; i < n; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("fork error");
            exit(EXIT_SUCCESS);
        }

        if (pid == 0) {
            printf("Child %d (PID: %d) created\n", i, getpid());
            sleep(5);
            exit(EXIT_SUCCESS);
        }
    }
    exit(EXIT_SUCCESS);
}
