#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

typedef struct {
    int id;
} PlayerArg;

static pthread_barrier_t lobby_barrier;

void *player_thread(void *arg) {
    PlayerArg *player = (PlayerArg *)arg;
    int id = player->id;

    int wait_time = rand() % 5 + 1;
    printf("Player %d is getting ready... (%d sec)\n", id + 1, wait_time);
    sleep(wait_time);

    printf("Player %d is ready and waiting in the lobby.\n", id + 1);

    int rc = pthread_barrier_wait(&lobby_barrier);

    if (rc == PTHREAD_BARRIER_SERIAL_THREAD) {
        printf("\nAll players are ready. Game Started!\n\n");
    }

    printf("Player %d entered the game.\n", id + 1);

    return NULL;
}

int main(void) {
    int n_players;

    printf("Enter number of players: ");
    scanf("%d", &n_players);

    if (n_players <= 0) {
        fprintf(stderr, "Number of players must be positive.\n");
        return 1;
    }

    pthread_t *threads = malloc((size_t)n_players * sizeof(pthread_t));
    PlayerArg *args = malloc((size_t)n_players * sizeof(PlayerArg));

    if (!threads || !args) {
        fprintf(stderr, "Memory allocation failed.\n");
        free(threads);
        free(args);
        return 1;
    }

    srand((unsigned)time(NULL));

    if (pthread_barrier_init(&lobby_barrier, NULL, (unsigned)n_players) != 0) {
        fprintf(stderr, "Barrier initialization failed.\n");
        free(threads);
        free(args);
        return 1;
    }

    for (int i = 0; i < n_players; i++) {
        args[i].id = i;
        if (pthread_create(&threads[i], NULL, player_thread, &args[i]) != 0) {
            fprintf(stderr, "Failed to create thread %d\n", i);
            pthread_barrier_destroy(&lobby_barrier);
            free(threads);
            free(args);
            return 1;
        }
    }

    for (int i = 0; i < n_players; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&lobby_barrier);
    free(threads);
    free(args);

    return 0;
}