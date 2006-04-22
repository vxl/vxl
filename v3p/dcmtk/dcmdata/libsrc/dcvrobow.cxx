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
 *  Purpose: Implementation of class DcmOtherByteOtherWord
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofstd.h"
#include "ofstream.h"
#include "dcvrobow.h"
#include "dcdeftag.h"
#include "dcswap.h"
#include "dcvm.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSTDLIB
#define INCLUDE_CSTRING
#include "ofstdinc.h"


// ********************************


DcmOtherByteOtherWord::DcmOtherByteOtherWord(const DcmTag &tag,
                                             const Uint32 len)
  : DcmElement(tag, len)
{
}


DcmOtherByteOtherWord::DcmOtherByteOtherWord(const DcmOtherByteOtherWord &old)
  : DcmElement(old)
{
}


DcmOtherByteOtherWord::~DcmOtherByteOtherWord()
{
}


DcmOtherByteOtherWord &DcmOtherByteOtherWord::operator=(const DcmOtherByteOtherWord &obj)
{
    DcmElement::operator=(obj);
    return *this;
}


// ********************************


DcmEVR DcmOtherByteOtherWord::ident() const
{
    return Tag.getEVR();
}


unsigned long DcmOtherByteOtherWord::getVM()
{
    /* value multiplicity for OB/OW is defined as 1 */
    return 1;
}


OFCondition DcmOtherByteOtherWord::setVR(DcmEVR vr)
{
    Tag.setVR(vr);
    return EC_Normal;
}


// ********************************


void DcmOtherByteOtherWord::print(ostream &out,
                                  const size_t flags,
                                  const int level,
                                  const char * /*pixelFileName*/,
                                  size_t * /*pixelCounter*/)
{
    if (valueLoaded())
    {
        const DcmEVR evr = Tag.getEVR();
        Uint16 *wordValues = NULL;
        Uint8 *byteValues = NULL;
        /* get 8 or 16 bit data respectively */
        if (evr == EVR_OW)
            errorFlag = getUint16Array(wordValues);
        else
            errorFlag = getUint8Array(byteValues);
        /* check data */
        if ((wordValues != NULL) || (byteValues != NULL))
        {
            /* determine number of values to be printed */
            const unsigned int vrSize = (evr == EVR_OW) ? 4 : 2;
            const unsigned long count = (evr == EVR_OW) ? (Length / 2) : Length;
            unsigned long expectedLength = count * (vrSize + 1) - 1;
            const unsigned long printCount =
                ((expectedLength > DCM_OptPrintLineLength) && (flags & DCMTypes::PF_shortenLongTagValues)) ?
                (DCM_OptPrintLineLength - 3 /* for "..." */ + 1 /* for last "\" */) / (vrSize + 1) : count;
            unsigned long printedLength = printCount * (vrSize + 1) - 1;
            /* print line start with tag and VR */
            printInfoLineStart(out, flags, level);
            /* print multiple values */
            if (printCount > 0)
            {
                out << hex << setfill('0');
                if (evr == EVR_OW)
                {
                    /* print word values in hex mode */
                    out << setw(vrSize) << (*(wordValues++));
                    for (unsigned long i = 1; i < printCount; i++)
                        out << "\\" << setw(vrSize) << (*(wordValues++));
                } else {
                    /* print byte values in hex mode */
                    out << setw(vrSize) << (int)(*(byteValues++));
                    for (unsigned long i = 1; i < printCount; i++)
                        out << "\\" << setw(vrSize) << (int)(*(byteValues++));
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


void DcmOtherByteOtherWord::printPixel(ostream &out,
                                       const size_t flags,
                                       const int level,
                                       const char *pixelFileName,
                                       size_t *pixelCounter)
{
    if (pixelFileName != NULL)
    {
        /* create filename for pixel data file */
        OFString fname = pixelFileName;
        fname += ".";
        if (pixelCounter != NULL)
        {
            char num[20];
            sprintf(num, "%ld", (long)((*pixelCounter)++));
            fname += num;
        }
        fname += ".raw";
        /* create reference to pixel data file in dump output */
        OFString str = "=";
        str += fname;
        printInfoLine(out, flags, level, str.c_str());
        /* check whether pixel data file already exists */
        if (!OFStandard::fileExists(fname))
        {
            /* create binary file for pixel data */
            FILE *file = fopen(fname.c_str(), "wb");
            if (file != NULL)
            {
                if (Tag.getEVR() == EVR_OW)
                {
                    /* write 16 bit data in little endian byte-order */
                    Uint16 *data = NULL;
                    getUint16Array(data);
                    if (data != NULL)
                    {
                        swapIfNecessary(EBO_LittleEndian, gLocalByteOrder, data, Length, sizeof(Uint16));
                        fwrite(data, sizeof(Uint16), (size_t)(Length / sizeof(Uint16)), file);
                        swapIfNecessary(gLocalByteOrder, EBO_LittleEndian, data, Length, sizeof(Uint16));
                    }
                } else {
                    Uint8 *data = NULL;
                    getUint8Array(data);
                    if (data != NULL)
                        fwrite(data, sizeof(Uint8), (size_t)Length, file);
                }
                fclose(file);
            } else {
                ofConsole.lockCerr() << "Warning: can't open output file for pixel data: " << fname << endl;
                ofConsole.unlockCerr();
            }
        } else {
            ofConsole.lockCerr() << "Warning: output file for pixel data already exists: " << fname << endl;
            ofConsole.unlockCerr();
        }
    } else
        DcmOtherByteOtherWord::print(out, flags, level, pixelFileName, pixelCounter);
}


// ********************************


OFCondition DcmOtherByteOtherWord::alignValue()
{
    errorFlag = EC_Normal;
    /* add padding byte in case of 8 bit data */
    if ((Tag.getEVR() != EVR_OW) && (Length > 0))
    {
        Uint8 *bytes = NULL;
        bytes = (Uint8 *)getValue(fByteOrder);
        /* check for odd length */
        if ((bytes != NULL) && ((Length & 1) != 0))
        {
            bytes[Length] = 0;
            Length++;
        }
    }
    return errorFlag;
}


void DcmOtherByteOtherWord::postLoadValue()
{
    if (dcmEnableAutomaticInputDataCorrection.get())
        alignValue();
}


// ********************************


OFCondition DcmOtherByteOtherWord::putUint8Array(const Uint8 *byteValue,
                                                 const unsigned long numBytes)
{
    errorFlag = EC_Normal;
    if (numBytes > 0)
    {
        /* check for valid 8 bit data */
        if ((byteValue != NULL) && (Tag.getEVR() != EVR_OW))
        {
            errorFlag = putValue(byteValue, sizeof(Uint8) * (Uint32)numBytes);
            alignValue();
        } else
            errorFlag = EC_CorruptedData;
    } else
        putValue(NULL, 0);
    return errorFlag;
}


OFCondition DcmOtherByteOtherWord::putUint16Array(const Uint16 *wordValue,
                                                  const unsigned long numWords)
{
    errorFlag = EC_Normal;
    if (numWords > 0)
    {
        /* check for valid 16 bit data */
        if ((wordValue != NULL) && (Tag.getEVR() == EVR_OW))
            errorFlag = putValue(wordValue, sizeof(Uint16) * (Uint32)numWords);
        else
            errorFlag = EC_CorruptedData;
    }
    else
        errorFlag = putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmOtherByteOtherWord::putString(const char *stringVal)
{
    errorFlag = EC_Normal;
    /* check input string */
    if ((stringVal != NULL) && (strlen(stringVal) > 0))
    {
        unsigned long vm = getVMFromString(stringVal);
        if (vm > 0)
        {
            const DcmEVR evr = Tag.getEVR();
            Uint8 *byteField = NULL;
            Uint16 *wordField = NULL;
            /* create new value field */
            if (evr == EVR_OW)
                wordField = new Uint16[vm];
            else
                byteField = new Uint8[vm];
            const char *s = stringVal;
            Uint16 intVal = 0;
            char *value;
            /* retrieve binary data from hexa-decimal string */
            for (unsigned long i = 0; (i < vm) && errorFlag.good(); i++)
            {
                /* get first value stored in 's', set 's' to beginning of the next value */
                value = getFirstValueFromString(s);
                if (value != NULL)
                {
                    /* integer overflow is currently not checked! */
                    if (sscanf(value, "%hx", &intVal) != 1)
                        errorFlag = EC_CorruptedData;
                    else if (evr == EVR_OW)
                        wordField[i] = (Uint16)intVal;
                    else
                        byteField[i] = (Uint8)intVal;
                    delete[] value;
                } else
                    errorFlag = EC_CorruptedData;
            }
            /* set binary data as the element value */
            if (errorFlag.good())
            {
                if (evr == EVR_OW)
                    errorFlag = putUint16Array(wordField, vm);
                else
                    errorFlag = putUint8Array(byteField, vm);
            }
            /* delete temporary buffers */
            delete[] byteField;
            delete[] wordField;
        } else
            putValue(NULL, 0);
    } else
        putValue(NULL, 0);
    return errorFlag;
}


// ********************************


OFCondition DcmOtherByteOtherWord::getUint8(Uint8 &byteVal,
                                            const unsigned long pos)
{
    /* get 8 bit data */
    Uint8 *uintValues = NULL;
    errorFlag = getUint8Array(uintValues);
    /* check data before returning */
    if (errorFlag.good())
    {
        if (uintValues == NULL)
            errorFlag = EC_IllegalCall;
        else if (pos >= getLength() /*bytes*/)
            errorFlag = EC_IllegalParameter;
        else
            byteVal = uintValues[pos];
    }
    /* clear value in case of error */
    if (errorFlag.bad())
        byteVal = 0;
    return errorFlag;
}


OFCondition DcmOtherByteOtherWord::getUint8Array(Uint8 *&byteVals)
{
    errorFlag = EC_Normal;
    if (Tag.getEVR() != EVR_OW)
        byteVals = (Uint8 *)getValue();
    else
        errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


OFCondition DcmOtherByteOtherWord::getUint16(Uint16 &wordVal,
                                             const unsigned long pos)
{
    Uint16 *uintValues = NULL;
    errorFlag = getUint16Array(uintValues);
    /* check data before returning */
    if (errorFlag.good())
    {
        if (uintValues == NULL)
            errorFlag = EC_IllegalCall;
        else if (pos >= getLength() / sizeof(Uint16) /*words*/)
            errorFlag = EC_IllegalParameter;
        else
            wordVal = uintValues[pos];
    }
    /* clear value in case of error */
    if (errorFlag.bad())
        wordVal = 0;
    return errorFlag;
}


OFCondition DcmOtherByteOtherWord::getUint16Array(Uint16 *&wordVals)
{
    errorFlag = EC_Normal;
    if (Tag.getEVR() == EVR_OW)
        wordVals = (Uint16 *)getValue();
    else
        errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


OFCondition DcmOtherByteOtherWord::getOFString(OFString &stringVal,
                                               const unsigned long pos,
                                               OFBool /*normalize*/)
{
    if (Tag.getEVR() == EVR_OW)
    {
        Uint16 uint16Val;
        /* get the specified numeric value (16 bit) */
        errorFlag = getUint16(uint16Val, pos);
        if (errorFlag.good())
        {
            /* ... and convert it to a character string (hex mode) */
            char buffer[32];
            sprintf(buffer, "%4.4hx", uint16Val);
            /* assign result */
            stringVal = buffer;
        }
    } else {
        Uint8 uint8Val;
        /* get the specified numeric value (8 bit) */
        errorFlag = getUint8(uint8Val, pos);
        if (errorFlag.good())
        {
            /* ... and convert it to a character string (hex mode) */
            char buffer[32];
            sprintf(buffer, "%2.2hx", uint8Val);
            /* assign result */
            stringVal = buffer;
        }
    }
    return errorFlag;
}


OFCondition DcmOtherByteOtherWord::getOFStringArray(OFString &stringVal,
                                                    OFBool /*normalize*/)
{
    if (Tag.getEVR() == EVR_OW)
    {
        /* get array of 16 bit values */
        Uint16 *uint16Vals = (Uint16 *)getValue();
        const size_t count = (size_t)(getLength() / sizeof(Uint16));
        if ((uint16Vals != NULL) && (count > 0))
        {
            /* reserve number of bytes expected */
            stringVal.reserve(5 * count);
            char *bufPtr = &stringVal[0];
            /* for all array elements ... */
            for (size_t i = 0; i < count; i++)
            {
                /* ... convert numeric value to hexadecimal string representation */
                sprintf(bufPtr, "%4.4hx\\", uint16Vals[i]);
                bufPtr += 5;
            }
            /* remove last '\' */
            *(--bufPtr) = '\0';
            errorFlag = EC_Normal;
        } else
            errorFlag = EC_IllegalCall;
    } else {
        /* get array of 8 bit values */
        Uint8 *uint8Vals = (Uint8 *)getValue();
        const size_t count = (size_t)getLength();
        if ((uint8Vals != NULL) && (count > 0))
        {
            /* reserve number of bytes expected */
            stringVal.reserve(3 * count);
            char *bufPtr = &stringVal[0];
            /* for all array elements ... */
            for (size_t i = 0; i < count; i++)
            {
                /* ... convert numeric value to hexadecimal string representation */
                sprintf(bufPtr, "%2.2hx\\", uint8Vals[i]);
                bufPtr += 3;
            }
            /* remove last '\' */
            *(--bufPtr) = '\0';
            errorFlag = EC_Normal;
        } else
            errorFlag = EC_IllegalCall;
    }
    return errorFlag;
}


// ********************************


OFCondition DcmOtherByteOtherWord::verify(const OFBool autocorrect)
{
    errorFlag = EC_Normal;
    if (autocorrect)
        errorFlag = alignValue();
    return errorFlag;
}


// ********************************


OFBool DcmOtherByteOtherWord::canWriteXfer(const E_TransferSyntax newXfer,
                                           const E_TransferSyntax /*oldXfer*/)
{
    DcmXfer newXferSyn(newXfer);
    return (Tag != DCM_PixelData) || !newXferSyn.isEncapsulated();
}


// ********************************


OFCondition DcmOtherByteOtherWord::write(DcmOutputStream &outStream,
                                         const E_TransferSyntax oxfer,
                                         const E_EncodingType enctype)
{
    if (fTransferState == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        if (fTransferState == ERW_init) alignValue();
        /* call inherited method */
        errorFlag = DcmElement::write(outStream, oxfer, enctype);
    }
    return errorFlag;
}


OFCondition DcmOtherByteOtherWord::writeSignatureFormat(DcmOutputStream &outStream,
                                                        const E_TransferSyntax oxfer,
                                                        const E_EncodingType enctype)
{
    if (fTransferState == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        if (fTransferState == ERW_init) alignValue();
        /* call inherited method */
        errorFlag = DcmElement::writeSignatureFormat(outStream, oxfer, enctype);
    }
    return errorFlag;
}


// ********************************


OFCondition DcmOtherByteOtherWord::writeXML(ostream &out,
                                            const size_t flags)
{
    /* XML start tag: <element tag="gggg,eeee" vr="XX" ...> */
    if (!(flags & DCMTypes::XF_writeBinaryData))
        writeXMLStartTag(out, flags, "binary=\"hidden\"");
    else if (flags & DCMTypes::XF_encodeBase64)
        writeXMLStartTag(out, flags, "binary=\"base64\"");
    else
        writeXMLStartTag(out, flags, "binary=\"yes\"");
    /* write element value (if loaded) */
    if (valueLoaded() && (flags & DCMTypes::XF_writeBinaryData))
    {
        OFString value;
        /* encode binary data as Base64 */
        if (flags & DCMTypes::XF_encodeBase64)
        {
            Uint8 *byteValues = (Uint8 *)getValue();
            if (Tag.getEVR() == EVR_OW)
            {
                /* Base64 encoder requires big endian input data */
                swapIfNecessary(gLocalByteOrder, EBO_BigEndian, byteValues, Length, sizeof(Uint16));
            }
            out << OFStandard::encodeBase64(byteValues, (size_t)Length, value);
        } else {
            /* encode as sequence of hexadecimal numbers */
            if (getOFStringArray(value).good())
                out << value;
        }
    }
    /* XML end tag: </element> */
    writeXMLEndTag(out, flags);
    /* always report success */
    return EC_Normal;
}
