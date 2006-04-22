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
 *  Author:  Gerd Ehlers
 *
 *  Purpose: Implementation of class DcmShortString
 *
 */


#include "dcvrsh.h"


// ********************************


DcmShortString::DcmShortString(const DcmTag &tag,
                               const Uint32 len)
  : DcmCharString(tag, len)
{
    maxLength = 16;
}


DcmShortString::DcmShortString(const DcmShortString &old)
  : DcmCharString(old)
{
}


DcmShortString::~DcmShortString()
{
}


DcmShortString &DcmShortString::operator=(const DcmShortString &obj)
{
    DcmCharString::operator=(obj);
    return *this;
}


// ********************************


DcmEVR DcmShortString::ident() const
{
    return EVR_SH;
}


// ********************************


OFCondition DcmShortString::getOFString(OFString &stringVal,
                                        const unsigned long pos,
                                        OFBool normalize)
{
    OFCondition l_error = DcmCharString::getOFString(stringVal, pos, normalize);
    if (l_error.good() && normalize)
        normalizeString(stringVal, !MULTIPART, DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}
