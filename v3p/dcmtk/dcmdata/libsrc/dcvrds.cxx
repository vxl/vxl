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
 *  Purpose: Implementation of class DcmDecimalString
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvrds.h"
#include "ofstring.h"
#include "ofstd.h"


// ********************************


DcmDecimalString::DcmDecimalString(const DcmTag &tag,
                                   const Uint32 len)
  : DcmByteString(tag, len)
{
    maxLength = 16;
}


DcmDecimalString::DcmDecimalString(const DcmDecimalString &old)
  : DcmByteString(old)
{
}


DcmDecimalString::~DcmDecimalString()
{
}


DcmDecimalString &DcmDecimalString::operator=(const DcmDecimalString &obj)
{
    DcmByteString::operator=(obj);
    return *this;
}


// ********************************


DcmEVR DcmDecimalString::ident() const
{
    return EVR_DS;
}


// ********************************


OFCondition DcmDecimalString::getFloat64(Float64 &doubleVal,
                                         const unsigned long pos)
{
    /* get decimal string value */
    OFString str;
    OFCondition l_error = getOFString(str, pos, OFTrue /*normalize*/);
    if (l_error.good())
    {
        OFBool success = OFFalse;
        /* convert string to float value */
        doubleVal = OFStandard::atof(str.c_str(), &success);
        if (!success)
            l_error = EC_CorruptedData;
    }
    return l_error;
}


// ********************************


OFCondition DcmDecimalString::getOFString(OFString &stringVal,
                                          const unsigned long pos,
                                          OFBool normalize)
{
    /* call inherited method */
    OFCondition l_error = DcmByteString::getOFString(stringVal, pos);
    /* normalize string if required */
    if (l_error.good() && normalize)
        normalizeString(stringVal, !MULTIPART, DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}
