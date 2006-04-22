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
 *  Purpose: Implementation of class DcmSignedLong
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstream.h"
#include "dcvrsl.h"
#include "dcvm.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "ofstdinc.h"


// ********************************


DcmSignedLong::DcmSignedLong(const DcmTag &tag,
                             const Uint32 len)
  : DcmElement(tag, len)
{
}


DcmSignedLong::DcmSignedLong(const DcmSignedLong &old)
  : DcmElement(old)
{
}


DcmSignedLong::~DcmSignedLong()
{
}


DcmSignedLong &DcmSignedLong::operator=(const DcmSignedLong &obj)
{
    DcmElement::operator=(obj);
    return *this;
}


// ********************************


DcmEVR DcmSignedLong::ident() const
{
    return EVR_SL;
}


unsigned long DcmSignedLong::getVM()
{
    return Length / sizeof(Sint32);
}


// ********************************


void DcmSignedLong::print(ostream &out,
                          const size_t flags,
                          const int level,
                          const char * /*pixelFileName*/,
                          size_t * /*pixelCounter*/)
{
    if (valueLoaded())
    {
        /* get signed integer data */
        Sint32 *sintVals;
        errorFlag = getSint32Array(sintVals);
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
#if SIZEOF_LONG == 8
                if (i == 0)
                    sprintf(buffer, "%d", *sintVals);
                else
                    sprintf(buffer, "\\%d", *sintVals);
#else
                if (i == 0)
                    sprintf(buffer, "%ld", *sintVals);
                else
                    sprintf(buffer, "\\%ld", *sintVals);
#endif
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


OFCondition DcmSignedLong::getSint32(Sint32 &sintVal,
                                     const unsigned long pos)
{
    /* get signed integer data */
    Sint32 *sintValues = NULL;
    errorFlag = getSint32Array(sintValues);
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


OFCondition DcmSignedLong::getSint32Array(Sint32 *&sintVals)
{
    sintVals = (Sint32 *)getValue();
    return errorFlag;
}


// ********************************


OFCondition DcmSignedLong::getOFString(OFString &stringVal,
                                       const unsigned long pos,
                                       OFBool /*normalize*/)
{
    Sint32 sintVal;
    /* get the specified numeric value */
    errorFlag = getSint32(sintVal, pos);
    if (errorFlag.good())
    {
        /* ... and convert it to a character string */
        char buffer[32];
        sprintf(buffer, "%li", (long)sintVal);
        /* assign result */
        stringVal = buffer;
    }
    return errorFlag;
}


// ********************************


OFCondition DcmSignedLong::putSint32(const Sint32 sintVal,
                                     const unsigned long pos)
{
    Sint32 val = sintVal;
    errorFlag = changeValue(&val, sizeof(Sint32) * pos, sizeof(Sint32));
    return errorFlag;
}


OFCondition DcmSignedLong::putSint32Array(const Sint32 *sintVals,
                                          const unsigned long numSints)
{
    errorFlag = EC_Normal;
    if (numSints > 0)
    {
        /* check for valid data */
        if (sintVals != NULL)
            errorFlag = putValue(sintVals, sizeof(Sint32) * (Uint32)numSints);
        else
            errorFlag = EC_CorruptedData;
    } else
        errorFlag = putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmSignedLong::putString(const char *stringVal)
{
    errorFlag = EC_Normal;
    /* check input string */
    if ((stringVal != NULL) && (strlen(stringVal) > 0))
    {
        const unsigned long vm = getVMFromString(stringVal);
        if (vm > 0)
        {
            Sint32 *field = new Sint32[vm];
            const char *s = stringVal;
            char *value;
            /* retrieve signed integer data from character string */
            for (unsigned long i = 0; (i < vm) && errorFlag.good(); i++)
            {
                /* get first value stored in 's', set 's' to beginning of the next value */
                value = getFirstValueFromString(s);
                if ((value == NULL) ||
#if SIZEOF_LONG == 8
                    (sscanf(value, "%d", &field[i]) != 1)
#else
                    (sscanf(value, "%ld", &field[i]) != 1)
#endif
                    )
                {
                    errorFlag = EC_CorruptedData;
                }
                delete[] value;
            }
            /* set binary data as the element value */
            if (errorFlag.good())
                errorFlag = putSint32Array(field, vm);
            /* delete temporary buffer */
            delete[] field;
        } else
            errorFlag = putValue(NULL, 0);
    } else
        errorFlag = putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmSignedLong::verify(const OFBool autocorrect)
{
    /* check for valid value length */
    if (Length % (sizeof(Sint32)) != 0)
    {
        errorFlag = EC_CorruptedData;
        if (autocorrect)
        {
            /* strip to valid length */
            Length -= (Length % (sizeof(Sint32)));
        }
    } else
        errorFlag = EC_Normal;
    return errorFlag;
}
