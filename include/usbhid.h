#ifndef __USBHID_H__
#define __USBHID_H__


#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/hid.h>
#include <stdlib.h>

// Defines
#define PACKET_SIZE 17

// Variables
extern usbd_device *usbd_dev;

// Function  prototypes
 enum usbd_request_return_codes
hid_control_request (usbd_device *dev, struct usb_setup_data *req,
                     uint8_t **buf, uint16_t *len,
                     void (**complete) (usbd_device *dev,
                                        struct usb_setup_data *req));
void hid_set_config (usbd_device *dev, uint16_t wValue);
void usb_setup(void);


#endif