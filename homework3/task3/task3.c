#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define DEFAULT_SENSORS 5
#define READINGS_PER_SENSOR 10

typedef struct {
    int sensor_id;
    int readings_count;
    double *temperatures;
    double average;
} SensorData;

pthread_barrier_t barrier;

double generate_temperature(unsigned int *seed) {
    return -10.0 + (rand() / RAND_MAX) * 50.0;
}

void *sensor_thread(void *arg) {
    SensorData *data = (SensorData *)arg;
    srand(time(NULL));
    unsigned int seed = data->sensor_id;

    printf("Sensor %d started reading\n", data->sensor_id);

    for (int i = 0; i < data->readings_count; i++) {
        data->temperatures[i] = generate_temperature(&seed);

        printf("Sensor %d read temperature %.2f\n",
               data->sensor_id,
               data->temperatures[i]);
    }

    printf("Sensor %d finished reading\n",
           data->sensor_id);

    pthread_barrier_wait(&barrier);

    double sum = 0.0;

    for (int i = 0; i < data->readings_count; i++) {
        sum += data->temperatures[i];
    }

    data->average = sum / data->readings_count;

    printf("Sensor %d processed data. Average temperature: %.2f\n",
           data->sensor_id,
           data->average);

    return NULL;
}

int main(int argc, char *argv[]) {
    int sensor_count = DEFAULT_SENSORS;
    sensor_count = 5; // defaulting

    pthread_t *threads = malloc(sensor_count * sizeof(pthread_t));
    SensorData *sensor_data = malloc(sensor_count * sizeof(SensorData));

    if (threads == NULL || sensor_data == NULL) {
        printf("error\n");
        free(threads);
        free(sensor_data);
        return 1;
    }

    pthread_barrier_init(&barrier, NULL, sensor_count);

    for (int i = 0; i < sensor_count; i++) {
        sensor_data[i].sensor_id = i;
        sensor_data[i].readings_count = READINGS_PER_SENSOR;
        sensor_data[i].average = 0.0;

        sensor_data[i].temperatures = malloc(READINGS_PER_SENSOR * sizeof(double));

        if (sensor_data[i].temperatures == NULL) {
            printf("error\n", i);
            return 1;
        }

        pthread_create(&threads[i], NULL, sensor_thread, &sensor_data[i]);
    }

    for (int i = 0; i < sensor_count; i++) {
        pthread_join(threads[i], NULL);
    }

    double global_sum = 0.0;

    for (int i = 0; i < sensor_count; i++) {
        global_sum += sensor_data[i].average;
    }

    double global_average = global_sum / sensor_count;

    printf("average temperature from all sensors: %.2f\n", global_average);

    for (int i = 0; i < sensor_count; i++) {
        free(sensor_data[i].temperatures);
    }

    pthread_barrier_destroy(&barrier);

    free(threads);
    free(sensor_data);

    return 0;
}