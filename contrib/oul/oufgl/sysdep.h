/*
 * sysdep.h -- a trial to centralize changes between 2.0 and 2.2
 */


#ifndef _SYSDEP_H_
#define _SYSDEP_H_

#ifndef LINUX_VERSION_CODE
#  include <linux/version.h>
#endif

#ifndef VERSION_CODE
#  define VERSION_CODE(vers,rel,seq) ( ((vers)<<16) | ((rel)<<8) | (seq) )
#endif

/* only allow 2.0.x and 2.2.y */

#if LINUX_VERSION_CODE < VERSION_CODE(2,0,0) /* not < 2.0 */
#  error "This kernel is too old: not supported by this file"
#endif
#if LINUX_VERSION_CODE > VERSION_CODE(2,3,0) /* not > 2.2, by now */
#  error "This kernel is too recent: not supported by this file"
#endif
#if (LINUX_VERSION_CODE & 0xff00) == 1 /* not 2.1 */
#  error "Please don't use linux-2.1, use 2.2 instead"
#endif

/* remember about the current version */
#if LINUX_VERSION_CODE < VERSION_CODE(2,1,0)
#  define LINUX_20
#else
#  define LINUX_22
#endif

#include <linux/types.h> /* used later in this header */

/* Modularization issues */
#ifdef LINUX_20
#  define __USE_OLD_SYMTAB__
#  define EXPORT_NO_SYMBOLS register_symtab(NULL);
#  define REGISTER_SYMTAB(tab) register_symtab(tab)
#else
#  define REGISTER_SYMTAB(tab) /* nothing */
#endif

#ifdef __USE_OLD_SYMTAB__
#  define __MODULE_STRING(s)         /* nothing */
#  define MODULE_PARM(v,t)           /* nothing */
#  define MODULE_PARM_DESC(v,t)      /* nothing */
#  define MODULE_AUTHOR(n)           /* nothing */
#  define MODULE_DESCRIPTION(d)      /* nothing */
#  define MODULE_SUPPORTED_DEVICE(n) /* nothing */
#endif


/*
 * "select" changed in 2.1.23. The implementation is twin, but this
 * header is new
 */
#ifdef LINUX_22
#  include <linux/poll.h>
#  define INODE_FROM_F(filp) ((filp)->f_dentry->d_inode)
#else
#  define __USE_OLD_SELECT__
#  define INODE_FROM_F(filp) ((filp)->f_inode)
#endif

/* Other change in the fops are solved using wrappers */
#ifdef LINUX_20
#endif


/*
 * access to user space: use the 2.1 functions,
 * and implement them as macros for 2.0
 */

#ifdef LINUX_20
#  include <asm/segment.h>
#  define access_ok(t,a,sz)           (verify_area((t),(a),(sz)) ? 0 : 1)
#  define verify_area_20              verify_area
#  define copy_to_user(t,f,n)         (memcpy_tofs(t,f,n), 0)
#  define __copy_to_user(t,f,n)       copy_to_user((t),(f),(n))
#  define copy_to_user_ret(t,f,n,r)   copy_to_user((t),(f),(n))
#  define copy_from_user(t,f,n)       (memcpy_fromfs((t),(f),(n)), 0)
#  define __copy_from_user(t,f,n)     copy_from_user((t),(f),(n))
#  define copy_from_user_ret(t,f,n,r) copy_from_user((t),(f),(n))
#  define PUT_USER(val,add)           (put_user((val),(add)), 0)
#  define __PUT_USER(val,add)         PUT_USER((val),(add))
#  define PUT_USER_RET(val,add,ret)   PUT_USER((val),(add))
#  define GET_USER(dest,add)          ((dest)=get_user((add)), 0)
#  define __GET_USER(dest,add)        GET_USER((dest),(add))
#  define GET_USER_RET(dest,add,ret)  GET_USER((dest),(add))
#else
#  include <asm/uaccess.h>
#  include <asm/io.h>
#  define verify_area_20(t,a,sz) (0) /* == success */
#  define PUT_USER put_user
#  define __PUT_USER __put_user
#  define PUT_USER_RET put_user_ret
#  define GET_USER get_user
#  define __GET_USER __get_user
#  define GET_USER_RET get_user_ret
#endif

/* ioremap */
#ifdef LINUX_20
# define ioremap vremap
# define iounmap vfree
#endif

/* The use_count of exec_domain and binfmt changed in 2.1.23 */

#ifdef LINUX_20
#  define INCRCOUNT(p)  ((p)->module ? __MOD_INC_USE_COUNT((p)->module) : 0)
#  define CURRCOUNT(p)  ((p)->module && (p)->module->usecount)
#  define DECRCOUNT(p)  ((p)->module ? __MOD_DEC_USE_COUNT((p)->module) : 0)
#else
#  define INCRCOUNT(p)  ((p)->use_count++)
#  define CURRCOUNT(p)  ((p)->use_count)
#  define DECRCOUNT(p)  ((p)->use_count--)
#endif

/* register_dynamic no more existent -- just have 0 as inum */

#ifdef LINUX_22
#  define proc_register_dynamic proc_register
#endif

#ifdef LINUX_20
#  define test_and_set_bit(nr,addr)  test_bit((nr),(addr))
#  define test_and_clear_bit(nr,addr) clear_bit((nr),(addr))
#  define test_and_change_bit(nr,addr) change_bit((nr),(addr))
#endif

/* 2.1.30 removed these functions. Let's define them, just in case */
#ifdef LINUX_22
#  define queue_task_irq      queue_task
#  define queue_task_irq_off  queue_task
#endif

/* 2.1.10 and 2.1.43 introduced new functions. They are worth using */

#ifdef LINUX_20

#  include <asm/byteorder.h>
#  ifdef __LITTLE_ENDIAN
#    define cpu_to_le16(x) (x)
#    define cpu_to_le32(x) (x)
#    define cpu_to_be16(x) htons((x))
#    define cpu_to_be32(x) htonl((x))
#  else
#    define cpu_to_be16(x) (x)
#    define cpu_to_be32(x) (x)
     extern inline __u16 cpu_to_le16(__u16 x) { return (x<<8) | (x>>8);}
     extern inline __u32 cpu_to_le32(__u32 x) { return((x>>24) |
             ((x>>8)&0xff00) | ((x<<8)&0xff0000) | (x<<24));}
#  endif

#  define le16_to_cpu(x)  cpu_to_le16(x)
#  define le32_to_cpu(x)  cpu_to_le32(x)
#  define be16_to_cpu(x)  cpu_to_be16(x)
#  define be32_to_cpu(x)  cpu_to_be32(x)

#  define cpu_to_le16p(addr) (cpu_to_le16(*(addr)))
#  define cpu_to_le32p(addr) (cpu_to_le32(*(addr)))
#  define cpu_to_be16p(addr) (cpu_to_be16(*(addr)))
#  define cpu_to_be32p(addr) (cpu_to_be32(*(addr)))

   extern inline void cpu_to_le16s(__u16 *a) {*a = cpu_to_le16(*a);}
   extern inline void cpu_to_le32s(__u16 *a) {*a = cpu_to_le32(*a);}
   extern inline void cpu_to_be16s(__u16 *a) {*a = cpu_to_be16(*a);}
   extern inline void cpu_to_be32s(__u16 *a) {*a = cpu_to_be32(*a);}

#  define le16_to_cpup(x) cpu_to_le16p(x)
#  define le32_to_cpup(x) cpu_to_le32p(x)
#  define be16_to_cpup(x) cpu_to_be16p(x)
#  define be32_to_cpup(x) cpu_to_be32p(x)

#  define le16_to_cpus(x) cpu_to_le16s(x)
#  define le32_to_cpus(x) cpu_to_le32s(x)
#  define be16_to_cpus(x) cpu_to_be16s(x)
#  define be32_to_cpus(x) cpu_to_be32s(x)

#endif

#ifdef LINUX_20
#  define __USE_OLD_REBUILD_HEADER__
#endif

/* other things that are virtualized: define the new functions for the old k */
#ifdef LINUX_20
#  define in_interrupt() (intr_count!=0)
#  define mdelay(x) udelay((x)*1000)
#  define signal_pending(current)  (current->signal & ~current->blocked)
#endif

#ifdef LINUX_20 /* a whole set of replacement functions */
#  include "pci-compat.h"
#else /* a placeholder is needed. as 2.0 allocs/clears pci structures */
#  define  pci_release_device(d)
#endif

#ifdef LINUX_20 /* physical and virtual addresses had the same value */
#  define __pa(a) (a)
#  define __va(a) (a)
#endif


#endif /* _SYSDEP_H_ */
