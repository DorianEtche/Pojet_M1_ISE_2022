/**
 * @brief Ce programme communique en I²C au composant PCF8574. 
 * Le composant est connecté à un joystick fonctionant avec de bouttons (renvoie un signal numérique).
 * Apres avoir lut l'octet de données, si elle est différente de ca valeur null (0xFF), 
 * on  évalue quel bouton est préssé et on l'affiche sur le terminal. Et enfin on allume la led 2 tant que le bouton est préssé.         
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
#define RIGHT_PRESS 0xF7
#define UP_LEFT_PRESS 0xFC
#define UP_RIGHT_PRESS 0xF5
#define DOWN_LEFT_PRESS 0xFA
#define DOWN_RIGHT_PRESS 0xF3

#define LED2 4

/* library */
#include "PCF8574.h"

void bp_name(uint8_t bp_bin);

int main(void)
{
    PCF8574_init();

    uint8_t joystick;
    PCF8574_write_data(0xFF);
    while(1)
    {
        joystick = PCF8574_read_data() & 0x0F;
        while((joystick = PCF8574_read_data() & 0x0F) == 0xF); // Tant que le bus de donnée est égale à 0bxxxx 1111, on attend

        bp_name(joystick);

        PCF8574_digitalWrite(LED2, HIGH); // on allume la led 2
        while(joystick == (PCF8574_read_data() & 0x0F)); // on attend un changement sur le joystick
        PCF8574_digitalWrite(LED2,LOW); // on éteint la led 2
    }
    return 0;
}

/**
 * @brief Ecrit sur le terminal le bouton_PRESSé.
 * @param bp_bin donnée en 8 bit du bouton_PRESSé (seul les 4 premiers bits sont pris en compte).
 * @return rien.
 */
void bp_name(uint8_t bp_bin)
{
    bp_bin |= 0xF0; 
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