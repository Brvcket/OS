#include <stdio.h>

void count(char str[], char c) {
    int n = 0;
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == c || str[i] == c + 32 || str[i] == c - 32) {
            n++;
        }
    }
    if (c >= 'A' && c <= 'Z') {
        c += 32;
    }
    printf("%c:%d", c, n);
}

void countAll(char str[]) {
    for (int i = 0; str[i] != '\0'; i++) {
        count(str, str[i]);
        if (str[i + 1] != '\0') {
            printf(", ");
        }
    }
}

int main() {
    char str[256];
    scanf("%s", str);
    countAll(str);
    return 0;
}
