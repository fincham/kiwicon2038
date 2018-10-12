/* Terminal emulation things */

void print(uint8_t *buffer) {
    for (size_t i = 0; buffer[i] != 0; i++) {
        print_char(buffer[i]);
    }
}

uint8_t string_match(uint8_t *first, uint8_t *second) {
    for (size_t i = 0; first[i] != 0 && second[i] != 0; i++) {
        if (first[i] != second[i])
            return false;
    }
    return true;
}

void screensaver() {
    for (size_t x = 0; x < 25; x++) {
        print("\r\n");
    }
    while (true) {
        uint32_t line = ticks() % 24;
        uint32_t col = ticks() % 44;

        for (size_t x = 0; x < line; x++) {
            print("\r\n");
        }

        for (size_t y = 0; y < col; y++) {
            print_char(' ');
        }

        print("Press any key to activate terminal.");

        for (size_t x = 25 - line; x > 0; x--) {
            print("\r\n");
        }

        if (soft_delay(5)) {  /* a key was pressed */
            break;
        }
    }
}
