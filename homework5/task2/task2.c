#include <stdio.h>
#include <omp.h>

#define NUM_ORDERS 10000
#define NUM_THREADS 4

typedef enum {
    PRIORITY_NORMAL,
    PRIORITY_HIGH
} Priority; // I wasn't sure if enum or a string should have been used, but enum is a better use
// this could also be done the same way as the previous assignment, with a matrix with 2 columns
// where 0 represents high, 1 represents normal

typedef struct {
    int order_id;
    float distance_km;
    Priority priority;
} Order;

int main(void) {
    Order orders[NUM_ORDERS];
    int thread_high_count[NUM_THREADS] = {0};
    float threshold = 0.0f;
    int i;

    // before doing any multithreading, we let our main thread to initialize orders
    // by default the orders have a normal priority
    for (i = 0; i < NUM_ORDERS; i++) {
        orders[i].order_id = i;
        orders[i].distance_km = (float)((i % 40) + 1); // distance ranges from 1 to 40 kilometers
        orders[i].priority = PRIORITY_NORMAL;
    }

    #pragma omp parallel num_threads(NUM_THREADS) shared(orders, threshold, thread_high_count)
    {
        int tid = omp_get_thread_num();
        int local_high_count = 0;

        // here we set the threshold which will define if an order is high or normal priority
        #pragma omp single
        {
            threshold = 20.0f;
        }

        // we let the 4 threads to classify the orders by their priority
        #pragma omp for
        for (int j = 0; j < NUM_ORDERS; j++) {
            if (orders[j].distance_km < threshold) {
                orders[j].priority = PRIORITY_HIGH;
            } else {
                orders[j].priority = PRIORITY_NORMAL;
            }
        }

        // we use a barrier to ensure that during the next step, 
        // all of the orders are processed
        #pragma omp barrier

        // we let one of our running threads to print the message of completion of priority assignment
        #pragma omp single
        {
            printf("Priority assignment is finished.\n");
        }

        // there are 4 threads and 10000 orders, so each thread is expected to do 2500 
        // iterations. So, each thread checks this number of orders, if priority is high,
        // increments the local variable(each thread has a local variable called local_high_count, so no race conditions can happen)
        // and then stores the result in their corresponding slot in the thread_high_count array.
        
        #pragma omp for
        for (int j = 0; j < NUM_ORDERS; j++) {
            if (orders[j].priority == PRIORITY_HIGH) {
                local_high_count++;
            }
        }

        thread_high_count[tid] = local_high_count;

        // we use a barrier to ensure that all threads have
        // finished calculating the number of high priority orders
        #pragma omp barrier

        // now, we let the final thread to perform the final calculations, outputting results.
        #pragma omp single
        {
            int total_high = 0;

            for (int t = 0; t < NUM_THREADS; t++) {
                // we print the high priority orders processed by each thread
                printf("thread %d processed %d high priority orders\n", t, thread_high_count[t]);
                // then we sum them up to show at the end the total number of high priority orders
                total_high += thread_high_count[t];
            }

            printf("Total high priority orders = %d\n", total_high);
        }
    }

    return 0;
}