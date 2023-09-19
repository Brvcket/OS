#include <stdio.h>
#include <string.h>

#define MAX_STRING_SIZE 256

int main() {
    char input[MAX_STRING_SIZE];
    char c;
    int length = 0;

    printf("Enter a string character by character (maximum %d characters):\n", MAX_STRING_SIZE);

    while (1) {
        c = getchar();

        if (c == '.' || c == '\n') {
            break;
        }

        if (length < MAX_STRING_SIZE - 1) {
            input[length++] = c;
        }
    }

    input[length] = '\0';

    printf("\"");
    for (int i = length - 1; i >= 0; i--) {
        putchar(input[i]);
    }
    printf("\"\n");

    return 0;
}
