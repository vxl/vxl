#ifndef vxl_config_h_config_stlport_win32_vc60_
#define vxl_config_h_config_stlport_win32_vc60_

/* This file was *not* generated from vxl_config.h.in */
/* It is maintained manually. */

#define VXL_LITTLE_ENDIAN 1
#define VXL_BIG_ENDIAN    0

typedef char      vxl_int8;
typedef short     vxl_int16;
typedef int       vxl_int32;
typedef void      vxl_int64;           /* ! */

typedef unsigned char      vxl_uint8;
typedef unsigned short     vxl_uint16;
typedef unsigned int       vxl_uint32;
typedef void               vxl_uint64; /* ! */

typedef signed char      vxl_sint8;
typedef signed short     vxl_sint16;
typedef signed int       vxl_sint32;
typedef void             vxl_sint64;   /* ! */

#define vxl_ieee_32  float
#define vxl_ieee_64  double

#define VXL_HAS_PTHREAD_H         0
#define VXL_HAS_SEMAPHORE_H       0

#define VXL_UNISTD_HAS_USECONDS_T 0
#define VXL_UNISTD_HAS_INTPTR_T   0
#define VXL_UNISTD_HAS_UALARM     1
#define VXL_UNISTD_HAS_USLEEP     1
#define VXL_UNISTD_HAS_LCHOWN     1
#define VXL_UNISTD_HAS_PREAD      1
#define VXL_UNISTD_HAS_PWRITE     1
#define VXL_UNISTD_HAS_TELL       1

#define VXL_UNISTD_USLEEP_IS_VOID 0
#define VXL_TWO_ARG_GETTIME       0
#define VXL_STDLIB_HAS_QSORT      1
#define VXL_STDLIB_HAS_LRAND48    1 /* ? */
#define VXL_STDLIB_HAS_DRAND48    0
#define VXL_IEEEFP_HAS_FINITE     0 /* ? */
#define VXL_MATH_HAS_FINITE       0 /* ? */
#define VXL_MATH_HAS_SQRTF        1 /* ? */
#define VXL_HAS_IEEEFP_H          1

#endif
