#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <float.h>

#define N 50000000 
// when N is large enough, most of the time the minimum distance is 0. I suspect the reason
// is that the chance of adjacent numbers that are equal to each other is getting higher, thus
// the distance is becoming 0. I tried 500 instead of fifty mil, and got a number other than 0.
// I didn't include a screenshot because it's simply saying that the minimum diffrence is 0.

int main() {
    double *A = malloc(sizeof(double) * N);

    if (!A) {
        perror("Error");
        return 1;
    }
    // here we initialize the array
    for (int i = 0; i < N; i++) {
        A[i] = (double)rand();
    }

    double min_diff = A[0];

    #pragma omp parallel for reduction(min:min_diff)
    for (int i = 2; i < N; i++) { 
        // each iteration technically should depend on each other, but because
        // of reduction, each thread uses its copy of min_diff, thus no race condition occurs.
        // when all threads complete their calculations, a merge of results happens, as we 
        // told openmp in the reduction's definition, and it calculates the minimum of these.
        double diff = fabs(A[i] - A[i-1]);
        if (diff < min_diff) {
            min_diff = diff;
        }
    }

    printf("Minimum difference: %f\n", min_diff);

    free(A);
    return 0;
}