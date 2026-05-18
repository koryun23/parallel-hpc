#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

typedef struct {
    int playerId;
} player_data;

static int* rolls = NULL;
static int* wins = NULL;
static int n_rounds = 5;
static int n_players = 4;
static pthread_mutex_t game_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_barrier_t round_barrier;

void* player_thread(void* arg) {
    player_data* arg_data = (player_data*) arg;

    for (int i = 0; i < n_rounds; i++) {
        pthread_mutex_lock(&game_mutex);
        int roll = rand() % 6 + 1;
        rolls[arg_data->playerId] = roll;
        printf("\nPlayer %d rolled %d\n", arg_data->playerId, roll);
        pthread_mutex_unlock(&game_mutex);

        pthread_t rc = pthread_barrier_wait(&round_barrier);

        if (rc == 1) {
            int max_roll = 0;
            int winner_index = 0;
            for (int i = 0; i < n_players; i++) {
                if (rolls[i] > max_roll) {
                    max_roll = rolls[i];
                    winner_index = i;
                }
            }
            wins[winner_index]++;
            printf("\nWinner of current round is player %d, with a roll %d!\n", winner_index, max_roll);
        }

        pthread_barrier_wait(&round_barrier);

    }

    return NULL;
}

int main() {

    rolls = malloc(n_players * sizeof(int));
    wins = malloc(n_players * sizeof(int));

    pthread_t* threads = malloc(n_players * sizeof(pthread_t));
    player_data* data = malloc(n_players * sizeof(player_data));

    if (!rolls || !wins || !threads || !data) {
        perror("error");
        free(rolls);
        free(wins);
        free(threads);
        free(data);
        exit(1);
    }

    if (pthread_barrier_init(&round_barrier, NULL, n_players) != 0) {
        perror("error");
        free(rolls);
        free(wins);
        free(threads);
        free(data);
        exit(1);
    }

    for (int i = 0; i < n_players; i++) {
        data[i].playerId = i;
        if(pthread_create(&threads[i], NULL, player_thread, &data[i]) != 0) {
            perror("error");
            free(threads);
            free(data);
            free(rolls);
            free(wins);
            exit(1);
        }
    }

    for (int i = 0; i < n_players; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("error");
            free(threads);
            free(data);
            free(rolls);
            free(wins);
            exit(1);
        }
    }

    int max_wins = wins[0];
    for (int i = 0; i < n_players; i++) {
        if (wins[i] > max_wins) {
            max_wins = wins[i];
        }
    }
    printf("Final results:\n");
    for (int i = 0; i < n_players; i++) {
        printf("Player %d won %d round(s)\n", i + 1, wins[i]);
    }

    printf("\nOverall winner(s): ");
    for (int i = 0; i < n_players; i++) {
        if (wins[i] == max_wins) {
            printf("Player %d ", i + 1);
        }
    }
    printf("with %d win(s)\n", max_wins);

    pthread_barrier_destroy(&round_barrier);
    pthread_mutex_destroy(&game_mutex);
    free(rolls);
    free(wins);
    free(threads);
    free(data);

    return 0;
}