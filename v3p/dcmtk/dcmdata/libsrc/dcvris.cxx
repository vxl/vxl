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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Implementation of class DcmIntegerString
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcvris.h"
#include "ofstring.h"

#define INCLUDE_CSTDIO
#include "ofstdinc.h"


// ********************************


DcmIntegerString::DcmIntegerString(const DcmTag &tag,
                                   const Uint32 len)
  : DcmByteString(tag, len)
{
    maxLength = 12;
}


DcmIntegerString::DcmIntegerString(const DcmIntegerString &old)
  : DcmByteString(old)
{
}


DcmIntegerString::~DcmIntegerString()
{
}


DcmIntegerString &DcmIntegerString::operator=(const DcmIntegerString &obj)
{
    DcmByteString::operator=(obj);
    return *this;
}


// ********************************


DcmEVR DcmIntegerString::ident() const
{
    return EVR_IS;
}


// ********************************


OFCondition DcmIntegerString::getSint32(Sint32 &sintVal,
                                        const unsigned long pos)
{
    /* get integer string value */
    OFString str;
    OFCondition l_error = getOFString(str, pos, OFTrue);
    if (l_error.good())
    {
        /* convert string to integer value */
#if SIZEOF_LONG == 8
        if (sscanf(str.c_str(), "%d", &sintVal) != 1)
#else
        if (sscanf(str.c_str(), "%ld", &sintVal) != 1)
#endif
            l_error = EC_CorruptedData;
    }
    return l_error;
}


// ********************************


OFCondition DcmIntegerString::getOFString(OFString &stringVal,
                                          const unsigned long pos,
                                          OFBool normalize)
{
    /* call inherited method */
    OFCondition l_error = DcmByteString::getOFString(stringVal, pos, normalize);
    /* normalize string if required */
    if (l_error.good() && normalize)
        normalizeString(stringVal, !MULTIPART, DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}
