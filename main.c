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

#define MINOR_BASE	192

// USB skeleton struct
struct usb_skel {
	struct usb_device *	udev;			/* the usb device for this device */
	struct usb_interface *	interface;		/* the interface for this device */
	unsigned char *		bulk_in_buffer;		/* the buffer to receive data */
	size_t			bulk_in_size;		/* the size of the receive buffer */
	__u8			bulk_in_endpointAddr;	/* the address of the bulk in endpoint */
	__u8			bulk_out_endpointAddr;	/* the address of the bulk out endpoint */
	struct kref		kref;
};

static struct usb_class_driver skel_class = {
	.name = "usb/skel%d",
	// .fops = &skel_fops,
	// .mode = S_IFCHR | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH,
	.minor_base = MINOR_BASE,
};

// Output for usb connect interruption
static int dev_probe(struct usb_interface *interface, const struct usb_device_id *id)
{
    struct usb_skel *device_skeleton = NULL;
	struct usb_host_interface *iface_descriptor;
	struct usb_endpoint_descriptor *endpoint;
	size_t buffer_size;
    int return_output = -ENOMEM;
    int i;

    // Device skeleton memory allocation
    device_skeleton = kzalloc(sizeof(struct usb_skel), GFP_KERNEL);
	if (device_skeleton == NULL) {
		printk(KERN_INFO "Out of memory");
    }
    memset(device_skeleton, 0x00, sizeof (*device_skeleton));
    kref_init(&device_skeleton->kref);

    device_skeleton->udev = usb_get_dev(interface_to_usbdev(interface));
	device_skeleton->interface = interface;
    iface_descriptor = interface->cur_altsetting;

    // Endpoint memory allocation and declaration
    for(i = 0; i < iface_descriptor->desc.bNumEndpoints; i++){
        endpoint = &iface_descriptor->endpoint[i].desc;
        
        if((!device_skeleton->bulk_in_endpointAddr) && (endpoint->bEndpointAddress & USB_DIR_IN) &&
            ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_BULK)){
            buffer_size = endpoint->wMaxPacketSize;
            device_skeleton->bulk_in_size = buffer_size;
            device_skeleton->bulk_in_endpointAddr = endpoint->bEndpointAddress;
            device_skeleton->bulk_in_buffer = kmalloc(buffer_size, GFP_KERNEL);

            // Error in bulk in buffer
            if (!device_skeleton->bulk_in_buffer) {
                printk(KERN_INFO "Could not allocate bulk_in_buffer");
            }
        
        }

        if (!device_skeleton->bulk_out_endpointAddr && !(endpoint->bEndpointAddress & USB_DIR_IN) &&
		    ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_BULK)) {
			
            // There is an endpoint
			device_skeleton->bulk_out_endpointAddr = endpoint->bEndpointAddress;
		}
        
    }

    if (!(device_skeleton->bulk_in_endpointAddr && device_skeleton->bulk_out_endpointAddr)) {
        printk(KERN_INFO "Could not find both bulk-in and bulk-out endpoints");
	}

	// Save to pointer
	usb_set_intfdata(interface, device_skeleton);

	// Device registration
	return_output = usb_register_dev(interface, &skel_class);

	if (return_output) {

		// Register device error
		printk(KERN_INFO "Not able to get a minor for this device.");
		usb_set_intfdata(interface, NULL);
	}

	printk(KERN_INFO "USB Skeleton device now attached to USBSkel-%d", interface->minor);
    printk(KERN_INFO "Dev drive (%04X:%04X) plugged\n", id->idVendor, id->idProduct);
	return 0;
    
}

// Output for usb disconnect interruption
static void dev_disconnect(struct usb_interface *interface)
{
    printk(KERN_INFO "Dev drive removed\n");
}

// Hardcoded device table to check which device is plugged (on which device we should interrupt)
static struct usb_device_id dev_table[] =
{
    { USB_DEVICE(0x04e8, 0x6860) }, // Hardcoded usb device vendor and products id
    {}
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
