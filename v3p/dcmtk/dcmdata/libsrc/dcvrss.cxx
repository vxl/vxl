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
 *  Purpose: Implementation of class DcmSignedShort
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstream.h"
#include "dcvrss.h"
#include "dcvm.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "ofstdinc.h"


// ********************************


DcmSignedShort::DcmSignedShort(const DcmTag &tag,
                               const Uint32 len)
  : DcmElement(tag, len)
{
}


DcmSignedShort::DcmSignedShort(const DcmSignedShort &old)
  : DcmElement(old)
{
}


DcmSignedShort::~DcmSignedShort()
{
}


DcmSignedShort &DcmSignedShort::operator=(const DcmSignedShort &obj)
{
    DcmElement::operator=(obj);
    return *this;
}


// ********************************


DcmEVR DcmSignedShort::ident() const
{
    return EVR_SS;
}


unsigned long DcmSignedShort::getVM()
{
    return Length / sizeof(Sint16);
}


// ********************************


void DcmSignedShort::print(ostream &out,
                           const size_t flags,
                           const int level,
                           const char * /*pixelFileName*/,
                           size_t * /*pixelCounter*/)
{
    if (valueLoaded())
    {
        /* get signed integer data */
        Sint16 *sintVals;
        errorFlag = getSint16Array(sintVals);
        if (sintVals != NULL)
        {
            const unsigned long count = getVM();
            const unsigned long maxLength = (flags & DCMTypes::PF_shortenLongTagValues) ?
                DCM_OptPrintLineLength : (unsigned long)-1 /*unlimited*/;
            unsigned long printedLength = 0;
            unsigned long newLength = 0;
            char buffer[32];
            /* print line start with tag and VR */
            printInfoLineStart(out, flags, level);
            /* print multiple values */
            for (unsigned int i = 0; i < count; i++, sintVals++)
            {
                /* check whether first value is printed (omit delimiter) */
                if (i == 0)
                    sprintf(buffer, "%hd", *sintVals);
                else
                    sprintf(buffer, "\\%hd", *sintVals);
                /* check whether current value sticks to the length limit */
                newLength = printedLength + strlen(buffer);
                if ((newLength <= maxLength) && ((i + 1 == count) || (newLength + 3 <= maxLength)))
                {
                    out << buffer;
                    printedLength = newLength;
                } else {
                    /* check whether output has been truncated */
                    if (i + 1 < count)
                    {
                        out << "...";
                        printedLength += 3;
                    }
                    break;
                }
            }
            /* print line end with length, VM and tag name */
            printInfoLineEnd(out, flags, printedLength);
        } else
            printInfoLine(out, flags, level, "(no value available)");
    } else
        printInfoLine(out, flags, level, "(not loaded)");
}


// ********************************


OFCondition DcmSignedShort::getSint16(Sint16 &sintVal,
                                      const unsigned long pos)
{
    /* get signed integer data */
    Sint16 *sintValues = NULL;
    errorFlag = getSint16Array(sintValues);
    /* check data before returning */
    if (errorFlag.good())
    {
        if (sintValues == NULL)
            errorFlag = EC_IllegalCall;
        else if (pos >= getVM())
            errorFlag = EC_IllegalParameter;
        else
            sintVal = sintValues[pos];
    }
    /* clear value in case of error */
    if (errorFlag.bad())
        sintVal = 0;
    return errorFlag;
}


OFCondition DcmSignedShort::getSint16Array(Sint16 *&sintVals)
{
    sintVals = (Sint16 *)getValue();
    return errorFlag;
}


// ********************************


OFCondition DcmSignedShort::getOFString(OFString &stringVal,
                                        const unsigned long pos,
                                        OFBool /*normalize*/)
{
    Sint16 sintVal;
    /* get the specified numeric value */
    errorFlag = getSint16(sintVal, pos);
    if (errorFlag.good())
    {
        /* ... and convert it to a character string */
        char buffer[32];
        sprintf(buffer, "%i", sintVal);
        /* assign result */
        stringVal = buffer;
    }
    return errorFlag;
}


// ********************************


OFCondition DcmSignedShort::putSint16(const Sint16 sintVal,
                                      const unsigned long pos)
{
    Sint16 val = sintVal;
    errorFlag = changeValue(&val, sizeof(Sint16) * pos, sizeof(Sint16));
    return errorFlag;
}


OFCondition DcmSignedShort::putSint16Array(const Sint16 *sintVals,
                                           const unsigned long numSints)
{
    errorFlag = EC_Normal;
    if (numSints > 0)
    {
        /* check for valid data */
        if (sintVals != NULL)
            errorFlag = putValue(sintVals, sizeof(Sint16) * (Uint32)numSints);
        else
            errorFlag = EC_CorruptedData;
    } else
        errorFlag = putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmSignedShort::putString(const char *stringVal)
{
    errorFlag = EC_Normal;
    /* check input string */
    if ((stringVal != NULL) && (strlen(stringVal) > 0))
    {
        const unsigned long vm = getVMFromString(stringVal);
        if (vm > 0)
        {
            Sint16 *field = new Sint16[vm];
            const char *s = stringVal;
            char *value;
            /* retrieve signed integer data from character string */
            for (unsigned long i = 0; (i < vm) && errorFlag.good(); i++)
            {
                /* get first value stored in 's', set 's' to beginning of the next value */
                value = getFirstValueFromString(s);
                if ((value == NULL) || (sscanf(value, "%hd", &field[i]) != 1))
                    errorFlag = EC_CorruptedData;
                delete[] value;
            }
            /* set binary data as the element value */
            if (errorFlag.good())
                errorFlag = putSint16Array(field, vm);
            /* delete temporary buffer */
            delete[] field;
        } else
            errorFlag = putValue(NULL, 0);
    } else
        errorFlag = putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmSignedShort::verify(const OFBool autocorrect)
{
    /* check for valid value length */
    if (Length % (sizeof(Sint16)) != 0)
    {
        errorFlag = EC_CorruptedData;
        if (autocorrect)
        {
            /* strip to valid length */
            Length -= (Length % (sizeof(Sint16)));
        }
    } else
        errorFlag = EC_Normal;
    return errorFlag;
}
