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
 *  Purpose: Implementation of class DcmApplicationEntity
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcvrae.h"


// ********************************


DcmApplicationEntity::DcmApplicationEntity(const DcmTag &tag,
                                           const Uint32 len)
  : DcmByteString(tag, len)
{
    maxLength = 16;
}


DcmApplicationEntity::DcmApplicationEntity(const DcmApplicationEntity &old)
  : DcmByteString(old)
{
}


DcmApplicationEntity::~DcmApplicationEntity()
{
}


DcmApplicationEntity &DcmApplicationEntity::operator=(const DcmApplicationEntity &obj)
{
    DcmByteString::operator=(obj);
    return *this;
}


// ********************************


DcmEVR DcmApplicationEntity::ident() const
{
    return EVR_AE;
}


// ********************************


OFCondition DcmApplicationEntity::getOFString(OFString &stringVal,
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
