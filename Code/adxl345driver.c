#include <linux/slab.h>

#include "adxl345driver.h"

#define DRIVER_NAME "adxl345"

MODULE_LICENSE("GPL");              ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Nemanja Djuric");      ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A simple Linux Kernel Module for ADXL345");  ///< The description -- see modinfo
MODULE_VERSION("0.1");              ///< The version of the module

// ID_Table for ADXL_345 Driver
static struct i2c_device_id adxl345_idtable[] = {
	{ DRIVER_NAME, 0},
	{}
};

MODULE_DEVICE_TABLE(i2c, adxl345_idtable); 

struct coordinates* init_coords() {
    struct coordinates* new_coords = (struct coordinates*) kzalloc(sizeof(struct coordinates), GFP_KERNEL);
    new_coords->x_val = kzalloc(sizeof(s32), GFP_KERNEL);
    new_coords->y_val = kzalloc(sizeof(s32), GFP_KERNEL);
    new_coords->z_val = kzalloc(sizeof(s32), GFP_KERNEL);

    return new_coords;
}

static int adxl345_probe(struct i2c_client *client, const struct i2c_device_id* id) { 
    adxl345_chip = client; 

    //Initialisation
    adxl345_init(adxl345_chip); 

    struct coordinates* accel_cords = init_coords();
    get_values(adxl345_chip, accel_cords->x_val, accel_cords->y_val, accel_cords->z_val, 0);
	get_values(adxl345_chip, accel_cords->x_val, accel_cords->y_val, accel_cords->z_val, 0);
    return 0; 
}

static int adxl345_remove(struct i2c_client *client) { 
    adxl345_chip = NULL; 
    return 0; 
}

void adxl345_init(struct i2c_client *client) {
    power_sequence(client);
    set_sensitivity(client, _2g);
    set_bandwidth(client, ADXL345_BW_100);
}

//Driver for ADXL345 struct
static struct i2c_driver adxl345_driver = {
	.driver = {
		.name = DRIVER_NAME
	},
	.id_table = adxl345_idtable,
	.probe = adxl345_probe, 
	.remove = adxl345_remove
};

//Board info of ADXL345
/*I2C_BOARD_INFO(DRIVER_NAME, I2C_ADDRESS); 

static struct i2c_board_info adxl345_board_info { 
    I2C_BOARD_INFO(DRIVER_NAME, I2C_ADDRESS)
};
*/


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

void power_sequence(struct i2c_client *client) {
    printk("Starting power sequence\n");
    adxl345_write_byte(client, ADXL345_POWER_CTL, 0x00);
    adxl345_write_byte(client, ADXL345_POWER_CTL, 0x10);
    adxl345_write_byte(client, ADXL345_POWER_CTL, 0x08);
    printk("End of power sequence\n");
}

void set_sensitivity(struct i2c_client *client, u8 value) {
    adxl345_write_byte(client, ADXL345_DATA_FORMAT, value);
}

void set_bandwidth(struct i2c_client *client, u8 value) {
    adxl345_write_byte(client, ADXL345_BW_RATE, value);
}

static int __init adxl345_driver_init(void){
    printk("Hello, driver initiated"); 
    return i2c_add_driver(&adxl345_driver); 
}
module_init(adxl345_driver_init);

static void __exit adxl345_driver_exit(void){
    printk("Hello, driver deinitialized");
    i2c_del_driver(&adxl345_driver); 
}
module_exit(adxl345_driver_exit);

void get_values(struct i2c_client *client, s32 *x_val, s32 *y_val, s32 *z_val, float scale) {
    s32 val_x = 0x0000;
    s32 val_y = 0x0000;
    s32 val_z = 0x0000;
    u8 samples = 0x04;

    while (samples > 0) {
//        printf("Samples : %d\n", samples);
        val_x += adxl345_read_word(client, ADXL345_DATAX0);
//        printf("Value of X is %d\n", val_x);
        val_y += adxl345_read_word(client, ADXL345_DATAY0);
//        printf("Value of Y is %d\n", val_x);
        val_z += adxl345_read_word(client, ADXL345_DATAZ0);
//        printf("Value of Z is %d\n", val_x);
        samples--;
    }

//    printf("Sampling finished\n");

    val_x >>= 0x02;
    val_y >>= 0x02;
    val_z >>= 0x02;

    *x_val = (s32) (val_x); //* scale);
    *y_val = (s32) (val_y); //* scale);
    *z_val = (s32) (val_z); //* scale);

    printk("Value of X=%d, Y=%d, Z=%d\n", *x_val, *y_val, *z_val);
}

