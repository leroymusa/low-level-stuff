void delay(int count) {
    for (int ii = 0; ii < count; ++ii) {
        for (int jj = 0; jj < 1000; ++jj) {
            __asm__ __volatile__("nop");
        }
    }
}

void kernel_main() {
    const char *str = "Hello from the kernel!";
    char *vidptr = (char*)0xb8000;
    unsigned int ii = 0;

    while (ii < 80 * 25 * 2) {
        vidptr[ii] = ' ';
        vidptr[ii+1] = 0x07;
        ii += 2;
    }
    ii = 0;
    while (str[ii] != '\0') {
        vidptr[ii*2] = str[ii];
        vidptr[ii*2+1] = 0x07;
        ii++;
    }
    delay(1000);
}
