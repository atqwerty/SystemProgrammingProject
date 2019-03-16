#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/usb.h>
 
static int dev_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    printk(KERN_INFO "Dev drive (%04X:%04X) plugged\n", id->idVendor, id->idProduct);
    return 0;
}
 
static void dev_disconnect(struct usb_interface *interface)
{
    printk(KERN_INFO "Dev drive removed\n");
}
 
static struct usb_device_id dev_table[] =
{
    { USB_DEVICE(0x04e8, 0x6860) },
    {} /* Terminating entry */
};
MODULE_DEVICE_TABLE (usb, dev_table);
 
static struct usb_driver dev_driver =
{
    .name = "dev_driver",
    .id_table = dev_table,
    .probe = dev_probe,
    .disconnect = dev_disconnect,
};
 
static int __init dev_init(void)
{
    return usb_register(&dev_driver);
}
 
static void __exit dev_exit(void)
{
    usb_deregister(&dev_driver);
}
 
module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
