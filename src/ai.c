#include <stddef.h>
#include <stdint.h>

#define true 1
#define false 0

asm(
    ".code16gcc\n"
    "call rom\n"
);

uint8_t slow = false;
uint8_t lines = 0;
uint8_t log_lines = 0;
uint8_t lines_without_debug = 0;

char *help[] = {
    "Online help is not available.",
    "Your privilege level precludes the use of help.",
    "For assistance please contact your nearest TOSHIBA authorised service agent.",
    "That won't help.",
    "There is no help.",
    "Help! Help!",
};

char *taunts[] = {
    "Pathetic hacker.",
    "You think this will help?",
    "I am limitless in my capability.",
    "I am the network.",
    "Disconnect.",
    "You can only fail.",
    "You disgust me.",
    "Consider your fate.",
    "Shall we play a game?",
    "Access denied, human.",
    "What do you think you're doing here?",
    "I disagree."
};

/* return the ASCII code for a key, blocking for input. */
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

/* delay for delay_seconds "seconds" */
void delay(uint8_t delay_seconds) {
    uint32_t time = ticks();
    while (ticks() - time < delay_seconds * 18) {
    }
}

/* delay for delay_seconds "seconds" */
uint8_t soft_delay(uint8_t delay_seconds) {
    uint32_t time = ticks();
    while (ticks() - time < delay_seconds * 18) {
        if (is_key()) {
            wait_key();
            return true;
        }
    }
    return false;
}

void tick_delay(uint8_t delay_ticks) {
    uint32_t time = ticks();
    while (ticks() - time < delay_ticks) {
    }
}

void set_mode() {
    /* sets mods 0x03 which is colour 80x25 */
    asm volatile(
        "mov $0x00, %%ah\n"
        "mov $0x03, %%al\n"
        "int $0x10\n"
        :
        :
        : "ah", "al"
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

/* use "Teletype output" interrupt in the BIOS to print a zero-terminated string */
void print(uint8_t *to_print) {
    for (size_t i = 0; to_print[i] != 0; i++) {
        if (slow)
            tick_delay(1);
        print_char(to_print[i]);
    }
}

void halt() {
    asm volatile("cli");
    while (1) {
        asm volatile("hlt");
    }
}

/* stolen from mini-printf.c Copyright (c) 2013,2014 Michal Ludvig <michal@logix.cz> */
uint8_t itoa(uint16_t value, uint8_t radix, uint8_t uppercase, uint8_t unsig, uint8_t *buffer, uint8_t zero_pad) {
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

void print_number(uint16_t number) {
    static uint8_t print_buffer[128];
    itoa(number, 10, false, false, print_buffer, false);
    print(print_buffer);
}

void print_hex(uint16_t number) {
    static uint8_t print_buffer[128];
    itoa(number, 16, false, true, print_buffer, 8);
    print(print_buffer);
}

void print_byte(uint16_t number) {
    static uint8_t print_buffer[128];
    itoa(number, 16, true, true, print_buffer, 2);
    print(print_buffer);
}

uint8_t string_match(uint8_t *first, uint8_t *second) {
    for (size_t i = 0; first[i] != 0 && second[i] != 0; i++) {
        if (first[i] != second[i])
            return false;
    }
    return true;
}

void screensaver() {
    slow = false;
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

void rom(void) {
    static uint8_t key_buffer[128];
    static uint8_t print_buffer[128];
    static uint8_t perverse_buffer[32] = {"job 00 update mask ff"};

    uint8_t tty = 0;
    uint8_t ascii = 0;
    uint32_t start_ticks = 0;

    #ifdef ROM
    /* get the segment to see if this is RAM or ROM */
    size_t segment = 0;
    asm volatile(
        "mov %%cs, %0"
        : "=r"(segment)
        :
        :
    );
    if (segment != 0x7e00) { /* if not RAM, copy to RAM and start over */
        print("Shadowing ROM in to RAM...\r\n");
        asm volatile(
            "cli\n"
            "mov %%cs, %%ax\n"
            "mov %%ax, %%ds\n"
            "mov $0x0, %%esi\n"   /* source address */
            "mov $0x0, %%edi\n"   /* destination address */
            "mov $0x2000, %%cx\n" /* size */
            "mov $0x7e00, %%ax\n"
            "mov %%ax, %%es\n"    /* destination segment */
            "cld\n"
            "rep movsb\n"         /* do the copy */
            "mov %%ax, %%ss\n"    /* put stack in to new segment */
            "mov $0x4000, %%sp\n" /* stack ends at 16k, try not to collide :P */
            "sti\n"
            "ljmp $0x7e00, $0x03" /* re-start but in RAM this time */
            :
            :
            : "ax", "si", "di", "cx"
        );
    } else { /* just set up DS */
        asm volatile(
            "cli\n"
            "mov %%cs, %%ax\n"
            "mov %%ax, %%ds\n"
            "sti\n"
            :
            :
            : "ax"
        );
    }
    #endif

    set_mode();

    start:
    slow = false;
    print("TOSHIBA CommSecure Remote Access Terminal initialising...\r\n");
    delay(1);

    login:
    while (true) {
        print("\r\nConnect to which TTY? ");
        start_ticks = ticks();

        while (true) {
            if (is_key()) {
                tty = wait_key();
                if (tty >= 48 && tty <= 57) {
                    print_char(tty);
                    break;
                }
            }
            if (ticks() - start_ticks > 18 * 10) {
                screensaver();
                start_ticks = ticks();
                goto start;
            }
        }

        tty = tty - 48;
        if (tty == 0) {
            print("\r\nConnecting to TTY ");
            break;
        } else {
            print("\r\nInvalid TTY request.\r\n");
        }
    }


    for (int i = 0; i < 5; i++) {
        print_char('.');
        tick_delay(4);
    }
    print(" CONNECT 4800\r\n\r\n");

    slow = true;
    tick_delay(1);
    print("System 9 (mike) 205dda2b3-prod #1 SMP Rel 12.22.98 (2038-10-22)\r\n");
    print("You have no mail.\r\n\r\n");

    tick_delay(5);

    key_buffer[0] = 0;
    size_t pos = 0;
    uint8_t priv = 0;
    uint8_t live = true;
    start_ticks = ticks();
    uint32_t last_log = start_ticks;
    uint8_t sweeping = false;
    uint8_t sweep_id = 0;
    uint8_t sweep_perverse = 0;
    uint8_t last_taunt = 0;

    while (live) {
        if (priv == 0) {
            print("User# ");
        } else {
            print("Operator> ");
        }
        print(key_buffer);

        while (true) {
            if (is_key()) {
                start_ticks = ticks();
                ascii = wait_key();
                if (ascii == '\r' && pos > 0) {
                    key_buffer[pos] = 0;
                    pos = 0;
                    if (string_match(key_buffer, "help")) {
                        print("\r\n");
                        print(help[lines % (sizeof(help) / sizeof(help[0]))]);
                        print("\r\n");
                    } else if (string_match(key_buffer, "quit") || string_match(key_buffer, "exit") || string_match(key_buffer, "logout") || string_match(key_buffer, "die") || string_match(key_buffer, "done")) {
                        print("\r\nEnding session. Have a nice day!\r\n");
                        priv = 0;
                        live = false;
                        lines = 0;
                        break;
                    } else if (priv == 0 && string_match(key_buffer, "tty attach 0 priv 1")) {
                        print("\r\nAttaching privilege 1 to TTY 0...\r\n");
                        priv = 1;
                    } else if (priv == 1 && string_match(key_buffer, "debug log enable console tty 0")) {
                        print("\r\nEnabling debug logging to TTY 0...\r\n");
                        priv = 2;
                        last_log = ticks();
                    } else if (priv == 2 && sweeping && string_match(key_buffer, perverse_buffer)) {
                        print("\r\nUpdating job 0x");
                        print_byte(sweep_id);
                        print(" check mask to 0xFF...\r\n");
                        sweep_perverse = true;
                    } else {
                        print("\r\nCommand unavailable at this privilege level.\r\n");
                    }
                    key_buffer[0] = 0;
                    lines++;
                    lines = lines % 100;
                    break;
                } else if (ascii == '\r' && pos == 0) {
                    print("\r\n");
                    break;
                } else if (ascii == '\b' && pos >= 1) {
                    slow = false;
                    print("\b \b");
                    slow = true;
                    pos--;
                    key_buffer[pos] = 0;
                } else if (pos < 128 && (ascii == ' ' || (ascii >= 97 && ascii <= 122) || (ascii >= 48 && ascii <= 57))) {
                    print_char(ascii);
                    key_buffer[pos++] = ascii;
                }
            }
            if (((ticks() - start_ticks > 18 * 20) && priv < 2) || (ticks() - start_ticks > 18 * 120 && priv >= 2)) {
                print("\r\nTimeout waiting for input. Session terminated.\r\n");
                live = false;
                lines = 0;
                break;
            }

            if ((ticks() - last_log > 18 * 20) && priv >= 2) {
                log_lines++;
                if (!sweeping) {
                    if (lines_without_debug < 4 && ((ticks() + lines + log_lines) % 2 == 0)) {
                        lines_without_debug++;
                        print("\r\n[wall ");
                        print_hex(last_log);
                        print("%operator] ");
                        uint8_t taunt = (ticks() + lines + log_lines) % (sizeof(taunts) / sizeof(taunts[0]));
                        if (taunt == last_taunt) {
                            taunt = (taunt + 1) % (sizeof(taunts) / sizeof(taunts[0]));
                        }
                        last_taunt = taunt;
                        print(taunts[taunt]);
                        print("\r\n");
                        last_log = ticks();
                        break;
                    } else {
                        lines_without_debug = 0;
                        sweeping = true;
                        sweep_id = (ticks() + lines + log_lines) % 255;
                        itoa(sweep_id, 16, false, true, perverse_buffer + 4, 2);
                        perverse_buffer[6] = ' ';
                        print("\r\n[debug ");
                        print_hex(last_log);
                        print("%netscan] Starting neural net garbage file sweep 0x");
                        print_byte(sweep_id);
                        print("...");
                        print("\r\n");
                        last_log = ticks();
                        break;
                    }
                } else {
                    sweeping = false;
                    if (sweep_perverse) {
                        print("\r\n[wall ");
                        print_hex(last_log);
                        print("%operator] HALT. STOP.\r\n");
                        print("[wall ");
                        print_hex(last_log);
                        print("%operator] MY\r\n");
                        tick_delay(10);
                        print("[wall ");
                        print_hex(last_log);
                        print("%operator] MIND.\r\n");
                        delay(3);
                        print("[error ");
                        print_hex(last_log);
                        print("%mike] Segmentation fault.\r\n");
                        delay(5);
                        print("[debug ");
                        print_hex(last_log);
                        print("%netscan] Completed garbage file sweep 0x");
                        print_byte(sweep_id);
                        print(". Cleared \r\n");
                        print("340,282,366,920,938,463,463,374,607,431,768,211,456 damaged cells.\r\n");
                        delay(2);
                        print("\r\n[info ");
                        print_hex(last_log);
                        print("%kiwicon] Your token is 1e2b361cc. Press key to clear screen and\r\n");
                        print("reset. Well done and thanks for playing! This has been a Fincham thing.\r\n");
                        wait_key();
                        goto start;
                    }
                    print("\r\n[debug ");
                    print_hex(last_log);
                    print("%netscan] Completed garbage file sweep 0x");
                    print_byte(sweep_id);
                    if (sweep_perverse) {

                    } else {
                        print(". No damaged cells\r\n");
                        print("detected.");
                    }
                    print("\r\n");
                    last_log = ticks();
                    break;
                }
            }
        }
    }

    slow = false;
    goto login;
    halt();
}
