
#include "usbhid.h"

// Variables
usbd_device *usbd_dev;

const struct usb_device_descriptor dev_descr = {
    .bLength = USB_DT_DEVICE_SIZE,
    .bDescriptorType = USB_DT_DEVICE,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0,
    .bDeviceSubClass = 0,
    .bDeviceProtocol = 0,
    .bMaxPacketSize0 = 64,
    .idVendor = 0x0483,
    .idProduct = 0x5710,
    .bcdDevice = 0x0200,
    .iManufacturer = 1,
    .iProduct = 2,
    .iSerialNumber = 3,
    .bNumConfigurations = 1,
};

static const uint8_t hid_report_descriptor[] = {
    0x05, 0x01,        // USAGE_PAGE (Generic Desktop)
    0x09, 0x04,        // USAGE (Joystick)
    0xa1, 0x01,        // COLLECTION (Application)
    0x05, 0x01,        //   USAGE_PAGE (Generic Desktop)
    0x09, 0x30,        //   USAGE (X)
    0x09, 0x31,        //   USAGE (Y)
    0x09, 0x32,        //   USAGE (Z)
    0x09, 0x33,        //   USAGE (Rx)
    0x09, 0x34,        //   USAGE (Ry)
    0x09, 0x35,        //   USAGE (Rz)
    0x16, 0x01, 0x80,  //   LOGICAL_MINIMUM (-32767)
    0x26, 0xff, 0x7f,  //   LOGICAL_MAXIMUM (32767)
    0x75, 0x10,        //   REPORT_SIZE (16)
    0x95, 0x06,        //   REPORT_COUNT (6)
    0x81, 0x02,        //   INPUT (Data,Var,Abs)
    0x05, 0x09,        //   USAGE_PAGE (Button)
    0x09, 0x01,        //   USAGE (Button 1)
    0x09, 0x02,        //   USAGE (Button 2)
    0x09, 0x03,        //   USAGE (Button 3)
    0x09, 0x04,        //   USAGE (Button 4)
    0x09, 0x05,        //   USAGE (Button 5)
    0x09, 0x06,        //   USAGE (Button 6)
    0x09, 0x07,        //   USAGE (Button 7)
    0x09, 0x08,        //   USAGE (Button 8)
    0x09, 0x09,        //   USAGE (Button 9)
    0x09, 0x0a,        //   USAGE (Button 10)
    0x09, 0x0b,        //   USAGE (Button 11)
    0x09, 0x0c,        //   USAGE (Button 12)
    0x09, 0x0d,        //   USAGE (Button 13)
    0x09, 0x0e,        //   USAGE (Button 14)
    0x09, 0x0f,        //   USAGE (Button 15)
    0x09, 0x10,        //   USAGE (Button 16)
    0x09, 0x11,        //   USAGE (Button 17)
    0x09, 0x12,        //   USAGE (Button 18)
    0x09, 0x13,        //   USAGE (Button 19)
    0x09, 0x14,        //   USAGE (Button 20)
    0x09, 0x15,        //   USAGE (Button 21)
    0x09, 0x16,        //   USAGE (Button 22)
    0x09, 0x17,        //   USAGE (Button 23)
    0x09, 0x18,        //   USAGE (Button 24)
    0x09, 0x19,        //   USAGE (Button 25)
    0x09, 0x1a,        //   USAGE (Button 26)
    0x09, 0x1b,        //   USAGE (Button 27)
    0x09, 0x1c,        //   USAGE (Button 28)
    0x09, 0x1d,        //   USAGE (Button 29)
    0x09, 0x1e,        //   USAGE (Button 30)
    0x09, 0x1f,        //   USAGE (Button 31)
    0x09, 0x20,        //   USAGE (Button 32)
    0x09, 0x21,        //   USAGE (Button 33)
    0x09, 0x22,        //   USAGE (Button 34)
    0x09, 0x23,        //   USAGE (Button 35)
    0x15, 0x00,        //   LOGICAL_MINIMUM (0)
    0x25, 0x01,        //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,        //   REPORT_SIZE (1)
    0x95, 0x23,        //   REPORT_COUNT (35)
    0x81, 0x02,        //   INPUT (Data,Var,Abs)
    0x75, 0x01,        //   REPORT_SIZE (1)
    0x95, 0x05,        //   REPORT_COUNT (5)
    0x81, 0x01,        // INPUT (Cnst,Ary,Abs)
    0xc0               // END_COLLECTION
};

/*static const uint8_t hid_report_descriptor[] =
  {
    0x05, 0x01,                     // USAGE_PAGE (Generic Desktop)
    0x09, 0x04,                     // USAGE (Joystick)
    0xa1, 0x01,                     // COLLECTION (Application)

        0x85, 0x01,				 	 //		REPORT_ID	(JOY_REPORT_ID)

        // axis data
        0x05, 0x01,                     // 	USAGE_PAGE (Generic Desktop)
        0x09, 0x30,                     //   USAGE (X)
    0x09, 0x31,                     //   USAGE (Y)
    0x09, 0x32,                     //   USAGE (Z)
    0x09, 0x33,                     //   USAGE (Rx)
    0x09, 0x34,                     //   USAGE (Ry)
    0x09, 0x35,                     //   USAGE (Rz)
    0x16, 0x01, 0x80,               //  	LOGICAL_MINIMUM (-32767)
    0x26, 0xFF, 0x7F,			    //   LOGICAL_MAXIMUM (32767)
    0x75, 0x10,                     //   REPORT_SIZE (16)
    0x95, 0x06,       		        //   REPORT_COUNT (6)
    0x81, 0x02,                     //   INPUT (Data,Var,Abs)

        // buttons data
        0x05, 0x09,                     //   USAGE_PAGE (Button)
    0x19, 0x01,                     //   USAGE_MINIMUM (Button 1)
    0x29, 0x20,                     //   USAGE_MAXIMUM (Button 32)
    0x15, 0x00,                     //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                     //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                     //   REPORT_SIZE (1)
    0x95, 0x23,                     //   REPORT_COUNT (35)
        0x81, 0x02, 					//   INPUT (Data,Var,Abs)
        //0x75, 0x0D, 					//	 REPORT_SIZE (13)
        //0x95, 0x01, 					//	 REPORT_COUNT (1)
        //0x81, 0x02, 					//   INPUT (Data,Var,Abs)
    //0x81, 0x01, 					//   INPUT (Cnst,Ary,Abs)



        0xc0,                           // END_COLLECTION
  };*/

/*static const uint8_t hid_report_descriptor[] = {
        0x05, 0x01, 	//		USAGE_PAGE (Generic Desktop)
        0x09, 0x02, 	//		USAGE (Mouse)
        0xa1, 0x01, 	//		COLLECTION (Application)
        0x09, 0x01, 	//		USAGE (Pointer)
        0xa1, 0x00, 	//		COLLECTION (Physical)
        0x05, 0x09, 	//		USAGE_PAGE (Button)
        0x19, 0x01, 	//		USAGE_MINIMUM (Button 1)
        0x29, 0x03, 	//		USAGE_MAXIMUM (Button 3)
        0x15, 0x00, 	//     LOGICAL_MINIMUM (0)
        0x25, 0x01, 	//     LOGICAL_MAXIMUM (1)
        0x95, 0x03, 	//     REPORT_COUNT (3)
        0x75, 0x01, 	//     REPORT_SIZE (1)
        0x95, 0x01, 	//     REPORT_COUNT (1)
        0x75, 0x05, 	//     REPORT_SIZE (5)
        0x81, 0x01, 	//     INPUT (Cnst,Ary,Abs)
        0x05, 0x01, 	//     USAGE_PAGE (Generic Desktop)
        0x09, 0x30, 	//     USAGE (X)
        0x09, 0x31, 	//     USAGE (Y)
        0x09, 0x38, 	//     USAGE (Wheel)
        0x15, 0x81, 	//     LOGICAL_MINIMUM (-127)
        0x25, 0x7f, 	//     LOGICAL_MAXIMUM (127)
        0x75, 0x08, 	//     REPORT_SIZE (8)
        0x95, 0x03, 	//     REPORT_COUNT (3)
        0x81, 0x06, 	//     INPUT (Data,Var,Rel)
        0xc0,       	//   END_COLLECTION
        0x09, 0x3c, 	//   USAGE (Motion Wakeup)
        0x05, 0xff, 	//   USAGE_PAGE (Vendor Defined Page 1)
        0x09, 0x01, 	//   USAGE (Vendor Usage 1)
        0x15, 0x00, 	//   LOGICAL_MINIMUM (0)
        0x25, 0x01, 	//   LOGICAL_MAXIMUM (1)
        0x75, 0x01, 	//   REPORT_SIZE (1)
        0x95, 0x02, 	//   REPORT_COUNT (2)
        0xb1, 0x22, 	//FEATURE (Data,Var,Abs,NPrf)
        0x75, 0x06, 	//REPORT_SIZE (6)
        0x95, 0x01, 	//REPORT_COUNT (1)
        0xb1, 0x01, 	//FEATURE (Cnst,Ary,Abs)
        0xc0         	//END_COLLECTION
};*/

static const struct {
    struct usb_hid_descriptor hid_descriptor;
    struct {
        uint8_t bReportDescriptorType;
        uint16_t wDescriptorLength;
    } __attribute__((packed)) hid_report;
} __attribute__((packed)) hid_function = {.hid_descriptor =
                                              {
                                                  .bLength = sizeof(hid_function),
                                                  .bDescriptorType = USB_DT_HID,
                                                  .bcdHID = 0x0100,
                                                  .bCountryCode = 0,
                                                  .bNumDescriptors = 1,
                                              },
                                          .hid_report = {
                                              .bReportDescriptorType = USB_DT_REPORT,
                                              .wDescriptorLength = sizeof(hid_report_descriptor),
                                          }};

const struct usb_endpoint_descriptor hid_endpoint = {
    .bLength = USB_DT_ENDPOINT_SIZE,
    .bDescriptorType = USB_DT_ENDPOINT,
    .bEndpointAddress = 0x81,
    .bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,
    .wMaxPacketSize = PACKET_SIZE,
    .bInterval = 0x01,
};

const struct usb_interface_descriptor hid_iface = {
    .bLength = USB_DT_INTERFACE_SIZE,
    .bDescriptorType = USB_DT_INTERFACE,
    .bInterfaceNumber = 0,
    .bAlternateSetting = 0,
    .bNumEndpoints = 1,
    .bInterfaceClass = USB_CLASS_HID,
    .bInterfaceSubClass = 1, /* boot */
    .bInterfaceProtocol = 2, /* mouse */
    .iInterface = 0,

    .endpoint = &hid_endpoint,

    .extra = &hid_function,
    .extralen = sizeof(hid_function),
};

const struct usb_interface ifaces[] = {{
    .num_altsetting = 1,
    .altsetting = &hid_iface,
}};

const struct usb_config_descriptor config = {
    .bLength = USB_DT_CONFIGURATION_SIZE,
    .bDescriptorType = USB_DT_CONFIGURATION,
    .wTotalLength = 0,
    .bNumInterfaces = 1,
    .bConfigurationValue = 1,
    .iConfiguration = 0,
    .bmAttributes = 0xC0,
    .bMaxPower = 0x32,

    .interface = ifaces,
};

static const char *usb_strings[] = {
    "LibreMiG-S",
    "Flight Stick",
    "v0.2",
};

/* Buffer to be used for control requests. */
uint8_t usbd_control_buffer[128];

enum usbd_request_return_codes hid_control_request(usbd_device *dev, struct usb_setup_data *req, uint8_t **buf,
                                                   uint16_t *len,
                                                   void (**complete)(usbd_device *, struct usb_setup_data *)) {
    (void)complete;
    (void)dev;

    if ((req->bmRequestType != 0x81) || (req->bRequest != USB_REQ_GET_DESCRIPTOR) || (req->wValue != 0x2200))
        return USBD_REQ_NOTSUPP;

    /* Handle the HID report descriptor. */
    *buf = (uint8_t *)hid_report_descriptor;
    *len = sizeof(hid_report_descriptor);

    return USBD_REQ_HANDLED;
}

void hid_set_config(usbd_device *dev, uint16_t wValue) {
    (void)wValue;
    (void)dev;

    usbd_ep_setup(dev, 0x81, USB_ENDPOINT_ATTR_INTERRUPT, PACKET_SIZE, NULL);

    usbd_register_control_callback(dev, USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE,
                                   USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT, hid_control_request);
#ifdef INCLUDE_DFU_INTERFACE
    usbd_register_control_callback(dev, USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
                                   USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT, dfu_control_request);
#endif
    
    // Systick Setup
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
    STK_CVR = 0;
    systick_set_reload(rcc_ahb_frequency / 1000);
    systick_clear();
    systick_interrupt_enable();
    systick_counter_enable();
}

void usb_setup(void) {
    /*
     * This is a somewhat common cheap hack to trigger device re-enumeration
     * on startup.  Assuming a fixed external pullup on D+, (For USB-FS)
     * setting the pin to output, and driving it explicitly low effectively
     * "removes" the pullup.  The subsequent USB init will "take over" the
     * pin, and it will appear as a proper pullup to the host.
     * The magic delay is somewhat arbitrary, no guarantees on USBIF
     * compliance here, but "it works" in most places.
     */
    gpio_mode_setup(GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);
    gpio_clear(GPIOA, GPIO12);
    for (unsigned i = 0; i < 800000; i++) {
        __asm__("nop");
    }

    usbd_dev = usbd_init(&st_usbfs_v2_usb_driver, &dev_descr, &config, usb_strings, 3, usbd_control_buffer,
                         sizeof(usbd_control_buffer));
    usbd_register_set_config_callback(usbd_dev, hid_set_config);
}
