#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#define N 100000000
#define BINS 256

static void init_array(unsigned char *A) {
    for (long i = 0; i < N; i++) {
        A[i] = rand() % 256;
    }
}

static void print_histogram_summary(const int hist[BINS]) {
    // we print the total count of the elements, because I'm not sure what needed to be printed.
    // the histogram though is being correctly created.
    long long total = 0;
    for (int i = 0; i < BINS; i++) {
        total += hist[i];
    }
    printf("total count = %lld\n", total);
}

int main(void) {
    unsigned char *A = malloc((size_t)N * sizeof(unsigned char));
    if (!A) {
        printf("error");
        return 1;
    }

    init_array(A);

    // three different histograms for each method
    int hist_naive[BINS] = {0};
    int hist_critical[BINS] = {0};
    int hist_reduction[BINS] = {0};

    // 1) Naive parallel version, race condition is expected because the operation
    // being performed in the loop is not atomic, thus unexpected results may happen.
    // in the screenshot, it's visible that the total count is something unexpected,
    // exactly because of the race condition.
    #pragma omp parallel for
    for (long i = 0; i < N; i++) {
        hist_naive[A[i]]++;   // race condition
    }
    print_histogram_summary(hist_naive);

    // 2) Version using critical
    // this is a better solution but because of synchronization
    // which takes long time, it lasts longer. However, at the very least, no race condition happens.
    // I didn't fix time in the output, but when running, there was a significant 
    // difference between these two executions. It took several seconds for this version to complete, while the reduction one was almost isntant.
    #pragma omp parallel for
    for (long i = 0; i < N; i++) {
        #pragma omp critical
        {
            hist_critical[A[i]]++;
        }
    }
    print_histogram_summary(hist_critical);

    // 3) Version using reduction
    // this version is better. We parallelize the whole process
    // but instead of naive version, we make each thread process local
    // copies of the histogram, thus avoiding race condition, because
    // threads don't share any objects. After all threads finish their calculations,
    // we simply add, or merge results at the end (or rather openmp, not us), and
    // it works faster because it's done only at the end with some internal optimizations.
    #pragma omp parallel for reduction(+:hist_reduction[:BINS])
    for (long i = 0; i < N; i++) {
        hist_reduction[A[i]]++;
    }
    print_histogram_summary(hist_reduction);

    free(A);
    return 0;
}