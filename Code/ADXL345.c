#include <fcntl.h>
<<<<<<< HEAD
#include <stdio.h>
#include <unistd.h> // for close
#include <stdlib.h>
=======
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
>>>>>>> 63c3d1b415376bd7ea3528f08cf7aeeb9057dc02

#include "Utils.h"
#include "ADXL345.h"

#define I2C_BUS 1 // Default I2C Bus that Device is using
#define I2C_ADDRESS 0x1d // Address of the device on the I2C Bus, first slot on ClickShield
<<<<<<< HEAD
#define MAX_BUS 64 // size of buffer for i2c bus

char namebuf[MAX_BUS];
int i2cbus_file; //file of i2cbus

int main(int argc, char* arvg[]) {

    snprintf(namebuf, sizeof(namebuf), "/dev/i2c-%d", I2C_BUS);

    if ((i2cbus_file = open(namebuf, O_RDWR)) < 0) {
        printf("I2CBus %s failed to open successfully\n", namebuf);
        return (1);
    } else {
        printf("I2CBus successfully opened %s\n", namebuf);
    }

    if (ioctl(i2cbus_file, I2C_SLAVE, I2C_ADDRESS) < 0) {
        printf("I2C_SLAVE address failed...\n");
        return (2);
    }

    ADXL345_init();

    struct coordinates* accel_cords = init_coords();
    struct coordinates* old_cords = init_coords();

    float scale = 1.0f;
    printf("Getting values\n");

    while (1) {
        get_values(accel_cords->x_val, accel_cords->y_val, accel_cords->z_val,
                scale);
        if (coords_difference(accel_cords, old_cords)) {
            old_cords = accel_cords;
            printf("X = %d , Y = %d, Z = %d\n", *(old_cords->x_val),
                    *(old_cords->y_val), *(old_cords->z_val));
        }
=======
#define MAX_BUS 64

char namebuf[MAX_BUS];
int i2cbus_file; //file of i2cbus

int main(int argc, char* arvg[]) {

    snprintf(namebuf, sizeof(namebuf), "/dev/i2c-%d", I2C_BUS);

    if ((i2cbus_file = open(namebuf, O_RDWR)) < 0) {
        printf("Failed to open I2C Bus on %s I2CBus:", namebuf, I2C_BUS);
        return (1);
    } else {
        printf("I2CBus successfully opened %s", namebuf);
    }

    if (ioctl(i2cbus_file, I2C_SLAVE, I2C_ADDRESS) < 0) {
        printf("I2C_SLAVE address failed...");
        return (2);
>>>>>>> 63c3d1b415376bd7ea3528f08cf7aeeb9057dc02
    }

    close(i2cbus_file);
}

void ADXL345_init() {
    power_sequence();
    set_sensitivity(_2g);
    set_bandwidth(ADXL345_BW_100);
}

void ADXL345_write_byte(unsigned char address, unsigned char value) {
<<<<<<< HEAD
    printf("Writing byte at address %d, with value %d\n", address, value);
    i2c_smbus_write_byte_data(i2cbus_file, address, value);
}

void ADXL345_write_word(__u8 address, __u16 value) {
    printf("Writing word at address %d, with value %d\n", address, value);
=======

    i2c_smbus_write_byte_data(i2cbus_file, address, value);

}
//
void ADXL345_write_word(unsigned char address, unsigned int value) {
>>>>>>> 63c3d1b415376bd7ea3528f08cf7aeeb9057dc02
    i2c_smbus_write_word_data(i2cbus_file, address, value);
}

__s32 ADXL345_read_byte(unsigned char address) {
<<<<<<< HEAD
    printf("Reading byte at address %d\n", address);
=======
>>>>>>> 63c3d1b415376bd7ea3528f08cf7aeeb9057dc02
    __s32 value = i2c_smbus_read_byte_data(i2cbus_file, address);
    return value;
}

<<<<<<< HEAD
__s32 ADXL345_read_word(unsigned char address) {
//    printf("Reading word at address %d\n", address);
=======
unsigned int ADXL345_read_word(unsigned char address) {

>>>>>>> 63c3d1b415376bd7ea3528f08cf7aeeb9057dc02
    __s32 value = i2c_smbus_read_word_data(i2cbus_file, address);
    return value;
}

void power_sequence() {
    printf("Starting power sequence\n");
    ADXL345_write_byte(ADXL345_POWER_CTL, 0x00);
    ADXL345_write_byte(ADXL345_POWER_CTL, 0x10);
    ADXL345_write_byte(ADXL345_POWER_CTL, 0x08);
    printf("End of power sequence\n");
}

void set_sensitivity(unsigned char value) {
    ADXL345_write_byte(ADXL345_DATA_FORMAT, value);
}

void set_bandwidth(unsigned char value) {
    ADXL345_write_byte(ADXL345_BW_RATE, value);
}

void get_values(__s32 *x_val, __s32 *y_val, __s32 *z_val, float scale) {
    signed int val_x = 0x0000;
    signed int val_y = 0x0000;
    signed int val_z = 0x0000;
    unsigned char samples = 0x04;

    while (samples > 0) {
//        printf("Samples : %d\n", samples);
        val_x += ADXL345_read_word(ADXL345_DATAX0);
//        printf("Value of X is %d\n", val_x);
        val_y += ADXL345_read_word(ADXL345_DATAY0);
//        printf("Value of Y is %d\n", val_x);
        val_z += ADXL345_read_word(ADXL345_DATAZ0);
//        printf("Value of Z is %d\n", val_x);
        samples--;
    }

//    printf("Sampling finished\n");

    val_x >>= 0x02;
    val_y >>= 0x02;
    val_z >>= 0x02;

    *x_val = (__s32) (val_x * scale);
    *y_val = (__s32) (val_y * scale);
    *z_val = (__s32) (val_z * scale);

    printf("Value of X=%d, Y=%d, Z=%d\n", *x_val, *y_val, *z_val);

}

struct coordinates* init_coords() {
    struct coordinates* new_coords = (struct coordinates*) malloc(
            sizeof(struct coordinates));
    new_coords->x_val = malloc(sizeof(__s32 ));
    new_coords->y_val = malloc(sizeof(__s32 ));
    new_coords->z_val = malloc(sizeof(__s32 ));

    return new_coords;
}

