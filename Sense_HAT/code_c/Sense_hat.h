/**
 * @brief Library pour le shield Raspberry PI Sense HAT
 * 
 * @file Sense_hat.h
 * @ingroup SenseHAT
 * @copyright (c) Dorian ETCHEBER
 * @date 16.01.2023
 * 
 * @details
 * (A remplir)
 *
 * 
 * @note affiche une image sur une matrice de pixel 8x8
 * 
 * ## Usage
 * 
 * Basic usage is:
 * ```c
 * // Initialize 
 * struct tlc1543 *tlc;
 * tlc1543_init(&tlc, options);
 * 
 * // Read ADC channel
 * int value = tlc1543_get_sample(&tlc, channel);
 * 
 * // Free access
 * lc1543_delete(&tlc);
 * ``` 
 * 
 */

#ifndef SENSE_HAT_H
#define SENSE_HAT_H


#ifndef SENSE_HAT_FILEPATH
/** @brief Default frame buffer device sue by led matrix*/
#define SENSE_HAT_FILEPATH "/dev/fb0"
#endif

#ifndef SENSE_HAT_NUM_WORDS
/** @brief nombre de pixel présent sur le frame buffer */
#define SENSE_HAT_NUM_WORDS 64
#endif

#ifndef SENSE_HAT_FILESIZE
/** @brief taille du fichier contenant le mapping du frame buffer*/
#define SENSE_HAT_FILESIZE (SENSE_HAT_NUM_WORDS * sizeof(uint16_t))
#endif

#define DEV_PATH "/dev/i2c-1"
#define DEV_ID 0x5F
#define WHO_AM_I 0x0F

#define CTRL_REG1 0x20
#define CTRL_REG2 0x21

#define T0_OUT_L 0x3C
#define T0_OUT_H 0x3D
#define T1_OUT_L 0x3E
#define T1_OUT_H 0x3F
#define T0_degC_x8 0x32
#define T1_degC_x8 0x33
#define T1_T0_MSB 0x35

#define TEMP_OUT_L 0x2A
#define TEMP_OUT_H 0x2B

#define H0_T0_OUT_L 0x36
#define H0_T0_OUT_H 0x37
#define H1_T0_OUT_L 0x3A
#define H1_T0_OUT_H 0x3B
#define H0_rH_x2 0x30
#define H1_rH_x2 0x31

#define H_T_OUT_L 0x28
#define H_T_OUT_H 0x29


/**
 * @}
 * @name Error defines returned by functions 
 * @{
 */

/** @brief No error */
#define SENSE_HAT_SUCCESS 0
/** @brief Generic error */
#define SENSE_HAT_ERR -1
/** @brief Bad argument provided to function */
#define SENSE_HAT_ERR_ARG -10
/** @brief TLC1543 not initialized */
#define SENSE_HAT_ERR_NOINIT -11

#define DEFAULT_CLEAR 0


#include <fcntl.h>
#include <linux/fb.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/input.h>
#include <linux/fb.h>
#include <poll.h>
#include <dirent.h>
#include <i2c/smbus.h>
#include <linux/i2c-dev.h>








/**
 * @brief initialisation des modules pour la matrice de leds
 * 
 * @param direction vers le frame buffer du sense HAT 
 * @return retourne une valeur negatif en cas d'erreur  
 */



int fbfd;
int joystickDev;
struct fb_fix_screeninfo fix_info;
uint16_t *map;
uint16_t *p;


int senseHat_init()
{
    extern int fbfd;
    extern struct fb_fix_screeninfo fix_info;

	 fbfd = open(SENSE_HAT_FILEPATH, O_RDWR);
    if (fbfd == -1) {
        perror("Error (call to 'open')");
        return SENSE_HAT_ERR_NOINIT;
    }
 
    /* read fixed screen info for the open device */
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &fix_info) == -1) {
        perror("Error (call to 'ioctl')");
        close(fbfd);
        return SENSE_HAT_ERR_NOINIT;
    }

    /* now check the correct device has been found */
    if (strcmp(fix_info.id, "RPi-Sense FB") != 0) {
        printf("%s\n", "Error: RPi-Sense FB not found");
        close(fbfd);
        return SENSE_HAT_ERR_NOINIT;
    }

    map =
        mmap(NULL, SENSE_HAT_FILESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if (map == MAP_FAILED) {
        close(fbfd);
        perror("Error mmapping the file");
        exit(SENSE_HAT_ERR_NOINIT);
    }
    p = map; // on met la position du pointeur à 0
    return SENSE_HAT_SUCCESS;
}

void senseHat_clear(int color)
{
    for (int i = 0; i < SENSE_HAT_NUM_WORDS; i++)
        {
            *(p + i) = color;
        }
}

void senseHat_setPixel(int x, int y, int color)
{
    p = map;
    *(p + x + y * 8) = color;
}

void senseHat_setPixels(int mapping[64])
{
    p = map;
    for(int i = 0; i < SENSE_HAT_NUM_WORDS; i++)
    {
        *(p + i) = mapping[i];
    }
}

int senseHat_getPixel(int x, int y)
{
    p = map;
    return *(p + x + y *8);
}

int* senseHat_getPixels()
{
    static int mapping[64];
    p = map;
    for (int i = 0; i < SENSE_HAT_NUM_WORDS; i++)
    {
        mapping[i] = *(p + i);
    }
    return mapping;
}

void senseHat_flipR()
{

    int* mapping = senseHat_getPixels();

    for(int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            *(p + j + i * 8) = mapping[7 - i + j * 8];
        } 
    }
}

void delay(int);

int senseHat_humidity(void) {
    int fd = 0;
    uint8_t status = 0;

    /* open i2c comms */
    if ((fd = open(DEV_PATH, O_RDWR)) < 0) {
        perror("Unable to open i2c device");
        exit(1);
    }

    /* configure i2c slave */
    if (ioctl(fd, I2C_SLAVE, DEV_ID) < 0) {
        perror("Unable to configure i2c slave device");
        close(fd);
        exit(1);
    }

    /* check we are who we should be */
    if (i2c_smbus_read_byte_data(fd, WHO_AM_I) != 0xBC) {
        printf("%s\n", "who_am_i error");
        close(fd);
        exit(1);
    }

    /* Power down the device (clean start) */
    i2c_smbus_write_byte_data(fd, CTRL_REG1, 0x00);

    /* Turn on the humidity sensor analog front end in single shot mode  */
    i2c_smbus_write_byte_data(fd, CTRL_REG1, 0x84);

    /* Run one-shot measurement (temperature and humidity). The set bit will be reset by the
     * sensor itself after execution (self-clearing bit) */
    i2c_smbus_write_byte_data(fd, CTRL_REG2, 0x01);

    /* Wait until the measurement is completed */
    do {
        delay(25); /* 25 milliseconds */
        status = i2c_smbus_read_byte_data(fd, CTRL_REG2);
    } while (status != 0);

    /* Read calibration temperature LSB (ADC) data
     * (temperature calibration x-data for two points)
     */
    uint8_t t0_out_l = i2c_smbus_read_byte_data(fd, T0_OUT_L);
    uint8_t t0_out_h = i2c_smbus_read_byte_data(fd, T0_OUT_H);
    uint8_t t1_out_l = i2c_smbus_read_byte_data(fd, T1_OUT_L);
    uint8_t t1_out_h = i2c_smbus_read_byte_data(fd, T1_OUT_H);

    /* Read calibration temperature (°C) data
     * (temperature calibration y-data for two points)
     */
    uint8_t t0_degC_x8 = i2c_smbus_read_byte_data(fd, T0_degC_x8);
    uint8_t t1_degC_x8 = i2c_smbus_read_byte_data(fd, T1_degC_x8);
    uint8_t t1_t0_msb = i2c_smbus_read_byte_data(fd, T1_T0_MSB);

    /* Read calibration relative humidity LSB (ADC) data
     * (humidity calibration x-data for two points)
     */
    uint8_t h0_out_l = i2c_smbus_read_byte_data(fd, H0_T0_OUT_L);
    uint8_t h0_out_h = i2c_smbus_read_byte_data(fd, H0_T0_OUT_H);
    uint8_t h1_out_l = i2c_smbus_read_byte_data(fd, H1_T0_OUT_L);
    uint8_t h1_out_h = i2c_smbus_read_byte_data(fd, H1_T0_OUT_H);

    /* Read relative humidity (% rH) data
     * (humidity calibration y-data for two points)
     */
    uint8_t h0_rh_x2 = i2c_smbus_read_byte_data(fd, H0_rH_x2);
    uint8_t h1_rh_x2 = i2c_smbus_read_byte_data(fd, H1_rH_x2);

    /* make 16 bit values (bit shift)
     * (temperature calibration x-values)
     */
    int16_t T0_OUT = t0_out_h << 8 | t0_out_l;
    int16_t T1_OUT = t1_out_h << 8 | t1_out_l;

    /* make 16 bit values (bit shift)
     * (humidity calibration x-values)
     */
    int16_t H0_T0_OUT = h0_out_h << 8 | h0_out_l;
    int16_t H1_T0_OUT = h1_out_h << 8 | h1_out_l;

    /* make 16 and 10 bit values (bit mask and bit shift) */
    uint16_t T0_DegC_x8 = (t1_t0_msb & 3) << 8 | t0_degC_x8;
    uint16_t T1_DegC_x8 = ((t1_t0_msb & 12) >> 2) << 8 | t1_degC_x8;

    /* Calculate calibration values
     * (temperature calibration y-values)
     */
    double T0_DegC = T0_DegC_x8 / 8.0;
    double T1_DegC = T1_DegC_x8 / 8.0;

    /* Humidity calibration values
     * (humidity calibration y-values)
     */
    double H0_rH = h0_rh_x2 / 2.0;
    double H1_rH = h1_rh_x2 / 2.0;

    /* Solve the linear equasions 'y = mx + c' to give the
     * calibration straight line graphs for temperature and humidity
     */
    double t_gradient_m = (T1_DegC - T0_DegC) / (T1_OUT - T0_OUT);
    double t_intercept_c = T1_DegC - (t_gradient_m * T1_OUT);

    double h_gradient_m = (H1_rH - H0_rH) / (H1_T0_OUT - H0_T0_OUT);
    double h_intercept_c = H1_rH - (h_gradient_m * H1_T0_OUT);

    /* Read the ambient temperature measurement (2 bytes to read) */
    uint8_t t_out_l = i2c_smbus_read_byte_data(fd, TEMP_OUT_L);
    uint8_t t_out_h = i2c_smbus_read_byte_data(fd, TEMP_OUT_H);

    /* make 16 bit value */
    int16_t T_OUT = t_out_h << 8 | t_out_l;

    /* Read the ambient humidity measurement (2 bytes to read) */
    uint8_t h_t_out_l = i2c_smbus_read_byte_data(fd, H_T_OUT_L);
    uint8_t h_t_out_h = i2c_smbus_read_byte_data(fd, H_T_OUT_H);

    /* make 16 bit value */
    int16_t H_T_OUT = h_t_out_h << 8 | h_t_out_l;

    /* Calculate ambient temperature */
    double T_DegC = (t_gradient_m * T_OUT) + t_intercept_c;

    /* Calculate ambient humidity */
    double H_rH = (h_gradient_m * H_T_OUT) + h_intercept_c;

    /* Output */
    printf("Temp (from humid) = %.1f°C\n", T_DegC);
    printf("Humidity = %.0f%% rH\n", H_rH);

    /* Power down the device */
    i2c_smbus_write_byte_data(fd, CTRL_REG1, 0x00);
    close(fd);

    return (0);
}

void delay(int t) {
    usleep(t * 1000);
}

#endif