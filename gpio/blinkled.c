/*
 * Timer driven blink module
 *
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/timer.h>

#define TIMER_ON_PERIOD 200
#define TIMER_OFF_PERIOD 1500

static struct timer_list my_timer;

static unsigned int gpioLED = 17;
static bool ledOn = false;

void my_timer_callback(unsigned long data){
    int timer_period = TIMER_ON_PERIOD;

    ledOn = !ledOn;

    gpio_set_value(gpioLED, ledOn);
    
    if(!ledOn)
        timer_period = TIMER_OFF_PERIOD;

    mod_timer(&my_timer, jiffies + msecs_to_jiffies(timer_period));
}

static int __init blinkled_init(void){
    int ret = 0;

    printk(KERN_INFO "BLINKLED: INIT Called\n");

    if(!gpio_is_valid(gpioLED)){
        printk(KERN_INFO "BLINKLED: invalid GPIO number\n"); 
        return -ENODEV;
    }

    gpio_request(gpioLED, "sysfs");
    gpio_direction_output(gpioLED, ledOn);
    gpio_export(gpioLED, false);


    //Setting up timer
    setup_timer(&my_timer, my_timer_callback, 0);
    ret = mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIMER_OFF_PERIOD));
    if(ret) printk("BLINKLED: Error in mod_timer\n");

    return ret;
}

static void __exit blinkled_exit(void){
    int ret = 0;

    gpio_set_value(gpioLED, 0);
    gpio_unexport(gpioLED);
    gpio_free(gpioLED);

    ret = del_timer(&my_timer);
    if(ret) printk(KERN_INFO "BLINKLED: Timer is still in user\n");

    printk(KERN_INFO "BLINKLED: EXIT Called\n");
}

MODULE_LICENSE("GPL");

module_init(blinkled_init);
module_exit(blinkled_exit);
