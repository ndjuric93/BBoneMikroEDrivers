#ifndef _ADXL345DRIVER_H_
#define _ADXL345DRIVER_H_

#define I2C_BUS 1 // Default I2C Bus that Device is using
#define I2C_ADDRESS 0x1d // Address of the device on the I2C Bus, first slot on ClickShield

static int adxl345_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int adxl345_remove(struct i2c_client *client); 

int adxl345_write_word(struct i2c_client *client, u8 reg, u16 value); 
int adxl345_write_byte(struct i2c_client *client, u8 reg, u16 value); 

int adxl345_read_word(struct i2c_client *client, u8 reg); 
int adxl345_read_byte(struct i2c_client *client, u8 reg); 

#endif