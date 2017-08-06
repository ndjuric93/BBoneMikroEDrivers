#include "adxl345utils.h"

MODULE_LICENSE("GPL");              ///< The license type -- this affects runtime behavior

int adxl345_write_word(struct i2c_client *client, u8 reg, u16 value) {
    printk("Writing byte at address %d, with value %d\n", reg, value);
    return i2c_smbus_write_byte_data(client, reg, value);
}

int adxl345_write_byte(struct i2c_client *client, u8 reg, u16 value) {
    printk("Writing word at address %d, with value %d\n", reg, value);
    return i2c_smbus_write_byte_data(client, reg, value);
}

int adxl345_read_byte(struct i2c_client *client, u8 reg) {
    printk("Reading byte at address %d\n", reg);
    return i2c_smbus_read_byte_data(client, reg);    
}

int adxl345_read_word(struct i2c_client *client, u8 reg) {
    printk("Reading word at address %d\n", reg);
    return i2c_smbus_read_word_data(client, reg);
}
