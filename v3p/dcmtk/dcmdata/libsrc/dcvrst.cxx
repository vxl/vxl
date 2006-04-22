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
 *  Purpose: Implementation of class DcmShortText
 *
 */


#include "dcvrst.h"


// ********************************


DcmShortText::DcmShortText(const DcmTag &tag,
                           const Uint32 len)
  : DcmCharString(tag, len)
{
    maxLength = 1024;
}


DcmShortText::DcmShortText(const DcmShortText &old)
  : DcmCharString(old)
{
}


DcmShortText::~DcmShortText()
{
}


DcmShortText &DcmShortText::operator=(const DcmShortText &obj)
{
    DcmCharString::operator=(obj);
    return *this;
}


// ********************************


DcmEVR DcmShortText::ident() const
{
    return EVR_ST;
}


unsigned long DcmShortText::getVM()
{
    /* value multiplicity is 1 for non-empty string, 0 otherwise */
    return (getRealLength() > 0) ? 1 : 0;
}


// ********************************


OFCondition DcmShortText::getOFString(OFString &stringVal,
                                      const unsigned long /*pos*/,
                                      OFBool normalize)
{
    /* treat backslash as a normal character */
    return getOFStringArray(stringVal, normalize);
}


OFCondition DcmShortText::getOFStringArray(OFString &stringVal,
                                           OFBool normalize)
{
    /* get string value without handling the "\" as a delimiter */
    OFCondition l_error = getStringValue(stringVal);
    if (l_error.good() && normalize)
        normalizeString(stringVal, !MULTIPART, !DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}
