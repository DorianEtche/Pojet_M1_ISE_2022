/**
 * @brief Ce programme lit les valeurs d'entrée des 4 ADC (du composant PCF8591) 
 *        et l'affiche apres conversion en mv (1 capteur à la suite) mais aussi 
 *        hexadécimale (8 bits en mode auto-incrémentation).
 
 * @file PCF8591_ADC.c
 * @copyright (c) Dorian ETCHEBER
 * @date 30.01.2023
 *
 * @details 
 * Hardware : Rpi4, PCF8591
 * compilation : gcc -Wall -o PCF8591_ADC PCF8591_ADC.c 
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
unsigned short int PCF8591_read_voltage_mv(uint8_t channel);
void PCF8591_i2c_connect (void);
void PCF8591_read_data(const void *BufControl, size_t sizeWBuf, int sizeRBuf,uint8_t *data_in);


char *i2c_chip = RPI_I2C_DEVICE;
int fd;


int main(void)
{
    PCF8591_i2c_connect();

    uint8_t A_voltage_data[5];
    int A_voltage_mv[4];
    uint8_t buffer[1];

    while(1)
    {
        /* Methode 1 : récupérer les données une par une et l'afficher */

        A_voltage_mv[0] = PCF8591_read_voltage_mv(0); // lit la tension sur le channel 0
        A_voltage_mv[1] = PCF8591_read_voltage_mv(1); // lit la tension sur le channel 1
        A_voltage_mv[2] = PCF8591_read_voltage_mv(2); // lit la tension sur le channel 2
        A_voltage_mv[3] = PCF8591_read_voltage_mv(3); // lit la tension sur le channel 3

        printf("Voltage on ADC :\n> A0 --> %d mV\n> A1 --> %d mV\n> A2 --> %d mV\n> A3 --> %d mV\n\n",A_voltage_mv[0],A_voltage_mv[1],A_voltage_mv[2],A_voltage_mv[3]);
        sleep(1);


        /* Methode 2 : activer l'auto incrémentation et récupérer toute les valeurs en une seul fois et l'afficher */

        buffer[0] = 0x04; ///< Control byte 0b0000 0100 -> Auto-increment mode
        PCF8591_read_data(buffer,1,5,A_voltage_data); ///< Read voltage on all inputs

        printf("Voltage on ADC :\n> A0 --> %xx\n> A1 --> %xx\n> A2 --> %xx\n> A3 --> %xx\n\n",A_voltage_data[1],A_voltage_data[2],A_voltage_data[3],A_voltage_data[4]);
        sleep(1);
    }
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



unsigned short int PCF8591_read_voltage_mv(uint8_t channel)
{
    uint8_t select_channel[1] = {0x40 | channel}; ///< mettre le bit de la sortie analogique à 1 permet d'avoir une réponse plus rapide
    write(fd,select_channel,1); //> Selecte channel >> 0x48 channel 0x01 
    usleep(100); ///< metrre à 100 000 si le bit sortie analogique est à 0

    uint8_t data_in[1]; ///< 1 byte buffer that will get ADC data.
    read(fd, data_in, 1); ///< it represent the last value read and stored in the register during the previous use of the ADC
    read(fd, data_in, 1); ///< Data reading.

    unsigned short int data_in_mv = *data_in *(Vref/255); ///< Data conversion from ADC resolution to mV.
    return data_in_mv;
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
    usleep(100000);
    read(fd, data_in, sizeRBuf);
}

/*
void PCF8591_write_data(const void *BufControl,  int sizeWBuf)
{
    write(fd, BufControl,sizeWBuf);
    usleep(100000);
}
*/