#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void* thread_function(void* arg) {

    printf("\nThread %lu is running\n", (unsigned long) pthread_self());
    return NULL;
}

int main() {
    pthread_t threads[3];

    for (int i = 0; i < 3; i++) {
        if (pthread_create(&threads[i], NULL, thread_function, NULL) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    for (int i = 0; i < 3; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("pthread_join");
            return 1;
        }
    }

    printf("All threads have executed");

    return 0;
}