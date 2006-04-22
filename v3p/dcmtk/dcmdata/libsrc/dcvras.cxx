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
 *  Purpose: Implementation of class DcmAgeString
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcvras.h"


// ********************************


DcmAgeString::DcmAgeString(const DcmTag &tag,
                           const Uint32 len)
  : DcmByteString(tag, len)
{
    maxLength = 4;
}


DcmAgeString::DcmAgeString(const DcmAgeString &old)
  : DcmByteString(old)
{
}


DcmAgeString::~DcmAgeString()
{
}

DcmAgeString &DcmAgeString::operator=(const DcmAgeString &obj)
{
    DcmByteString::operator=(obj);
    return *this;
}


// ********************************


DcmEVR DcmAgeString::ident() const
{
    return EVR_AS;
}
