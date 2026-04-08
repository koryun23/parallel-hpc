#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <float.h>

#define N 50000000

int main() {
    double *A = malloc(sizeof(double) * N);

    if (!A) {
        perror("error");
        return 1;
    }

    // Initialize array
    for (int i = 0; i < N; i++) {
        A[i] = (double)rand();
    }

    double max_val = A[0];

    // this is similar to what we did in the previous assignment.
    // Each thread uses a local copy of max_val, calculates the max_val of its iteration
    // and then openmp calculates the max of these results by the definition we gave
    // in the reduction part.
    #pragma omp parallel for reduction(max:max_val)
    for (int i = 1; i < N; i++) {
        if (A[i] > max_val) {
            max_val = A[i];
        }
    }

    // Now having the max_val, we can calculate the threshold
    double T = 0.8 * max_val;

    double sum = 0.0;

    // We do something similar here, but instead of finding max, we sum up the results.
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < N; i++) {
        if (A[i] > T) {
            sum += A[i];
        }
    }

    printf("maximum of array = %f\n", max_val);
    printf("threshold = %f\n", T);
    printf("sum of elements greater than the threshold = %f\n", sum);

    free(A);
    return 0;
}