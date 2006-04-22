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
 *  Author:  Andrew Hewett
 *
 *  Purpose: Implementation of class DcmUnlimitedText
 *           Value Representation UT is defined in Correction Proposal 101
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvrut.h"


// ********************************


DcmUnlimitedText::DcmUnlimitedText(const DcmTag &tag,
                                   const Uint32 len)
  : DcmCharString(tag, len)
{
    maxLength = DCM_UndefinedLength;
}


DcmUnlimitedText::DcmUnlimitedText(const DcmUnlimitedText &old)
  : DcmCharString(old)
{
}


DcmUnlimitedText::~DcmUnlimitedText()
{
}


DcmUnlimitedText &DcmUnlimitedText::operator=(const DcmUnlimitedText &obj)
{
    DcmCharString::operator=(obj);
    return *this;
}


// ********************************


DcmEVR DcmUnlimitedText::ident() const
{
    return EVR_UT;
}


unsigned long DcmUnlimitedText::getVM()
{
    /* value multiplicity is 1 for non-empty string, 0 otherwise */
    return (getRealLength() > 0) ? 1 : 0;
}


// ********************************


OFCondition DcmUnlimitedText::getOFString(OFString &strValue,
                                          const unsigned long /*pos*/,
                                          OFBool normalize)
{
    /* treat backslash as a normal character */
    return getOFStringArray(strValue, normalize);
}


OFCondition DcmUnlimitedText::getOFStringArray(OFString &strValue,
                                               OFBool normalize)
{
    /* get string value without handling the "\" as a delimiter */
    OFCondition l_error = getStringValue(strValue);
    // leading spaces are significant and backslash is normal character
    if (l_error.good() && normalize)
        normalizeString(strValue, !MULTIPART, !DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}
