#ifndef _ADXL345DRIVER_H_
#define _ADXL345DRIVER_H_

#include <linux/module.h>
#include <linux/i2c.h>

typedef struct coordinates {
    s8* x_val;
    s8* y_val;
} coordinates;

struct coordinates* init_coords(void); 

//static int adxl345_probe(struct i2c_client *client, const struct i2c_device_id *id);
//static int adxl345_remove(struct i2c_client *client); 

//Init function
void as5013_init(struct i2c_client *client);
void offset_calibrate(struct i2c_client *client);
void set_deadzone(struct i2c_client *client);
void get_values(struct i2c_client *client, struct coordinates* coords);

int as5013_write_word(struct i2c_client *client, u8 reg, u16 value); 
int as5013_write_byte(struct i2c_client *client, u8 reg, u16 value); 

int as5013_read_word(struct i2c_client *client, u8 reg); 
int as5013_read_byte(struct i2c_client *client, u8 reg); 

#endif