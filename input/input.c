/*
 * Interrupt driven LED module
 *
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/interrupt.h>

#define DRVNAME "gpiokey"

static unsigned int gpioButton = 4;
static unsigned int irqNumber;

struct gp_button_data{
    unsigned short btncode[1];
    struct input_dev *dev;
    char name[64];
    char phys[32];
    unsigned char laststate;
};

static irq_handler_t gpintput_callback(unsigned int irq, void *dev_id, struct pt_regs *regs){

    ledOn = !ledOn;

    gpio_set_value(gpioLED, ledOn);
    return (irq_handler_t)IRQ_HANDLED;
}

static int __init gpintput_init(void){
    int ret = 0;

    printk(KERN_INFO "GPINPUT: INIT Called\n");

    if(!gpio_is_valid(gpioButton)){
        printk(KERN_INFO "GPINPUT: invalid GPIO number\n"); 
        return -ENODEV;
    }

    gpio_request(gpioButton, "sysfs");
    gpio_direction_input(gpioButton);
    gpio_set_debounce(gpioButton, 250);
    gpio_export(gpioButton, false);

    irqNumber = gpio_to_irq(gpioButton);

    ret = request_irq(irqNumber, 
            (irq_handler_t)intled_callback, 
            IRQF_TRIGGER_RISING, 
            "gpinput", 
            NULL);

    return ret;
}

static void __exit gpintput_exit(void){
    int ret = 0;


    gpio_unexport(gpioButton);
    gpio_free(gpioButton);

    free_irq(irqNumber, NULL);

    printk(KERN_INFO "INTLED: EXIT Called\n");
}

MODULE_LICENSE("GPL");

module_init(gpinput_init);
module_exit(gpinput_exit);
