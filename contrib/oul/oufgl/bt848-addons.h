/*
 * This file defines additional symbols, that I prefer to use in order
 * to simplify dumping data to the user. It's a different file from bt848.h
 * 'cause I'd better not modify Ralph's header (it's very good)
 */

#ifndef _BT848_ADDONS_H_
#define _BT848_ADDONS_H_


#define BT848_NR_REGISTERS 0x200

/* To access multi-bit values, I prefer to use shift values */

#define BT848_DSTATUS_FIELD_S 5
#define BT848_DSTATUS_CSEL_S  3

#define BT848_IFORM_S         0
#define BT848_IFORM_XTSEL_S   3
#define BT848_IFORM_MUXSEL_S  5

/* Odd registers are always offset from the even ones */

#define BT848_ODD_OFFSET  0x80

/* The CROP register is split in 4 fields */

#define BT848_CROP_HACTIVE     0x03
#define BT848_CROP_HACTIVE_S       0
#define BT848_CROP_HDELAY      0x0C
#define BT848_CROP_HDELAY_S        2
#define BT848_CROP_VACTIVE     0x30
#define BT848_CROP_VACTIVE_S       4
#define BT848_CROP_VDELAY      0xC0
#define BT848_CROP_VDELAY_S        6

/* the shift value for risc status */

#define BT848_INT_RISCS_S      28

/* same for set/reset */
#define BT848_RISC_SET_STATUS_S   16
#define BT848_RISC_RESET_STATUS_S 20

#define BT848_RISC_DO_STATUS(set,clr) \
        ((((set) & 0xf) << BT848_RISC_SET_STATUS_S) | \
         (((clr) & 0xf) << BT848_RISC_RESET_STATUS_S))


#define BT848_I2C_DIV_S        4


#endif /* _BT848_ADDONS_H_ */
