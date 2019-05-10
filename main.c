#include <linux/kernel.h>
// #include <linux/config.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/errno.h>
// #include <asm/uaccess.h>
// #include <linux/smp_lock.h>
#include <linux/usb.h>
#include <linux/kref.h>


struct usb_skel {
	struct usb_device *	udev;			/* the usb device for this device */
	struct usb_interface *	interface;		/* the interface for this device */
	unsigned char *		bulk_in_buffer;		/* the buffer to receive data */
	size_t			bulk_in_size;		/* the size of the receive buffer */
	__u8			bulk_in_endpointAddr;	/* the address of the bulk in endpoint */
	__u8			bulk_out_endpointAddr;	/* the address of the bulk out endpoint */
	struct kref		kref;
};

// output for usb connect interruption
static int dev_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    struct usb_skel *device_skeleton = NULL;
	struct usb_host_interface *iface_descriptor;
	struct usb_endpoint_descriptor *endpoint;
	size_t buffer_size;
    int return_output = -ENOMEM;

    // Device skeleton memory allocation
    device_skeleton = kzalloc(sizeof(struct usb_skel), GFP_KERNEL);
	if (device_skeleton == NULL) {
		printk(KERN_INFO "Out of memory");
		// goto error;
    }
    memset(device_skeleton, 0x00, sizeof (*device_skeleton));
    kref_init(&device_skeleton->kref);

    printk(KERN_INFO "Dev drive (%04X:%04X) plugged\n", id->idVendor, id->idProduct);
    printk(KERN_INFO "Test: %d\n", device_skeleton->udev->devnum);
    
// error:
// 	if (device_skeleton)
//         printk(KERN_INFO "Error");
// 		// kref_put(&device_skeleton->kref, skel_delete);
// 	return return_output;

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
    {}// { USB_DEVICE(0X0566, 0x3002)} // govno 
};
MODULE_DEVICE_TABLE (usb, dev_table);

// device structure
static struct usb_driver dev_driver =
{
    // .owner = THIS_MODULE,
    .name = "Markitanov Denis driver",
    .id_table = dev_table,
    .probe = dev_probe,
    .disconnect = dev_disconnect,
};

// start of module
static int __init dev_init(void)
{
    int result = usb_register(&dev_driver);
    if(result){
        printk(KERN_INFO "Usb device register failed. Error: %d\n", result);
    }
    return result;
}
 
// module destructor
static void __exit dev_exit(void)
{
    usb_deregister(&dev_driver);
    printk(KERN_INFO "Shutting down the driver");
}

// modules initialization
module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
