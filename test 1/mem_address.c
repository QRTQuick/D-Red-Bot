// Software Name: mem address.c
#include <stdio.h>

int main() {
    // Variable declarations
    int x = 3000;
    int y = 2000;
    char x_y[] = "chisom";

    // Print the memory addresses
    printf("Memory address of x:   %p\n", (void*)&x);
    printf("Memory address of y:   %p\n", (void*)&y);
    printf("Memory address of x_y: %p\n", (void*)x_y);

    return 0;
}