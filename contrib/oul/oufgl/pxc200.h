/*
 * pxc200.h -- definitions for the pxc200 frame grabber
 *
 * Copyright (C) 1997   rubini@linux.it (Alessandro Rubini)
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _LINUX_PXC200_H_
#define _LINUX_PXC200_H_

#include <linux/types.h>
#include <linux/ioctl.h>
#include <asm/page.h>
/*
 * Macros to help debugging
 */

#define PX_MSG "pxc200: "
#undef PDEBUG             /* undef it, just in case */
#undef DEBUG_CODE
#ifdef PX_DEBUG
#  ifdef __KERNEL__
     /* This one if debugging is on, and kernel space */
#    define PDEBUG(fmt, args...) printk( KERN_DEBUG PX_MSG fmt, ## args)
#    define DEBUG_CODE(code) code
#  else
     /* This one for user space */
#    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...) /* not debugging: nothing */
#  define DEBUG_CODE(code)
#endif

#undef PDEBUGG
#define PDEBUGG(fmt, args...) /* nothing: it's a placeholder */

#include "bt848.h"
#include "bt848-addons.h"   /* I prefer to add some symbols... */

struct px_programpage {
    unsigned long page;
    struct px_programpage *next;
};

/* This structure is not used in the driver, currently, but it will */
typedef struct Px_Roi {
    int wid;
    int hei;
    int x0;
    int y0;
    struct Px_Roi *next;
} Px_Roi;

/*
 * This one is used to control continuous acquisition,
 * it is passed to IOCSEQUENCE
 */
typedef struct Px_AcqControl {
    __u32 flags; /* Future use... */
    __u32 count; /* How many images to grab */
    __u32 step;  /* Get one every that many frames */
    __u32 buflen; /* Use a vmalloc buffer, that big */
} Px_AcqControl;

typedef struct Px_Dev {
    unsigned long dmabuffer;   /* physical address */
    unsigned long dmaremap;    /* ioremapped region, to be accessed by soft */
    unsigned long hwoverrun;   /* number of "faulty target" overruns */
    unsigned long flags;
#define PX_FLAG_PERSIST  0x00000001 /* keep status on close/open */
#define PX_FLAG_CONTACQ  0x00000002 /* continuous acquisition */
#define PX_USER_FLAGS    0x000000FF /* user can change */

#define PX_FLAG_HIRES    0x00000100 /* low res is default */
#define PX_FLAG_COLOR    0x00000200 /* B/W is default */
#define PX_INIT_FLAGS    0x00000F00 /* these can't be changed after open */

#define PX_FLAG_SEQUENCE 0x00001000 /* one sequence is active */

#define PX_FLAG_BTONLY   0x00002000 /* not a pxc200 */
#define PX_FLAG_PXC200F  0x00004000 /* pxc200-F (not pxc200-L) */
#define PX_FLAG_USEPLL   0x00008000 /* need the internal PLL for PAL clk */
#define PX_HWTYPE_FLAGS  0x0000E000

#define PX_FLAG_DODMA    0x00010000 /* positively ask for DMA at open time  */
#define PX_FLAG_RUNNING  0x00020000 /* status flag */

#define PX_FLAG_TRIGMODE 0x00200000 /* use trigger-based acquisition */
#define PX_FLAG_TRIGEDGE 0x00400000 /* edge-triggered */
#define PX_FLAG_TRIGLEVL 0x00000000 /* level-triggered */
#define PX_FLAG_TRIGPOS  0x00800000 /* positive */
#define PX_FLAG_TRIGNEG  0x00000000 /* negative */
#define PX_TRIG_FLAGS    0x00F00000


    struct Px_Dev *next;
    volatile __u8 *regs;           /* where registers are accessed */
    struct wait_queue *queue;
    unsigned long irq_count;

    unsigned long dmaprogramE;     /* address of even-field RISC program */
    unsigned long dmaprogramO;     /* address of odd-field RISC program */
    int usage;                     /* number of opens */
    short isup;                    /* whether dma is active */
    short device_id;               /* 848, 849, 878 */

    unsigned int dmasize;          /* allocated size */
    unsigned int imagesize;        /* used size */
    unsigned int xsize;            /* the pgm node needs such info */
    unsigned int ysize;

    unsigned long physdmaprog;     /* physical address of program */
    __u8 prglen;             /* length of program list (if any) */
    __u8 prgcurr;            /* current program page (index) */
    __u8 cfgword;            /* status: config word */
    __u8 refv;               /* status: reference voltage */

    __u16 roix, roiy, roiwid, roihei;    /* Region of interest */
    struct px_programpage *program;      /* Multiple program pages are */
                                         /* used for seq. acquisition  */

    __u16 hactive;           /* these eight are 848 info */
    __u16 vactive;
    __u16 hdelay;
    __u16 vdelay;

    __u16 hscale;
    __u16 vscale;
    __u16 bright;
    __u8 adelay, bdelay;

    __u32 phys_add;          /* where do registers live */
    struct pci_dev *pcidev;  /* pci information */

    __u8 irq;                /* irq number from pci config */
    __u8 irq_active;         /* irq currently used. 0 if unused */
    __u8 mux;
    __u8 vtype;
#define PX_CVID 0 /* Composite video */
#define PX_SVID 1 /* S video */

#define PX_CFG_PXC200F 0x01
#define PX_CFG_ADPOT   0x02

    char pnmheader[32];      /* the pgm/ppm header is 17 bytes */
    #define PX_HEADERLEN 17  /* remember it.. */
    #define PX_HEADERPGM "P5 %4i %4i 255\n"
    #define PX_HEADERPPM "P6 %4i %4i 255\n"
    #define PX_HEADERFMT(dev) \
        ((dev)->flags & PX_FLAG_COLOR ? PX_HEADERPPM : PX_HEADERPGM)

/* The following section is used with acquisition of sequences */

    __u8               *seq_buffer;  /* vmalloc buffer */
    struct file        *seq_filp;    /* the owner of this sequence */
    unsigned long       seq_bufsize; /* len, in bytes */
    struct wait_queue  *seq_queue;

    unsigned long       seq_head;    /* write position */
    unsigned long       seq_tail;    /* read position */

    volatile __u32      seq_pending;  /* from user space, decr to 0 */
    __u32               seq_step2;    /* get one every that many IRQ */
    __u32               seq_next;     /* next acquisition time */
    __u32               seq_overrun;  /* buffer overflow */

} Px_Dev;

#define PX_PIXEL_SIZE(dev) ((dev)->flags & PX_FLAG_COLOR ? 3 : 1)

/* These are the GPIO bits for the AD pot */
#define PX_GPIO_CSL 0x40 /* GPIO 6 */
#define PX_GPIO_CLK 0x20 /* GPIO 5 */
#define PX_GPIO_SDI 0x10 /* GPIO 4 */


#define PX_IOC_MAGIC  'p' /* Use 'p' as magic number */

typedef char __px_page[PAGE_SIZE];  /* horrible */

#define PX_IOCRESET         _IO(PX_IOC_MAGIC,  0)
#define PX_IOCHARDRESET     _IO(PX_IOC_MAGIC,  1) /* debugging tool */
#define PX_IOCGFLAGS       _IOR(PX_IOC_MAGIC,  2, unsigned long)
#define PX_IOCSFLAGS       _IOW(PX_IOC_MAGIC,  3, unsigned long)
#define PX_IOCGDMASIZE     _IOR(PX_IOC_MAGIC,  4, unsigned long)
#define PX_IOCGDMABUF      _IOR(PX_IOC_MAGIC,  5, unsigned long)
#define PX_IOCGRISCADDE    _IOR(PX_IOC_MAGIC,  6, unsigned long)
#define PX_IOCGRISCADDO    _IOR(PX_IOC_MAGIC,  7, unsigned long)
#define PX_IOCGPROGRAME    _IOR(PX_IOC_MAGIC,  8, __px_page)
#define PX_IOCGPROGRAMO    _IOR(PX_IOC_MAGIC,  9, __px_page)
#define PX_IOCGIRQCOUNT    _IOR(PX_IOC_MAGIC, 10, unsigned long)
#define PX_IOCGREFV        _IOR(PX_IOC_MAGIC, 11, unsigned long)
#define PX_IOCSREFV        _IOW(PX_IOC_MAGIC, 12, unsigned long)
#define PX_IOCSMUX         _IOW(PX_IOC_MAGIC, 13, unsigned long)
#define PX_IOCGMUX         _IOR(PX_IOC_MAGIC, 14, unsigned long)
#define PX_IOCSTRIG        _IOW(PX_IOC_MAGIC, 15, unsigned long)
#define PX_IOCSACQLEN      _IOW(PX_IOC_MAGIC, 16, unsigned long)
#define PX_IOCGACQLEN      _IOR(PX_IOC_MAGIC, 17, unsigned long)
#define PX_IOCACQNOW        _IO(PX_IOC_MAGIC, 18)
#define PX_IOCWAITVB      _IOWR(PX_IOC_MAGIC, 19, unsigned long)
#define PX_IOCSEQUENCE     _IOW(PX_IOC_MAGIC, 20, Px_AcqControl)
#define PX_IOCGHWOVERRUN   _IOR(PX_IOC_MAGIC, 21, unsigned long)
#define PX_IOCGSWOVERRUN   _IOR(PX_IOC_MAGIC, 22, unsigned long)
#define PX_IOCGWHOLEDEVICE _IOR(PX_IOC_MAGIC, 23, Px_Dev)
#define PX_IOCGBRIGHT      _IOR(PX_IOC_MAGIC, 24, signed long)
#define PX_IOCSBRIGHT      _IOW(PX_IOC_MAGIC, 25, signed long)
#define PX_IOCGCONTRAST    _IOR(PX_IOC_MAGIC, 26, unsigned long)
#define PX_IOCSCONTRAST    _IOW(PX_IOC_MAGIC, 27, unsigned long)
#define PX_IOCGHUE         _IOR(PX_IOC_MAGIC, 28, signed long)
#define PX_IOCSHUE         _IOW(PX_IOC_MAGIC, 29, signed long)
#define PX_IOCGSATU        _IOR(PX_IOC_MAGIC, 30, unsigned long)
#define PX_IOCSSATU        _IOW(PX_IOC_MAGIC, 31, unsigned long)
#define PX_IOCGSATV        _IOR(PX_IOC_MAGIC, 32, unsigned long)
#define PX_IOCSSATV        _IOW(PX_IOC_MAGIC, 33, unsigned long)
#define PX_IOCGVTYPE       _IOR(PX_IOC_MAGIC, 34, unsigned long)
#define PX_IOCSVTYPE       _IOW(PX_IOC_MAGIC, 35, unsigned long)

#define PX_IOC_MAXNR                          35 /* max implemented */


#ifdef __KERNEL__ /* =================================================== */


#include <linux/delay.h>

/* version dependencies have been confined to a separate file */

#include "sysdep.h"

/*
 * The configurable parameters
 */

#define    PX_MAJOR 60   /* not dynamic major by default -- FIXME -- */
extern int major;

/*
 * Use macros to access regs
 */

#define PX_READ32(d, r) (readl((volatile u8 *)((d)->regs+(r))))
#define PX_READ16(d, r) (readw((d)->regs+(r)))
#define PX_READ8(d, r)  (readb((d)->regs+(r)))

#define PX_WRITE32(d, v, r) (writel(v,(volatile u8 *)((d)->regs+(r))))
#define PX_WRITE16(d, v, r) (writew(v,(d)->regs+(r)))
#define PX_WRITE8(d, v, r)  (writeb(v,(d)->regs+(r)))

/*
 * Split minors in two parts
 */

#define TYPE(dev)   (MINOR(dev) >> 4)  /* high nibble */
#define NUM(dev)    (MINOR(dev) & 0xf) /* low  nibble */

enum px_types {
    PX_BIN = 0,   /* binary grabber */
    PX_PGM,       /* pgm grabber */
    PX_CTL,       /* no grab: only control operations */
    PX_HI_BIN,    /* hi-res, binary */
    PX_HI_PGM,    /* hi-res, pgm */
    PX_PPM,       /* ppm (header + rgb) */
    PX_RGB,       /* rgb (converted on read()) */
    PX_BGR,       /* bgr (straight from DMA) */
    PX_HI_PPM,    /* hi-res ppm (header + rgb) */
    PX_HI_RGB,    /* hi-res rgb (converted on read()) */
    PX_HI_BGR     /* hi-res bgr (straight from DMA) */
};

#define PX_MAX_TYPE 10

extern struct file_operations px_bin_fops;
extern struct file_operations px_pnm_fops;
extern struct file_operations px_ctl_fops;

extern struct file_operations px_fops[];   /* one fops for type */

extern Px_Dev *px_devices;

/*
 * And now some hw-level defaults
 */

#define PX_UPTIME_MIN  4 /* dev->isup is 1 at init, 2 when grab begins */

/* I2C definitions */

#define PX_I2C_DIVISOR (7 << BT848_I2C_DIV_S)
/* #define PX_I2C_DIVISOR (5 << BT848_I2C_DIV_S) */

#define PX_I2C_DELAY      700 /* microseconds */

#define PX_I2C_READ         1
#define PX_I2C_WRITE        0

#define PX_I2C_PIC       0x0f
#define PX_I2C_DAC       0x2f
#define PX_DEFAULT_REF    128 /* default reference video voltage */

#define PX_I2C_CMD_CFG   0x00
#define PX_I2C_CMD_REV   0x01
#define PX_I2C_CMD_ID    0x02
#define PX_I2C_CMD_HNR   0x03 /* high-byte of serial number */
#define PX_I2C_CMD_LNR   0x04 /* low byte of serial number */
#define PX_I2C_CMD_PIC   0x05
#define PX_I2C_CMD_STAT  0x06
#define PX_I2C_CMD_ECHO  0x07
#define PX_I2C_CMD_E00   0x08
#define PX_I2C_CMD_Eff   0x09
#define PX_I2C_CMD_E55   0x0a
#define PX_I2C_CMD_Eaa   0x0b
#define PX_I2C_CMD_E0f   0x0c
#define PX_I2C_CMD_Ef0   0x0d

/* Another default */
#define PX_DEFAULT_IRQ_MASK 0x7b800 /* discard gpio interrupts */

#ifdef PX_DEBUG
#  define PX_USE_PROC
#endif

#ifndef min
#  define min(a,b) ((a)<(b) ? (a) : (b))
#endif

#else /* not __KERNEL__ ================================================== */

/*
 * These macros are wrappers to ioctl()
 * They use gcc extensions to the C language, but every Linux runs gcc
 */

/* Reset. Hardreset not wrapped, as it is only a debugging tool */

#define Px_Reset(fd) (ioctl((fd), PX_IOCRESET))


#endif /* __KERNEL__ */

#endif /* _LINUX_PXC200_H_ */
