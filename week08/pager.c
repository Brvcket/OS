#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define PAGE_SIZE 8

struct PTE {
    bool valid;
    int frame;
    bool dirty;
    int referenced;
};

char **memory;
char **disk;
int *frames;

int number_of_frames;
int number_of_pages;
struct PTE *pagetable;

int accesses_count = 0;
int evict_count = 0;

void evict_page(int page, int mmu_pid) {
    int victim_frame = rand() % number_of_frames;
    evict_count++;
    accesses_count++;

    printf("Choose a random victim page %d\n", victim_frame);
    printf("Replace/Evict it with page %d to be allocated to frame %d\n", page, victim_frame);
    printf("Copy data from the disk (page=%d) to RAM (frame=%d)\n", page, victim_frame);

    int page_to_evict = -1;
    for (int i = 0; i < number_of_pages; i++) {
        if (pagetable[i].frame != victim_frame) continue;
        page_to_evict = i;
        break;
    }


    if (pagetable[page_to_evict].dirty) {
        memcpy(disk[page_to_evict], memory[victim_frame], PAGE_SIZE);
        accesses_count++;
    }

    memcpy(memory[victim_frame], disk[page], PAGE_SIZE);

    pagetable[page_to_evict].frame = -1;
    pagetable[page_to_evict].valid = false;
    pagetable[page_to_evict].dirty = false;
    pagetable[page_to_evict].referenced = 0;

    pagetable[page].frame = victim_frame;
    pagetable[page].valid = true;
    pagetable[page].dirty = false;
    pagetable[page].referenced = 0;

    printf("RAM array\n");
    for (int i = 0; i < number_of_frames; i++)
        printf("Frame %d ---> %s\n", i, memory[i]);

    printf("disk accesses is %d so far\n", accesses_count);
    printf("Resume MMU process\n");

    kill(mmu_pid, SIGCONT);
}

void request() {
    int free = 0;
    for (int i = 0; i < number_of_pages; i++)
        if (pagetable[i].referenced != 0) {
            int mmu_pid = pagetable[i].referenced;
            printf("-------------------------\n");
            printf("A disk access request from MMU Process (pid=%d)\n", pagetable[i].referenced);
            printf("Page %d is referenced\n", i);
            if (!pagetable[i].valid) {
                if (number_of_frames <= 0) continue;
                for (int j = 0; j < number_of_frames; j++)
                    if (!frames[j]) {
                        free++;
                        printf("We can allocate it to frame %d\n", j);
                        printf("Copy data from the disk (page=%d) to RAM (frame=%d)\n", i, j);
                        memcpy(memory[j], disk[i], PAGE_SIZE);
                        frames[j] = 1;

                        pagetable[i].frame = j;
                        pagetable[i].valid = true;
                        pagetable[i].dirty = false;
                        pagetable[i].referenced = 0;

                        printf("RAM array\n");

                        for (int i1 = 0; i1 < number_of_frames; i1++)
                            printf("Frame %d ---> %s\n", i1, memory[i1]);

                        accesses_count++;

                        printf("disk accesses is %d so far\n", accesses_count);
                        printf("Resume MMU process\n");

                        kill(mmu_pid, SIGCONT);
                        break;
                    }

                if (!free) {
                    printf("We do not have free frames in RAM\n");
                    evict_page(i, mmu_pid);
                    break;
                }
            } else {
                printf("Page %d is in RAM\n", i);
                pagetable[i].referenced = 0;
                kill(pagetable[i].referenced, SIGCONT);
            }
        }

}

void sigcont_handler(int signo) {
    if (signo == SIGCONT) request();
}

void sigusr1_handler(int signo) {
    if (signo == SIGUSR1) {
        printf("Pager received SIGCONT signal from MMU\n");
        printf("-------------------------\n");
        printf("%d disk accesses in total\n", accesses_count);
        printf("Pager is terminated\n");

        exit(0);
    }
}

void initialize_page_table() {
    int fd = open("/tmp/ex2/pagetable", 02 | 0100, 0400 | 0200);
    if (fd == -1) {
        perror("open /tmp/ex2/pagetable");
        exit(EXIT_FAILURE);
    }

    ftruncate(fd, number_of_pages * sizeof(struct PTE));
    pagetable = mmap(NULL, number_of_pages * sizeof(struct PTE), 0x1 | 0x2, 0x01, fd, 0);
    if (pagetable == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < number_of_pages; i++) {
        pagetable[i].valid = false;
        pagetable[i].frame = -1;
        pagetable[i].dirty = false;
        pagetable[i].referenced = 0;
    }

    printf("-------------------------\n");
    printf("Initialized page table\n");
    for (int i = 0; i < number_of_pages; i++)
        printf("Page %d ---> valid=%d, frame=%d, dirty=%d, referenced=%d\n",
               i, pagetable[i].valid, pagetable[i].frame, pagetable[i].dirty, pagetable[i].referenced);

    close(fd);
}

void initialize_disk() {
    disk = (char **)malloc(number_of_pages * sizeof(char *));
    static const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()";
    for (int i = 0; i < number_of_pages; i++) {
        disk[i] = (char *)malloc(PAGE_SIZE);

        for (int j = 0; j < PAGE_SIZE - 1; j++) {
            int index = rand() % (sizeof(charset) - 1);
            disk[i][j] = charset[index];
        }

        disk[i][PAGE_SIZE - 1] = '\0';
    }

    printf("-------------------------\n");
    printf("Initialized disk\nDisk array\n");
    for (int i = 0; i < number_of_pages; i++)
        printf("Disk %d ---> %s\n", i, disk[i]);

}

void initialize_memory() {
    memory = (char **) malloc(number_of_frames * sizeof(char *));

    for (int i = 0; i < number_of_frames; i++) {
        memory[i] = (char *) malloc(PAGE_SIZE);
        memset(memory[i], 0, PAGE_SIZE);
    }

    frames = (int *) malloc(number_of_frames * sizeof(int));
    for (int i = 0; i < number_of_frames; i++) frames[i] = 0;

    printf("-------------------------\n");
    printf("Initialized RAM\nRAM array\n");
    for (int i = 0; i < number_of_frames; i++)
        printf("RAM %d ---> %s\n", i, memory[i]);
}


void cleanup() {
    for (int i = 0; i < number_of_pages; i++) free(disk[i]);
    free(disk);
    for (int i = 0; i < number_of_frames; i++) free(memory[i]);
    free(memory);
    munmap(pagetable, number_of_pages * sizeof(struct PTE));

    printf("Total disk accesses: %d\n", accesses_count);
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr, "Usage: %s number_of_pages number_of_frames\n", argv[0]);
        return 1;
    }

    printf("Pager process, ID: %d\n", getpid());

    number_of_pages = atoi(argv[1]);
    number_of_frames = atoi(argv[2]);

    initialize_page_table();
    initialize_memory();
    initialize_disk();

    signal(SIGUSR1, sigusr1_handler);
    signal(SIGCONT, sigcont_handler);

    for (;;) pause();
}
