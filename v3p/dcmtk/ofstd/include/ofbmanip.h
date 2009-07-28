/*
 *
 *  Copyright (C) 1997-2002, OFFIS
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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Template class for bit manipulations (Header)
 *
 */


#ifndef __OFBMANIP_H
#define __OFBMANIP_H

#include "osconfig.h"

#define INCLUDE_CSTRING
#include "ofstdinc.h"


#ifdef HAVE_BZERO
#ifndef HAVE_PROTOTYPE_BZERO
BEGIN_EXTERN_C
extern void bzero(char* s, int len);
END_EXTERN_C
#endif
#endif


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** A template class for bit manipulations.
 *  This class is used to perform platform independent operations on typed memory areas.
 */
template<class T>
class OFBitmanipTemplate
{
 public:

    /** copies specified number of elements from source to destination
     *
     ** @param  src    pointer to source memory
     *  @param  dest   pointer to destination memory
     *  @param  count  number of elements to be copied
     */
    static void copyMem(const T *src,
                        T *dest,
                        const unsigned long count)
    {
#if defined(HAVE_MEMCPY)
        memcpy((void *)dest, (const void *)src, (size_t)count * sizeof(T));
#elif defined(HAVE_BCOPY)
        bcopy((const void *)src, (void *)dest, (size_t)count * sizeof(T));
#else
        register unsigned long i;
        register const T *p = src;
        register T *q = dest;
        for (i = count; i != 0; i--)
            *q++ = *p++;
#endif
    }


    /** sets specified number of elements in destination memory to defined value
     *
     ** @param  dest   pointer to destination memory
     *  @param  value  value to be set
     *  @param  count  number of elements to be set
     */
    static void setMem(T *dest,
                       const T value,
                       const unsigned long count)
    {
#ifdef HAVE_MEMSET
        if ((value == 0) || (sizeof(T) == sizeof(unsigned char)))
            memset((void *)dest, (int)value, (size_t)count * sizeof(T));
        else
#endif
        {
            register unsigned long i;
            register T *q = dest;
            for (i = count; i != 0; i--)
                *q++ = value;
        }
    }


    /** sets specified number of elements in destination memory to zero
     *
     ** @param  dest   pointer to destination memory
     *  @param  count  number of elements to be set to zero
     */
    static void zeroMem(T *dest,
                        const unsigned long count)
    {
#ifdef HAVE_BZERO
        // some platforms, e.g. OSF1, require the first parameter to be char *.
        bzero((char *)dest, (size_t)count * sizeof(T));
#else
        setMem(dest, 0, count);
#endif
    }
};


#endif
