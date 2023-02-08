/**
 * @brief This program reads and displays the 4 voltages in mv present on the ADC of the PCF8591 component 
 
 * @file PCF8591_ADC.c
 * @copyright (c) Dorian ETCHEBER
 * @date 30.01.2023
 *
 * @details 
 * Hardware : Rpi4, PCF8591
 * compilation : gcc -Wall -o PCF8591_ADC PCF8591_ADC.c 
 *  
 * @warning Don't forget copy the PCF8591.h library in the same folder as this program.
 * 
*/

/* Library */
#include "PCF8591.h"

int main(void)
{
    PCF8591_i2c_connect(); ///< initialization of I²C communication (address : 0x48)
    while(1)
    {
        int A0_voltage = PCF8591_read_voltage_mv(0); ///< Read voltage on A0 input
        int A1_voltage = PCF8591_read_voltage_mv(1); ///< Read voltage on A1 input
        int A2_voltage = PCF8591_read_voltage_mv(2); ///< Read voltage on A2 input
        int A3_voltage = PCF8591_read_voltage_mv(3); ///< Read voltage on A3 input

        printf("Voltage on ADC :\n> A0 --> %d mV\n> A1 --> %d mV\n> A2 --> %d mV\n> A3 --> %d mV\n\n",A0_voltage,A1_voltage,A2_voltage,A3_voltage);
        sleep(1);
    }
}