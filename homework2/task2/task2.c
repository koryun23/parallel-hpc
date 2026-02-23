#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int low;
    int high;
    int* numbers;
} thread_data_t;

int* create_array(int size) {
    int* arr = malloc((size_t)size * sizeof(int));
    if (!arr) {
        perror("malloc");
        exit(1);
    }
    for (int i = 0; i < size; i++) {
        arr[i] = i; // fill with 0..size-1
    }
    return arr;
}

void* calculate_sum(void* arg) {
    thread_data_t* data = (thread_data_t*)arg;
    int low = data->low;
    int high = data->high;
    int* arr = data->numbers;

    long long* result = malloc(sizeof(long long));
    if (!result) {
        perror("malloc");
        pthread_exit(NULL);
    }

    long long sum = 0;
    for (int i = low; i < high; i++) {
        sum += arr[i];
    }

    *result = sum;
    return result; // returned via pthread_join
}

int main(void) {
    int numbers_amount = 50000000;
    int thread_amount  = 50;

    int* arr = create_array(numbers_amount);

    pthread_t* threads = malloc((size_t)thread_amount * sizeof(pthread_t));
    thread_data_t* data = malloc((size_t)thread_amount * sizeof(thread_data_t));
    if (!threads || !data) {
        perror("malloc");
        free(arr);
        free(threads);
        free(data);
        return 1;
    }

    int chunk_size = numbers_amount / thread_amount;

    for (int i = 0; i < thread_amount; i++) {
        data[i].low = i * chunk_size;
        data[i].high = (i == thread_amount - 1) ? numbers_amount : (i + 1) * chunk_size; // last takes remainder
        data[i].numbers = arr;

        if (pthread_create(&threads[i], NULL, calculate_sum, &data[i]) != 0) {
            perror("pthread_create");
            free(arr);
            free(threads);
            free(data);
            return 1;
        }
    }

    long long total = 0;
    for (int i = 0; i < thread_amount; i++) {
        void* ret = NULL;
        if (pthread_join(threads[i], &ret) != 0) {
            perror("pthread_join");
            free(arr);
            free(threads);
            free(data);
            return 1;
        }

        long long* partial = (long long*)ret;
        if (partial) {
            total += *partial;
            free(partial);
        }
    }

    printf("Total sum using multithreading: %lld\n", total);

    free(arr);
    free(threads);
    free(data);
    return 0;
}