#include <stdio.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define FILE_SIZE 500 * 1024 * 1024
#define BUFFER_SIZE 500 * 1024

int main() {
    int text = open("text.txt", 0102, 0644);
    if (text == -1) {
        perror("Unable to create/open text.txt");
        return 1;
    }
    int rand = open("/dev/random", 00);
    if (rand == -1) {
        perror("Unable to open /dev/random");
        return 1;
    }

    ftruncate(text, FILE_SIZE);

    char *map = (char *) mmap(NULL, FILE_SIZE, 0x1 | 0x2, 0x01, text, 0);

    int capital_letters_count = 0;
    char buffer[BUFFER_SIZE];
    int char_count = 0;

    while (char_count < FILE_SIZE) {
        char symbol;

        if (char_count % 1024 == 0) {
            map[char_count++] = '\n';
        } else if (read(rand, buffer, sizeof(buffer)) > 0) {
            for (int i = 0; i < sizeof(buffer); i++) {
                symbol = buffer[i];
                if (char_count >= FILE_SIZE) {
                    break;
                }
                if (isprint(symbol)) {
                    if (isupper(symbol)) {
                        capital_letters_count++;
                        map[char_count++] = tolower(symbol);
                    } else {
                        map[char_count++] = symbol;
                    }
                }
            }
        }
    }

    printf("Total number of capital letters in the file is: %i\n", capital_letters_count);

    munmap(map, FILE_SIZE);

    close(text);
    close(rand);

    return 0;
}
