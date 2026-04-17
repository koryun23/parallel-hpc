#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

long long fib(int n) {
    // base case 1: for n = 0 or n = 1, the return value is 1
    if (n <= 1) {
        return n;
    }

    // not exactly a base case, but still sort of similar
    // if n is less than the defined threshold, we calculate the fibonacci number
    // with regular recursion, without any parallelization
    if (n <= 10) {
        return fib(n - 1) + fib(n - 2);
    }

    // a fibonacci number is essentially sum of two numbers.
    // we first allocate space for these two numbers.
    long long x; 
    long long y;

    // we create two openmp tasks for calculating the two numbers to be 
    // added to each other in a concurrent way.

    #pragma omp task shared(x)
    x = fib(n - 1);

    #pragma omp task shared(y)
    y = fib(n - 2);

    // this is important - we wait until both x and y are calculated, and only then proceed with adding them to each other.
    #pragma omp taskwait
    
    return x + y;
}

long long sequential_recursive_fibnacci(int n) {

    if (n <= 1) {
        return n;
    }

    return sequential_recursive_fibnacci(n - 1) + sequential_recursive_fibnacci(n - 2);
}
int main(void) {
    int num = 38; // I am too lazy to do an input so I just initialized it, sorry
    long long result;

    result = sequential_recursive_fibnacci(num); // let's first calculate with regular recursion
    printf("result is %lld\n", result);

    // we want to have multiple threads to execute the tasks
    // created inside the recursive function in concurrent manner.
    #pragma omp parallel
    {
        // the design we are seeking to achieve is the following - 
        // one thread makes a call to this function, while the other 
        // threads that remain without a work, start executing omp tasks
        // that are being created in recursive function.
        #pragma omp single
        {
            result = fib(num);
        }
    }

    printf("result is %lld\n", result);

    // I was curious to see how much faster my solution with omp tasks will be
    // compared to regular recursion. However, 
    // what I noticed is that the regular recursive execution was actually faster
    // than my concurrent solution. This makes me think I did something wrong. 

    return 0;
}