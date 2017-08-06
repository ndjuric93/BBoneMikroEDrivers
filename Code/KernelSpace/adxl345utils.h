#ifndef _ADXL345UTILS_H_
#define _ADXL345UTILS_H_

#include <linux/module.h>
#include <linux/i2c.h>

int adxl345_write_word(struct i2c_client *client, u8 reg, u16 value); 
int adxl345_write_byte(struct i2c_client *client, u8 reg, u16 value); 

int adxl345_read_word(struct i2c_client *client, u8 reg); 
int adxl345_read_byte(struct i2c_client *client, u8 reg); 

#endif