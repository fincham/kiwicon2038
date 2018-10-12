#include <stdint.h>
#include <stdio.h>
#include "random.c"

void main() {
    uint16_t period = 0;
    uint16_t last = 0;

    while (1) {
        printf("%i\n", randint(0,1));
    }
}
