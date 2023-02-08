/**
 * @brief This library contains the basic functions needed to use the PCF8594 Remote 8-Bit I/O Expander
 
 * @file PCF8574.h
 * @copyright (c) Dorian ETCHEBER
 * @date 06-02-2023
 *
 * @details 
 * Hardware : Rpi4, PCF8574
 **/

#ifndef PCF8594_H
#define PCF8594_H

/* Library */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <linux/i2c-dev.h>

#ifndef PCF8574_I2C_ADDR 
#define PCF8574_I2C_ADDR 0x20 ///< PCF8574 i2c address
#endif
#ifndef RPI_I2C_DEVICE
#define RPI_I2C_DEVICE "/dev/i2c-1"
#endif

#define HIGH 1
#define LOW 0

char *i2c_chip = RPI_I2C_DEVICE;
int fd;
uint8_t data_byte[1] = {0xFF};

/**
 * @brief Initialise la connexion I²C avec le commposant PCF8574
 * @param Nothing.
 * @return Nothing.
 */
void PCF8574_init(void)
{
        if ((fd = open(i2c_chip, O_RDWR)) < 0) ///< The i2c communication is initiated by opening a file.
    {
        printf("Cannot initiate i2c connection\n");
        exit(1);
    }
    else
    {
        ioctl(fd, I2C_SLAVE, PCF8574_I2C_ADDR); ///< Change default i2c address in i2c-dev.h file by PCF8591 address.
        printf("i2c connection initiated\n");
    }
}

/**
 * @brief renvoie l'octet de donnée transmis par le composant PCF8574
 * @param Nothing.
 * @return Octet de donnée
 * @warning executer PCF8574_init() avant d'utiliser cette fonction
 */
uint8_t PCF8574_read_data(void)
{
    uint8_t buffer[1];
    read(fd,buffer,0x01);
    return buffer[0];
}
/**
 * @brief transmet l'octet d'entré au composant
 * @param data octet de donnée
 * @return rien
 * @warning executer PCF8574_init() avant d'utiliser cette fonction
 */
void PCF8574_write_data(uint8_t data)
{
    uint8_t buffer[1] = {data};
    write(fd,buffer,0x01);
}

/**
 * @brief change l'état d'une des sortie du composant PCF8574 (HIGH / LOW)
 * @param output_pin numéro de la sortie (entre 0 et 7)
 * @param state Etat de la sortie (HIGH / LOW)
 * @return rien
 * @warning executer PCF8574_init() avant d'utiliser cette fonction
 */
void PCF8574_digitalWrite(short output_pin, short state)
{
    if ((output_pin > 7) | (output_pin < 0))
    {
        /* error */
        return;
    }
    uint8_t output = ~(1 << output_pin);
    
    if (state == HIGH)
    {
        data_byte[0] &= output; 
    }else if(state == LOW)
    {
        data_byte[0] |= ~output;
    }
    write(fd,data_byte,1);
}


/**
 * @brief Renvoie l'etat de l'entrée sélectioné
 * @param input_pin numéro de l'entrée (entre 0 et 7)
 * @return rien
 * @warning executer PCF8574_init() avant d'utiliser cette fonction
 */
short PCF8574_digitalRead(short input_pin)
{
    if ((input_pin > 7) | (input_pin < 0))
    {
        /* error */
        return 0;
    }
    uint8_t input = 1 << input_pin;
    
    if((data_byte[0] & input) == HIGH)
    {
        return HIGH;
    }else{
        return LOW;
    }
}

#endif