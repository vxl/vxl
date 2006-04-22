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
 *  Purpose: Implementation of class DcmByteString
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstream.h"
#include "ofstring.h"
#include "ofstd.h"
#include "dcbytstr.h"
#include "dcvr.h"
#include "dcdebug.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "ofstdinc.h"


// ********************************


DcmByteString::DcmByteString(const DcmTag &tag,
                             const Uint32 len)
  : DcmElement(tag, len),
    paddingChar(' '),
    maxLength(DCM_UndefinedLength),
    realLength(len),
    fStringMode(DCM_UnknownString)
{
}


DcmByteString::DcmByteString(const DcmByteString &old)
  : DcmElement(old),
    paddingChar(old.paddingChar),
    maxLength(old.maxLength),
    realLength(old.realLength),
    fStringMode(old.fStringMode)
{
}


DcmByteString::~DcmByteString()
{
}


DcmByteString &DcmByteString::operator=(const DcmByteString &obj)
{
    DcmElement::operator=(obj);
    /* copy member variables */
    realLength = obj.realLength;
    fStringMode = obj.fStringMode;
    paddingChar = obj.paddingChar;
    maxLength = obj.maxLength;
    return *this;
}


// ********************************


DcmEVR DcmByteString::ident() const
{
    /* valid type identifier is set by derived classes */
    return EVR_UNKNOWN;
}


unsigned long DcmByteString::getVM()
{
    char *s = NULL;
    /* get stored string value */
    getString(s);
    unsigned long vm = 0;
    /*  check for empty string */
    if ((s == NULL) || (Length == 0))
        vm = 0;
    else
    {
        /* count number of delimiters */
        vm = 1;
        char c;
        while ((c = *s++) != 0)
        {
            if (c == '\\')
                vm++;
        }
    }
    return vm;
}


OFCondition DcmByteString::clear()
{
    /* call inherited method */
    errorFlag = DcmElement::clear();
    /* set string representation to unknown */
    fStringMode = DCM_UnknownString;
    return errorFlag;
}


Uint32 DcmByteString::getRealLength()
{
    /* convert string to internal representation (if required) */
    if (fStringMode != DCM_MachineString)
    {
        /* strips non-significant trailing spaces (padding) and determines 'realLength' */
        makeMachineByteString();
    }
    /* strig length of the internal representation */
    return realLength;
}


Uint32 DcmByteString::getLength(const E_TransferSyntax /*xfer*/,
                                const E_EncodingType /*enctype*/)
{
    /* convert string to DICOM representation, i.e. add padding if required */
    makeDicomByteString();
    /* DICOM value length is always an even number */
    return Length;
}


// ********************************


void DcmByteString::print(ostream &out,
                          const size_t flags,
                          const int level,
                          const char * /*pixelFileName*/,
                          size_t * /*pixelCounter*/)
{
    if (valueLoaded())
    {
        /* get string data */
        char *string = NULL;
        getString(string);
        if (string != NULL)
        {
            unsigned long printedLength = strlen(string) + 2 /* for enclosing brackets */;
            /* print line start with tag and VR */
            printInfoLineStart(out, flags, level);
            out << '[';
            /* check whether full value text should be printed */
            if ((flags & DCMTypes::PF_shortenLongTagValues) && (printedLength > DCM_OptPrintLineLength))
            {
                char output[DCM_OptPrintLineLength - 1 /* for "[" */ + 1];
                /* truncate value text and append "..." */
                OFStandard::strlcpy(output, string, (size_t)DCM_OptPrintLineLength - 4 /* for "[" and "..." */ + 1);
                OFStandard::strlcat(output, "...", (size_t)DCM_OptPrintLineLength - 1 /* for "[" */ + 1);
                out << output;
                printedLength = DCM_OptPrintLineLength;
            } else
                out << string << ']';
            /* print line end with length, VM and tag name */
            printInfoLineEnd(out, flags, printedLength);
        } else
            printInfoLine(out, flags, level, "(no value available)" );
    } else
        printInfoLine(out, flags, level, "(not loaded)" );
}


// ********************************


OFCondition DcmByteString::write(DcmOutputStream &outStream,
                                 const E_TransferSyntax writeXfer,
                                 const E_EncodingType encodingType)
{
    if (fTransferState == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        /* convert string value to DICOM representation and call inherited method */
        if (fTransferState == ERW_init)
            makeDicomByteString();
        errorFlag = DcmElement::write(outStream, writeXfer, encodingType);
    }
    return errorFlag;
}


OFCondition DcmByteString::writeSignatureFormat(DcmOutputStream &outStream,
                                                const E_TransferSyntax writeXfer,
                                                const E_EncodingType encodingType)
{
    if (fTransferState == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        /* convert string value to DICOM representation and call inherited method */
        if (fTransferState == ERW_init)
            makeDicomByteString();
        errorFlag = DcmElement::writeSignatureFormat(outStream, writeXfer, encodingType);
    }
    return errorFlag;
}


// ********************************


OFCondition DcmByteString::getOFString(OFString &stringVal,
                                       const unsigned long pos,
                                       OFBool /*normalize*/)
{
    errorFlag = EC_Normal;
    /* check given string position index */
    if (pos >= getVM())
        errorFlag = EC_IllegalParameter;
    else
    {
        /* get string data */
        char *s = (char *)getValue();
        /* extract specified string component */
        errorFlag = getStringPart(stringVal, s, pos);
    }
    return errorFlag;
}


OFCondition DcmByteString::getStringValue(OFString &stringVal)
{
    const char *s = (char *)getValue();
    /* check whether string value is present */
    if (s != NULL)
        stringVal = s;
    else {
        /* return empty string in case of empty value field */
        stringVal = "";
    }
    return errorFlag;
}


OFCondition DcmByteString::getString(char *&stringVal)
{
    /* get string data */
    stringVal = (char *)getValue();
    /* convert to internal string representation (without padding) if required */
    if ((stringVal != NULL) && (fStringMode != DCM_MachineString))
        makeMachineByteString();
    return errorFlag;
}


// ********************************


OFCondition DcmByteString::putString(const char *stringVal)
{
    errorFlag = EC_Normal;
    /* check for an empty string parameter */
    if ((stringVal != NULL) && (strlen(stringVal) > 0))
        putValue(stringVal, strlen(stringVal));
    else
        putValue(NULL, 0);
    /* make sure that extra padding is removed from the string */
    fStringMode = DCM_UnknownString;
    makeMachineByteString();
    return errorFlag;
}


OFCondition DcmByteString::putOFStringArray(const OFString &stringVal)
{
    /* sets the value of a complete (possibly multi-valued) string attribute */
    return putString(stringVal.c_str());
}


// ********************************


OFCondition DcmByteString::makeDicomByteString()
{
    /* get string data */
    char *value = NULL;
    errorFlag = getString(value);
    if (value != NULL)
    {
        /* check for odd length */
        if (realLength & 1)
        {
            /* if so add a padding character */
            Length = realLength + 1;
            value[realLength] = paddingChar;
        } else if (realLength < Length)
            Length = realLength;
        /* terminate string (removes additional trailing padding characters) */
        value[Length] = '\0';
    }
    /* current string representation is now the DICOM one */
    fStringMode = DCM_DicomString;
    return errorFlag;
}


OFCondition DcmByteString::makeMachineByteString()
{
    errorFlag = EC_Normal;
    /* get string data */
    char *value = (char *)getValue();
    /* determine initial string length */
    if (value != NULL)
    {
        realLength = strlen(value);
        /* remove all trailing spaces if automatic input data correction is enabled */
        if (dcmEnableAutomaticInputDataCorrection.get())
        {
            /*
            ** This code removes extra padding chars at the end of
            ** a ByteString.  Trailing padding can cause problems
            ** when comparing strings.
            */
            if (realLength > 0)
            {
                size_t i = 0;
                for (i = (size_t)realLength; (i > 0) && (value[i - 1] == paddingChar); i--)
                     value[i - 1] = '\0';
                realLength = (Uint32)i;
            }
        }
    } else
        realLength = 0;
    /* current string representation is now the internal one */
    fStringMode = DCM_MachineString;
    return errorFlag;
}


// ********************************


Uint8 *DcmByteString::newValueField()
{
    Uint8 *value = NULL;
    /* check for odd length (in case of a protocol error) */
    if (Length & 1)
    {
        /* allocate space for extra padding character (required for the DICOM representation of the string) */
        value = new Uint8[Length + 2];
        /* terminate string after real length */
        if (value != NULL)
            value[Length] = 0;
        /* enforce old (pre DCMTK 3.5.2) behaviour? */
        if (!dcmAcceptOddAttributeLength.get())
        {
            /* make length even */
            Length++;
        }
    } else {
        /* length is even */
        value = new Uint8[Length + 1];
    }
    /* make sure that the string is properly terminates by a 0 byte */
    if (value != NULL)
        value[Length] = 0;
    return value;
}


// ********************************


void DcmByteString::postLoadValue()
{
    /* initially, after loading an attribute the string mode is unknown */
    fStringMode = DCM_UnknownString;
    /* correct value length if automatic input data correction is enabled */
    if (dcmEnableAutomaticInputDataCorrection.get())
    {
        /* check for odd length */
        if (Length & 1)
        {
            // newValueField always allocates an even number of bytes and sets
            // the pad byte to zero, so we can safely increase Length here.
            Length++;
        }
    }
}


// ********************************


OFCondition DcmByteString::verify(const OFBool autocorrect)
{
    char *value = NULL;
    /* get string data */
    errorFlag = getString(value);
    /* check for non-empty string */
    if ((value != NULL) && (realLength != 0))
    {
        /* create a temporary buffer for the string value */
        char *tempstr = new char[realLength + 1];
        unsigned long field = 0;
        unsigned long num = getVM();
        unsigned long pos = 0;
        unsigned long temppos = 0;
        char c;
        /* check all string components */
        while (field < num )
        {
            unsigned long fieldlen = 0;
            /* check size limit for each string component */
            while (((c = value[pos++]) != 0) && (c != '\\'))
            {
                if ((fieldlen < maxLength) && autocorrect)
                    tempstr[temppos++] = c;
                fieldlen++;
            }
            if (fieldlen >= maxLength)
                errorFlag = EC_CorruptedData;
            /* 'c' is either '\\' or NULL */
            if (autocorrect)
                tempstr[temppos++] = c;
            field++;
            if (pos > Length)
                break;
        }
        /* replace current string value if auto correction is enabled */
        if (autocorrect)
            putString(tempstr);
        delete[] tempstr;
    }
    /* report a debug message if an error occurred */
    Cdebug(3, errorFlag.bad(),
            ("DcmByteString::verify: Illegal values in Tag=(0x%4.4x,0x%4.4x) VM=%d",
            getGTag(), getETag(), getVM() ));
    return errorFlag;
}


// ********************************


// global function to get a particular component of a DICOM string
OFCondition getStringPart(OFString &result,
                          const char *orgStr,
                          const unsigned long pos)
{
    OFCondition l_error = EC_Normal;
    /* check string parameter */
    if (orgStr != NULL)
    {
        /* search for beginning of specified string component  */
        unsigned long i = 0;
        while ((i < pos) && (*orgStr != '\0'))
        {
            if (*orgStr++ == '\\')
                i++;
        }
        /* if found ... */
        if (i == pos)
        {
            /* search for end of specified string component  */
            const char *t = orgStr;
            while ((*t != '\0') && (*t != '\\'))
                t++;
            /* check whether string component is non-empty */
            if (t - orgStr > 0)
                result.assign(orgStr, t - orgStr);
            else
                result = "";
        } else {
            /* specified component index not found in string */
            l_error = EC_IllegalParameter;
        }
    } else
        l_error = EC_IllegalParameter;
    return l_error;
}


// global function for normalizing a DICOM string
void normalizeString(OFString &string,
                     const OFBool multiPart,
                     const OFBool leading,
                     const OFBool trailing)
{
    /* check for non-empty string */
    if (!string.empty())
    {
        size_t partindex = 0;
        size_t offset = 0;
        size_t len = string.length();
        while (partindex < len)
        {
            // remove leading spaces in every part of the string
            if (leading)
            {
                offset = 0;
                while ((partindex + offset < len) && (string[partindex + offset] == ' '))
                    offset++;
                if (offset > 0)
                    string.erase(partindex, offset);
            }
            len = string.length();
            // compute begin to the next separator index!
            if (multiPart)
            {
                partindex = string.find('\\', partindex);
                if (partindex == OFString_npos)
                    partindex = len;
            } else
                partindex = len;
            // remove trailing spaces in every part of the string
            if (trailing && partindex)
            {
                offset = partindex - 1;
                while ((offset > 0) && (string[offset] == ' '))
                    offset--;
                if (offset != partindex - 1)
                {
                    if (string[offset] == ' ')
                    {
                        string.erase(offset, partindex - offset);
                        partindex = offset;
                    } else {
                        string.erase(offset+1, partindex - offset-1);
                        partindex = offset+1;
                    }
                }
            }
            len = string.length();
            if (partindex != len)
                ++partindex;
        }
    }
}
