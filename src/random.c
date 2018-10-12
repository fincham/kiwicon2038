/* A "random" number generator */

uint16_t random() {
    static uint16_t lfsr = 0x1337;

    uint8_t lsb = lfsr & 1;
    lfsr >>= 1;
    if (lsb) {
        lfsr ^= 0xB400;
    }

    return lfsr;
}

uint16_t randint(uint16_t start, uint16_t end) {
    /* this is an unsafe way to implement randint, it is biased! */
    return start + random() % (end - start + 1);
}
