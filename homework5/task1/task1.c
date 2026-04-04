#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define NUM_LOGS 20000
#define NUM_THREADS 4

typedef struct {
    int request_id;
    int user_id;
    int response_time_ms;
} LogEntry;

int main(void) {

    LogEntry logs[NUM_LOGS]; // we initialize an empty array of logs to later populate

    // we initialize a matrix called counts
    // it has 4 rows, each representing one thread, and 3 columns, each representing the type of log(slow, medium, fast)
    // column index is 0 - fast, 1 - medium, 2 - slow
    // so, for example, counts[1][1] represents the number of medium logs processed by thread with id 1
    int counts[NUM_THREADS][3] = {0};

    #pragma omp parallel num_threads(NUM_THREADS)
    {
        int tid = omp_get_thread_num();

        // critical section - one thread is entering it
        #pragma omp single
        {
            for (int i = 0; i < NUM_LOGS; i++) {
                logs[i].request_id = i + 1;
                logs[i].user_id = i;
                logs[i].response_time_ms = rand() % 700; // so response time is from 0 to 699 ms
            }
        }

        // one of the threads is executing the for loop, all the threads are
        // waiting until it finishes and reaches this barrier
        #pragma omp barrier

        // this for loop is for editing the count of fast/medium/slow logs of the threads.
        // since the iterations of the loop do not depend on one another, this is a safe and optimal use case.
        #pragma omp for
        for (int i = 0; i < NUM_LOGS; i++) {
            int rt = logs[i].response_time_ms;

            if (rt < 100) {
                counts[tid][0]++; // fast
            } else if (rt <= 300) {
                counts[tid][1]++; // medium
            } else {
                counts[tid][2]++; // slow
            }
        }

        // again, we wait until all threads finish their execution
        // to make sure that all logs were processed
        #pragma omp barrier

        // here, we let only one of the threads to do the final calculations based on the
        // counts matrix, and we print the final result
        #pragma omp single
        {
            int fast = 0;
            int medium = 0; 
            int slow = 0;

            for (int t = 0; t < NUM_THREADS; t++) {
                fast += counts[t][0];
                medium += counts[t][1];
                slow += counts[t][2];
            }

            printf("FAST: %d\n", fast);
            printf("MEDIUM: %d\n", medium);
            printf("SLOW: %d\n", slow);
        }
    }

    return 0;
}