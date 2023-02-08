/**
 * @brief this program generates a pwm signal on the DAC output of the PCF8591 component
 
 * @file PCF8591_DAC_pwm.c
 * @copyright (c) Dorian ETCHEBER
 * @date 30.01.2023
 *
 * @details 
 * Hardware : Rpi4, PCF8591
 * compilation : gcc -Wall -o PCF8591_DAC_pwm PCF8591_DAC_pwm.c
 *  
 * @warning Don't forget copy the PCF8591.h library in the same folder as this program.
 * 
*/

/* Library */
#include "PCF8591.h"

/* PWM frequency (Hz) and cycle ratio (%) */
#define FREQUENCY 1000
#define CYCLE_RATIO 50

/* define the voltages of the high and low states */
#define HIGH_STATE 3000
#define LOW_STATE  1000

int main(void)
{
    /* define sleep time for high and low states */
    int HighSleepTime = 1000000 * CYCLE_RATIO /  FREQUENCY;
    int LowSleepTime = 1000000 * (100 - CYCLE_RATIO) /  FREQUENCY;

    PCF8591_i2c_connect(); ///< initialization of I²C communication (address : 0x48)
    while(1)
    {
        PCF8591_write_voltage_mv(HIGH_STATE);
        usleep(HighSleepTime);
        PCF8591_write_voltage_mv(LOW_STATE);
        usleep(LowSleepTime);
    }
}