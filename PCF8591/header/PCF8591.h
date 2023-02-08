/* 
 * Copyright © 2022 Paul FILLIETTE
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and 
 * associated documentation files (the “Software”), to deal in the Software without restriction, including 
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
 * copies of the Software, and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies 
 * or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 */

/**
 * @brief This library contains the basic functions needed to use the PCF8591 DAC an ADC module
 
 * @file PCF8591.h
 * @copyright (c) Dorian ETCHEBER | inspiré du travail de Paul FILLIETTE
 * 
 * @date 31.01.23
 *
 * @details 
 * Hardware : Rpi4, DAC/ADC PCF8591
 * 
 * Controle register configuration (8 bits):
 * 
 * 1st bit set to zero by default.
 * 2nd bit can be set to 1 in order to enable the DAC output (this enable the DAC amplifier wich allows the DAC to output a voltage).
 * 3rd and 4th bit allows to switch between 4 modes:    
 *					   - 00 -> 4 analog input with the operating voltage and the ground as references.
 *					   - 01 -> The 3 input A0, A1 and A2 are differencial input with A4 input as a reference voltage.
 *					   - 10 -> 2 basic analog input (A0 and A1) and 1 differential input (A3 with A4 as reference voltage).
 *					   - 11 -> 2 differential input with their own reference voltage input (A0 input with A1 as reference voltage input and A2 input with A3 as reference voltage input).
 * 5th bit set to zero by default.
 * 6th bit allows to cycle between each of the 4 input (A0 then A1 then A2 then A3 than A0 etc...).
 * 7th and 8th control bit allows to choose wich input to listen:    
 *									   - 00 -> A0.
 *									   - 01 -> A1.
 *									   - 10 -> A2.
 *									   - 11 -> A3.
 * 
 * @warning Don't forget to flush the first value read from the ADC because it correspond to the last value measured in the previous use of the ADC.
 * 
 * The byte sent to the PCF8591 right after the control byte is stored in the DAC register and correspond to the value that the DAC will output.
 * The DAC have a feature which allows it to keep outputting the last voltage value until the next conversion is over. 
 * The DAC output voltage is Vout = Vgnd + ((Vref-Vgnd) * (data / 256))      with Vref the operating voltage and data the given output value between 0 to 255.
 * The DAC maximum output voltage is 3700mV.
 * 
 */


/**
 * Pour récupérer la valeur en 8 bits sur l'ADC du composant PCF8591, il faut transmettre dans un 1er temps, quel entrée on veut utiliser.
 * 
 * Pour cela il faut transmettre le bus suivant :
 * adresse esclave | octet de controle | nombre d'octet de controle
 * 
 * Decription de l'octet de controle (8 bits) :
 * > bit 0 et 1 --> sélection du channel :
 *      00 -> channel 0
 *      01 -> channel 1
 *      10 -> channel 2
 *      11 -> channel 3
 * Reception : 1 octet contenant la donnée du channel sélectioné
 * /!\ à chaque changement de l'octet de controle, la donnée stoqué représente la précédente donnée enregistré, c'est une valeur à ne pas lire
 * 
 * > bit 2 --> active la selection par auto-incrementation :
 *      0 -> desactivé
 *      1 -> activé
 * Reception : envoie à la suite les données des différents channels
 * /!\ Le 1er octets représente la dernière valeurs stoqué, C'est une valeur à ne pas lire
 * 
 * > bit 3 --> toujour égale à "0"
 * 
 * > bit 4 et 5 --> Programmation des entrées analogiques
 *      00 -> quatre entrée asymétrique
 *          * channel 0 => A0
 *          * channel 1 => A1
 *          * channel 2 => A2
 *          * channel 3 => A3
 * 
 *      01 -> 3 entrées différentielles 
 *          * channel 0 => A0 - A3
 *          * channel 1 => A1 - A3
 *          * channel 2 => A2 - A3
 * 
 *      10 -> 2 entrées asymetrique et une entrée différentielles
 *          * channel 0 => A0
 *          * channel 1 => A1
 *          * channel 2 => A2 - A3
 * 
 *      11 -> 2 entrées différentielles
 *          * channel 0 => A0 - A1
 *          * channel 1 => A2 - A3
 * 
 * > bit 6 --> sorite analogique
 *      0 -> désactivé
 *      1 -> activé
 *  
 * ___________________________________________________________________________________________________________________________
 * | Description : représentation de l'octet de controle pour le composant PCF8591 AD/DA module                               |
 * |--------------------------------------------------------------------------------------------------------------------------|
 * | 7 |           6          |           5          |         4           | 3 |          2           |     1     |     0     |
 * |--------------------------------------------------------------------------------------------------------------------------|
 * | 0 |   Sortie analogique  |    Programmation des entrées analogique    | 0 | auto-incrémmentation | Numéro du channel A/D |
 * |__________________________________________________________________________________________________________________________|
*/

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
    uint8_t select_channel[1] = {0x40 | channel};
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
    read(fd, data_in, 1); ///< it represent the last value read and stored in the register during the previous use of the ADC
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
