#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

// Function to calculate dot product of two vectors
int dot_product(const int* u, const int* v, int start, int end) {
    int result = 0;
    for (int i = start; i < end; i++) {
        result += u[i] * v[i];
    }
    return result;
}

int main() {
    // Create vectors u and v
    int u[120], v[120];
    srand(time(NULL)); // Seed for random numbers to generate random number between 0 and 99
    for (int i = 0; i < 120; i++) {
        u[i] = rand() % 100;
        v[i] = rand() % 100;
    }

    // Get the number of processes (n)
    int n;
    printf("Enter the number of processes (n): ");
    scanf("%d", &n);

    if (n % 2 != 0 || n > 10 || n < 2) {
        printf("Error: n should be an even number between 2 and 10.\n");
        return 1;
    }

    // Fork n processes
    for (int i = 0; i < n; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            // Child process
            int start = i * 120 / n;
            int end = start + (120 / n);

            // Calculate dot product and write to temp.txt
            int result = dot_product(u, v, start, end);
            FILE* file = fopen("temp.txt", "a");
            fprintf(file, "%d\n", result);
            fclose(file);

            exit(EXIT_SUCCESS);
        }
    }

    // Wait for all child processes to finish
    for (int i = 0; i < n; i++) {
        wait(NULL);
    }

    // Read the temporal result from temp.txt and add to result
    FILE* file = fopen("temp.txt", "r");
    int result = 0;
    int temp_result;
    while (fscanf(file, "%d", &temp_result) != EOF) {
        result += temp_result;
    }
    fclose(file);

    // Print final result
    printf("Dot Product: %d\n", result);

    return 0;
}
