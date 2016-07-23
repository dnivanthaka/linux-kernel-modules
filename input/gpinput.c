/*
 * Interrupt driven input module
 *
 * /proc/bus/input/devices
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/interrupt.h>
#include <linux/input.h>


#define DRVNAME "gpiokey"

static unsigned int gpioButton = 4;
static unsigned int irqNumber;
static struct input_dev *idev;

struct gp_button_data{
    unsigned short btncode[1];
    struct input_dev *dev;
    char name[64];
    char phys[32];
    unsigned char laststate;
};

static irq_handler_t gpinput_callback(unsigned int irq, void *dev_id, struct pt_regs *regs){

    printk(KERN_INFO "GPINPUT: Button pressed\n");

    input_report_key(idev, BTN_0, 1); //keypress
    //input_report_key(idev, BTN_0, 0); //release
    input_sync(idev);

    return (irq_handler_t)IRQ_HANDLED;
}

static int __init gpinput_init(void){
    int ret = 0;
    struct gp_button_data *ddata; 

    printk(KERN_INFO "GPINPUT: INIT Called\n");

    if(!gpio_is_valid(gpioButton)){
        printk(KERN_INFO "GPINPUT: invalid GPIO number\n"); 
        return -ENODEV;
    }

    //ddata = devm_kzalloc();

    idev = input_allocate_device();
    if(!idev){
        printk(KERN_ERR "Cant allocate input device\n");
        return -ENOMEM;
    }

    //idev->evbit[0] = BIT_MASK(EV_KEY);
    //idev->keycode = BTN_0;
    //idev->keycodesize = 1;
    __set_bit(EV_KEY, idev->evbit);
    //idev->keybit[BIT_WORD(LED_NUML)] = BIT_MASK(LED_NUML);
    idev->name = "gpinput";
    idev->phys = "gpinput/input0";
    idev->id.bustype = BUS_HOST;
    idev->id.vendor = 0x0001;
    idev->id.product = 0x0001;
    idev->id.version = 0x0100;

    gpio_request(gpioButton, "sysfs");
    gpio_direction_input(gpioButton);
    gpio_set_debounce(gpioButton, 250);
    gpio_export(gpioButton, false);

    irqNumber = gpio_to_irq(gpioButton);

    ret = request_irq(irqNumber, 
            (irq_handler_t)gpinput_callback, 
            IRQF_TRIGGER_RISING, 
            "gpinput", 
            NULL);

    ret = input_register_device(idev);
    if(ret){
        printk(KERN_ERR "GPINPUT: Failed to register device\n");
    }

    return ret;
}

static void __exit gpinput_exit(void){
    int ret = 0;


    gpio_unexport(gpioButton);
    gpio_free(gpioButton);

    free_irq(irqNumber, NULL);
    input_unregister_device(idev);

    printk(KERN_INFO "GPINPUT: EXIT Called\n");
}

MODULE_LICENSE("GPL");

module_init(gpinput_init);
module_exit(gpinput_exit);
