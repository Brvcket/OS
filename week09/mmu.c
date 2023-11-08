#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/mman.h>

#define PAGE_SIZE 8

struct PTE {
    bool valid;
    int frame;
    bool dirty;
    int referenced;
    int counter;
};

struct TLBEntry {
    int page;
    int frame;
};

int pager_pid;
struct PTE *pagetable;
struct TLBEntry *tlb;
int tlb_index = 0;
int tlb_size = 0;

int number_of_pages;
char *str;

int accesses_count = 0;
int hits = 0;

void process_memory_access(const char *access) {
    char mode = access[0];
    int page = atoi(&access[1]);
    printf("-------------------------\n");

    if (mode == 'W') printf("Write Request for page %d\n", page);
    else if (mode == 'R') printf("Read Request for page %d\n", page);
    else {
        printf("Invalid mode\n");
        exit(1);
    }

    // Check the TLB for the requested page
    int frame = -1;
    for (int i = 0; i < tlb_size; i++) {
        if (tlb[i].page == page) {
            frame = tlb[i].frame;
            break;
        }
    }

    if (frame != -1) {
        printf("Page is valid (hit) in TLB\n");
        hits++;
    } else {
        printf("Page is not in TLB --> TLB miss\n");
        if (!pagetable[page].valid) {
            printf("It is not a valid page --> page fault\n");
            printf("Ask pager to load it from disk (SIGUSR1 signal) and wait\n");

            pagetable[page].referenced = getpid();
            kill(pager_pid, SIGCONT);

            pause();
        } else {
            printf("Page is valid (hit) in page table\n");
            hits++;
        }

        // Update TLB with the new mapping
        tlb[tlb_index].page = page;
        tlb[tlb_index].frame = pagetable[page].frame;
        tlb_index = (tlb_index + 1) % tlb_size;
    }

    accesses_count++;
    printf("MMU resumed by SIGCONT signal from pager\n");

    if (mode == 'W') pagetable[page].dirty = true;
}

void sigusr1_handler(int signo) {
    if (signo == SIGUSR1) (void) signo;
}

void sigcont_handler(int signo) {
    if (signo == SIGCONT) (void) signo;
}

void initialize_page_table(int num_pages) {
    int fd = open("/tmp/ex2/pagetable", O_RDWR);
    if (fd == -1) {
        perror("open");
        exit(1);
    }

    ftruncate(fd, num_pages * sizeof(struct PTE) * 1000);
    pagetable = mmap(NULL, num_pages * sizeof(struct PTE) * 1000, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    tlb_size = num_pages * 20 / 100;
    if (tlb_size <= 0)tlb_size = 1;
    tlb = malloc(tlb_size * sizeof(struct TLBEntry));
    for (int i = 0; i < tlb_size; i++) {
        tlb[i].page = -1;
        tlb[i].frame = -1;
    }
    if (pagetable == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(1);
    }

    printf("-------------------------\n");
    printf("Initialized page table\nPage table\n");
    for (int i = 0; i < num_pages; i++)
        printf("Page %d ---> valid=%d, frame=%d, dirty=%d, referenced=%d\n",
               i, pagetable[i].valid, pagetable[i].frame, pagetable[i].dirty, pagetable[i].referenced);
    close(fd);
}

void cleanup() {
    printf("-------------------------\n");
    printf("Done all requests.\n");
    printf("MMU sends SIGUSR1 signal to the pager.\n");

    kill(pager_pid, SIGUSR1);

    double tlb_miss_ratio = 1.0 - (double) hits / accesses_count;
    printf("TLB Miss Ratio: %.4f\n", tlb_miss_ratio);

    printf("MMU terminates.\n");
    exit(0);
}

int main(int argc, char *argv[]) {
    struct sigaction sigact;

    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = 0;
    sigact.sa_handler = sigcont_handler;
    sigaction(SIGCONT, &sigact, NULL);

    int num_pages = atoi(argv[1]);

    pager_pid = atoi(argv[3]);
    str = argv[2];
    initialize_page_table(num_pages);

    signal(SIGUSR1, sigusr1_handler);
    signal(SIGCONT, sigcont_handler);


    char *token = strtok(str, " ");
    while (token != NULL) {
        process_memory_access(token);
        printf("Page table\n");
        for (int i = 0; i < num_pages; i++)
            printf("Page %d ---> valid=%d, frame=%d, dirty=%d, referenced=%d, counter=%d\n",
                   i, pagetable[i].valid, pagetable[i].frame, pagetable[i].dirty, pagetable[i].referenced,
                   pagetable[i].counter);

        token = strtok(NULL, " ");
    }

    cleanup();
}
