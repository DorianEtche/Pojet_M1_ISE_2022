/**
 * @brief Ce programme inverse la tension sur l'entrée de l'ADC (A0) et aplique cette tension sur le DAC
 
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

/**
 * @brief Description de l'octet de controle (8 bits) pour le composant PCF8591 en DAC / ADC.
 * 
 * Protocole du bus I²C : 
 * adresse esclave | octets de controle | nombre d'octets de controle
 * 
 * @details Description de l'octet de controle (8 bits) :
 
 * > bit 0 et 1 --> sélection du channel :
 *      00 -> channel 0
 *      01 -> channel 1
 *      10 -> channel 2
 *      11 -> channel 3
 * Reception : 1 octet contenant la donnée du channel sélectioné.
 * /!\ à chaque changement de l'octet de controle, la première donnée stoquée par le composant 
 *     représente la précédente donnée enregistré, c'est une valeur à ne pas lire.

 
 * > bit 2 --> active la selection par auto-incrementation :
 *      0 -> desactivé
 *      1 -> activé
 * Reception : envoie à la suite les données des différents channels (5 octets).
 * /!\ Le 1er octets représente la dernière valeurs stoquée, C'est une valeur à ne pas lire.
 

 * > bit 3 --> toujour égale à "0"
 

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


 * > bit 6 --> sorite analogique
 *      0 -> désactivé
 *      1 -> activé


 * _____________________________________________________________________________________________________________________________________________
 *       Description : Description de l'octet de controle pour le composant PCF8591 AD/DA module                                                |
 * ------------------|--------------------------------------------------------------------------------------------------------------------------|
 * Numéro du bit     | 7 |           6          |           5          |         4           | 3 |          2           |     1     |     0     |
 * ------------------|--------------------------------------------------------------------------------------------------------------------------|
 * Description du bit| 0 |   Sortie analogique  |    Programmation des entrées analogique    | 0 | auto-incrémmentation | Numéro du channel A/D |
 * __________________|__________________________________________________________________________________________________________________________|
*/

/* Library */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <linux/i2c-dev.h>
#include <math.h>
 
#define PCF8591_I2C_ADDR 0x48 ///< PCF8591 i2c address
#define RPI_I2C_DEVICE "/dev/i2c-1"
#define PCF8591_DAC_RQST 0x40 ///< PCF8591 DAC write request
#define Vref 3300 ///< PCF8591 supply voltage.
#define resolution 255 ///< PCF8591 DAC and ADC resolution.

/* Fonctions */
void PCF8591_i2c_connect (void);
void PCF8591_read_data(const void *BufControl, size_t sizeWBuf, int sizeRBuf,uint8_t *data_in);
void PCF8591_write_data(const void *BufControl,  int sizeWBuf);

char *i2c_chip = RPI_I2C_DEVICE;
int fd;


int main(void)
{
    PCF8591_i2c_connect();

    uint8_t A0_data[2];
    uint8_t DAC_data[2] = {PCF8591_DAC_RQST};
    uint8_t A0_voltage_data;
    uint8_t buffer[1];

    while(1)
    {
        /* On lit la valeur sur l'entrée A0. /!\ il ne faut pas désactiver le DAC */
        buffer[0] = 0x40; // Channel 0 lié à l'entrée A0
        PCF8591_read_data(buffer,0x01,0x02,A0_data);
        printf("data de A0 = 0x%x\n",A0_data[1]);

        /* On inverse la tension d'entrée et on l'applique sur le DAC */
        DAC_data[1] = ~A0_data[1];
        PCF8591_write_data(DAC_data,0x02);
    }
}

/**
 * @brief Transmet en communication I²C les octets de controls "BufControl" et stock la reception dans le buffer "data_in"
 * @param BufControl --> buffer des octets de controles 
 * @param sizeWbuf   --> taille du buffer BufControl
 * @param data_in    --> buffer de reception
 * @param sizeRbuf   --> taille du buffer de reception 
 * @return rien 
 * @warning neccesite l'execution de PCF8591_i2c_connect() avant utilisation 
 */
void PCF8591_read_data(const void *BufControl, size_t sizeWBuf, int sizeRBuf,uint8_t *data_in)
{
    

    write(fd, BufControl,sizeWBuf);

    uint8_t *buffer =  BufControl;
    if ((buffer[0] & 0x40) == 0x40){usleep(100);}else{usleep(100000);}
    
    read(fd, data_in, sizeRBuf);
}

/**
 * @brief Transmet en communication I²C les octets de controls "BufControl"
 * @param BufControl --> buffer des octets de controles 
 * @param sizeWbuf   --> taille du buffer BufControl
 * @return rien
 * @warning neccesite l'execution de PCF8591_i2c_connect() avant utilisation 
 */
void PCF8591_write_data(const void *BufControl,  int sizeWBuf)
{
    write(fd, BufControl,sizeWBuf);
}

/**
 * @brief Initialise la connexion I²C avec le commposant PCF8591
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
