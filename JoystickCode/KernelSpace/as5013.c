#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/gpio.h>

#include "as5013.h"

#define DRIVER_NAME "as5013"
#define INT1_GPIO 27 // GPIO number of Number 1 Place at MikroBUS

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
#define CTRL_M 0x2B
#define CTRL_T 0x2C
#define CTRL_J 0x2D

#define X_REG 0x10
#define Y_REG 0x11

#define POS_X 0x12
#define NEG_X 0x13

#define POS_Y 0x14
#define NEG_Y 0x15

__s32 offset_x;
__s32 offset_y;

struct as5013 {
    struct device *dev; 
    struct i2c_client *as5013_chip; 
    struct coordinates *coords; 
    struct mutex mutex; 
    int irq; 
} as5013;  

struct as5013* joystick; 
// ID_Table for AS5013 Driver
static struct i2c_device_id as5013_idtable[] = {
	{ DRIVER_NAME, 0},
	{}
};

static irqreturn_t as5013_irq(int irq, void *handle) {
    //printk("AS5013 Interrupt Handled\n"); 
    get_values(joystick -> as5013_chip, joystick -> coords);
    return IRQ_HANDLED; 
}


MODULE_DEVICE_TABLE(i2c, as5013_idtable); 

static const struct of_device_id as5013_of_id[] = {
	{.compatible = "as5013" }, 
	{ }
};

MODULE_DEVICE_TABLE(of, as5013_of_id);

struct coordinates* init_coords() {
    struct coordinates* new_coords = (struct coordinates*) kzalloc(sizeof(struct coordinates), GFP_KERNEL);
    new_coords->x_val = kzalloc(sizeof(s8), GFP_KERNEL);
    new_coords->y_val = kzalloc(sizeof(s8), GFP_KERNEL);

    return new_coords;
}

static int as5013_probe(struct i2c_client *client, const struct i2c_device_id* id) { 
    int err; 

    joystick = kzalloc(sizeof(struct as5013), GFP_KERNEL); 
	printk("Commencing AS5013 Probe function\n"); 

    joystick -> coords = init_coords();
    joystick -> dev = &(client -> dev); 
    joystick -> irq = gpio_to_irq(INT1_GPIO); printk("IRQ Used is %d\n", joystick -> irq);

    printk("Initiating mutex\n"); 
    mutex_init(&(joystick->mutex)); 
    printk("Initiated mutex\n"); 

    joystick -> as5013_chip = client; 
    //Initialisation
    as5013_init(joystick -> as5013_chip); 
    offset_calibrate(joystick -> as5013_chip);
    get_values(joystick -> as5013_chip, joystick -> coords);
    /*err = request_threaded_irq(joystick -> irq, 
                                as5013_irq,
                                as5013_irq,
                                IRQF_TRIGGER_HIGH | IRQF_ONESHOT, 
                                dev_name(joystick -> dev), 
                                joystick); */

    /*err = request_irq(joystick -> irq,           // requested interrupt
                      as5013_irq, // pointer to handler function
                      IRQF_TRIGGER_HIGH | IRQF_ONESHOT, // interrupt mode flag
                      "adxl345 interrupt",        // used in /proc/interrupts
                      NULL);   */            // the *dev_id shared interrupt lines, NULL is okay

    err = request_threaded_irq(joystick->irq, NULL, as5013_irq,
                   IRQF_TRIGGER_HIGH | IRQF_ONESHOT,
                   dev_name(joystick->dev), joystick);

    if(err) { 
        printk("Threaded IRQ not received\n"); 
    } else {
        printk("Threaded IRQ Received!!!\n");
    }
	printk("End of AS5013 Probe function\n"); 

    return 0; 
}

static int as5013_remove(struct i2c_client *client) { 
	printk("Commencing AS5013 Remove function\n"); 
    // Coordinates of the AS5013 should be freed aswell.
    free_irq(joystick->irq, joystick);
    kfree(joystick); 

    printk("End of AS5013 Probe function\n"); 
    return 0; 
}

void as5013_init(struct i2c_client *client) {
    as5013_write_byte(client, CONTROL_1, 0x02);

    as5013_write_byte(client, CTRL_T, 0x2d);
    as5013_write_byte(client, CTRL_M, 0x2d);
    as5013_write_byte(client, CTRL_J, 0x2d);

    as5013_write_byte(client, AGC, 0x3f);

    as5013_write_byte(client, CONTROL_1, 0x89); // Put Idle mode, integers on, and ready to read from x and y regitsers.
    as5013_write_byte(client, CONTROL_2, 0x84); // Put control bits from data sheet.
}

void offset_calibrate(struct i2c_client *client) {
    int i; 
    int x_calibrate = 0;
    int y_calibrate = 0;

    as5013_read_byte(client, X_REG);
    as5013_read_byte(client, Y_REG);

    for (i = 0; i < 32; i++) {
        x_calibrate += as5013_read_byte(client, X_REG);
        y_calibrate += as5013_read_byte(client, Y_REG);
    }

    offset_x = -(x_calibrate >> 5);
    offset_y = -(y_calibrate >> 5);
}

void set_deadzone(struct i2c_client *client) {
    as5013_write_byte(client, POS_X, 0);
    as5013_write_byte(client, POS_Y, 0);
    as5013_write_byte(client, NEG_X, 0);
    as5013_write_byte(client, NEG_Y, 0);
}

void get_values(struct i2c_client *client, struct coordinates* coords) {
    s8 val_x = 0x00;
    s8 val_y = 0x00;

    val_x = as5013_read_byte(client, X_REG);
    val_y = as5013_read_byte(client, Y_REG);

    val_x += offset_x;
    val_y += offset_y;

    if (val_x > 127) {
        val_x = 127;
    } else if (val_x < -128) {
        val_x = -128;
    }

    if (val_y > 127) {
        val_y = 127;
    } else if (val_y < -128) {
        val_y = -128;
    }

    coords -> x_val = val_x; 
    coords -> y_val = val_y; 

    printk("Value of X=%d, Y=%d\n", val_x, val_y);
}

//Driver for ADXL345 struct
static struct i2c_driver as5013_driver = {
	.driver = {
		.name = DRIVER_NAME,
        .of_match_table = as5013_of_id,
	},
	.probe = as5013_probe, 
	.remove = as5013_remove,
	.id_table = as5013_idtable 
};

//module init and exit macro
module_i2c_driver(as5013_driver); 

int as5013_write_word(struct i2c_client *client, u8 reg, u16 value) {
    return i2c_smbus_write_byte_data(client, reg, value);
}

int as5013_write_byte(struct i2c_client *client, u8 reg, u16 value) {
    return i2c_smbus_write_byte_data(client, reg, value);
}

int as5013_read_byte(struct i2c_client *client, u8 reg) {
    return i2c_smbus_read_byte_data(client, reg);    
}

int as5013_read_word(struct i2c_client *client, u8 reg) {
    return i2c_smbus_read_word_data(client, reg);
}

MODULE_LICENSE("GPL");              ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Nemanja Djuric");      ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A simple Linux Kernel Module for AS5013");  ///< The description -- see modinfo
MODULE_VERSION("0.1");              ///< The version of the module