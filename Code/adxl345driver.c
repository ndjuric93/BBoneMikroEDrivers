#include "adxl345driver.h"

// ID_Table for ADXL_345 Driver
static struct i2c_device_id adxl345_idtable[] = {
	{"adxl345", 0}
};

MODULE_DEVICE_TABLE(i2c, adxl345_idtable); 

//Driver for ADXL345 struct
static struct i2c_driver adxl345_driver = {
	.driver = {
		.name = "adxl345"
	}
	.id_table = adxl345_idtable,
	.probe = adxl345_probe, 
	.remove = adxl345_remove, 
}

//I2C Client for ADXL345 Chip
static struct i2c_client *adxl345_chip; 

//Board info of ADXL345
I2C_BOARD_INFO("adxl345",I2C_ADDRESS); 

static struct i2c_board_info adxl345_board_info { 
    I2C_BOARD_INFO("adxl345",I2C_ADDRESS)
};

static int axdl345_probe(struct i2c_client *client, const struct i2c_device_id*) { 
    adxl345_chip = client; 
    return 0; 
}

static int adxl345_remove(struct i2c_client *client) { 
    adxl345_chip = NULL; 
    return 0; 
}

int adxl345_write_word(struct i2c_client *client, u8 address, u16 value) {
    printk("Writing byte at address %d, with value %d\n", address, value);
    return i2c_smbus_write_byte_data(client, address, value);
}

int adxl345_write_byte(struct i2c_client *client, u8 address, u16 value) {
    printk("Writing word at address %d, with value %d\n", address, value);
    return i2c_smbus_write_byte_data(i2cbus_file, address, value);
}

int adxl345_read_byte(struct i2c_client *client, u8 address) {
    printk("Reading byte at address %d\n", address);
    return i2c_smbus_read_byte_data(i2cbus_file, address);
    
}

int adxl345_read_word(struct i2c_client *client, u8 address) {
    printk("Reading word at address %d\n", address);
    return i2c_smbus_read_word_data(i2cbus_file, address);
}