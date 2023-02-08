/**
 * @brief this program generates a sinusoidal signal on the DAC output of the PCF8591 component
 
 * @file PCF8591_DAC_sin.c
 * @copyright (c) Dorian ETCHEBER
 * @date 30.01.2023
 *
 * @details 
 * Hardware : Rpi4, PCF8591
 * compilation : gcc -Wall -o PCF8591_DAC_sin PCF8591_DAC_sin.c -lm
 *  
 * @warning Don't forget copy the PCF8591.h library in the same folder as this program.
 * 
*/

/* Library */
#include "PCF8591.h"
#include <math.h>

/* Define frequency signal (Hz) */
#define FREQUENCY 10

/* define pic voltage */
#define PIC 1500

int main(void)
{
    double i;
    PCF8591_i2c_connect(); ///< initialization of I²C communication (address : 0x48)
    while(1)
    {
        /* for i in ranging from 0 to 2pi, put sin(i) on DAC */
        for (i = 0; i <= 6.28; i+= 0.01)
        {
            PCF8591_write_voltage_mv(PIC * sin(i) + PIC);
            usleep(1592 / FREQUENCY);
        }
    }
}