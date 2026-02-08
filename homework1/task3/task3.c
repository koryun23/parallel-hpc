#include <stdio.h>

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int main(void) {
    int firstNumber = 10;
    int secondNumber = 23;

    printf("First number before swapping: %d\n", firstNumber);
    printf("Second number before swapping: %d\n", secondNumber);
    swap(&firstNumber, &secondNumber);
    printf("First number after swapping: %d\n", firstNumber);
    printf("Second number after swapping: %d\n", secondNumber);
    return 0;
}

