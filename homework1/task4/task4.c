#include <stdio.h>

int main(void) {
    int number = 10;
    int *a = &number;
    int **b = &a;
    printf("\nValue of integer using single pointer: %d", *a);
    printf("\nValue of integer using double pointer: %d", **b);
    return 0;
}