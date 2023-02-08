/**
 * @brief Ce programme fait clignoter la LED 2 du sheild RPI-Explorer-700 via le composant PCF8574
 
 * @file PCF8574_clignotement.c
 * @copyright (c) Dorian ETCHEBER
 * @date 08-02-2023
 *
 * @details 
 * Hardware : Rpi4, PCF8574
 * 
 * @warning n'oublier pas d'inclure la library PCF8574.h
 **/

/* sortie led 2 du composant PCF8574 */
#define LED2 4

/* library */
#include "PCF8574.h"

int main(void)
{
    /* initialisation de la communication i²C avec le composant PCF8574 */
    PCF8574_init();
    while(1){
        PCF8574_digitalWrite(LED2, HIGH);
        sleep(1);
        PCF8574_digitalWrite(LED2, LOW);
        sleep(1);
    }
}