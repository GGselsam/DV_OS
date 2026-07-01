#include "vga.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY ((volatile vga_char*)0xB8000)

/*video mem & cursor*/
static volatile vga_char* video = (vga_char*)0xB8000;
static int cursor = 0;

/*clear entire screen*/
void vga_clear(){
    for (int i = 0; i < 80 * 25; i++) {
        video[i].character = ' ';
        video[i].color = COLOR_WHITE | (COLOR_BLACK << 4);
    }
    cursor = 0;
}

/*print single character*/
void vga_putchar(char c, uint8_t color) {
    if (c == '\n') {
        cursor = (cursor / 80 +1) * 80;
        return;
    }

    video[cursor].character = c;
    video[cursor].color = color;
    cursor++;

    /*scroll if needed*/
    if (cursor >= 80 * 25) {
        for (int i = 0; i < 80 * 24; i++){
            video[i] = video[i+80];
        }
        for (int i = 80 * 24; i < 80 * 25; i++){
            video[i].character = ' ';
             video[i].color = COLOR_WHITE | (COLOR_BLACK << 4);
        }
        cursor = 80 * 24;
    }
}

/*print string with color*/
void vga_print(const char* str, uint8_t color){
    while (*str) {
        vga_putchar(*str++, color);
    }
}

/*convert number to hex*/
void vga_print_hex(unsigned long long num, uint8_t color) {
    const char hex[] = "0123456789ABCDEF";
    char buffer[18];
    buffer[0] = '0';
    buffer[1] = 'x';
    int pos = 2;

    int started = 0;
    for (int i = 60; i >= 0; i -= 4) {
        int digit = (num >> i) & 0xF;
        if (digit != 0 || started || i == 0) {
            buffer[pos++] = hex[digit];
            started = 1;
        }
    }
    buffer[pos] = '\0';
    vga_print(buffer, color);
}
