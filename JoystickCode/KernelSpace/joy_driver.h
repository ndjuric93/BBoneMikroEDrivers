#ifndef _ADXL345DRIVER_H_
#define _ADXL345DRIVER_H_

#include <linux/module.h>
#include <linux/i2c.h>

#define I2C_BUS 1 // Default I2C Bus that Device is using
#define I2C_ADDRESS 0x1d // Address of the device on the I2C Bus, first slot on ClickShield

/*       Device Addressing      */
#define ADXL345_Write 0xA6
#define ADXL345_Read 0xA7

/*      Register names      */

#define ADXL345_DEVID 0x00
#define ADXL345_RESERVED1 0x01
#define ADXL345_THRESH_TAP 0x1D
#define ADXL345_OFSX 0x1E
#define ADXL345_OFSY 0x1F
#define ADXL345_OFSZ 0x20
#define ADXL345_DUR 0x21
#define ADXL345_LATENT 0x22
#define ADXL345_WINDOW 0x23
#define ADXL345_THRESH_ACT 0x24
#define ADXL345_THRESH_INACT 0x25
#define ADXL345_TIME_INACT 0x26
#define ADXL345_ACT_INACT_CTL 0x27
#define ADXL345_THRESH_FF 0x28
#define ADXL345_TIME_FF 0x29
#define ADXL345_TAP_AXES 0x2A
#define ADXL345_ACT_TAP_STATUS 0x2B
#define ADXL345_BW_RATE 0x2C
#define ADXL345_POWER_CTL 0x2D
#define ADXL345_INT_ENABLE 0x2E
#define ADXL345_INT_MAP 0x2F
#define ADXL345_INT_SOURCE 0x30
#define ADXL345_DATA_FORMAT 0x31
#define ADXL345_DATAX0 0x32
#define ADXL345_DATAX1 0x33
#define ADXL345_DATAY0 0x34
#define ADXL345_DATAY1 0x35
#define ADXL345_DATAZ0 0x36
#define ADXL345_DATAZ1 0x37
#define ADXL345_FIFO_CTL 0x38
#define ADXL345_FIFO_STATUS 0x39

#define ADXL345_BW_1600 0x0F
#define ADXL345_BW_800 0x0E
#define ADXL345_BW_400 0x0D
#define ADXL345_BW_200 0x0C
#define ADXL345_BW_100 0x0B
#define ADXL345_BW_50 0x0A
#define ADXL345_BW_25 0x09
#define ADXL345_BW_12_5 0x08
#define ADXL345_BW_6_25 0x07
#define ADXL345_BW_3_13 0x06
#define ADXL345_BW_1_56 0x05
#define ADXL345_BW_0_78 0x04
#define ADXL345_BW_0_39 0x03
#define ADXL345_BW_0_20 0x02
#define ADXL345_BW_0_10 0x01
#define ADXL345_BW_0_05 0x00

/* Power options */
#define ADXL345_AUTO_SLEEP 0x08
#define ADXL345_CLEAR_ALL  0x00
#define ADXL345_LINK       0x10

/*      Interrupt pins      */
//             INT1: 0
//             INT2: 1
#define ADXL345_INT1_PIN 0x00
#define ADXL345_INT2_PIN 0x01

/*       Interrupt bit position       */

#define ADXL345_INT_DATA_READY_BIT 0x07
#define ADXL345_INT_SINGLE_TAP_BIT 0x06
#define ADXL345_INT_DOUBLE_TAP_BIT 0x05
#define ADXL345_INT_ACTIVITY_BIT 0x04
#define ADXL345_INT_INACTIVITY_BIT 0x03
#define ADXL345_INT_FREE_FALL_BIT 0x02
#define ADXL345_INT_WATERMARK_BIT 0x01
#define ADXL345_INT_OVERRUNY_BIT 0x00

#define ADXL345_DATA_READY 0x07
#define ADXL345_SINGLE_TAP 0x06
#define ADXL345_DOUBLE_TAP 0x05
#define ADXL345_ACTIVITY 0x04
#define ADXL345_INACTIVITY 0x03
#define ADXL345_FREE_FALL 0x02
#define ADXL345_WATERMARK 0x01
#define ADXL345_OVERRUN 0x00

/*      G Sensitivity      */

#define _2g 0x00
#define _4g 0x01
#define _8g 0x02
#define _16g 0x03

#define FULL_RESOLUTION 0x08
#define JUSTIFY 0x04

typedef struct coordinates {
    s32* x_val;
    s32* y_val;
    s32* z_val;
} coordinates;

struct coordinates* init_coords(void); 

//static int adxl345_probe(struct i2c_client *client, const struct i2c_device_id *id);
//static int adxl345_remove(struct i2c_client *client); 

//Init function
void adxl345_init(struct i2c_client *client);
//ADXL Initiation functions
void power_sequence(struct i2c_client *client);
void initiate_interrupts(struct i2c_client *client);

void set_sensitivity(struct i2c_client *client, u8 value);
void set_bandwidth(struct i2c_client *client, u8 value);
//Adxl input functions
void get_values(struct i2c_client *client, struct coordinates* coords, float scale);

int adxl345_write_word(struct i2c_client *client, u8 reg, u16 value); 
int adxl345_write_byte(struct i2c_client *client, u8 reg, u16 value); 

int adxl345_read_word(struct i2c_client *client, u8 reg); 
int adxl345_read_byte(struct i2c_client *client, u8 reg); 

#endif