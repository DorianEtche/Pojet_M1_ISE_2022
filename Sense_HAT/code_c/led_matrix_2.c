/*
 *  C code to demonstrate control of the LED matrix for the
 *  Raspberry Pi Sense HAT add-on board.
 *
 *  Uses the mmap method to map the led device into memory
 *
 *  Build with:  gcc -Wall -O2 led_matrix.c -o led_matrix
 *               or just 'make'
 *
 *  Tested with:  Sense HAT v1.0 / Raspberry Pi 3 B+ / Raspbian GNU/Linux 10 (buster)
 *
 */


#include <fcntl.h>
#include <linux/fb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "Sense_hat.h"

#define FILEPATH "/dev/input"

#define RGB_BLUE  0x00FF
#define RGB_RED   0xF000
#define RGB_GREEN 0x0F00
#define RGB_WITHE 0xFFFF
#define RGB_BLACK 0x0000




int main(void)
{
    int map = senseHat_init();
    if( map < 0)
    {
        printf("erreur d'unitialisation ! Code erreur : %d\n",map);
        return -1;
    }
    senseHat_clear(DEFAULT_CLEAR);
    sleep(1);
    senseHat_setPixel(1,1,RGB_RED);
    sleep(1);

    int mapping[64] = {
        RGB_RED,   RGB_RED,   RGB_RED,   RGB_RED,   RGB_RED,   RGB_RED,   RGB_RED,   RGB_RED,
        RGB_BLUE,  RGB_BLUE,  RGB_BLUE,  RGB_BLUE,  RGB_BLUE,  RGB_BLUE,  RGB_BLUE,  RGB_BLUE,
        RGB_GREEN, RGB_GREEN, RGB_GREEN, RGB_GREEN, RGB_GREEN, RGB_GREEN, RGB_GREEN, RGB_GREEN,
        RGB_WITHE, RGB_WITHE, RGB_WITHE, RGB_WITHE, RGB_WITHE, RGB_WITHE, RGB_WITHE, RGB_WITHE, 
        RGB_BLUE,  RGB_BLUE,  RGB_BLUE,  RGB_BLUE,  RGB_BLUE,  RGB_BLUE,  RGB_BLUE,  RGB_BLUE,
        RGB_GREEN, RGB_GREEN, RGB_GREEN, RGB_GREEN, RGB_GREEN, RGB_GREEN, RGB_GREEN, RGB_GREEN,
        RGB_RED,   RGB_RED,   RGB_RED,   RGB_RED,   RGB_RED,   RGB_RED,   RGB_RED,   RGB_RED,
        RGB_WITHE, RGB_WITHE, RGB_WITHE, RGB_WITHE, RGB_WITHE, RGB_WITHE, RGB_WITHE, RGB_WITHE
    };
    senseHat_setPixels(mapping);
    sleep(3);

    printf("couleur du pixel en x = 4 et y = 6 : %xx\n",senseHat_getPixel(4,6));
    
    int* return_mapping = senseHat_getPixels();
    
    printf("mapping : \n");
    for(int i = 0; i < 8; i++)
    {
        printf("ligne %d : ",i);
        for(int j = 0; j < 8; j++)
        {
            printf("%xx, ",return_mapping[i * 8 + j]);
        }
        printf("\n");
    }
    senseHat_flipR();
    senseHat_humidity();
}