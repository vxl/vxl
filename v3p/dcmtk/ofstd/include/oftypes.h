/*
 *
 *  Copyright (C) 1997-2001, OFFIS
 *
 *  This software and supporting documentation were developed by
 *
 *    Kuratorium OFFIS e.V.
 *    Healthcare Information and Communication Systems
 *    Escherweg 2
 *    D-26121 Oldenburg, Germany
 *
 *  THIS SOFTWARE IS MADE AVAILABLE,  AS IS,  AND OFFIS MAKES NO  WARRANTY
 *  REGARDING  THE  SOFTWARE,  ITS  PERFORMANCE,  ITS  MERCHANTABILITY  OR
 *  FITNESS FOR ANY PARTICULAR USE, FREEDOM FROM ANY COMPUTER DISEASES  OR
 *  ITS CONFORMITY TO ANY SPECIFICATION. THE ENTIRE RISK AS TO QUALITY AND
 *  PERFORMANCE OF THE SOFTWARE IS WITH THE USER.
 *
 *  Module:  ofstd
 *
 *  Author:  Andreas Barth
 *
 *  Purpose:
 *      Defines some C++ standard types that are not consistently
 *      supported by all C++ Compilers
 *
 */

#ifndef OFTYPES_H
#define OFTYPES_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */


#ifdef __CHAR_UNSIGNED__
typedef signed char     Sint8;
#else
typedef char            Sint8;
#endif

typedef unsigned char   Uint8;

#if SIZEOF_LONG == 8
typedef signed int      Sint32;
typedef unsigned int    Uint32;
#else
typedef signed long     Sint32;
typedef unsigned long   Uint32;
#endif

typedef signed short    Sint16;
typedef unsigned short  Uint16;

typedef float           Float32;    /* 32 Bit Floating Point Single */
typedef double          Float64;    /* 64 Bit Floating Point Double */


// Definition of type OFBool

#ifdef HAVE_CXX_BOOL

#define OFBool bool
#define OFTrue true
#define OFFalse false

#else

/** the boolean type used throughout the DCMTK project. Mapped to the
 *  built-in type "bool" if the current C++ compiler supports it. Mapped
 *  to int for old-fashioned compilers which do not yet support bool.
 */
typedef int OFBool;

#ifndef OFTrue
#define OFTrue (1)
#endif

#ifndef OFFalse
#define OFFalse (0)
#endif

#endif
#endif
