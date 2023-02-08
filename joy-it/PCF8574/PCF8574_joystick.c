/**
 * @brief Ce programme communique en I²C au composant PCF8574. 
 * Le composant est connecté à un joystick fonctionant avec de bouttons (renvoie un signal numérique).
 * Apres avoir lut l'octet de données, si elle est différente de ca valeur null (0xFF), 
 * on  évalue quel bouton est préssé et on l'affiche sur le terminal.         
 * @file PCF8574_joystick.c
 * @copyright (c) Dorian ETCHEBER
 * @date 06.02.2023
 *
 * @details 
 * Hardware : Rpi4, joy-it
 * compilation : gcc -Wall -o Joystick PCF8574_joystick.c 
*/

/**
 * Decription du bus data :
 *  ____________________________________________________________
 * |bits        |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
 * |------------|-----|-----|-----|-----|-----|-----|-----|-----|
 * |assignement |  X  |  X  |  X  |  X  |right|down | up  |left |
 * |____________|_____|_____|_____|_____|_____|_____|_____|_____|
 * 
 * Valeurs possible de sorite :
 * 
 * 0xFF / ~0x00 >> NULL
 * 0xFE / ~0x01 >> LEFT
 * 0xFD / ~0x02 >> UP
 * 0xFB / ~0x04 >> DOWN
 * 0xF6 / ~0x08 >> RIGHT
 * 0xFC / ~0x03 >> UP / LEFT
 * 0xF5 / ~0X0A >> UP / RIGHT
 * 0xFA / ~0x05 >> DOWN / LEFT
 * 0xF2 / ~0x0C >> DOWN / RIGHT
*/

#define NULL_PRESS 0xFF
#define LEFT_PRESS 0xFE
#define UP_PRESS 0xFD
#define DOWN_PRESS 0xFB
#define RIGHT_PRESS 0xF6
#define UP_LEFT_PRESS 0xFC
#define UP_RIGHT_PRESS 0xF5
#define DOWN_LEFT_PRESS 0xFA
#define DOWN_RIGHT_PRESS 0xF2


#define RPI_I2C_DEVICE "/dev/i2c-1"
#define PCF8571_I2C_ADDR 0x20

/* library */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <linux/i2c-dev.h>
#include <math.h>

char *i2c_chip = RPI_I2C_DEVICE;
int fd;

/* fonctions */
void PCF8574_i2c_connect (void);
int verif_joystick_PRESS();
void bp_name(uint8_t bp_bin);

int main(void)
{
    PCF8574_i2c_connect();

    uint8_t buffer[1];
    uint8_t joystick;
    while(1)
    {
        while((joystick = verif_joystick_PRESS()) == 0); // On attend une valeur autre que la valeur NULL
        bp_name(joystick);

        /* On attend que la direction ne soit plus_PRESSé */
        int fix_joystick = joystick;
        while ((joystick = verif_joystick_PRESS()) == fix_joystick);
    }
    return 0;
}

/**
 * @brief Initialise la connexion I²C avec le commposant PCF8574
 * @param Nothing.
 * @return Nothing.
 */
void PCF8574_i2c_connect (void)
{
    if ((fd = open(i2c_chip, O_RDWR)) < 0) ///< The i2c communication is initiated by opening a file.
    {
        printf("Cannot initiate i2c connection\n");
        exit(1);
    }
    else
    {
        ioctl(fd, I2C_SLAVE, PCF8571_I2C_ADDR); ///< Change default i2c address in i2c-dev.h file by PCF8591 address.
        printf("i2c connection initiated\n");
    }
}


/**
 * @brief lit l'octet de donnée du composant PCF8574 et renvoie son inverse 
 * @return Octet de donnée.
 */
int verif_joystick_PRESS()
{
    uint8_t buffer[1];
    read(fd, buffer, 0x01);
    
    return (~buffer[0]);
}

/**
 * @brief Ecrit sur le terminal le bouton_PRESSé.
 * @param bp_bin donnée en 8 bit du bouton_PRESSé (inverse de l'octet de donnée).
 * @return rien.
 */
void bp_name(uint8_t bp_bin)
{
    bp_bin = (~bp_bin) & 0x0F;
    switch(bp_bin)
    {
        case LEFT_PRESS :
            printf("LEFT \n");
            break;
        case UP_PRESS :
            printf("UP \n");
            break;
        case DOWN_PRESS :
            printf("DOWN \n");
            break;
        case RIGHT_PRESS :
            printf("RIGHT \n");
            break;
        case UP_LEFT_PRESS :
            printf("UP-LEFT \n");
            break;
        case UP_RIGHT_PRESS :
            printf("UP-RIGHT \n");
            break;
        case DOWN_RIGHT_PRESS :
            printf("DOWN-RIGHT \n");
            break;
        case DOWN_LEFT_PRESS :
            printf("DOWN-LEFT \n");
            break;
    }
}