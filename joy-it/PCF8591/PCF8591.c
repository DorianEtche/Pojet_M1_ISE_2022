#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <linux/i2c-dev.h>
#include <math.h>

#define RPI_I2C_DEVICE "/dev/i2c-1"
#define PCF8591_I2C_ADDR 0x48 ///< PCF8591 i2c address
#define Vref 3300 ///< PCF8591 supply voltage.
#define resolution 256 ///< PCF8591 DAC and ADC resolution.

char *i2c_chip = RPI_I2C_DEVICE;
int fd;

void PCF8591_write_voltage_mv(unsigned char DAC_tension_mv)
{
    unsigned char dac_data_out = DAC_tension_mv;
    dac_data_out *= resolution; ///< mv to data resolution conversion.
    dac_data_out /= Vref; ///< mv to data resolution conversion.
    unsigned char dac_voltage[2] = {0b01000100,dac_data_out}; ///< 2 bytes buffer. 1st byte => config, 2nd byte => Data.
    write(fd, dac_voltage, 2); ///< Write to the DAC.
}

int main(void)
{
    if ((fd = open(i2c_chip, O_RDWR)) < 0) ///< The i2c communication is initiated by opening a file.
    {
        printf("Cannot initiate i2c connection\n");
        exit(1);
    }
    else
    {
        ioctl(fd, I2C_SLAVE, PCF8591_I2C_ADDR); ///< Change default i2c address in i2c-dev.h file by PCF8591 address.
        printf("i2c connection initiated\n");
    }

    while(1)
    {
        PCF8591_write_voltage_mv(0xFF);
        sleep(1);
        PCF8591_write_voltage_mv(0x00);
        sleep(1);
    }
    
}