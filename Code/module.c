/**
 * @file    module.c
 * @author  Nemanja Djuric
*/

#include <linux/init.h>             // Macros used to mark up functions e.g., __init __exit
#include <linux/module.h>           // Core header for loading LKMs into the kernel
#include <linux/kernel.h>           // Contains types, macros, functions for the kernel

#include "adxl345.h"
#include "adxl345driver.h"

MODULE_LICENSE("GPL");              ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Derek Molloy");      ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A simple Linux driver for the BBB.");  ///< The description -- see modinfo
MODULE_VERSION("0.1");              ///< The version of the module

static char *name = "Nemanja!";        ///< An example LKM argument -- default value is "world"
module_param(name, charp, S_IRUGO); ///< Param desc. charp = char ptr, S_IRUGO can be read/not changed
MODULE_PARM_DESC(name, "The name to display in /var/log/kern.log");  ///< parameter description

#define I2C_BUS 1 // Default I2C Bus that Device is using
#define I2C_ADDRESS 0x1d // Address of the device on the I2C Bus, first slot on ClickShield
#define MAX_BUS 64 // size of buffer for i2c bus

char namebuf[MAX_BUS];
int i2cbus_file; //file of i2cbus

/** @brief The LKM initialization function
 *  The static keyword restricts the visibility of the function to within this C file. The __init
 *  macro means that for a built-in driver (not a LKM) the function is only used at initialization
 *  time and that it can be discarded and its memory freed up after that point.
 *  @return returns 0 if successful
 */
static int __init adxl345_driver_init(void){
    printk("Hello, driver initiated"); 
    return i2c_add_driver(&adxl345_driver); 
}
module_init(adxl345_init_driver)
;

static void __exit adxl345_exit_driver(void){i
    printk("Hello, driver deinitialized")
    i2c_del_driver(&adxl345_driver); 
}
module_exit(adxl345_exit_driver);
