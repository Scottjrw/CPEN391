#include <stdio.h>
#include <io.h>
#include "system.h"
#include "altera_avalon_pio_regs.h"

/* A janky program that will capture a portable pixmap format image (.pgm, .ppm)
 * https://en.wikipedia.org/wiki/Netpbm_format
 *
 * Usage:
 *
 * Run program in eclipse or use nios2-download to download ELF into processor
 *
 * If using eclipse, stop the nios terminal by pressing the red box
 *
 * Quickly open a new nios command shell run the following command:
 *
 * nios2-terminal -q | tee myimage.pgm
 *
 * Wait a few minutes for the command to complete... the last line that will print is #END OF IMAGE
 *
 * Stop the nios2-terminal with Ctrl-C
 *
 * Open the file in some image view capable of opening .pgm/.ppm images
 * 
 */

#define VGA_X_DIM 320
#define VGA_Y_DIM 240

#define SKIP 1

volatile unsigned char *frame_buffer = (volatile unsigned char *) VIDEO_FRAME_BUFFER_BASE;

int main()
{
    printf("#Waiting...\n\n");
    for (int c = 10; c > 0; c--) {
        for (int i = 0; i < 1000000; i++) {
            // Put a memory write here to prevent it from being optimized out
            IOWR_ALTERA_AVALON_PIO_DATA(LED_OUT_PIO_BASE, 0x5);
        }
    }
    printf("P3\n");
    printf("%d %d 255\n", VGA_X_DIM / SKIP, VGA_Y_DIM / SKIP);

    for (int i = 0; i < 1000000; i++) {
        IOWR_ALTERA_AVALON_PIO_DATA(LED_OUT_PIO_BASE, 0x5);
    }

    for (int y = 0; y < VGA_Y_DIM * 4; y += SKIP * 4) {
        for (int x = 0; x < VGA_X_DIM * 4; x += SKIP * 4) {
            unsigned int pix = *((unsigned int *) &frame_buffer[y * VGA_X_DIM + x]);
            unsigned char pix_r = pix >> 16;
            unsigned char pix_g = pix >> 8;
            unsigned char pix_b = pix;

            printf("%d %d %d  ", pix_r, pix_g, pix_b);
        }

        putchar('\n');
    }

    printf("\n#END OF IMAGE\n");

    while (1) { }

    return 0;
}
