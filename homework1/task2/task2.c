#include <stdio.h>

int main(void) {
    int arr[5] = {1, 3, 5, 7, 9};

    int *p = &arr[0];  

    printf("Traversing the array via pointers:\n");
    for (int i = 0; i < 5; i++) {
        printf("Value at index %d: %d\n", i, *(p + i));
    }

    for (int i = 0; i < 5; i++) {
        *(p + i) *= 10;
    }

    printf("\nModified array via pointer:\n");
    for (int i = 0; i < 5; i++) {
        printf("%d ", *(p + i));
    }
    printf("\n");

    printf("Modified array, standard traversal:\n");
    for (int i = 0; i < 5; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");

    return 0;
}
