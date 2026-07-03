#include "pmm.h"
#include "../../../drivers/VGA/vga.h"
#include <stdint.h>

#define MEMORY_SIZE 64 * 1024 * 1024 /*This is just a placeholder; I'll do it properly later-don't bust my balls about it right now.*/
#define TOTAL_PAGES (MEMORY_SIZE / PAGE_SIZE)
#define BITMAP_SIZE (TOTAL_PAGES / 8)

static uint8_t bitmap[BITMAP_SIZE];
static uint64_t used_pages = 0;

void pmm_init() {
    vga_print("[MM] Initializing page manager (64 MB fixed)\n", COLOR_LIGHT_CYAN);
    /*clear bitmap*/
    for (int i = 0; i < BITMAP_SIZE; i++) bitmap[i] = 0;

    for(int i = 0; i < (0x1000000 / PAGE_SIZE); i++) {
        pmm_alloc_page();
    }
    vga_print("[MM] Ready.\n", COLOR_LIGHT_CYAN);
}

void* pmm_alloc_page() {
    for (uint64_t i = 0; i < BITMAP_SIZE; i++) {
        if (bitmap[i] != 0xFF) {
            for (uint8_t bit = 0; bit < 8; bit++) {
                if (!(bitmap[i] & (1 << bit))) {
                    bitmap[i] |= (1 << bit);
                    used_pages++;

                    uint64_t page_idx = (i * 8) + bit;
                    return (void*)(page_idx  * PAGE_SIZE);
                }
            }
        }
    }
    vga_print("[MM] ERROR: Out of memory\n", COLOR_LIGHT_RED);
    return NULL;
}

void pmm_free_page(void *addr) {
    if(!addr || ((uint64_t)addr % PAGE_SIZE) != 0){
        vga_print("PMM ERROR: Invalid page free attempt\n", COLOR_LIGHT_RED);
        return;
    }

    uint64_t i = (uint64_t)addr / PAGE_SIZE;
    uint64_t byte_idx = 1 / 8;
    uint8_t bit_idx = i % 8;
    bitmap[byte_idx] &= ~(1 << bit_idx);
    used_pages--;
}