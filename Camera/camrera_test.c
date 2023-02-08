/**
 * @brief Ce programme prend une photo via la piCam 
 *        et l'enregistre dans le dossier Projet_M1_ISE sous le format jpg 
 
 * @file camera_test.c
 * @copyright (c) Dorian ETCHEBER
 * @date 02.02.2023
 *
 * @details 
 * Hardware : Rpi4, PiCam
 * compilation : gcc -Wall -o camera_test.c test 
 * 
*/

/* Library */
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

#define RASPISTILL_PATH "/usr/bin/raspistill"
#define RASPISTILL_EXEC "raspistill"

/**
 * @brief Prend une photo via le port camera et l'enregsitre au dans le repertoir sélectioné
 * @details execute la commande shell : 
 * raspistill -n -vf -o filename
 * @param filename : chemin vers la ou on veur enregistrer l'image
 * @warning le filename doit finit par le nom de l'image (exemple : /image.jpg)
*/
void takePic(char* filename)
{
   execl(RASPISTILL_PATH,
    RASPISTILL_EXEC,
    "-n",
    "-vf",
    "-o",
    filename,
    NULL
    );
}

int main (void)
{
    char filname[] = "/home/pi/Desktop/Projet_M1_ISE_2022/image.jpg";
    takePic(filname);
    printf("Image successful..\n");
    return 0;
}