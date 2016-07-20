#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>

static struct timer_list my_timer;

void my_timer_callback(unsigned long data){
    printk(KERN_INFO "my_timer_callback called (%ld).\n", jiffies);
}

int init_module(){
    int ret;
    printk(KERN_INFO "Timer module installing\n");

    setup_timer(&my_timer, my_timer_callback, 0);
    printk(KERN_INFO "Starting timer to fire in 200ms (%ld)\n", jiffies);
    ret = mod_timer(&my_timer, jiffies + msecs_to_jiffies(200));

    if(ret)
        printk(KERN_INFO "Error in mod_timer\n");

    return 0;
}


void cleanup_module(){
    int ret;

    ret = del_timer(&my_timer);

    if(ret)
        printk("The timer is still in use..\n");

    printk(KERN_INFO "Timer module uninstalling\n");

    return;
}
MODULE_LICENSE("GPL");
