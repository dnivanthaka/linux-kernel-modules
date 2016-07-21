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


static unsigned int gpioLED = 17;
static unsigned int gpioButton = 4;
static unsigned int irqNumber;
static bool ledOn = 0;

static irq_handler_t intled_callback(unsigned int irq, void *dev_id, struct pt_regs *regs){

    ledOn = !ledOn;

    gpio_set_value(gpioLED, ledOn);
    return (irq_handler_t)IRQ_HANDLED;
}

static int __init intled_init(void){
    int ret = 0;

    printk(KERN_INFO "INTLED: INIT Called\n");

    if(!gpio_is_valid(gpioLED)){
        printk(KERN_INFO "INTLED: invalid GPIO number\n"); 
        return -ENODEV;
    }

    gpio_request(gpioLED, "sysfs");
    gpio_direction_output(gpioLED, ledOn);
    gpio_export(gpioLED, false);


    gpio_request(gpioButton, "sysfs");
    gpio_direction_input(gpioButton);
    gpio_set_debounce(gpioButton, 250);
    gpio_export(gpioLED, false);

    irqNumber = gpio_to_irq(gpioButton);

    ret = request_irq(irqNumber, 
            (irq_handler_t)intled_callback, 
            IRQF_TRIGGER_RISING, 
            "myintled", 
            NULL);

    return ret;
}

static void __exit intled_exit(void){
    int ret = 0;

    gpio_set_value(gpioLED, 0);
    gpio_unexport(gpioLED);
    gpio_free(gpioLED);

    gpio_unexport(gpioButton);
    gpio_free(gpioButton);

    free_irq(irqNumber, NULL);

    printk(KERN_INFO "INTLED: EXIT Called\n");
}

MODULE_LICENSE("GPL");

module_init(intled_init);
module_exit(intled_exit);
