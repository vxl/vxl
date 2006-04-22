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
 *  Purpose: Implementation of class DcmPixelSequence
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#include "ofstdinc.h"

#include "ofstream.h"
#include "dcpixseq.h"
#include "dcpxitem.h"
#include "dcitem.h"
#include "dcvr.h"
#include "dcdebug.h"

#include "dcdeftag.h"


// ********************************


DcmPixelSequence::DcmPixelSequence(const DcmTag &tag,
                                   const Uint32 len)
  : DcmSequenceOfItems(tag, len),
    Xfer(EXS_Unknown)
{
    Tag.setVR(EVR_OB);
}


DcmPixelSequence::DcmPixelSequence(const DcmPixelSequence &old)
  : DcmSequenceOfItems(old),
    Xfer(old.Xfer)
{
    /* everything gets handled in DcmSequenceOfItems constructor */
}


DcmPixelSequence::~DcmPixelSequence()
{
}


DcmPixelSequence &DcmPixelSequence::operator=(const DcmPixelSequence &obj)
{
    DcmSequenceOfItems::operator=(obj);
    Xfer = obj.Xfer;
    return *this;
}


// ********************************


void DcmPixelSequence::print(ostream &out,
                             const size_t flags,
                             const int level,
                             const char *pixelFileName,
                             size_t *pixelCounter)
{
    /* print pixel sequence start line */
    if (flags & DCMTypes::PF_showTreeStructure)
    {
        /* empty text */
        printInfoLine(out, flags, level);
        /* print pixel sequence content */
        if (!itemList->empty())
        {
            /* reset internal flags */
            const size_t newFlags = flags & ~DCMTypes::PF_lastEntry;
            /* print pixel items */
            DcmObject *dO;
            itemList->seek(ELP_first);
            do {
                dO = itemList->get();
                dO->print(out, newFlags, level + 1, pixelFileName, pixelCounter);
            } while (itemList->seek(ELP_next));
        }
    } else {
        OFOStringStream oss;
        oss << "(PixelSequence ";
        if (Length != DCM_UndefinedLength)
            oss << "with explicit length ";
        oss << "#=" << card() << ")" << OFStringStream_ends;
        OFSTRINGSTREAM_GETSTR(oss, tmpString)
        printInfoLine(out, flags, level, tmpString);
        OFSTRINGSTREAM_FREESTR(tmpString)
        /* print pixel sequence content */
        if (!itemList->empty())
        {
            DcmObject *dO;
            itemList->seek(ELP_first);
            do {
                dO = itemList->get();
                dO->print(out, flags, level + 1, pixelFileName, pixelCounter);
            } while (itemList->seek(ELP_next));
        }
        /* print pixel sequence end line */
        DcmTag delimItemTag(DCM_SequenceDelimitationItem);
        if (Length == DCM_UndefinedLength)
            printInfoLine(out, flags, level, "(SequenceDelimitationItem)", &delimItemTag);
        else
            printInfoLine(out, flags, level, "(SequenceDelimitationItem for re-enc.)", &delimItemTag);
    }
}


// ********************************


Uint32 DcmPixelSequence::calcElementLength(const E_TransferSyntax xfer,
                                           const E_EncodingType enctype)
{
    Uint32 seqlen = DcmElement::calcElementLength(xfer, enctype);
    if (Length == DCM_UndefinedLength)
        seqlen += 8;     // for Sequence Delimitation Tag
    return seqlen;
}


// ********************************


OFCondition DcmPixelSequence::makeSubObject(DcmObject *&subObject,
                                            const DcmTag &newTag,
                                            const Uint32 newLength)
{
    OFCondition l_error = EC_Normal;
    DcmObject *newObject = NULL;

    switch (newTag.getEVR())
    {
        case EVR_na:
            if (newTag.getXTag() == DCM_Item)
                newObject = new DcmPixelItem(newTag, newLength);
            else if (newTag.getXTag() == DCM_SequenceDelimitationItem)
                l_error = EC_SequEnd;
            else if (newTag.getXTag() == DCM_ItemDelimitationItem)
                l_error = EC_ItemEnd;
            else
                l_error = EC_InvalidTag;
            break;

        default:
            newObject = new DcmPixelItem(newTag, newLength);
            l_error = EC_CorruptedData;
            break;
    }

    subObject = newObject;
    return l_error;
}


// ********************************


OFCondition DcmPixelSequence::insert(DcmPixelItem *item,
                                     unsigned long where)
{
    errorFlag = EC_Normal;
    if (item != NULL)
    {
        itemList->seek_to(where);
        itemList->insert(item);
        Cdebug(3, where< itemList->card(), ("DcmPixelSequence::insert() item at position %d inserted", where));
        Cdebug(3, where>=itemList->card(), ("DcmPixelSequence::insert() item at last position inserted"));
    } else
        errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


OFCondition DcmPixelSequence::getItem(DcmPixelItem *&item,
                                      const unsigned long num)
{
    errorFlag = EC_Normal;
    item = (DcmPixelItem*)(itemList->seek_to(num));  // read item from list
    if (item == NULL)
        errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


OFCondition DcmPixelSequence::remove(DcmPixelItem *&item,
                                     const unsigned long num)
{
    errorFlag = EC_Normal;
    item = (DcmPixelItem*)(itemList->seek_to(num));  // read item from list
    if (item != (DcmPixelItem*)NULL)
        itemList->remove();
    else
        errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


OFCondition DcmPixelSequence::remove(DcmPixelItem *item)
{
    errorFlag = EC_IllegalCall;
    if (!itemList->empty() && item != NULL)
    {
        DcmObject *dO;
        itemList->seek(ELP_first);
        do {
            dO = itemList->get();
            if (dO == item)
            {
                itemList->remove();         // remove element from list, but do no delete it
                errorFlag = EC_Normal;
                break;
            }
        } while (itemList->seek(ELP_next));
    }
    return errorFlag;
}


// ********************************


OFCondition DcmPixelSequence::changeXfer(const E_TransferSyntax newXfer)
{
    if (Xfer == EXS_Unknown || canWriteXfer(newXfer, Xfer))
    {
        Xfer = newXfer;
        return EC_Normal;
    } else
        return EC_IllegalCall;
}


// ********************************


OFBool DcmPixelSequence::canWriteXfer(const E_TransferSyntax newXfer,
                                      const E_TransferSyntax oldXfer)
{
    DcmXfer newXferSyn(newXfer);

    return newXferSyn.isEncapsulated() && newXfer == oldXfer && oldXfer == Xfer;
}


// ********************************


OFCondition DcmPixelSequence::read(DcmInputStream &inStream,
                                   const E_TransferSyntax ixfer,
                                   const E_GrpLenEncoding glenc,
                                   const Uint32 maxReadLength)
{
    OFCondition l_error = changeXfer(ixfer);
    if (l_error.good())
        return DcmSequenceOfItems::read(inStream, ixfer, glenc, maxReadLength);
    else
        return l_error;
}


// ********************************


OFCondition DcmPixelSequence::write(DcmOutputStream &outStream,
                                      const E_TransferSyntax oxfer,
                                      const E_EncodingType /*enctype*/)
{
    OFCondition l_error = changeXfer(oxfer);
    if (l_error.good())
        return DcmSequenceOfItems::write(outStream, oxfer, EET_UndefinedLength);
    else return l_error;
}


// ********************************


OFCondition DcmPixelSequence::writeSignatureFormat(DcmOutputStream &outStream,
                                                   const E_TransferSyntax oxfer,
                                                   const E_EncodingType /*enctype*/)
{
    OFCondition l_error = changeXfer(oxfer);
    if (l_error.good())
        return DcmSequenceOfItems::writeSignatureFormat(outStream, oxfer, EET_UndefinedLength);
    else return l_error;
}


OFCondition DcmPixelSequence::storeCompressedFrame(DcmOffsetList &offsetList,
                                                   Uint8 *compressedData,
                                                   Uint32 compressedLen,
                                                   Uint32 fragmentSize)
{
    if (compressedData == NULL)
        return EC_IllegalCall;

    OFCondition result = EC_Normal;
    if (fragmentSize >= 0x400000)
        fragmentSize = 0;    // prevent overflow
    else
        fragmentSize <<= 10; // unit is kbytes
    if (fragmentSize == 0)
        fragmentSize = compressedLen;

    Uint32 offset = 0;
    Uint32 currentSize = 0;
    Uint32 numFragments = 0;
    DcmPixelItem *fragment = NULL;

    while ((offset < compressedLen) && (result.good()))
    {
        fragment = new DcmPixelItem(DcmTag(DCM_Item,EVR_OB));
        if (fragment == NULL)
            result = EC_MemoryExhausted;
        else
        {
            insert(fragment);
            numFragments++;
            currentSize = fragmentSize;
            if (offset + currentSize > compressedLen)
                currentSize = compressedLen - offset;
            result = fragment->putUint8Array(compressedData+offset, currentSize);
            if (result.good())
                offset += currentSize;
        }
    }

    currentSize = offset + (numFragments << 3); // 8 bytes extra for each item header
    offsetList.push_back(currentSize);
    return result;
}
