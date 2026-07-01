/* Thanks to SQWOZ BAB, bbno$, emimem and the creators of Minecraft videos-otherwise, I would have lost my mind */

#include "../../drivers/VGA/vga.h"

void kernel_main(){
    vga_clear();

    /*header*/
    vga_print("+===================+\n", COLOR_LIGHT_GREEN);
    vga_print("|       DV OS       |\n", COLOR_GREEN);
    vga_print("+===================+\n", COLOR_LIGHT_GREEN);
    vga_print("\n", COLOR_WHITE);

    /*system information*/
    vga_print("[OK] kernel loader at address", COLOR_LIGHT_GREEN);
    vga_print_hex((unsigned long long) kernel_main, COLOR_YELLOW);
    vga_print("\n", COLOR_WHITE);

    vga_print("[OK] running in 64 bit long mode",COLOR_LIGHT_GREEN);
    vga_print("[OK] page tables at 0x1000\n",COLOR_LIGHT_GREEN);
    vga_print("[OK] stack at: ",COLOR_LIGHT_GREEN);

    unsigned long long  stack_addr;
    __asm__ volatile("mov %%rsp, %0" : "=r"(stack_addr));
    vga_print_hex(stack_addr, COLOR_YELLOW);
    vga_print("\n\n", COLOR_WHITE);

    /*CPU detection*/
    vga_print("CPU features:\n", COLOR_LIGHT_CYAN);

    unsigned int eax, ebx, ecx, edx;
    __asm__ volatile("cpuid"
            : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
            : "a"(1));

    vga_print("  - SSE:  ", COLOR_WHITE);
    vga_print((edx & (1 << 25)) ? "YES\n" : "NO\n",
        (edx & (1 << 25)) ? COLOR_LIGHT_GREEN : COLOR_LIGHT_RED);

       vga_print("  - SSE2:  ", COLOR_WHITE);
    vga_print((edx & (1 << 26)) ? "YES\n" : "NO\n",
        (edx & (1 << 26)) ? COLOR_LIGHT_GREEN : COLOR_LIGHT_RED);

        vga_print("  - SSE3:  ", COLOR_WHITE);
    vga_print((ecx & (1 << 0)) ? "YES\n" : "NO\n",
        (ecx & (1 << 0)) ? COLOR_LIGHT_GREEN : COLOR_LIGHT_RED);

        vga_print("\n", COLOR_WHITE);
        vga_print("+==============================+\n", COLOR_LIGHT_GREEN);
        vga_print("|   System ready. Halting...   |\n", COLOR_GREEN);
        vga_print("+==============================+\n", COLOR_LIGHT_GREEN);

    /*infinite loop*/
    while (1) {
        __asm__ volatile("hlt");
    }
}