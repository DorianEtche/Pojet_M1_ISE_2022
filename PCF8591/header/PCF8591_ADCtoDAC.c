/**
 * @brief this program reads the voltage value on the A0 input, divides this voltage by 2 and applies it to the DAC
 
 * @file PCF8591_ADCtoDAC.c
 * @copyright (c) Dorian ETCHEBER
 * @date 30.01.2023
 *
 * @details 
 * Hardware : Rpi4, PCF8591
 * compilation : gcc -Wall -o PCF8591_ADCtoDAC PCF8591_ADCtoDAC.c
 *  
 * @warning Don't forget copy the PCF8591.h library in the same folder as this program.
 * 
*/

/* Libary */
#include "PCF8591.h"

int main(void)
{
    uint8_t A0_data;
    uint8_t DAC_data;

    /* initialization of I²C communication (address : 0x48) and put first DAC voltage at 0 mV */
    PCF8591_i2c_connect();
    PCF8591_write_data(0);
    while(1)
    {
        /* read the ADC voltage (8 bits) on A0 input */
        A0_data = PCF8591_read_data(0); 
        printf("data de A0 = %xx\n",A0_data);

        /* halves the ADC voltage and put it on the DAC */
        DAC_data = A0_data/2;
        PCF8591_write_data(DAC_data);
    }
}