/*
 *
 *  Copyright (C) 1994-2002, OFFIS
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
 *  Module:  dcmdata
 *
 *  Author:  Andreas Barth
 *
 *  Purpose: byte order functions
 *
 */

#ifndef DCSWAP_H
#define DCSWAP_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dctypes.h"
#include "dcerror.h"
#include "dcxfer.h"

#define INCLUDE_CSTDLIB
#include "ofstdinc.h"


// Swap byteLength bytes, if newByteOrder != oldByteOrder. The bytes are
// parted in valWidth bytes, each swapped separately
OFCondition swapIfNecessary(const E_ByteOrder newByteOrder, 
			    const E_ByteOrder oldByteOrder,
			    void * value, const Uint32 byteLength,
			    const size_t valWidth);


void swapBytes(void * value, const Uint32 byteLength, 
			   const size_t valWidth);

Uint16 swapShort(const Uint16 toSwap);

inline void swap2Bytes(Uint8 * toSwap)
// swaps [byte0][byte1] to [byte1][byte0]
{
    Uint8 tmp = toSwap[0];
    toSwap[0] = toSwap[1];
    toSwap[1] = tmp;
}

inline void swap4Bytes(Uint8 * toSwap)
// swaps [byte0][byte1][byte2][byte3] to [byte3][byte2][byte1][byte0]
{
    Uint8 tmp = toSwap[0];
    toSwap[0] = toSwap[3];
    toSwap[3] = tmp;
    tmp = toSwap[1];
    toSwap[1] = toSwap[2];
    toSwap[2] = tmp;
}

#endif // DCSWAP_H
