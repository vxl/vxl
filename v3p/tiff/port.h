/*
 * Warning, this file was automatically created by the TIFF configure script
 * VERSION:      v3.4beta037
 * DATE:         Tue Feb 10 16:22:58 MET 1998
 * TARGET:       sparc-sun-solaris2.6
 * CCOMPILER:    /tools/gcc-2.7/bin/gcc-2.7.2.3
 */
#ifndef _PORT_
#define _PORT_ 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include <sys/types.h>
#define HOST_FILLORDER FILLORDER_MSB2LSB
#ifdef WORDS_BIGENDIAN
#define HOST_BIGENDIAN  1
#else
#define HOST_BIGENDIAN  0
#endif
#include <stdio.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
typedef double dblparam_t;
#if defined(__GNUC__) || defined(__cplusplus)
#define INLINE  inline
#else
#define INLINE
#endif
#define GLOBALDATA(TYPE,NAME)   extern TYPE NAME
#ifdef __cplusplus
}
#endif
#endif
