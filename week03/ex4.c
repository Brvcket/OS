#include <stdio.h>
#include <stddef.h>
#include <float.h>
#include <stdlib.h>

void *aggregate(void *base, size_t size, int n, void *initial_value, void *(*opr)(const void *, const void *)) {
    char *result = initial_value;
    void* prev_result;

    for (int i = 0; i < n; i++) {
        prev_result = result;
        result = opr(result, (char *)base + i * size);
        if (i == 0) continue;
        free(prev_result);
    }

    return result;
}

void *addition_ints(const void *a, const void *b) {
    int *result = malloc(sizeof(int));;
    *result = *(int *)a + *(int *)b;
    return result;
}

void *addition_doubles(const void *a, const void *b) {
    double *result = malloc(sizeof(double));
    *result = *(double *)a + *(double *)b;
    return result;
}

void *multiplication_ints(const void *a, const void *b) {
    int *result = malloc(sizeof(int));
    *result = *(int *)a * *(int *)b;
    return result;
}

void *multiplication_doubles(const void *a, const void *b) {
    double *result = malloc(sizeof(double));
    *result = *(double *)a * *(double *)b;
    return result;
}

void *max_ints(const void *a, const void *b) {
    int *result = malloc(sizeof(int));
    *result = (*(int *)a > *(int *)b) ? *(int *)a : *(int *)b;
    return result;
}

void *max_doubles(const void *a, const void *b) {
    double *result = malloc(sizeof(double));
    *result = (*(double *)a > *(double *)b) ? *(double *)a : *(double *)b;
    return result;
}

int main() {
    double double_array[5] = {1.0, 2.0, 3.0, 4.0, 5.0};
    int int_array[5] = {1, 2, 3, 4, 5};

    double addition_initial_value_double = 0.0;
    double multiplication_initial_value_double = 1.0;
    double max_initial_value_double = -DBL_MAX;
    int addition_initial_value_int = 0;
    int multiplication_initial_value_int = 1;
    int max_initial_value_int = INT_MIN;

    double *result_double_addition = aggregate(double_array, sizeof(double), 5, &addition_initial_value_double, addition_doubles);
    printf("Addition (double): %lf\n", *result_double_addition);
    free(result_double_addition);

    double *result_double_multiplication = aggregate(double_array, sizeof(double), 5, &multiplication_initial_value_double, multiplication_doubles);
    printf("Multiplication (double): %lf\n", *result_double_multiplication);
    free(result_double_multiplication);

    double *result_double_max = aggregate(double_array, sizeof(double), 5, &max_initial_value_double, max_doubles);
    printf("Max (double): %lf\n", *result_double_max);
    free(result_double_max);

    int *result_int_addition = aggregate(int_array, sizeof(int), 5, &addition_initial_value_int, addition_ints);
    printf("Addition (int): %d\n", *result_int_addition);
    free(result_int_addition);

    int *result_int_multiplication = aggregate(int_array, sizeof(int), 5, &multiplication_initial_value_int, multiplication_ints);
    printf("Multiplication (int): %d\n", *result_int_multiplication);
    free(result_int_multiplication);

    int *result_int_max = aggregate(int_array, sizeof(int), 5, &max_initial_value_int, max_ints);
    printf("Max (int): %d\n", *result_int_max);
    free(result_int_max);

    return 0;
}
