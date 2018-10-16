/* 

This is the bootloader for the "backup" version of the game which can boot off a hard drive / floppy. 
Most likely one of my old Thinkpads. 

*/

.code16
.text
.global _start
_start:
/* set up segment registers */
xor %ax, %ax
mov %ax, %ds

/* print a reassuring message */
mov $string, %si
mov $0x0e, %ah
print:
lodsb
or %al, %al
jz load
int $0x10
jmp print

/* read the game from disk and jump to it */
load:
mov $0x02, %ah      /* "read from disk" */
mov $0x20, %al      /* 16k */
mov $0x7e00, %bx    /* in to segment 0x7e00 */
mov %bx, %es
mov $0x03, %bx      /* at offset 0x03 (like an option ROM) */
xor %dh, %dh
mov $0x0002, %cx    /* skip the MBR when reading from disk */
int $0x13

ljmp $0x7e00, $0x03 /* jump to the game */

string:
.asciz "Reading from disk..."

.space 510-(.-_start)
.word 0xaa55
