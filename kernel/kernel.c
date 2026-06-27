/* Thanks to SQWOZ BAB, bbno$ and the creators of Minecraft videos-otherwise, I would have lost my mind */

/*VGA color*/
#define COLOR_BLACK         0x0
#define COLOR_BLUE          0x1
#define COLOR_GREEN         0x2
#define COLOR_CYAN          0x3
#define COLOR_RED           0x4
#define COLOR_MAGENTA       0x5
#define COLOR_BROWN         0x6
#define COLOR_LIGHT_GRAY    0x7
#define COLOR_DARK_GRAY     0x8
#define COLOR_LIGHT_BLUE    0x9
#define COLOR_LIGHT_GREEN   0xA
#define COLOR_LIGHT_CYAN    0xB
#define COLOR_LIGHT_RED     0xC
#define COLOR_LIGHT_MAGENTA 0xD
#define COLOR_YELLOW        0xE
#define COLOR_WHITE         0xF

/*VGA character*/
typedef struct {
    char character;
    char color;
}__attribute__((packed)) vga_char;

/*video mem & cursor*/
volatile vga_char* video = (vga_char*)0xB8000;
int cursor = 0;

/*clear entire screen*/
void clear_screen(){
    for (int i = 0; i < 80 * 25; i++) {
        video[i].character = ' ';
        video[i].color = COLOR_WHITE | (COLOR_BLACK << 4);
    }
    cursor = 0;
}

/*print single character*/
void putchar(char c, char color) {
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
void print(const char* str, char color){
    while (*str) {
        putchar(*str++, color);
    }
}

/*convert number to hex*/
void print_hex(unsigned long long num, char color) {
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
    print(buffer, color);
}

/*main function*/
void kernel_main(){
    clear_screen();

    /*header*/
    print("+====================+\n", COLOR_LIGHT_GRAY);
    print("|       DV OS        |\n", COLOR_GREEN);
    print("+====================+\n", COLOR_LIGHT_GRAY);
    print("\n", COLOR_WHITE);

    /*system information*/
    print("[OK] kernel loader at address", COLOR_LIGHT_GREEN);
    print_hex((unsigned long long) kernel_main, COLOR_YELLOW);
    print("\n", COLOR_WHITE);

    print("[OK] running in 64 bit long mode",COLOR_LIGHT_GREEN);
    print("[OK] page tables at 0x1000\n",COLOR_LIGHT_GREEN);
    print("[OK] stake at: ",COLOR_LIGHT_GREEN);

    unsigned long long  stake_addr;
    __asm__ volatile("mov %%rsp, %0" : "=r"(stake_addr));
    print_hex(stake_addr, COLOR_YELLOW);
    print("\n\n", COLOR_WHITE);

    /*CPU detection*/
    print("CPU features:\n", COLOR_LIGHT_CYAN);

    unsigned int eax, ebx, ecx, edx;
    __asm__ volatile("cpuid"
            : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
            : "a"(1));

    print("  - SSE:  ", COLOR_WHITE);
    print((edx & (1 << 25)) ? "YES\n" : "NO\n",
        (edx & (1 << 25)) ? COLOR_LIGHT_GREEN : COLOR_LIGHT_RED);

       print("  - SSE2:  ", COLOR_WHITE);
    print((edx & (1 << 26)) ? "YES\n" : "NO\n",
        (edx & (1 << 26)) ? COLOR_LIGHT_GREEN : COLOR_LIGHT_RED);

        print("  - SSE:  ", COLOR_WHITE);
    print((edx & (1 << 0)) ? "YES\n" : "NO\n",
        (edx & (1 << 0)) ? COLOR_LIGHT_GREEN : COLOR_LIGHT_RED);

        print("\n", COLOR_WHITE);
        print("+==============================+\n", COLOR_LIGHT_GRAY);
        print("|   System ready. Halting...   |\n", COLOR_GREEN);
        print("+==============================+\n", COLOR_LIGHT_GRAY);

    /*infinite loop*/
    while (1) {
        __asm__ volatile("hlt");
    }
}