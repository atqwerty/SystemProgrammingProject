#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
// #include <sys/time.h>

// output for usb connect interruption
static int dev_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    // time_t current_time;
    // char* c_time_str;
    // c_time_str = ctime(&current_time);
    printk(KERN_INFO "Dev drive (%04X:%04X) plugged\n", id->idVendor, id->idProduct);
    // printk(KERN_INFO s_time_str);
    return 0;
}

// output for usb disconnect interruption
static void dev_disconnect(struct usb_interface *interface)
{
    printk(KERN_INFO "Dev drive removed\n");
}

// hardcoded device table to check which device is plugged (on which device we should interrupt)
static struct usb_device_id dev_table[] =
{
    { USB_DEVICE(0x04e8, 0x6860) }, // hardcoded usb device vendor and products id
    {} // govno 
};
MODULE_DEVICE_TABLE (usb, dev_table);

// device structure
static struct usb_driver dev_driver =
{
    .name = "dev_driver",
    .id_table = dev_table,
    .probe = dev_probe,
    .disconnect = dev_disconnect,
};

// start of module
static int __init dev_init(void)
{
    return usb_register(&dev_driver);
}
 
// module destructor
static void __exit dev_exit(void)
{
    usb_deregister(&dev_driver);
}

// modules initialization
module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
