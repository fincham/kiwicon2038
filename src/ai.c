#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* define DEBUG to have VGA output sent to Bochs terminal instead, and no delays */
/* #define DEBUG */
#define bochs_break() __asm__ __volatile__("xchg %%bx, %%bx");
#define bochs_print_char(c) outportb(0xe9, c);

#define choice(selection) selection[randint(0, sizeof(selection) / sizeof(selection[0]) - 1)]
#define print_choice(selection) print(choice(selection)); print("\r\n");

#ifdef ROM
/* if this isn't already in RAM, shadow it to RAM and start over */
asm(
    ".code16gcc\n"
    "mov %cs, %ax\n"
    "cmp $0x7e00, %ax\n"    /* check the code segment to see if it's in RAM */
    "je 1f\n"               /* if it is, skip the copy */
    "mov %ax, %ds\n"
    "mov $0x0, %esi\n"      /* source address */
    "mov $0x0, %edi\n"      /* destination address */
    "mov $" ROM_BYTES ", %cx\n" /* size */
    "mov $0x7e00, %ax\n"
    "mov %ax, %es\n"        /* destination segment */
    "cld\n"
    "rep movsb\n"           /* do the copy */
    "mov %ax, %ss\n"        /* put stack in to new segment */
    "mov $0xc000, %sp\n"    /* stack ends at 48k, try not to collide! */
    "ljmp $0x7e00, $0x03\n" /* re-start but in RAM this time */
    "1:\n"
    "mov %cs, %ax\n"
    "mov %ax, %ds\n"
    "mov %ax, %es\n"
    "jmp game"
);
#else
asm(
    ".code16gcc\n"
    "jmp game"
);
#endif

uint8_t *help[] = {
    "Online help is not available.",
    "Your privilege level precludes the use of help.",
    "For assistance please contact your nearest System 9 authorised service agent.",
    "That won't help.",
    "There is no help.",
    "Help! Help!",
    "No.",
    "Not found?",
    "Try again!",
};

uint8_t *taunts[] = {
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

uint8_t *missing[] = {
    "You might need to install that first...",
    "?SYNTAX",
    "No such command.",
    "No such file.",
    "Not around here...",
    "O RLY?",
    "Please...",
    "Bad command or file name.",
    "glitsh: command not found",
    "I don't think you're ready.",
    "glitsh: bad interpreter: one of the worst.",
    "Unlucky!",
};

uint8_t *process[] = {
    "glitsh: background",
    "[kworker/u9:2]",
    "[irq/26-uhhh]",
    "(ld-soft)",
    "[ext-unibus-flip]",
    "[kcopycat]",
    "/lib/oil/temp",
    "iotop",
    "trundle",
    "bucketd",
    "lloyd-browser",
    "euston: cracking",
    "[kmfdmflush]",
    "php3 /var/www/admin.php",
    "gibson.sh",
    "dd if=/dev/zero of=/dev/sda bs=1M...",
    "wopr.out"
};

#include "random.c"
#include "intel.c"
#include "terminal.c"

void game(void) {
    uint8_t tty = 0;
    uint32_t start_ticks = 0; /* re-used to count the passing of time below */

    set_mode();

    start:
    clear();
    print_fast("TOSHIBA ROMSecure Remote Access Terminal initialising...\r\n");
    delay(1);

    login:
    while (true) {
        print_fast("\r\nConnect to which TTY? ");
        start_ticks = ticks();

        while (true) {
            random(); /* "seed" the "RNG" */
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
            print_fast("\r\nConnecting to TTY 0");
            break;
        } else {
            print_fast("\r\nInvalid TTY request.\r\n");
        }
    }

    for (int i = 0; i < 5; i++) {
        print_char('.');
        tick_delay(4);
    }
    print_fast(" CONNECT 4800\r\n\r\n");

    remote:
    tick_delay(1);
    print("System 9 (mycroft) 205dda2b3-prod #1 SMP Rel 12.22.98 (2038-10-22)\r\n");
    print("You have no mail.\r\n\r\n");

    tick_delay(5);

    /* variables for the game and terminal */

    uint8_t ascii = 0; /* ascii code of key last pressed */
    uint8_t key_buffer[128]; /* buffer that user input is ready in to */
    size_t key_buffer_pos = 0; /* position of input in the buffer */

    uint8_t priv = 0;  /* game privilege level */
    uint8_t live = true; /* whether the game terminal is running */

    uint8_t perverse_buffer[32] = {"job 00 update mask ff"}; /* matched to see if the user can kill the AI or not, 00 is replaced with a job code */
    uint32_t last_log = start_ticks; /* last time a debug log entry was printed during the end part of the game */
    uint8_t sweeping = false; /* whether the conditions for a "sweep" have been met, interval in which you can win */
    uint8_t sweep_id = 0; /* the ID which is subbed in to the perverse_buffer and user must type */
    uint8_t sweep_perverse = 0; /* whether the current sweep has been subject to perversion */
    size_t lines_without_debug = 0; /* how many non-winnable lines have passed, to make the game fair this is capped */

    key_buffer[0] = 0;
    start_ticks = ticks();

    /* repl loop */
    while (live) {
        /* display prompt and whatever the user has typed in to the buffer up until now */
        if (priv == 0) {
            print("User# ");
        } else {
            print("Operator> ");
        }
        print(key_buffer);

        while (true) {
            if (is_key()) {
                start_ticks = ticks(); /* re-set the idle timeout */
                ascii = wait_key();
                if (ascii == '\r' && key_buffer_pos > 0) { /* if enter is pressed and the buffer isn't empty */
                    key_buffer[key_buffer_pos] = 0;
                    key_buffer_pos = 0;

                    /* look for commands to respond to */
                    if (match(key_buffer, "help")) {
                        print("\r\n");
                        print(help[randint(0, sizeof(help) / sizeof(help[0]) - 1)]);
                        print("\r\n");
                    } else if (match(key_buffer, "quit") || match(key_buffer, "exit") || match(key_buffer, "logout") || match(key_buffer, "die") || match(key_buffer, "done")) {
                        print("\r\nEnding session. Have a nice day!\r\n");
                        priv = 0;
                        live = false;
                        break;
                    } else if (priv == 0 && match(key_buffer, "tty attach 0 priv 1")) {
                        print("\r\nAttaching privilege 1 to TTY 0...\r\n");
                        priv = 1;
                    } else if (priv == 1 && match(key_buffer, "debug log enable console tty 0")) {
                        print("\r\nEnabling debug logging to TTY 0...\r\n");
                        priv = 2;
                        last_log = ticks();
                    } else if (priv == 2 && sweeping && match(key_buffer, perverse_buffer)) {
                        print("\r\nUpdating job 0x");
                        print_byte(sweep_id);
                        print(" check mask to 0xFF...\r\n");
                        sweep_perverse = true;
                    } else if (match(key_buffer, "ps") || starts(key_buffer, "ps ")) {
                        print("\r\n  PID TTY          TIME CMD\r\n");
                        for (int i = 0; i<4; i++) {
                            print_number(randint(10000, 32767));
                            print(" ?        00:00:00 ");
                            print_choice(process);
                        }
                        print("10678 tty0     00:00:00 ps\r\n");
                        print("16161 tty0     00:00:01 glitsh\r\n");
                        start_ticks = ticks();
                    } else if (match(key_buffer, "glitsh")) {
                        print("\r\nglitter shell, version 6.6.12(1)-release (smp-nine-clap)\r\n");
                    } else if (match(key_buffer, "pwd")) {
                        print("\r\n/usr/pak0/technician\r\n");\
                    } else if (match(key_buffer, "ls") || match(key_buffer, "dir")) {
                        print("\r\ncore  passwords  README.NOW  secaudit\r\n");
                    } else if (match(key_buffer, "type passwords") || match(key_buffer, "cat passwords")) {
                        print("\r\n*** SECAUDIT EXCEPTION: Rogue access to passwordfile. Session terminated.\r\n");
                        priv = 0;
                        live = false;
                        break;
                    } else if (match(key_buffer, "type secaudit") || match(key_buffer, "cat secaudit")) {
                        print("\r\nglitsh: cat: 'secaudit' is a binary file and displaying it on this terminal\r\nwould be foolish in the extreme.\r\n");
                    } else if (starts(key_buffer, "secaudit")) {
                        print("\r\nSECAUDIT 1.1 Free Trial (c) 2038 SecAudit Corporation.\r\nThe premium System 9 security hardening tool!\r\n\r\nConduct thorough system scan [y/N]? ");
                        ascii = soft_delay(20);
                        if (ascii != 'y') {
                            print("\r\nScan cancelled.\r\n");
                        } else {
                            print_char('y');
                            print("\r\nConducting system integrity check [ ");
                            for (int i = 0; i < 100; i=i+5) {
                                print_char(178);
                                print_char(' ');
                                print_number(i);
                                print_char('%');
                                print_char(8);
                                print_char(8);
                                print_char(8);
                                if (i>= 10)
                                    print_char(8);
                                tick_delay(randint(10,20));
                            }
                            print(" ] 100%\r\n\r\n");
                            delay(2);
                            print("No serious threats or misconfigurations detected.\r\n");
                            print_number(randint(100,999));
                            print(" mild threats detected.\r\n\r\n");
                            delay(2);
                            print("Would you like to upgrade to SECAUDIT Pro to find out about\r\nyour mild threats [Y/n]? ");
                            ascii = soft_delay(20);
                            if (ascii != 'y' && ascii != 13) {
                                print("\r\nAre you sure? They're pretty bad threats. Upgrade [Y/n]? ");
                                ascii = soft_delay(20);
                                if (ascii != 'y' && ascii != 13) {
                                    print("\r\nWell! Don't say we didn't warn you.\r\n");
                                } else {
                                    goto accept;
                                }
                            } else {
                                accept:
                                print_char('y');
                                print("\r\n\r\nBuilding KickChain transaction for payment of ");
                                print_number(randint(20000, 65535));
                                print_char('G');
                                print_char(157); /* yen */
                                print(" to SECAUDIT CORP... ");
                                for (int i = 0; i < 10; i++) {
                                    print_char('/');
                                    print_char(8);
                                    tick_delay(5);
                                    print_char('-');
                                    print_char(8);
                                    tick_delay(5);
                                    print_char('\\');
                                    print_char(8);
                                    tick_delay(5);
                                    print_char('|');
                                    print_char(8);
                                    tick_delay(5);
                                }
                                print("\r\nInsufficient KickChain funds in Sender Account. Transaction terminated.\r\nTransaction ID:");
                                print_hex(randint(10024,8192));
                                print("\r\n");
                            }
                        }
                        start_ticks = ticks();
                    } else if (match(key_buffer, "cat /proc/cpuinfo")) {
                        print("\r\nprocessor   : 0\r\n");
                        print("vendor_id   : PossiblyARMTel\r\n");
                        print("cpu family  : 6\r\n");
                        print("model       : 66\r\n");
                        print("model name  : NestWrangler CPU @ 1.21GW\r\n");
                        print("\r\n");
                    } else if (starts(key_buffer, "cd ")) {
                        print("\r\nglitsh: cd: ");
                        print(key_buffer + 3);
                        print(": No no no no no. No.\r\n");
                    } else {
                        print("\r\n");
                        if (randint(1,3) > 1) {
                            print("Command unavailable at this privilege level.\r\n");
                        } else {
                            print_choice(missing);
                        }
                    }
                    key_buffer[0] = 0;
                    break;
                } else if (ascii == '\r' && key_buffer_pos == 0) { /* pressed enter with no buffer, just repeat the prompt */
                    print("\r\n");
                    break;
                } else if (ascii == '\b' && key_buffer_pos >= 1) { /* pressed backspace when there is something in the buffer */
                    print("\b \b");
                    key_buffer_pos--;
                    key_buffer[key_buffer_pos] = 0;
                } else if (key_buffer_pos < 128 && (ascii == '/' || ascii == '.' || ascii == ' ' || (ascii >= 97 && ascii <= 122) || (ascii >= 48 && ascii <= 57))) { /* pressed any other key, add it to the buffer if it's alphanumeric */
                    print_char(ascii);
                    key_buffer[key_buffer_pos++] = ascii;
                }
            }

            /* timeout normal session after 20 seconds since last keypress, admin session aftr 2 minutes */
            if (((ticks() - start_ticks > 18 * 20) && priv < 2) || (ticks() - start_ticks > 18 * 120 && priv >= 2)) {
                print_fast("\r\nTimeout waiting for input. Session terminated.\r\n");
                live = false;
                break;
            }

            if ((ticks() - last_log > 18 * 20) && priv >= 2) { /* every 20 seconds print either a "funny" log or start / end a sweep */
                if (!sweeping) { /* if a sweep is running jump directly to finishing the sweep, never log a joke */
                    if (lines_without_debug < 4 && (randint(0,1) == 0)) { /* 50% chance a given debug log entry is a sweep or a joke. at most 4 consecutive jokes before a sweep. */
                        lines_without_debug++;
                        print("\r\n[wall ");
                        print_hex(last_log);
                        print("%operator] ");
                        print(taunts[randint(0, sizeof(taunts) / sizeof(taunts[0]) - 1)]);
                        print("\r\n");
                        last_log = ticks();
                        break;
                    } else { /* start a sweep */
                        lines_without_debug = 0;
                        sweeping = true;
                        sweep_id = (randint(0, 255));
                        mini_itoa(sweep_id, 16, false, true, perverse_buffer + 4, 2);
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
                } else { /* finish a sweep */
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
                        print("%mycroft] Segmentation fault.\r\n");
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

    goto login;
    halt();
}
