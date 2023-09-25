#include <stdio.h>
#include <math.h>

struct Point {
    double x;
    double y;
};

double distance(struct Point A, struct Point B) {
    return sqrt(pow(A.x - B.x, 2) + pow(A.y - B.y, 2));
}

double area(struct Point A, struct Point B, struct Point C) {
    return fabs(A.x * B.y + B.x * C.y + C.x * A.y - A.y * B.x - B.y * C.x - C.y * A.x) / 2;
}

int main() {
    struct Point A = {2.5, 6};
    struct Point B = {1, 2.2};
    struct Point C = {10, 6};

    printf("Distance between A and B: %lf\n", distance(A, B));
    printf("The area of ABC: %lf\n", area(A, B, C));
}
