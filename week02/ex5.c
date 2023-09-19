#include <stdio.h>

long long tribonacci(int n) {
    if (n == 0) return 0;
    if (n == 1 || n == 2) return 1;

    long long a = 0, b = 1, c = 1, temp;

    for (int i = 3; i <= n; i++) {
        temp = a + b + c;
        a = b;
        b = c;
        c = temp;
    }

    return c;
}

int main() {
    int n1 = 4;
    int n2 = 36;

    long long result1 = tribonacci(n1);
    long long result2 = tribonacci(n2);

    printf("Tribonacci(%d) = %lld\n", n1, result1);
    printf("Tribonacci(%d) = %lld\n", n2, result2);

    return 0;
}
