/* Terminal emulation things */

/* stolen from mini-printf.c Copyright (c) 2013,2014 Michal Ludvig <michal@logix.cz> */
uint8_t mini_itoa(uint16_t value, uint8_t radix, uint8_t uppercase, uint8_t unsig, uint8_t *buffer, uint8_t zero_pad) {
    uint8_t    *pbuffer = buffer;
    uint8_t negative = 0;
    size_t    i, len;

    /* No support for unusual radixes. */
    if (radix > 16)
        return 0;

    if (value < 0 && !unsig) {
        negative = 1;
        value = -value;
    }

    /* This builds the string back to front ... */
    do {
        uint8_t digit = value % radix;
        *(pbuffer++) = (digit < 10 ? '0' + digit : (uppercase ? 'A' : 'a') + digit - 10);
        value /= radix;
    } while (value > 0);

    for (i = (pbuffer - buffer); i < zero_pad; i++)
        *(pbuffer++) = '0';

    if (negative)
        *(pbuffer++) = '-';

    *(pbuffer) = '\0';

    /* ... now we reverse it (could do it recursively but will
     * conserve the stack space) */
    len = (pbuffer - buffer);
    for (i = 0; i < len / 2; i++) {
        uint8_t j = buffer[i];
        buffer[i] = buffer[len-i-1];
        buffer[len-i-1] = j;
    }

    return len;
}

void print_slow(uint8_t *buffer, uint8_t ticks) {
    for (size_t i = 0; buffer[i] != 0; i++) {
        if (tick_delay > 0) {
            tick_delay(ticks);
        }
        print_char(buffer[i]);
    }
}

void print_fast(uint8_t *buffer) {
    print_slow(buffer, 0);
}

void print(uint8_t *buffer) {
    #ifdef DEBUG
    print_slow(buffer, 0);
    #else
    print_slow(buffer, 1);
    #endif
}

void print_number(uint16_t number) {
    uint8_t print_buffer[128];
    mini_itoa(number, 10, false, false, print_buffer, false);
    print(print_buffer);
}

void print_hex(uint16_t number) {
    uint8_t print_buffer[128];
    mini_itoa(number, 16, false, true, print_buffer, 8);
    print(print_buffer);
}

void print_byte(uint16_t number) {
    uint8_t print_buffer[128];
    mini_itoa(number, 16, true, true, print_buffer, 2);
    print(print_buffer);
}

void clear() {
    top_left();
    for (size_t x = 0; x < 80*25 + 79; x++) {
        print_char(' ');
    }
    top_left();
}

void screensaver() {
    while (true) {
        clear();
        uint32_t line = randint(0, 24);
        uint32_t col = randint(0, 44);

        for (size_t x = 0; x < line; x++) {
            print_fast("\r\n");
        }

        for (size_t y = 0; y < col; y++) {
            print_char(' ');
        }

        print_fast("Press any key to activate terminal.");

        if (soft_delay(5)) {  /* a key was pressed */
            break;
        }
    }
}

size_t strlen(uint8_t *string) {
    size_t length = 0;
    while(string[length] != 0) {
        length++;
    }
    return length;
}

/* not really a terminal function but what are you gonna do */
uint8_t match(uint8_t *first, uint8_t *second) {
    if(strlen(first) != strlen(second)) {
        return false;
    }
    for (size_t i = 0; first[i] != 0 && second[i] != 0; i++) {
        if (first[i] != second[i])
            return false;
    }
    return true;
}

uint8_t starts(uint8_t *first, uint8_t *second) {
    if(strlen(first) < strlen(second)) {
        return false;
    }
    for (size_t i = 0; first[i] != 0 && second[i] != 0; i++) {
        if (first[i] != second[i])
            return false;
    }
    return true;
}
