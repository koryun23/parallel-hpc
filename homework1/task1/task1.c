#include <stdio.h>

int main(void) {

    int x = 23;
    int *ptr = &x;
    printf("\nAddress of integer variable %p", &x);
    printf("\nAddress of pointer %p", ptr); // same address
    printf("\nValue via variable %d", x); // value is 23
    printf("\nValue via pointer %d", *ptr); // value is 23
    *ptr = 43;
    printf("\nNew address of integer variable %p", &x); // Address doesn't change
    printf("\nNew address of integer pointer %p", ptr); // Address doesn't change
    printf("\nNew value of integer variable %d", x); // value is now 43
    printf("\nNew value of integer variable via pointer %d\n", *ptr); // value is now 43
    return 0;
}