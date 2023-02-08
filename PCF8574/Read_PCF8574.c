

#define RPI_I2C_DEVICE "/dev/i2c-1"
#define PCF8571_I2C_ADDR 0x20

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <linux/i2c-dev.h>
#include <math.h>

#include "PCF8574.h"


int main(void)
{
    PCF8574_init();

    uint8_t buffer;
    while(1)
    {
        buffer = PCF8574_read_data();

        printf("octets de donné :\n   binaire : 0b");
        for(int i = 7; i>=0; i--)
        {
            short val_bin = (buffer>>i)%2;
            printf("%d",val_bin);
        }
        printf("   \n   hexa : 0x%x\n\n",buffer);
        sleep(1);
    }
    return 0;
}
