/**
 * @brief This library contains the basic functions needed to use the PCF8591 DAC an ADC module
 
 * @file PCF8591.h
 * @copyright (c) Paul FILLIETTE
 * version midified by Dorian ETCHEBER 2023
 * @date 10-02-2022
 *
 * @details 
 * Hardware : Rpi4, DAC/ADC PCF8591
 
 **/

#ifndef PCF8591_H
#define PCF8591_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <linux/i2c-dev.h>
#include <math.h>

#ifndef PCF8591_I2C_ADDR 
#define PCF8591_I2C_ADDR 0x48 ///< PCF8591 i2c address
#endif
#ifndef RPI_I2C_DEVICE
#define RPI_I2C_DEVICE "/dev/i2c-1"
#endif
#ifndef PCF8591_DAC_RQST
#define PCF8591_DAC_RQST 0x40 ///< PCF8591 DAC write request
#endif

#define Vref 3300 ///< PCF8591 supply voltage.
#define resolution 255 ///< PCF8591 DAC and ADC resolution.

char *i2c_chip = RPI_I2C_DEVICE;
int fd;

int select_channel(uint8_t channel)
{
    if (channel > 4){ printf("/!\\ Error : channel not suported. Select a compliant channel.\n"); return -1;}
    uint8_t select_channel[1] = {0x48 | channel};
    write(fd,select_channel,1);
    usleep(100);

    return 1;
}


/**
 * @brief Write a voltage on the output pin of the DAC.
 * @param DAC_tension_mv The voltage value that we want to write on the DAC output.
 * @return Nothing.
 * @warning Use the PCF8591_i2c_connect() function before using this function.
 */
void PCF8591_write_voltage_mv(int DAC_tension_mv)
{
    uint8_t dac_data_out = DAC_tension_mv * resolution / Vref;
    uint8_t dac_voltage[2] = {PCF8591_DAC_RQST,dac_data_out}; ///< 2 bytes buffer. 1st byte => config, 2nd byte => Data.
    write(fd, dac_voltage, 2); ///< Write to the DAC.
}

/**
 * @brief Write a voltage on the output pin of the DAC.
 * @param DAC_data_in The 4 bits value that we want to write on the DAC output.
 * @return Nothing.
 * @warning Use the PCF8591_i2c_connect() function before using this function.
 */
void PCF8591_write_data(uint8_t DAC_data_in)
{
    unsigned char dac_voltage[2] = {PCF8591_DAC_RQST,DAC_data_in}; ///< 2 bytes buffer. 1st byte => config, 2nd byte => Data.
    write(fd, dac_voltage, 2); ///< Write to the DAC.
}

/**
 * @brief Read a voltage on each of the four pin of the ADC by cycling trough all of them. 
 * @param Channel number -> possible value : 0 / 1 / 2 / 3
 * @return Input value in mV.
 * @warning Use the PCF8591_i2c_connect() function before using this function.
 */
unsigned short int PCF8591_read_voltage_mv(uint8_t channel)
{
    if(select_channel(channel) < 0){return 0;}

    uint8_t data_in[1]; ///< 1 byte buffer that will get ADC data.
    read(fd, data_in, 1); ///< Data reading.
    unsigned short int data_in_mv = *data_in *(Vref/255); ///< Data conversion from ADC resolution to mV.
    return data_in_mv;
}

/**
 * @brief Read a voltage on each of the four pin of the ADC by cycling trough all of them. 
 * @param channel number -> possible value : 0 / 1 / 2 / 3
 * @return 8 bit data of the selected channel
 * @warning Use the PCF8591_i2c_connect() function before using this function.
 */
uint8_t PCF8591_read_data(uint8_t channel)
{
    if(select_channel(channel) < 0){return 0;}

    uint8_t data_in[1]; ///< 1 byte buffer that will get ADC data.
    read(fd, data_in, 1); ///< it represent the last value read and stored in the register during the previous use of the ADC
    read(fd, data_in, 1); ///< Data reading.
    return *data_in;
}

/**
 * @brief Initiate the I2C connection with PCF8591.
 * @param Nothing.
 * @return Nothing.
 */
void PCF8591_i2c_connect (void)
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
}

#endif
