#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>

// primality test ( from ex3 )
bool is_prime(int n) {
    if (n <= 1) return false;
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return false;
    }
    return true;
}

// The mutex
pthread_mutex_t global_lock = PTHREAD_MUTEX_INITIALIZER;


int k = 0;
int c = 0;

// input from command line
int n = 0;

// get next prime candidate
int get_number_to_check() {
    int ret;
    pthread_mutex_lock(&global_lock);
    ret = k;
    if (k != n) {
        k++;
    }
    pthread_mutex_unlock(&global_lock);
    return ret;
}

// increase prime counter
void increment_primes() {
    pthread_mutex_lock(&global_lock);
    c++;
    pthread_mutex_unlock(&global_lock);
}


// start_routine
void *check_primes(void *arg) {
    while (1) {
        int num_to_check = get_number_to_check();
        if (num_to_check >= n) {
            break;
        }
        if (is_prime(num_to_check)) {
            increment_primes();
        }
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <range_n> <number_of_threads>\n", argv[0]);
        return 1;
    }

    n = atoi(argv[1]);
    int m = atoi(argv[2]);

    pthread_t threads[m];

    int t_id[m];
    
    for (int i = 0; i < m; i++) {
        t_id[i] = i;
        pthread_create(&threads[i], NULL, check_primes, &t_id[i]);
    }

    for (int i = 0; i < m; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Total prime numbers found: %d\n", c);

    return 0;
}
