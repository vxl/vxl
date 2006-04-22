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
 *  Purpose: Implementation of class DcmUnsignedLong
 *
 */


#include "osconfig.h"
#include "ofstream.h"
#include "dcvrul.h"
#include "dcvm.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "ofstdinc.h"


// ********************************


DcmUnsignedLong::DcmUnsignedLong(const DcmTag &tag,
                                 const Uint32 len)
  : DcmElement(tag, len)
{
}


DcmUnsignedLong::DcmUnsignedLong(const DcmUnsignedLong &old)
  : DcmElement(old)
{
}


DcmUnsignedLong::~DcmUnsignedLong()
{
}


DcmUnsignedLong &DcmUnsignedLong::operator=(const DcmUnsignedLong &obj)
{
    DcmElement::operator=(obj);
    return *this;
}


// ********************************


DcmEVR DcmUnsignedLong::ident() const
{
    return EVR_UL;
}


unsigned long DcmUnsignedLong::getVM()
{
    return Length / sizeof(Uint32);
}


// ********************************


void DcmUnsignedLong::print(ostream &out,
                            const size_t flags,
                            const int level,
                            const char * /*pixelFileName*/,
                            size_t * /*pixelCounter*/)
{
    if (valueLoaded())
    {
        /* get unsigned integer data */
        Uint32 *uintVals;
        errorFlag = getUint32Array(uintVals);
        if (uintVals != NULL)
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
            for (unsigned int i = 0; i < count; i++, uintVals++)
            {
                /* check whether first value is printed (omit delimiter) */
                if (i == 0)
#if SIZEOF_LONG == 8
                    sprintf(buffer, "%u", *uintVals);
                else
                    sprintf(buffer, "\\%u", *uintVals);
#else
                    sprintf(buffer, "%lu", *uintVals);
                else
                    sprintf(buffer, "\\%lu", *uintVals);
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


OFCondition DcmUnsignedLong::getUint32(Uint32 &uintVal,
                                       const unsigned long pos)
{
    /* get unsigned integer data */
    Uint32 *uintValues = NULL;
    errorFlag = getUint32Array(uintValues);
    /* check data before returning */
    if (errorFlag.good())
    {
        if (uintValues == NULL)
            errorFlag = EC_IllegalCall;
        else if (pos >= getVM())
            errorFlag = EC_IllegalParameter;
        else
            uintVal = uintValues[pos];
    }
    /* clear value in case of error */
    if (errorFlag.bad())
        uintVal = 0;
    return errorFlag;
}


OFCondition DcmUnsignedLong::getUint32Array(Uint32 *&uintVals)
{
    uintVals = (Uint32 *)getValue();
    return errorFlag;
}


// ********************************


OFCondition DcmUnsignedLong::getOFString(OFString &stringVal,
                                         const unsigned long pos,
                                         OFBool /*normalize*/)
{
    Uint32 uintVal;
    /* get the specified numeric value */
    errorFlag = getUint32(uintVal, pos);
    if (errorFlag.good())
    {
        /* ... and convert it to a character string */
        char buffer[32];
        sprintf(buffer, "%lu", (unsigned long)uintVal);
        /* assign result */
        stringVal = buffer;
    }
    return errorFlag;
}


// ********************************


OFCondition DcmUnsignedLong::putUint32(const Uint32 uintVal,
                                       const unsigned long pos)
{
    Uint32 val = uintVal;
    errorFlag = changeValue(&val, sizeof(Uint32) * pos, sizeof(Uint32));
    return errorFlag;
}


OFCondition DcmUnsignedLong::putUint32Array(const Uint32 *uintVals,
                                            const unsigned long numUints)
{
    errorFlag = EC_Normal;
    if (numUints > 0)
    {
        /* check for valid data */
        if (uintVals != NULL)
            errorFlag = putValue(uintVals, sizeof(Uint32) * (Uint32)numUints);
        else
            errorFlag = EC_CorruptedData;
    } else
        errorFlag = putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmUnsignedLong::putString(const char *stringVal)
{
    errorFlag = EC_Normal;
    /* check input string */
    if ((stringVal != NULL) && (strlen(stringVal) > 0))
    {
        const unsigned long vm = getVMFromString(stringVal);
        if (vm > 0)
        {
            Uint32 *field = new Uint32[vm];
            const char *s = stringVal;
            char *value;
            /* retrieve unsigned integer data from character string */
            for (unsigned long i = 0; (i < vm) && errorFlag.good(); i++)
            {
                /* get first value stored in 's', set 's' to beginning of the next value */
                value = getFirstValueFromString(s);
                if ((value == NULL) ||
#if SIZEOF_LONG == 8
                    (sscanf(value, "%u", &field[i]) != 1)
#else
                    (sscanf(value, "%lu", &field[i]) != 1)
#endif
                    )
                {
                    errorFlag = EC_CorruptedData;
                }
                delete[] value;
            }
            /* set binary data as the element value */
            if (errorFlag.good())
                errorFlag = putUint32Array(field, vm);
            /* delete temporary buffer */
            delete[] field;
        } else
            errorFlag = putValue(NULL, 0);
    } else
        errorFlag = putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmUnsignedLong::verify(const OFBool autocorrect)
{
    /* check for valid value length */
    if (Length % (sizeof(Uint32)) != 0)
    {
        errorFlag = EC_CorruptedData;
        if (autocorrect)
        {
            /* strip to valid length */
            Length -= (Length % (sizeof(Uint32)));
        }
    } else
        errorFlag = EC_Normal;
    return errorFlag;
}
