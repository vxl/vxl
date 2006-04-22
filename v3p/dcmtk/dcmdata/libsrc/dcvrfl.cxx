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
 *  Purpose: Implementation of class DcmFloatingPointSingle
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "ofstream.h"
#include "ofstd.h"
#include "dcvrfl.h"
#include "dcvm.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "ofstdinc.h"


// ********************************


DcmFloatingPointSingle::DcmFloatingPointSingle(const DcmTag &tag,
                                               const Uint32 len)
  : DcmElement(tag, len)
{
}


DcmFloatingPointSingle::DcmFloatingPointSingle(const DcmFloatingPointSingle &old)
  : DcmElement(old)
{
}


DcmFloatingPointSingle::~DcmFloatingPointSingle()
{
}


DcmFloatingPointSingle &DcmFloatingPointSingle::operator=(const DcmFloatingPointSingle &obj)
{
    DcmElement::operator=(obj);
    return *this;
}


// ********************************


DcmEVR DcmFloatingPointSingle::ident() const
{
    return EVR_FL;
}


unsigned long DcmFloatingPointSingle::getVM()
{
    return Length / sizeof(Float32);
}


// ********************************


void DcmFloatingPointSingle::print(ostream &out,
                                   const size_t flags,
                                   const int level,
                                   const char * /*pixelFileName*/,
                                   size_t * /*pixelCounter*/)
{
    if (valueLoaded())
    {
        /* get float data */
        Float32 *floatVals;
        errorFlag = getFloat32Array(floatVals);
        if (floatVals != NULL)
        {
            const unsigned long count = Length / sizeof(Float32) /* do not use getVM()! */;
            const unsigned long maxLength = (flags & DCMTypes::PF_shortenLongTagValues) ?
                DCM_OptPrintLineLength : (unsigned long)-1;
            unsigned long printedLength = 0;
            unsigned long newLength = 0;
            char buffer[64];
            /* print line start with tag and VR */
            printInfoLineStart(out, flags, level);
            /* print multiple values */
            for (unsigned int i = 0; i < count; i++, floatVals++)
            {
                /* check whether first value is printed (omit delimiter) */
                if (i == 0)
                    OFStandard::ftoa(buffer, sizeof(buffer), *floatVals);
                else
                {
                    buffer[0] = '\\';
                    OFStandard::ftoa(buffer + 1, sizeof(buffer) - 1, *floatVals);
                }
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
            printInfoLine(out, flags, level, "(no value available)" );
    } else
        printInfoLine(out, flags, level, "(not loaded)" );
}


// ********************************


OFCondition DcmFloatingPointSingle::getFloat32(Float32 &floatVal,
                                               const unsigned long pos)
{
    /* get float data */
    Float32 *floatValues = NULL;
    errorFlag = getFloat32Array(floatValues);
    /* check data before returning */
    if (errorFlag.good())
    {
        if (floatValues == NULL)
            errorFlag = EC_IllegalCall;
        else if (pos >= Length / sizeof(Float32) /* do not use getVM()! */)
            errorFlag = EC_IllegalParameter;
        else
            floatVal = floatValues[pos];
    }
    /* clear value in case of error */
    if (errorFlag.bad())
        floatVal = 0;
    return errorFlag;
}


OFCondition DcmFloatingPointSingle::getFloat32Array(Float32 *&floatVals)
{
    floatVals = (Float32 *)getValue();
    return errorFlag;
}


// ********************************


OFCondition DcmFloatingPointSingle::getOFString(OFString &value,
                                                const unsigned long pos,
                                                OFBool /*normalize*/)
{
    Float32 floatVal;
    /* get the specified numeric value */
    errorFlag = getFloat32(floatVal, pos);
    if (errorFlag.good())
    {
        /* ... and convert it to a character string */
        char buffer[64];
        OFStandard::ftoa(buffer, sizeof(buffer), floatVal);
        /* assign result */
        value = buffer;
    }
    return errorFlag;
}


// ********************************


OFCondition DcmFloatingPointSingle::putFloat32(const Float32 floatVal,
                                               const unsigned long pos)
{
    Float32 val = floatVal;
    errorFlag = changeValue(&val, sizeof(Float32) * pos, sizeof(Float32));
    return errorFlag;
}


OFCondition DcmFloatingPointSingle::putFloat32Array(const Float32 *floatVals,
                                                    const unsigned long numFloats)
{
    errorFlag = EC_Normal;
    if (numFloats > 0)
    {
        /* check for valid float data */
        if (floatVals != NULL)
            errorFlag = putValue(floatVals, sizeof(Float32) * (Uint32)numFloats);
        else
            errorFlag = EC_CorruptedData;
    } else
        putValue(NULL, 0);

    return errorFlag;
}


// ********************************


OFCondition DcmFloatingPointSingle::putString(const char *stringVal)
{
    errorFlag = EC_Normal;
    /* check input string */
    if ((stringVal != NULL) && (strlen(stringVal) > 0))
    {
        const unsigned long vm = getVMFromString(stringVal);
        if (vm > 0)
        {
            Float32 *field = new Float32[vm];
            OFBool success = OFFalse;
            const char *s = stringVal;
            char *value;
            /* retrieve float data from character string */
            for (unsigned long i = 0; (i < vm) && errorFlag.good(); i++)
            {
                /* get first value stored in 's', set 's' to beginning of the next value */
                value = getFirstValueFromString(s);
                if (value != NULL)
                {
                    field[i] = (Float32)OFStandard::atof(value, &success);
                    if (!success)
                        errorFlag = EC_CorruptedData;
                    delete[] value;
                } else
                    errorFlag = EC_CorruptedData;
            }
            /* set binary data as the element value */
            if (errorFlag.good())
                errorFlag = putFloat32Array(field, vm);
            /* delete temporary buffer */
            delete[] field;
        } else
            errorFlag = putValue(NULL, 0);
    } else
        errorFlag = putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmFloatingPointSingle::verify(const OFBool autocorrect)
{
    /* check for valid value length */
    if (Length % (sizeof(Float32)) != 0)
    {
        errorFlag = EC_CorruptedData;
        if (autocorrect)
        {
            /* strip to valid length */
            Length -= (Length % (sizeof(Float32)));
        }
    } else
        errorFlag = EC_Normal;
    return errorFlag;
}
