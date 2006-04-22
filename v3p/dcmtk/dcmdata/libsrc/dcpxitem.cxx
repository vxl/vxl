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
 *  Purpose: Implementation of class DcmPixelItem
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "ofstdinc.h"

#include "ofstream.h"
#include "dcpxitem.h"
#include "dcswap.h"
#include "ofstring.h"
#include "ofstd.h"
#include "dcistrma.h"    /* for class DcmInputStream */
#include "dcostrma.h"    /* for class DcmOutputStream */


// ********************************


DcmPixelItem::DcmPixelItem(const DcmTag &tag,
                           const Uint32 len)
  : DcmOtherByteOtherWord(tag, len)
{
    Tag.setVR(EVR_pixelItem);
}


DcmPixelItem::DcmPixelItem(const DcmPixelItem &old)
  : DcmOtherByteOtherWord(old)
{
}


DcmPixelItem::~DcmPixelItem()
{
}


// ********************************


OFCondition DcmPixelItem::writeTagAndLength(DcmOutputStream &outStream,
                                            const E_TransferSyntax oxfer,
                                            Uint32 &writtenBytes) const
{
    OFCondition l_error = outStream.status();
    if (l_error.good())
    {
        /* write tag information */
        l_error = writeTag(outStream, Tag, oxfer);
        writtenBytes = 4;
        /* prepare to write the value field */
        Uint32 valueLength = Length;
        DcmXfer outXfer(oxfer);
        /* check byte-ordering */
        const E_ByteOrder oByteOrder = outXfer.getByteOrder();
        if (oByteOrder == EBO_unknown)
        {
            return EC_IllegalCall;
        }
        swapIfNecessary(oByteOrder, gLocalByteOrder, &valueLength, 4, 4);
        // availability of four bytes space in output buffer
        // has been checked by caller.
        writtenBytes += outStream.write(&valueLength, 4);
    } else
        writtenBytes = 0;
    return l_error;
}


void DcmPixelItem::print(ostream &out,
                         const size_t flags,
                         const int level,
                         const char *pixelFileName,
                         size_t *pixelCounter)
{
    /* call inherited method */
    printPixel(out, flags, level, pixelFileName, pixelCounter);
}


OFCondition DcmPixelItem::createOffsetTable(const DcmOffsetList &offsetList)
{
    OFCondition result = EC_Normal;

    unsigned long numEntries = offsetList.size();
    if (numEntries > 0)
    {
        Uint32 current = 0;
        Uint32 *array = new Uint32[numEntries];
        if (array)
        {
            OFListIterator(Uint32) first = offsetList.begin();
            OFListIterator(Uint32) last = offsetList.end();
            unsigned long idx = 0;
            while (first != last)
            {
                array[idx++] = current;
                current += *first;
                ++first;
            }
            result = swapIfNecessary(EBO_LittleEndian, gLocalByteOrder,
                array, numEntries * sizeof(Uint32), sizeof(Uint32));
            if (result.good())
                result = putUint8Array((Uint8 *)array, numEntries * sizeof(Uint32));
            delete[] array;
        } else
            result = EC_MemoryExhausted;
    }
    return result;
}


OFCondition DcmPixelItem::writeXML(ostream &out,
                                   const size_t flags)
{
    /* XML start tag for "item" */
    out << "<pixel-item";
    /* value length in bytes = 0..max */
    out << " len=\"" << Length << "\"";
    /* value loaded = no (or absent)*/
    if (!valueLoaded())
        out << " loaded=\"no\"";
    /* pixel item contains binary data */
    if (!(flags & DCMTypes::XF_writeBinaryData))
        out << " binary=\"hidden\"";
    else if (flags & DCMTypes::XF_encodeBase64)
        out << " binary=\"base64\"";
    else
        out << " binary=\"yes\"";
    out << ">";
    /* write element value (if loaded) */
    if (valueLoaded() && (flags & DCMTypes::XF_writeBinaryData))
    {
        OFString value;
        /* encode binary data as Base64 */
        if (flags & DCMTypes::XF_encodeBase64)
        {
            /* pixel items always contain 8 bit data, therefore, byte swapping not required */
            out << OFStandard::encodeBase64((Uint8 *)getValue(), (size_t)Length, value);
        } else {
            /* encode as sequence of hexadecimal numbers */
            if (getOFStringArray(value).good())
                out << value;
        }
    }
    /* XML end tag for "item" */
    out << "</pixel-item>" << endl;
    /* always report success */
    return EC_Normal;
}
