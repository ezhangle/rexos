/*
    RExOS - embedded RTOS
    Copyright (c) 2011-2014, Alexey Kramarenko
    All rights reserved.
*/

#include "lib_usb.h"
#include "../userspace/direct.h"
#include "../userspace/object.h"
#include "../userspace/usb.h"
#include "sys_config.h"

#define D_OFFSET(d, cfg)                            (((unsigned int)(d)) - ((unsigned int)(cfg)))
#define D_NEXT(d, cfg)                              ((USB_DESCRIPTOR_TYPE*)(((unsigned int)(d)) + (d)->bLength))

bool lib_usb_register_persistent_descriptor(USBD_DESCRIPTOR_TYPE type, unsigned int index, unsigned int lang, const void* d)
{
    HANDLE usbd = object_get(SYS_OBJ_USBD);
    char buf[USBD_DESCRIPTOR_REGISTER_STRUCT_SIZE_ALIGNED + sizeof(const void**)];
    USBD_DESCRIPTOR_REGISTER_STRUCT* udrs = (USBD_DESCRIPTOR_REGISTER_STRUCT*)buf;

    udrs->flags = USBD_FLAG_PERSISTENT_DESCRIPTOR;
    udrs->index = index;
    udrs->lang = lang;
    *(const void**)(buf + USBD_DESCRIPTOR_REGISTER_STRUCT_SIZE_ALIGNED) = d;
    direct_enable_read(usbd, buf, USBD_DESCRIPTOR_REGISTER_STRUCT_SIZE_ALIGNED + sizeof(const void**));
    ack(usbd, USBD_REGISTER_DESCRIPTOR, type, USBD_DESCRIPTOR_REGISTER_STRUCT_SIZE_ALIGNED + sizeof(const void**), 0);
    return get_last_error() == ERROR_OK;
}

USB_INTERFACE_DESCRIPTOR_TYPE* lib_usb_get_first_interface(const USB_CONFIGURATION_DESCRIPTOR_TYPE* cfg)
{
    USB_DESCRIPTOR_TYPE* d;
    for (d = (USB_DESCRIPTOR_TYPE*)cfg; D_OFFSET(d, cfg) < cfg->wTotalLength; d = D_NEXT(d, cfg))
        if (d->bDescriptorType == USB_INTERFACE_DESCRIPTOR_INDEX)
            return (USB_INTERFACE_DESCRIPTOR_TYPE*)d;
    return NULL;
}

USB_INTERFACE_DESCRIPTOR_TYPE* lib_usb_get_next_interface(const USB_CONFIGURATION_DESCRIPTOR_TYPE* cfg, const USB_INTERFACE_DESCRIPTOR_TYPE* start)
{
    USB_DESCRIPTOR_TYPE* d;
    for (d = D_NEXT(start, cfg); D_OFFSET(d, cfg) < cfg->wTotalLength; d = D_NEXT(d, cfg))
        if (d->bDescriptorType == USB_INTERFACE_DESCRIPTOR_INDEX)
            return (USB_INTERFACE_DESCRIPTOR_TYPE*)d;
    return NULL;
}

USB_DESCRIPTOR_TYPE* lib_usb_interface_get_first_descriptor(const USB_CONFIGURATION_DESCRIPTOR_TYPE* cfg, const USB_INTERFACE_DESCRIPTOR_TYPE* start, unsigned int type)
{
    USB_DESCRIPTOR_TYPE* d;
    for (d = D_NEXT(start, cfg); D_OFFSET(d, cfg) < cfg->wTotalLength; d = D_NEXT(d, cfg))
        if (d->bDescriptorType == type || type == 0)
            return d;
    return NULL;
}

USB_DESCRIPTOR_TYPE* lib_usb_interface_get_next_descriptor(const USB_CONFIGURATION_DESCRIPTOR_TYPE* cfg, const USB_DESCRIPTOR_TYPE* start, unsigned int type)
{
    USB_DESCRIPTOR_TYPE* d;
    for (d = D_NEXT(start, cfg); D_OFFSET(d, cfg) < cfg->wTotalLength; d = D_NEXT(d, cfg))
    {
        if (d->bDescriptorType == USB_INTERFACE_DESCRIPTOR_INDEX)
            return NULL;
        else if (d->bDescriptorType == type || type == 0)
            return d;
    }
    return NULL;
}

const LIB_USB __LIB_USB = {
    lib_usb_register_persistent_descriptor,
    lib_usb_get_first_interface,
    lib_usb_get_next_interface,
    lib_usb_interface_get_first_descriptor,
    lib_usb_interface_get_next_descriptor
};
