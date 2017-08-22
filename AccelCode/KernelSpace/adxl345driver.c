#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/gpio.h>

#include "adxl345utils.h"
#include "adxl345driver.h"

#define DRIVER_NAME "adxl34x"
#define INT1_GPIO 27 // GPIO number of Number 1 Place at MikroBUS

MODULE_LICENSE("GPL");              ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Nemanja Djuric");      ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A simple Linux Kernel Module for ADXL345");  ///< The description -- see modinfo
MODULE_VERSION("0.1");              ///< The version of the module

struct adxl345 {
    struct device *dev; 
    struct i2c_client *adxl345_chip; 
    struct coordinates *coords; 
    struct mutex mutex; 
    int irq; 
} adxl345;  

struct adxl345* accelerator; 
// ID_Table for ADXL_345 Driver
static struct i2c_device_id adxl345_idtable[] = {
	{ DRIVER_NAME, 0},
	{}
};

static irqreturn_t adxl34x_irq(int irq, void *handle) {
    printk("ADXL345 Interrupt Handled\n"); 
    return IRQ_HANDLED; 
}


MODULE_DEVICE_TABLE(i2c, adxl345_idtable); 

static const struct of_device_id adxl34x_of_id[] = {
	{.compatible = "adi, adxl345", }, 
	{.compatible = "adi, adxl34x", }, 
	{ }
};

MODULE_DEVICE_TABLE(of, adxl34x_of_id);

struct coordinates* init_coords() {
    struct coordinates* new_coords = (struct coordinates*) kzalloc(sizeof(struct coordinates), GFP_KERNEL);
    new_coords->x_val = kzalloc(sizeof(s32), GFP_KERNEL);
    new_coords->y_val = kzalloc(sizeof(s32), GFP_KERNEL);
    new_coords->z_val = kzalloc(sizeof(s32), GFP_KERNEL);

    return new_coords;
}

static int adxl345_probe(struct i2c_client *client, const struct i2c_device_id* id) { 
    int err; 

    accelerator = kzalloc(sizeof(struct adxl345), GFP_KERNEL); 
	printk("Commencing ADXL345 Probe function\n"); 

    accelerator -> coords = init_coords();
    accelerator -> dev = &(client -> dev); 
    accelerator -> irq = gpio_to_irq(INT1_GPIO);
    printk("IRQ Used is %d\n", accelerator -> irq); 

    printk("Initiating mutex\n"); 
    mutex_init(&(accelerator->mutex)); 
    printk("Initiated mutex\n"); 

    accelerator -> adxl345_chip = client; 
    //Initialisation
    adxl345_init(accelerator -> adxl345_chip); 
    
    /*err = request_threaded_irq(accelerator -> irq, 
                                adxl34x_irq,
                                adxl34x_irq,
                                IRQF_TRIGGER_HIGH | IRQF_ONESHOT, 
                                dev_name(accelerator -> dev), 
                                accelerator); */

    /*err = request_irq(accelerator -> irq,           // requested interrupt
                      adxl34x_irq, // pointer to handler function
                      IRQF_TRIGGER_HIGH | IRQF_ONESHOT, // interrupt mode flag
                      "adxl345 interrupt",        // used in /proc/interrupts
                      NULL);   */            // the *dev_id shared interrupt lines, NULL is okay

    err = request_threaded_irq(accelerator->irq, NULL, adxl34x_irq,
                   IRQF_TRIGGER_HIGH | IRQF_ONESHOT,
                   dev_name(accelerator->dev), accelerator);

    if(err) { 
        printk("Threaded IRQ not received\n"); 
    } else {
        printk("Threaded IRQ Received!!!\n");
    }
    initiate_interrupts(client); 

	printk("End of ADXL345 Probe function\n"); 

    return 0; 
}

static int adxl345_remove(struct i2c_client *client) { 
	printk("Commencing ADXL345 Remove function\n"); 
    // Coordinates of the ADXL345 should be freed aswell.
    free_irq(accelerator->irq, accelerator);
    kfree(accelerator); 

    printk("End of ADXL345 Probe function\n"); 
    return 0; 
}

void adxl345_init(struct i2c_client *client) {
    power_sequence(client);
    set_sensitivity(client, _2g);
    set_bandwidth(client, ADXL345_BW_0_05);
}

//Board info of ADXL345
/*I2C_BOARD_INFO(DRIVER_NAME, I2C_ADDRESS); 

static struct i2c_board_info adxl345_board_info { 
    I2C_BOARD_INFO(DRIVER_NAME, I2C_ADDRESS)
};
*/

void power_sequence(struct i2c_client *client) {
    printk("Starting power sequence\n");
    adxl345_write_byte(client, ADXL345_POWER_CTL, ADXL345_CLEAR_ALL);
    adxl345_write_byte(client, ADXL345_POWER_CTL, ADXL345_LINK);
    adxl345_write_byte(client, ADXL345_POWER_CTL, ADXL345_AUTO_SLEEP);
    printk("End of power sequence\n");
}

void initiate_interrupts(struct i2c_client *client) {
    int int_mask = 0; 
    printk("Interrupt mask is %d\n", int_mask); 

    adxl345_write_byte(client, ADXL345_INT_MAP, ADXL345_INT1_PIN);
    int_mask |= ADXL345_OVERRUN | ADXL345_INT_DATA_READY_BIT | ADXL345_INT_ACTIVITY_BIT | ADXL345_INT_INACTIVITY_BIT; 
    adxl345_write_byte(client, ADXL345_INT_ENABLE, int_mask);
    printk("Written Interrupt stuff to device.\n"); 

    printk("Interrupt mask is %d\n", int_mask); 
}

void set_sensitivity(struct i2c_client *client, u8 value) {
    adxl345_write_byte(client, ADXL345_DATA_FORMAT, value);
}

void set_bandwidth(struct i2c_client *client, u8 value) {
    adxl345_write_byte(client, ADXL345_BW_RATE, value);
}

/*static int __init adxl345_driver_init(void){
    printk("Hello, driver initiated"); 
    return i2c_add_driver(&adxl345_driver); 
}
module_init(adxl345_driver_init);

static void __exit adxl345_driver_exit(void){
    printk("Hello, driver deinitialized");
    i2c_del_driver(&adxl345_driver); 
}
module_exit(adxl345_driver_exit);*/

void get_values(struct i2c_client *client, struct coordinates* coords, float scale) {
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

    *(coords -> x_val) = (s32) (val_x); //* scale);
    *(coords -> y_val) = (s32) (val_y); //* scale);
    *(coords -> z_val) = (s32) (val_z); //* scale);

    printk("Value of X=%d, Y=%d, Z=%d\n", *(coords -> x_val), *(coords -> y_val), *(coords -> z_val));
}

//Driver for ADXL345 struct
static struct i2c_driver adxl345_driver = {
	.driver = {
		.name = DRIVER_NAME,
        .of_match_table = adxl34x_of_id,
	},
	.probe = adxl345_probe, 
	.remove = adxl345_remove,
	.id_table = adxl345_idtable 
};

//module init and exit macro
module_i2c_driver(adxl345_driver); 

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
