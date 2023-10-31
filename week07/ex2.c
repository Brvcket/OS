#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <ctype.h>

#define MAX_FILE_SIZE 500 * 1024 * 1024
#define BUFFER_SIZE 500 * 1024

int main() {
    int text = open("text.txt", 02);
    if (text == -1) {
        perror("Unable to open text.txt");
        exit(1);
    }

    long page_size = sysconf(_SC_PAGESIZE);
    long chunk_size = 1024 * page_size;

    if (ftruncate(text, MAX_FILE_SIZE) == -1) {
        perror("ftruncate");
        exit(1);
    }

    char *map = mmap(NULL, MAX_FILE_SIZE, 0x2, 0x01, text, 0);
    if (map == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    int rand = open("/dev/random", 00);
    if (rand == -1) {
        perror("open");
        munmap(map, MAX_FILE_SIZE);
        exit(1);
    }

    char buffer[BUFFER_SIZE];
    int buffer_index = 0;
    for (int i = 0; i < MAX_FILE_SIZE; i++) {
        if (!buffer_index) {
            ssize_t bytes_read = read(rand, buffer, BUFFER_SIZE);
            if (bytes_read != -1) {
                buffer_index = bytes_read;
            } else {
                perror("read");
                munmap(map, MAX_FILE_SIZE);
                exit(1);
            }
        }

        char symbol = buffer[--buffer_index];
        isprint(symbol) ? map[i] = symbol : i--;

        if (!(i % 1024)) {
            map[i] = '\n';
        }
    }

    int capital_count = 0;
    for (off_t offset = 0; offset < MAX_FILE_SIZE; offset += chunk_size) {
        off_t size = chunk_size;
        if (offset + size > MAX_FILE_SIZE) {
            size = MAX_FILE_SIZE - offset;
        }

        map = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, text, offset);
        if (map == MAP_FAILED) {
            perror("mmap");
            exit(1);
        }

        for (off_t i = 0; i < size; i++) {
            char c = map[i];
            if (isupper(c)) {
                capital_count++;
                map[i] = tolower(c);
            }
        }

        if (munmap(map, size) == -1) {
            perror("munmap");
            exit(1);
        }
    }

    printf("Total capital letters: %d\n", capital_count);

    close(rand);
    close(text);

    return 0;
}
