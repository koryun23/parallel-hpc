#include <stdio.h>

int str_length(char *str) {
    int size = 0;
    while (*str) {
        size++;
        str++;
    }
    return size;
}

int main(void) {
    char text[] = "Some random text";
    char *p = text;

    while (*p) {
        printf("%c", *p);
        *p++;
    }
    printf("\n");

    printf("Size of the string: %d\n", str_length(text));
    return 0;
}
