#include <stdio.h>
#include <stdlib.h>

int const_tri(int *p, int n) {
    if (n <= 2) return p[2];
    for (int i = 3; i <= n; i++) {
        int temp = p[0] + p[1] + p[2];
        p[0] = p[1];
        p[1] = p[2];
        p[2] = temp;
    }
    return p[2];
}

int main() {
    const int x = 1;
    int *q = &x;
    int *p = (int *)malloc(sizeof(int) * 3);

    printf("Memory addresses of the cells:\n");
    for (int i = 0; i < 3; i++) {
        if (i < 2) p[i] = x;
        else p[i] = x * 2;
        printf("%p\n", &p[i]);
    }

    if (&p[1] - &p[0] == 1 && &p[2] - &p[1] == 1) {
        printf("The cells are contiguous.\n");
    } else {
        printf("The cells are not contiguous.\n");
    }

    printf("Tribonacci number for n = 10: %d\n", const_tri(p, 10));

    free(p);
    return 0;
}
