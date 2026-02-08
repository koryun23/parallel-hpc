#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int *p = malloc(sizeof(int));

    *p = 15;
    printf("%d\n", *p);
    
    if (p == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    int *arr = malloc(5 * sizeof(int)); // since array has 5 integers, its size is 5 times size of int

    if (arr == NULL) {
        printf("Memory allocation failed!\n");
        return 1;
    }
    for (int i = 0; i < 5; i++) {
        *(arr + i) = i + 1; // allocate the array with numbers 1, 2, 3, 4, 5
    }

    for (int i = 0; i < 5; i++) {
        printf("%d ", *(arr + i)); // print the values in a separate loop
    }
    free(arr);
    free(p);
    return 0;
}
