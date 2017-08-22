#include <fcntl.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#define I2C_BUS 1 
#define I2C_ADDRESS 0x40 

//write and read addresses of joystick
#define JOY_W_ADDR 0x80
#define JOY_R_ADDR 0x81

//Control register
#define CONTROL_1 0x0f
#define CONTROL_2 0x2e

//AGC, maximum sensitivity 0x3f
#define AGC 0x2A                       

//Scale input to fit to the 8 bit result register
// Writre 10 to set scaling Factor to 90.8%
#define _T_CTRL 0x2D                    

#define X_REG 0x10
#define Y_REG 0x11

#define POS_X 0x12
#define NEG_X 0x13

#define POS_Y 0x14
#define NEG_Y 0x15

#define MAX_BUS 64 // size of buffer for i2c bus

char namebuf[MAX_BUS];
int i2cbus_file; //file of i2cbus

void joystick_init(); 
void get_values(); 

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

    joystick_init();

    printf("Getting values\n");

    while (1) {
        get_values();
    }

    close(i2cbus_file);
}


void write_byte(unsigned char address, unsigned char value) {
    printf("Writing byte at address %d, with value %d\n", address, value);
    i2c_smbus_write_byte_data(i2cbus_file, address, value);
}

void write_word(__u8 address, __u16 value) {
    printf("Writing word at address %d, with value %d\n", address, value);
    i2c_smbus_write_byte_data(i2cbus_file, address, value);
}

__s32 read_byte(unsigned char address) {
    printf("Reading byte at address %d\n", address);
    __s32 value = i2c_smbus_read_byte_data(i2cbus_file, address);
    return value;
}

__s32 read_word(unsigned char address) {
    printf("Reading word at address %d\n", address);
    __s32 value = i2c_smbus_read_word_data(i2cbus_file, address);
    return value;
}

void joystick_init() {
	int temp; 

	write_byte(CONTROL_2, 0x84); 
	write_byte(AGC, 0x30); 

	write_byte(_T_CTRL, 10); 

	temp = read_byte(CONTROL_1); 
	temp &= 0x01; 

	write_byte(CONTROL_1, 0x88 | temp); 
}

void get_values() {
    signed int val_x = 0x00;
    signed int val_y = 0x00;

    signed int pos_x = 0x00; 
    signed int neg_x = 0x00; 

    signed int pos_y = 0x00; 
    signed int neg_y = 0x00; 

    val_x += read_word(X_REG);
    val_y += read_word(Y_REG);

    printf("Value of X=%d, Y=%d\n", val_x, val_y);

    pos_x = read_word(POS_X);
    pos_y = read_word(POS_Y);

    neg_x = read_word(NEG_X);
    neg_y = read_word(NEG_Y);

    printf("Value of Positive X = %d \n", pos_x);
    printf("Value of Positive Y = %d \n", pos_y);

    printf("Value of Negative X = %d \n", neg_x);
    printf("Value of Negative Y = %d \n", neg_y);


}

