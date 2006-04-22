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
 *  Purpose: Implementation of class DcmFloatingPointDouble
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "ofstream.h"
#include "ofstd.h"
#include "dcvrfd.h"
#include "dcvm.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "ofstdinc.h"


// ********************************


DcmFloatingPointDouble::DcmFloatingPointDouble(const DcmTag &tag,
                                               const Uint32 len)
  : DcmElement(tag, len)
{
}


DcmFloatingPointDouble::DcmFloatingPointDouble(const DcmFloatingPointDouble &old)
  : DcmElement(old)
{
}


DcmFloatingPointDouble::~DcmFloatingPointDouble()
{
}


DcmFloatingPointDouble &DcmFloatingPointDouble::operator=(const DcmFloatingPointDouble &obj)
{
    DcmElement::operator=(obj);
    return *this;
}


// ********************************


DcmEVR DcmFloatingPointDouble::ident() const
{
    return EVR_FD;
}


unsigned long DcmFloatingPointDouble::getVM()
{
    return Length / sizeof(Float64);
}


// ********************************


void DcmFloatingPointDouble::print(ostream &out,
                                   const size_t flags,
                                   const int level,
                                   const char * /*pixelFileName*/,
                                   size_t * /*pixelCounter*/)
{
    if (valueLoaded())
    {
        /* get double data */
        Float64 *doubleVals;
        errorFlag = getFloat64Array(doubleVals);
        if (doubleVals != NULL)
        {
            const unsigned long count = getVM();
            const unsigned long maxLength = (flags & DCMTypes::PF_shortenLongTagValues) ?
                DCM_OptPrintLineLength : (unsigned long)-1 /*unlimited*/;
            unsigned long printedLength = 0;
            unsigned long newLength = 0;
            char buffer[64];
            /* print line start with tag and VR */
            printInfoLineStart(out, flags, level);
            /* print multiple values */
            for (unsigned int i = 0; i < count; i++, doubleVals++)
            {
                /* check whether first value is printed (omit delimiter) */
                if (i == 0)
                    OFStandard::ftoa(buffer, sizeof(buffer), *doubleVals);
                else
                {
                    buffer[0] = '\\';
                    OFStandard::ftoa(buffer + 1, sizeof(buffer) - 1, *doubleVals);
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


OFCondition DcmFloatingPointDouble::getFloat64(Float64 &doubleVal,
                                               const unsigned long pos)
{
    /* get double data */
    Float64 *doubleValues = NULL;
    errorFlag = getFloat64Array(doubleValues);
    /* check data before returning */
    if (errorFlag.good())
    {
        if (doubleValues == NULL)
            errorFlag = EC_IllegalCall;
        else if (pos >= getVM())
            errorFlag = EC_IllegalParameter;
        else
            doubleVal = doubleValues[pos];
    }
    /* clear value in case of error */
    if (errorFlag.bad())
        doubleVal = 0;
    return errorFlag;
}


OFCondition DcmFloatingPointDouble::getFloat64Array(Float64 *&doubleVals)
{
    doubleVals =(Float64 *)getValue();
    return errorFlag;
}


// ********************************


OFCondition DcmFloatingPointDouble::getOFString(OFString &stringVal,
                                                const unsigned long pos,
                                                OFBool /*normalize*/)
{
    Float64 doubleVal;
    /* get the specified numeric value */
    errorFlag = getFloat64(doubleVal, pos);
    if (errorFlag.good())
    {
        /* ... and convert it to a character string */
        char buffer[64];
        OFStandard::ftoa(buffer, sizeof(buffer), doubleVal);
        /* assign result */
        stringVal = buffer;
    }
    return errorFlag;
}


// ********************************


OFCondition DcmFloatingPointDouble::putFloat64(const Float64 doubleVal,
                                               const unsigned long pos)
{
    Float64 val = doubleVal;
    errorFlag = changeValue(&val, sizeof(Float64) * pos, sizeof(Float64));
    return errorFlag;
}


OFCondition DcmFloatingPointDouble::putFloat64Array(const Float64 *doubleVals,
                                                    const unsigned long numDoubles)
{
    errorFlag = EC_Normal;
    if (numDoubles > 0)
    {
        /* check for valid data */
        if (doubleVals != NULL)
            errorFlag = putValue(doubleVals, sizeof(Float64) * (Uint32)numDoubles);
        else
            errorFlag = EC_CorruptedData;
    } else
        putValue(NULL, 0);

    return errorFlag;
}


// ********************************


OFCondition DcmFloatingPointDouble::putString(const char *stringVal)
{
    errorFlag = EC_Normal;
    /* check input string */
    if ((stringVal != NULL) && (strlen(stringVal) > 0))
    {
        const unsigned long vm = getVMFromString(stringVal);
        if (vm > 0)
        {
            Float64 *field = new Float64[vm];
            const char *s = stringVal;
            OFBool success = OFFalse;
            char *value;
            /* retrieve double data from character string */
            for (unsigned long i = 0; (i < vm) && errorFlag.good(); i++)
            {
                /* get first value stored in 's', set 's' to beginning of the next value */
                value = getFirstValueFromString(s);
                if (value != NULL)
                {
                    field[i] = OFStandard::atof(value, &success);
                    if (!success)
                        errorFlag = EC_CorruptedData;
                    delete[] value;
                } else
                    errorFlag = EC_CorruptedData;
            }
            /* set binary data as the element value */
            if (errorFlag == EC_Normal)
                errorFlag = putFloat64Array(field, vm);
            /* delete temporary buffer */
            delete[] field;
        } else
            putValue(NULL, 0);
    } else
        putValue(NULL,0);
    return errorFlag;
}


// ********************************


OFCondition DcmFloatingPointDouble::verify(const OFBool autocorrect)
{
    /* check for valid value length */
    if (Length % (sizeof(Float64)) != 0)
    {
        errorFlag = EC_CorruptedData;
        if (autocorrect)
        {
            /* strip to valid length */
            Length -= (Length % (sizeof(Float64)));
        }
    } else
        errorFlag = EC_Normal;
    return errorFlag;
}
