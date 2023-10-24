#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_MEM 10000000

clock_t time0;

void reset_timer() {
    time0 = clock();
}

double elapsed_time() {
    clock_t time1 = clock();
    return ((double) (time1 - time0)) / CLOCKS_PER_SEC;
}

unsigned int mem[MAX_MEM];

void allocate_first_fit(unsigned int adrs, unsigned int size) {
    int i = 0;
    int j = 0;
    int free_block_size = 0;
    while (j < MAX_MEM) {
        if (mem[j] != 0) {
            free_block_size = 0;
            i = j + 1;
        } else {
            free_block_size++;
            if (free_block_size >= size) {
                for (int k = i; k < i + size; k++)
                    mem[k] = adrs;
                return;
            }
        }
        j++;
    }
}

void allocate_best_fit(unsigned int adrs, unsigned int size) {
    int ind = -1;
    int best_fit_size = MAX_MEM;
    int block_size = 0;
    for (int i = 0; i < MAX_MEM; i++)
        if (mem[i] != 0) {
            block_size = 0;
        } else {
            block_size++;
            if (block_size >= size && block_size < best_fit_size) {
                best_fit_size = block_size;
                ind = i - block_size + 1;
            }
        }

    if (ind != -1)
        for (int k = ind; k < ind + size; k++)
            mem[k] = adrs;
}


void allocate_worst_fit(unsigned int adrs, unsigned int size) {
    int ind = -1;
    int worst_fit_size = 0;
    int block_size = 0;
    for (int i = 0; i < MAX_MEM; i++)
        if (mem[i] != 0) {
            block_size = 0;
        } else {
            block_size++;
            if (block_size >= size && block_size > worst_fit_size) {
                worst_fit_size = block_size;
                ind = i - block_size + 1;
            }
        }


    if (ind == -1) return;
    for (int k = ind; k < ind + size; k++)
        mem[k] = adrs;
}

void clear(unsigned int adrs) {
    for (int i = 0; i < MAX_MEM; i++)
        if (mem[i] == adrs)
            mem[i] = 0;
}

void reset_memory() {
    memset(mem, 0, MAX_MEM * sizeof(unsigned int));
}

void execute(char *algo) {
    reset_memory();
    FILE *file = fopen("queries.txt", "r");
    if (file == NULL) {
        perror("open file");
        exit(EXIT_FAILURE);
    }
    char command[10];
    unsigned int adrs;
    unsigned int size;
    int query_cnt = 0;
    reset_timer();
    while (fscanf(file, "%s %u %u", command, &adrs, &size) != EOF)
        if (strcmp(command, "allocate") != 0)
            if (strcmp(command, "clear") != 0) {
                if (!strcmp(command, "end"))
                    break;
                else {
                    printf("Invalid command!\n");
                }
            } else {
                clear(adrs);
                query_cnt++;
            }
        else {
            if (strcmp(algo, "first_fit") == 0) 
                allocate_first_fit(adrs, size);
            else if (!strcmp(algo, "best_fit"))
                allocate_best_fit(adrs, size);
            else if (!strcmp(algo, "worst_fit")) {
                allocate_worst_fit(adrs, size);
            }
            query_cnt++;
        }

    double exec_time = elapsed_time();
    fclose(file);
    double throughput = (double) query_cnt / exec_time;
    printf("Throughput of %s (queries per second) is equal to %.2f\n", algo, throughput);
}

int main() {
    execute("first_fit");
    execute("best_fit");
    execute("worst_fit");

    return 0;
}
