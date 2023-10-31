#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <sys/resource.h>

#define MEMORY_SIZE 1000 * 1024 * 1024

int main() {
    void *p[10];
    struct rusage usage;
    for (int i = 0; i < 10; i++) {
        p[i] = malloc(MEMORY_SIZE);
        memset(p[i], 0, MEMORY_SIZE);
        
        getrusage(RUSAGE_SELF, &usage);
        printf("Memory usage: %ld KB\n", usage.ru_maxrss);
        
        sleep(1);
    }
    
    for (int i = 0; i < 10; i++){
        free(p[i]);
    }
}
