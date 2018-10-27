/* Intel-specific BIOS stuff */

void outportb(uint16_t port, uint8_t val) {
    asm volatile(
        "outb %%al, %%dx"
        :
        : "d" (port), "a" (val)
    );
}

void outportw(uint16_t port, uint16_t val) {
    asm volatile(
        "outw %%ax, %%dx"
        :
        : "d" (port), "a" (val)
    );
}

void set_mode() {
    /* sets mods 0x03 which is colour text @ 80x25 */
    asm volatile(
        "mov $0x00, %%ah\n"
        "mov $0x03, %%al\n"
        "int $0x10\n"
        :
        :
        : "ah", "al"
    );

    asm volatile(
        "mov $0x01, %%ah\n"
        "mov $0x07, %%cx\n"
        "int $0x10\n"
        :
        :
        : "ah", "cx"
    );
}

/* move cursor to 0,0 */
void top_left() {
    asm volatile(
        "mov $0x02, %%ah\n"
        "mov $0x00, %%bh\n"
        "mov $0x00, %%dh\n"
        "mov $0x00, %%dl\n"
        "int $0x10\n"
        :
        :
        : "ah", "bh", "dh", "dl"
    );
}

void print_char(uint8_t character) {
    #ifdef DEBUG
    bochs_print_char(character);
    #endif
    asm volatile(
        "mov $0x0e, %%ah\n"
        "mov $0x00, %%bh\n"
        "int $0x10\n"
        :
        : "a"(character)
        : "bh"
    );
}

void print_ob_char(uint8_t character) {
    #ifdef DEBUG
    bochs_print_char(character);
    #endif
    if (character >= 96 && character <= 121) {
        character = character + 1;
    }
    asm volatile(
        "mov $0x0e, %%ah\n"
        "mov $0x00, %%bh\n"
        "int $0x10\n"
        :
        : "a"(character)
        : "bh"
    );
}

/* return the ASCII code for a key. if no key is in the buffer, block */
uint8_t wait_key() {
    uint8_t ascii = 0;

    asm volatile(
        "mov $0x00, %%ah\n"
        "int $0x16"
        : "=a"(ascii)
        :
        :
    );

    return ascii;
}

/* returns true if there is a key in the buffer */
uint8_t is_key() {
    uint8_t flag = 0;

    asm volatile(
        "mov $0x01, %%ah\n"
        "int $0x16\n"
        "sete %0"
        : "=a"(flag)
        :
        :
    );

    return !flag;
}

/* get ticks elapsed since midnight */
uint32_t ticks() {
    uint16_t low_time = 0;
    uint16_t high_time = 0;
    uint16_t midnight = 0;

    asm volatile(
        "mov $0x00, %%ah\n"
        "int $0x1a"
        : "=c"(high_time), "=d"(low_time), "=a"(midnight)
        :
        :
    );

    return (high_time << 16 ) | (low_time & 0xffff);
}

void halt() {
    asm volatile("cli");
    while (1) {
        asm volatile("hlt");
    }
}
