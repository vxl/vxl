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
 *  Purpose: Implementation of class DcmAttributeTag
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstream.h"
#include "dcvrat.h"
#include "dcvm.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "ofstdinc.h"


// ********************************


DcmAttributeTag::DcmAttributeTag(const DcmTag &tag,
                                 const Uint32 len)
  : DcmElement(tag, len)
{
}


DcmAttributeTag::DcmAttributeTag(const DcmAttributeTag &old)
  : DcmElement(old)
{
}


DcmAttributeTag::~DcmAttributeTag()
{
}


DcmAttributeTag &DcmAttributeTag::operator=(const DcmAttributeTag &obj)
{
    DcmElement::operator=(obj);
    return *this;
}


// ********************************


DcmEVR DcmAttributeTag::ident() const
{
    return EVR_AT;
}


unsigned long DcmAttributeTag::getVM()
{
    /* attribute tags store pairs of 16 bit values */
    return (unsigned long)(Length / (2 * sizeof(Uint16)));
}


// ********************************


void DcmAttributeTag::print(ostream &out,
                            const size_t flags,
                            const int level,
                            const char * /*pixelFileName*/,
                            size_t * /*pixelCounter*/)
{
    if (valueLoaded())
    {
        /* get unsigned integer data */
        Uint16 *uintVals;
        errorFlag = getUint16Array(uintVals);
        const unsigned long count = getVM();
        if ((uintVals != NULL) && (count > 0))
        {
            /* determine number of values to be printed */
            unsigned long expectedLength = count * (11 + 1) - 1;
            const unsigned long printCount =
                ((expectedLength > DCM_OptPrintLineLength) && (flags & DCMTypes::PF_shortenLongTagValues)) ?
                (DCM_OptPrintLineLength - 3 /* for "..." */ + 1) / (11 /* (gggg,eeee) */ + 1 /* for "\" */) : count;
            unsigned long printedLength = printCount * (11 + 1) - 1;
            /* print line start with tag and VR */
            printInfoLineStart(out, flags, level);
            /* print multiple values */
            if (printCount > 0)
            {
                out << hex << setfill('0');
                /* print tag values in hex mode */
                out << setw(4) << '(' << (*(uintVals)) << ',' << setw(4) << (*(uintVals+1)) << ')';
                uintVals+=2;

                for (unsigned long i = 1; i < printCount; i++)
                {
                  out << setw(4) << '(' << (*(uintVals)) << ',' << setw(4) << (*(uintVals+1)) << ')';
                  uintVals+=2;
                }
                /* reset i/o manipulators */
                out << dec << setfill(' ');
            }
            /* print trailing "..." if data has been truncated */
            if (printCount < count)
            {
                out << "...";
                printedLength += 3;
            }
            /* print line end with length, VM and tag name */
            printInfoLineEnd(out, flags, printedLength);
        } else
            printInfoLine(out, flags, level, "(no value available)");
    } else
        printInfoLine(out, flags, level, "(not loaded)");
}


// ********************************


OFCondition DcmAttributeTag::getTagVal(DcmTagKey &tagVal,
                                       const unsigned long pos)
{
    /* get unsigned integer data */
    Uint16 *uintValues;
    errorFlag = getUint16Array(uintValues);
    /* check data before returning */
    if (errorFlag.good())
    {
        if (uintValues == NULL)
            errorFlag = EC_IllegalCall;
        else if (pos >= getVM())
            errorFlag = EC_IllegalParameter;
        else
            tagVal.set(uintValues[2 * pos] /*group*/, uintValues[2 * pos + 1] /*element*/);
    }
    /* clear value in case of error */
    if (errorFlag.bad())
        tagVal = DcmTagKey();
    return errorFlag;
}


OFCondition DcmAttributeTag::getUint16Array(Uint16 *&uintVals)
{
    uintVals = (Uint16 *)getValue();
    return errorFlag;
}


// ********************************


OFCondition DcmAttributeTag::getOFString(OFString &stringVal,
                                         const unsigned long pos,
                                         OFBool /*normalize*/)
{
    DcmTagKey tagVal;
    /* get the specified tag value */
    errorFlag = getTagVal(tagVal, pos);
    if (errorFlag.good())
    {
        /* ... and convert it to a character string */
        char buffer[32];
        sprintf(buffer, "(%4.4x,%4.4x)", tagVal.getGroup(), tagVal.getElement());
        /* assign result */
        stringVal = buffer;
    }
    return errorFlag;
}


// ********************************


OFCondition DcmAttributeTag::putTagVal(const DcmTagKey &tagVal,
                                       const unsigned long pos)
{
    /* create tag data */
    Uint16 uintVals[2];
    uintVals[0] = tagVal.getGroup();
    uintVals[1] = tagVal.getElement();
    /* change element value */
    errorFlag = changeValue(uintVals, 2 * sizeof(Uint16) * (Uint32)pos, 2 * sizeof(Uint16));
    return errorFlag;
}


OFCondition DcmAttributeTag::putUint16Array(const Uint16 *uintVals,
                                            const unsigned long numUints)
{
    errorFlag = EC_Normal;
    if (numUints > 0)
    {
        /* check for valid data */
        if (uintVals != NULL)
            errorFlag = putValue(uintVals, 2 * sizeof(Uint16) * Uint32(numUints));
        else
            errorFlag = EC_CorruptedData;
    } else
        errorFlag = putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmAttributeTag::putString(const char *stringVal)
{
    errorFlag = EC_Normal;
    /* check input string */
    if ((stringVal != NULL) && (strlen(stringVal) > 0))
    {
        unsigned long vm = getVMFromString(stringVal);
        if (vm > 0)
        {
            Uint16 * field = new Uint16[2 * vm];
            const char *s = stringVal;
            char *value;
            /* retrieve attribute tag data from character string */
            for (unsigned long i = 0; (i < 2 * vm) && errorFlag.good(); i += 2)
            {
                /* get first value stored in 's', set 's' to beginning of the next value */
                value = getFirstValueFromString(s);
                if ((value == NULL) || sscanf(value, "(%hx,%hx)", &field[i], &field[i + 1]) != 2)
                    errorFlag = EC_CorruptedData;
                delete[] value;
            }
            /* set binary data as the element value */
            if (errorFlag.good())
                errorFlag = putUint16Array(field, vm);
            /* delete temporary buffer */
            delete[] field;
        } else
            putValue(NULL, 0);
    } else
        putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmAttributeTag::verify(const OFBool autocorrect)
{
    /* check for valid value length */
    if (Length % (2 * sizeof(Uint16)) != 0)
    {
        errorFlag = EC_CorruptedData;
        if (autocorrect)
        {
            /* strip to valid length */
            Length -= (Length % (2* sizeof(Uint16)));
        }
    } else
        errorFlag = EC_Normal;
    return errorFlag;
}
