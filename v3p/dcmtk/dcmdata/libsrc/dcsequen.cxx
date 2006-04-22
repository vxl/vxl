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
 *  Purpose: Implementation of class DcmSequenceOfItems
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#include "ofstdinc.h"

#include "ofstream.h"
#include "ofstd.h"

#include "dcsequen.h"
#include "dcitem.h"
#include "dcdirrec.h"
#include "dcvr.h"
#include "dcpxitem.h"
#include "dcswap.h"
#include "dcdebug.h"
#include "dcmetinf.h"
#include "dcdeftag.h"
#include "dcistrma.h"    /* for class DcmInputStream */
#include "dcostrma.h"    /* for class DcmOutputStream */


// ********************************


DcmSequenceOfItems::DcmSequenceOfItems(const DcmTag &tag,
                                       const Uint32 len)
  : DcmElement(tag, len),
    itemList(NULL),
    lastItemComplete(OFTrue),
    fStartPosition(0)
{
    itemList = new DcmList;
}


// ********************************


DcmSequenceOfItems::DcmSequenceOfItems(const DcmSequenceOfItems &old)
  : DcmElement(old),
    itemList(NULL),
    lastItemComplete(OFTrue),
    fStartPosition(old.fStartPosition)
{
    itemList = new DcmList;

    switch (old.ident())
    {
        case EVR_SQ:
        case EVR_pixelSQ:
        case EVR_fileFormat:
            if (!old.itemList->empty())
            {
                DcmObject *oldDO;
                DcmObject *newDO;
                itemList->seek(ELP_first);
                old.itemList->seek(ELP_first);
                do {
                    oldDO = old.itemList->get();
                    switch (oldDO->ident())
                    {
                        case EVR_item:
                            newDO = new DcmItem(*(DcmItem*)oldDO);
                            break;
                        case EVR_pixelItem:
                            newDO = new DcmPixelItem(*(DcmPixelItem*)oldDO);
                            break;
                        case EVR_metainfo:
                            newDO = new DcmMetaInfo(*(DcmMetaInfo*)oldDO);
                            break;
                        case EVR_dataset:
                            newDO = new DcmDataset(*(DcmDataset*)oldDO);
                            break;
                        default:
                            newDO = new DcmItem(oldDO->getTag());
                            ofConsole.lockCerr() << "Error: DcmSequenceOfItems(): Element("
                                                 << hex << setfill('0')
                                                 << setw(4) << oldDO->getGTag() << ","
                                                 << setw(4) << oldDO->getETag()
                                                 << dec << setfill(' ')
                                                 << ") found, which was not an Item" << endl;
                            ofConsole.unlockCerr();
                            break;
                    }

                    itemList->insert(newDO, ELP_next);
                } while (old.itemList->seek(ELP_next));
            }
            break;
        default:
            // wrong use of copy constructor, should never happen
            break;
    }
}


// ********************************


DcmSequenceOfItems::~DcmSequenceOfItems()
{
    DcmObject *dO;
    itemList->seek(ELP_first);
    while (!itemList->empty())
    {
        dO = itemList->remove();
        if (dO != (DcmObject*)NULL)
            delete dO;
    }
    delete itemList;
}


// ********************************


DcmSequenceOfItems &DcmSequenceOfItems::operator=(const DcmSequenceOfItems &obj)
{
    DcmElement::operator=(obj);
    lastItemComplete = obj.lastItemComplete;
    fStartPosition = obj.fStartPosition;

    DcmList *newList = new DcmList; // DcmList has no copy constructor. Need to copy ourselves.
    if (newList)
    {
        switch (obj.ident())
        {
            case EVR_SQ:
            case EVR_pixelSQ:
            case EVR_fileFormat:
                if (!obj.itemList->empty())
                {
                    DcmObject *oldDO;
                    DcmObject *newDO;
                    newList->seek(ELP_first);
                    obj.itemList->seek(ELP_first);
                    do {
                        oldDO = obj.itemList->get();
                        switch (oldDO->ident())
                        {
                            case EVR_item:
                                newDO = new DcmItem(*(DcmItem*)oldDO);
                                break;
                            case EVR_pixelItem:
                                newDO = new DcmPixelItem(*(DcmPixelItem*)oldDO);
                                break;
                            case EVR_metainfo:
                                newDO = new DcmMetaInfo(*(DcmMetaInfo*)oldDO);
                                break;
                            case EVR_dataset:
                                newDO = new DcmDataset(*(DcmDataset*)oldDO);
                                break;
                            default:
                                newDO = new DcmItem(oldDO->getTag());
                                ofConsole.lockCerr() << "Error: DcmSequenceOfItems(): Element("
                                                     << hex << oldDO->getGTag() << "," << oldDO->getETag()
                                                     << dec << ") found, which was not an Item" << endl;
                                ofConsole.unlockCerr();
                                break;
                        }
                        newList->insert(newDO, ELP_next);
                    } while (obj.itemList->seek(ELP_next));
                }
                break;
            default:
                // wrong use of assignment operator, should never happen
                break;
        }
    }
    delete itemList;
    itemList = newList;

    return *this;
}


// ********************************


void DcmSequenceOfItems::print(ostream &out,
                               const size_t flags,
                               const int level,
                               const char *pixelFileName,
                               size_t *pixelCounter)
{
    /* print sequence start line */
    if (flags & DCMTypes::PF_showTreeStructure)
    {
        /* empty text */
        printInfoLine(out, flags, level);
        /* print sequence content */
        if (!itemList->empty())
        {
            /* reset internal flags */
            const size_t newFlags = flags & ~DCMTypes::PF_lastEntry;
            /* print all items contained in the sequence */
            DcmObject *dO;
            itemList->seek(ELP_first);
            do {
                dO = itemList->get();
                dO->print(out, newFlags, level + 1, pixelFileName, pixelCounter);
            } while (itemList->seek(ELP_next));
        }
    } else {
        OFOStringStream oss;
        oss << "(Sequence ";
        if (Length == DCM_UndefinedLength)
            oss << "undefined";
        else
            oss << "explicit";
        oss << " length #=" << card() << ")" << OFStringStream_ends;
        OFSTRINGSTREAM_GETSTR(oss, tmpString)
        printInfoLine(out, flags, level, tmpString);
        OFSTRINGSTREAM_FREESTR(tmpString)
        /* print sequence content */
        if (!itemList->empty())
        {
            DcmObject *dO;
            itemList->seek(ELP_first);
            do {
                dO = itemList->get();
                dO->print(out, flags, level + 1, pixelFileName, pixelCounter);
            } while (itemList->seek(ELP_next));
        }
        /* print sequence end line */
        DcmTag delimItemTag(DCM_SequenceDelimitationItem);
        if (Length == DCM_UndefinedLength)
            printInfoLine(out, flags, level, "(SequenceDelimitationItem)", &delimItemTag);
        else
            printInfoLine(out, flags, level, "(SequenceDelimitationItem for re-encod.)", &delimItemTag);
    }
}


// ********************************


OFCondition DcmSequenceOfItems::writeXML(ostream &out,
                                         const size_t flags)
{
    OFString xmlString;
    DcmVR vr(Tag.getVR());
    /* XML start tag for "sequence" */
    out << "<sequence";
    /* attribute tag = (gggg,eeee) */
    out << " tag=\"";
    out << hex << setfill('0')
        << setw(4) << Tag.getGTag() << ","
        << setw(4) << Tag.getETag() << "\""
        << dec << setfill(' ');
    /* value representation = VR */
    out << " vr=\"" << vr.getVRName() << "\"";
    /* cardinality (number of items) = 1..n */
    out << " card=\"" << card() << "\"";
    /* value length in bytes = 0..max (if not undefined) */
    if (Length != DCM_UndefinedLength)
        out << " len=\"" << Length << "\"";
    /* tag name (if known) */
    out << " name=\"" << OFStandard::convertToMarkupString(Tag.getTagName(), xmlString) << "\"";
    out << ">" << endl;
    /* write sequence content */
    if (!itemList->empty())
    {
        /* write content of all children */
        DcmObject *dO;
        itemList->seek(ELP_first);
        do
        {
            dO = itemList->get();
            dO->writeXML(out, flags);
        } while (itemList->seek(ELP_next));
    }
    /* XML end tag for "sequence" */
    out << "</sequence>" << endl;
    /* always report success */
    return EC_Normal;
}


// ********************************


OFBool DcmSequenceOfItems::canWriteXfer(const E_TransferSyntax newXfer,
                                        const E_TransferSyntax oldXfer)
{
    OFBool canWrite = OFTrue;

    if (newXfer == EXS_Unknown)
        canWrite = OFFalse;
    else if (!itemList->empty())
    {
        DcmObject *dO;
        itemList->seek(ELP_first);
        do
        {
            dO = itemList->get();
            canWrite = dO -> canWriteXfer(newXfer, oldXfer);
        } while (itemList->seek(ELP_next) && canWrite);
    }

    return canWrite;
}


// ********************************


Uint32 DcmSequenceOfItems::calcElementLength(const E_TransferSyntax xfer,
                                             const E_EncodingType enctype)
{
    Uint32 seqlen = DcmElement::calcElementLength(xfer, enctype);
    if (enctype == EET_UndefinedLength)
        seqlen += 8;     // for Sequence Delimitation Tag
    return seqlen;
}


// ********************************


Uint32 DcmSequenceOfItems::getLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype)
{
    Uint32 seqlen = 0;
    if (!itemList->empty())
    {
        DcmItem *dI;
        itemList->seek(ELP_first);
        do {
            dI = (DcmItem*)(itemList->get());
            seqlen += dI->calcElementLength(xfer, enctype);
        } while (itemList->seek(ELP_next));
    }
    return seqlen;
}


// ********************************


OFCondition DcmSequenceOfItems::computeGroupLengthAndPadding(const E_GrpLenEncoding glenc,
                                                             const E_PaddingEncoding padenc,
                                                             const E_TransferSyntax xfer,
                                                             const E_EncodingType enctype,
                                                             const Uint32 padlen,
                                                             const Uint32 subPadlen,
                                                             Uint32 instanceLength)
{
    OFCondition l_error = EC_Normal;

    if (!itemList->empty())
    {
        itemList->seek(ELP_first);
        do {
            DcmItem *dO = (DcmItem*)itemList->get();
            l_error = dO->computeGroupLengthAndPadding
                (glenc, padenc, xfer, enctype, padlen,
                 subPadlen, instanceLength);
        } while (itemList->seek(ELP_next));
    }
    return l_error;
}


// ********************************


OFCondition DcmSequenceOfItems::makeSubObject(DcmObject *&subObject,
                                              const DcmTag &newTag,
                                              const Uint32 newLength)
{
    OFCondition l_error = EC_Normal;
    DcmItem *subItem = NULL;

    switch (newTag.getEVR())
    {
        case EVR_na:
            if (newTag.getXTag() == DCM_Item)
            {
                if (getTag().getXTag() == DCM_DirectoryRecordSequence)
                    subItem = new DcmDirectoryRecord(newTag, newLength);
                else
                    subItem = new DcmItem(newTag, newLength);
            }
            else if (newTag.getXTag() == DCM_SequenceDelimitationItem)
                l_error = EC_SequEnd;
            else if (newTag.getXTag() == DCM_ItemDelimitationItem)
                l_error = EC_ItemEnd;
            else
                l_error = EC_InvalidTag;
            break;

        default:
            subItem = new DcmItem(newTag, newLength);
            l_error = EC_CorruptedData;
            break;
    }
    subObject = subItem;
    return l_error;
}


// ********************************


OFCondition DcmSequenceOfItems::readTagAndLength(DcmInputStream &inStream,
                                                 const E_TransferSyntax xfer,
                                                 DcmTag &tag,
                                                 Uint32 &length)
{
    Uint16 groupTag = 0xffff;
    Uint16 elementTag = 0xffff;

    OFCondition l_error = EC_Normal;
    if (inStream.avail() < 8)
        l_error = EC_StreamNotifyClient;

    if (l_error.good())
    {
        DcmXfer iXfer(xfer);
        const E_ByteOrder iByteOrder = iXfer.getByteOrder();
        if (iByteOrder == EBO_unknown)
            return EC_IllegalCall;
        inStream.mark();
        inStream.read(&groupTag, 2);
        inStream.read(&elementTag, 2);
        swapIfNecessary(gLocalByteOrder, iByteOrder, &groupTag, 2, 2);
        swapIfNecessary(gLocalByteOrder, iByteOrder, &elementTag, 2, 2);
        // Tag ist gelesen

        DcmTag newTag(groupTag, elementTag);

        Uint32 valueLength = 0;
        inStream.read(&valueLength, 4);
        swapIfNecessary(gLocalByteOrder, iByteOrder, &valueLength, 4, 4);
        if ((valueLength & 1)&&(valueLength != (Uint32) -1))
        {
            ofConsole.lockCerr() << "Warning: parse error in DICOM object: length of item in sequence " << Tag << " is odd" << endl;
            ofConsole.unlockCerr();
        }
        length = valueLength;
        tag = newTag; // return value: assignment-operator
    }

    debug(4, ("in Sequ.readTag errorFlag = %s", l_error.text()));
    return l_error;
}


// ********************************


OFCondition DcmSequenceOfItems::readSubItem(DcmInputStream &inStream,
                                            const DcmTag &newTag,
                                            const Uint32 newLength,
                                            const E_TransferSyntax xfer,
                                            const E_GrpLenEncoding glenc,
                                            const Uint32 maxReadLength)
{
    // For DcmSequenceOfItems, subObject is always inherited from DcmItem
    // For DcmPixelSequence, subObject is always inherited from DcmPixelItem
    DcmObject * subObject = NULL;
    OFCondition l_error = makeSubObject(subObject, newTag, newLength);
    if (l_error.good() && subObject != NULL)
    {
        // inStream.UnsetPutbackMark(); // not needed anymore with new stream architecture
        itemList->insert(subObject, ELP_next);
        l_error = subObject->read(inStream, xfer, glenc, maxReadLength); // read sub-item
        return l_error; // prevent subObject from getting deleted
    }
    else if (l_error == EC_InvalidTag)  // try to recover parsing
    {
        inStream.putback();
        ofConsole.lockCerr() << "Warning: DcmSequenceOfItems::readSubItem(): parse error occurred: " << newTag << endl;
        ofConsole.unlockCerr();
        debug(1, ("Warning: DcmSequenceOfItems::readSubItem(): parse error occurred:"
            " (0x%4.4hx,0x%4.4hx)", newTag.getGTag(), newTag.getETag()));
    }
    else if (l_error != EC_SequEnd)
    {
        // inStream.UnsetPutbackMark(); // not needed anymore with new stream architecture
        ofConsole.lockCerr() << "Error: DcmSequenceOfItems::readSubItem(): cannot create SubItem " << newTag << endl;
        ofConsole.unlockCerr();
        debug(1, ("Error: DcmSequenceOfItems::readSubItem(): cannot create SubItem"
            " (0x%4.4hx,0x%4.4hx)", newTag.getGTag(), newTag.getETag()));
    } else {
        // inStream.UnsetPutbackMark(); // not needed anymore with new stream architecture
    }

    if (subObject) delete subObject; // only executed if makeSubObject() has returned an error
    return l_error;
}


// ********************************


OFCondition DcmSequenceOfItems::read(DcmInputStream &inStream,
                                     const E_TransferSyntax xfer,
                                     const E_GrpLenEncoding glenc,
                                     const Uint32 maxReadLength)
{
    if (fTransferState == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        errorFlag = inStream.status();

        if (errorFlag.good() && inStream.eos())
            errorFlag = EC_EndOfStream;
        else if (errorFlag.good() && fTransferState != ERW_ready)
        {
            if (fTransferState == ERW_init)
            {
                fStartPosition = inStream.tell();   // Position Sequence-Value
                fTransferState = ERW_inWork;
            }

            itemList->seek(ELP_last); // append data at end
            while (inStream.good() && (fTransferredBytes < Length || !lastItemComplete))
            {
                DcmTag newTag;
                Uint32 newValueLength = 0;

                if (lastItemComplete)
                {
                    errorFlag = readTagAndLength(inStream, xfer, newTag, newValueLength);

                    if (errorFlag.bad())
                        break;                  // finish while loop
                    else
                        fTransferredBytes += 8;

                    lastItemComplete = OFFalse;
                    errorFlag = readSubItem(inStream, newTag, newValueLength,
                                            xfer, glenc, maxReadLength);
                    if (errorFlag.good())
                        lastItemComplete = OFTrue;
                }
                else
                {
                    errorFlag = itemList->get()->read(inStream, xfer, glenc,
                                                      maxReadLength);
                    if (errorFlag.good())
                        lastItemComplete = OFTrue;
                }
                fTransferredBytes = inStream.tell() - fStartPosition;

                if (errorFlag.bad())
                    break;
            } //while
            if ((fTransferredBytes < Length || !lastItemComplete) && errorFlag.good())
                errorFlag = EC_StreamNotifyClient;
        } // else errorFlag

        if (errorFlag == EC_SequEnd)
            errorFlag = EC_Normal;
        if (errorFlag.good())
            fTransferState = ERW_ready;      // sequence is complete
    }
    return errorFlag;
}


// ********************************


OFCondition DcmSequenceOfItems::write(DcmOutputStream & outStream,
                                      const E_TransferSyntax oxfer,
                                      const E_EncodingType enctype)
  {
    if (fTransferState == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        errorFlag = outStream.status();
        if (errorFlag.good() && fTransferState != ERW_ready)
        {
            if (fTransferState == ERW_init)
            {
                if (outStream.avail() >= DCM_TagInfoLength) // header might be smaller than this
                {
                    if (enctype == EET_ExplicitLength)
                        Length = getLength(oxfer, enctype);
                    else
                        Length = DCM_UndefinedLength;
                    Uint32 written_bytes = 0;
                    errorFlag = writeTagAndLength(outStream, oxfer, written_bytes);
                    if (errorFlag.good())
                    {
                        fTransferState = ERW_inWork;
                        itemList->seek(ELP_first);
                    }
                } else
                    errorFlag = EC_StreamNotifyClient;
            }
            if (fTransferState == ERW_inWork)
            {
                // itemList->get() can be NULL if buffer was full after
                // writing the last item but before writing the sequence delimitation.
                if (!itemList->empty() && (itemList->get() != NULL))
                {
                    DcmObject *dO;
                    do
                    {
                      dO = itemList->get();
                      if (dO->transferState() != ERW_ready)
                          errorFlag = dO->write(outStream, oxfer, enctype);
                    } while (errorFlag.good() && itemList->seek(ELP_next));
                }
                if (errorFlag.good())
                {
                    fTransferState = ERW_ready;
                    if (Length == DCM_UndefinedLength)
                    {
                        if (outStream.avail() >= 8)
                        {
                            // write sequence delimitation item
                            DcmTag delim(DCM_SequenceDelimitationItem);
                            errorFlag = writeTag(outStream, delim, oxfer);
                            Uint32 delimLen = 0L;
                            outStream.write(&delimLen, 4); // 4 bytes length
                        } else {
                            // the complete sequence is written but it
                            // is not possible to write the delimination item into the buffer.
                            errorFlag = EC_StreamNotifyClient;
                            fTransferState = ERW_inWork;
                        }
                    }
                }
            }
        }
    }
    return errorFlag;
}

// ********************************


OFCondition DcmSequenceOfItems::writeTagAndVR(DcmOutputStream &outStream,
                                              const DcmTag &tag,
                                              DcmEVR vr,
                                              const E_TransferSyntax oxfer)
{
    OFCondition l_error = outStream.status();
    if (l_error.good())
    {
        /* write the tag information (a total of 4 bytes, group number and element */
        /* number) to the stream. Mind the transfer syntax's byte ordering. */
        l_error = writeTag(outStream, tag, oxfer);
        /* create an object which represents the transfer syntax */
        DcmXfer oxferSyn(oxfer);
        /* if the transfer syntax is one with explicit value representation */
        /* this value's data type also has to be written to the stream. Do so */
        /* and also write the length information to the stream. */
        if (oxferSyn.isExplicitVR())
        {
            /* Create an object that represents this object's data type */
            DcmVR myvr(vr);
            /* get name of data type */
            const char *vrname = myvr.getValidVRName();
            /* write data type name to the stream (a total of 2 bytes) */
            outStream.write(vrname, 2);
            /* create another data type object on the basis of the above created object */
            DcmVR outvr(myvr.getValidEVR());
            /* in case we are dealing with a transfer syntax with explicit VR (see if above) */
            /* and the actual VR uses extended length encoding (see DICOM standard (year 2000) */
            /* part 5, section 7.1.2) (or the corresponding section in a later version of the */
            /* standard) we have to add 2 reserved bytes (set to a value of 00H) to the data */
            /* type field and the actual length field is 4 bytes wide. Write the corresponding */
            /* information to the stream. */
            if (outvr.usesExtendedLengthEncoding())
            {
              Uint16 reserved = 0;
              outStream.write(&reserved, 2);  // write 2 reserved bytes to stream
            }
        }
    }
    /* return result */
    return l_error;
}


OFCondition DcmSequenceOfItems::writeSignatureFormat(DcmOutputStream &outStream,
                                                     const E_TransferSyntax oxfer,
                                                     const E_EncodingType enctype)
{
    if (fTransferState == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        errorFlag = outStream.status();
        if (errorFlag.good() && fTransferState != ERW_ready)
        {
            if (fTransferState == ERW_init)
            {
                if (outStream.avail() >= DCM_TagInfoLength) // header might be smaller than this
                {
                    if (enctype == EET_ExplicitLength)
                        Length = getLength(oxfer, enctype);
                    else
                        Length = DCM_UndefinedLength;
                    errorFlag = writeTagAndVR(outStream, Tag, getVR(), oxfer);
                    /* we don't write the sequence length */
                    if (errorFlag.good())
                    {
                        fTransferState = ERW_inWork;
                        itemList->seek(ELP_first);
                    }
                } else
                    errorFlag = EC_StreamNotifyClient;
            }
            if (fTransferState == ERW_inWork)
            {
                // itemList->get() can be NULL if buffer was full after
                // writing the last item but before writing the sequence delimitation.
                if (!itemList->empty() && (itemList->get() != NULL))
                {
                    DcmObject *dO;
                    do {
                        dO = itemList->get();
                        if (dO->transferState() != ERW_ready)
                            errorFlag = dO->writeSignatureFormat(outStream, oxfer, enctype);
                    } while (errorFlag.good() && itemList->seek(ELP_next));
                }
                if (errorFlag.good())
                {
                    fTransferState = ERW_ready;
                    /* we always write a sequence delimitation item tag, but no length */
                    if (outStream.avail() >= 4)
                    {
                        // write sequence delimitation item
                        DcmTag delim(DCM_SequenceDelimitationItem);
                        errorFlag = writeTag(outStream, delim, oxfer);
                    } else {
                        // Every subelement of the item was written but it
                        // is not possible to write the delimination item
                        // into the buffer.
                        fTransferState = ERW_inWork;
                        errorFlag = EC_StreamNotifyClient;
                    }
                }
            }
        }
    }
    return errorFlag;
}


// ********************************


void DcmSequenceOfItems::transferInit()
{
    DcmObject::transferInit();
    fStartPosition = 0;
    lastItemComplete = OFTrue;
    if (!itemList->empty())
    {
        itemList->seek(ELP_first);
        do {
            itemList->get()->transferInit();
        } while (itemList->seek(ELP_next));
    }
}


// ********************************


void DcmSequenceOfItems::transferEnd()
{
    DcmObject::transferEnd();
    if (!itemList->empty())
    {
        itemList->seek(ELP_first);
        do {
            itemList->get()->transferEnd();
        } while (itemList->seek(ELP_next));
    }
}


// ********************************


unsigned long DcmSequenceOfItems::card()
{
    return itemList->card();
}


// ********************************


OFCondition DcmSequenceOfItems::prepend(DcmItem *item)
{
    errorFlag = EC_Normal;
    if (item != (DcmItem*)NULL)
        itemList->prepend(item);
    else
        errorFlag = EC_IllegalCall;

    return errorFlag;
}

// ********************************


OFCondition DcmSequenceOfItems::insert(DcmItem *item,
                                       unsigned long where,
                                       OFBool before)
{
    errorFlag = EC_Normal;
    if (item != (DcmItem*)NULL)
    {
        itemList->seek_to(where);
        // insert before or after "where"
        E_ListPos whichSide = (before) ? (ELP_prev) : (ELP_next);
        itemList->insert(item, whichSide);
        if (before)
        {
            debug(3, ("DcmSequenceOfItems::insert() item inserted before position %d", where));
        } else {
            debug(3, ("DcmSequenceOfItems::insert() item inserted after position %d", where));
        }
    } else
        errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


OFCondition DcmSequenceOfItems::append(DcmItem *item)
{
    errorFlag = EC_Normal;
    if (item != (DcmItem*)NULL)
        itemList->append(item);
    else
        errorFlag = EC_IllegalCall;
    return errorFlag;
}


// ********************************


DcmItem* DcmSequenceOfItems::getItem(const unsigned long num)
{
    errorFlag = EC_Normal;
    DcmItem *item;
    item = (DcmItem*)(itemList->seek_to(num));  // read item from list
    if (item == (DcmItem*)NULL)
        errorFlag = EC_IllegalCall;
    return item;
}


// ********************************


DcmObject *DcmSequenceOfItems::nextInContainer(const DcmObject *obj)
{
    if (!obj)
        return itemList->get(ELP_first);
    else
    {
        if (itemList->get() != obj)
        {
            for (DcmObject *search_obj = itemList -> seek(ELP_first);
                search_obj && search_obj != obj;
                search_obj = itemList -> seek(ELP_next)
               )
            { /* do nothing */ }
        }
        return itemList -> seek(ELP_next);
    }
}


// ********************************


OFCondition DcmSequenceOfItems::nextObject(DcmStack &stack,
                                           const OFBool intoSub)
{
    OFCondition l_error = EC_Normal;
    DcmObject *container = NULL;
    DcmObject *obj = NULL;
    DcmObject *result = NULL;
    OFBool examSub = intoSub;

    if (stack.empty())
    {
        stack.push(this);
        examSub = OFTrue;
    }

    obj = stack.top();
    if (obj->isLeaf() || !intoSub)
    {
        stack.pop();
        if (stack.card() > 0)
        {
            container = stack.top();
            result = container -> nextInContainer(obj);
        }
    } else if (examSub)
        result = obj -> nextInContainer(NULL);

    if (result)
        stack.push(result);
    else if (intoSub)
        l_error = nextUp(stack);
    else
        l_error = EC_SequEnd;

    return l_error;
}


// ********************************


DcmItem *DcmSequenceOfItems::remove(const unsigned long num)
{
    errorFlag = EC_Normal;
    DcmItem *item;
    item = (DcmItem*)(itemList->seek_to(num));  // read item from list
    if (item != (DcmItem*)NULL)
        itemList->remove();
    else
        errorFlag = EC_IllegalCall;
    return item;
}


// ********************************


DcmItem *DcmSequenceOfItems::remove(DcmItem *item)
{
    DcmItem *retItem = (DcmItem*)NULL;
    errorFlag = EC_IllegalCall;
    if (!itemList->empty() && item != (DcmItem*)NULL)
    {
        DcmObject *dO;
        itemList->seek(ELP_first);
        do {
            dO = itemList->get();
            if (dO == item)
            {
                itemList->remove();         // removes element from list but does not delete it
                errorFlag = EC_Normal;
                break;
            }
        } while (itemList->seek(ELP_next));
    }
    if (errorFlag == EC_IllegalCall)
        retItem = (DcmItem*)NULL;
    else
        retItem = item;
    return retItem;
}


// ********************************


OFCondition DcmSequenceOfItems::clear()
{
    errorFlag = EC_Normal;
    DcmObject *dO;
    itemList->seek(ELP_first);
    while (!itemList->empty())
    {
        dO = itemList->remove();
        if (dO != (DcmObject*)NULL)
        {
            delete dO;
            dO = (DcmObject*)NULL;
        }
    }
    Length = 0;
    return errorFlag;
}


// ********************************


OFCondition DcmSequenceOfItems::verify(const OFBool autocorrect)
{
    errorFlag = EC_Normal;
    if (!itemList->empty())
    {
        DcmObject *dO;
        itemList->seek(ELP_first);
        do {
            dO = itemList->get();
            if (dO->verify(autocorrect).bad())
                errorFlag = EC_CorruptedData;
        } while (itemList->seek(ELP_next));
    }
    if (autocorrect == OFTrue)
        Length = getLength();

    return errorFlag;
}


/*
 * precondition: itemList not empty.
 * result:
 *  - element pointer on resultStack if return value is EC_Normal
 *  - unmodified resultStack if return value is EC_TagNotFound
 * continue search: push pointer to sub-element onto resultStack and start sub-search
 */

OFCondition DcmSequenceOfItems::searchSubFromHere(const DcmTagKey &tag,
                                                  DcmStack &resultStack,
                                                  const OFBool searchIntoSub)
{
    DcmObject *dO;
    OFCondition l_error = EC_TagNotFound;
    if (!itemList->empty())
    {
        itemList->seek(ELP_first);
        do {
            dO = itemList->get();
            if (searchIntoSub)
            {
                resultStack.push(dO);
                if (tag == dO->getTag())
                    l_error = EC_Normal;
                else
                    l_error = dO->search(tag, resultStack, ESM_fromStackTop, OFTrue);
                if (l_error.bad())
                    resultStack.pop();
            } else {
                if (tag == dO->getTag())
                {
                    resultStack.push(dO);
                    l_error = EC_Normal;
                }
            }
        } while (l_error.bad() && itemList->seek(ELP_next));
    }
    return l_error;
}


// ********************************


OFCondition DcmSequenceOfItems::search(const DcmTagKey &tag,
                                       DcmStack &resultStack,
                                       E_SearchMode mode,
                                       OFBool searchIntoSub)
{
    DcmObject *dO = (DcmObject*)NULL;
    OFCondition l_error = EC_TagNotFound;
    if (mode == ESM_afterStackTop && resultStack.top() == this)
    {
        l_error = searchSubFromHere(tag, resultStack, searchIntoSub);
    }
    else if (!itemList->empty())
    {
        if (mode == ESM_fromHere || resultStack.empty())
        {
            resultStack.clear();
            l_error = searchSubFromHere(tag, resultStack, searchIntoSub);
        }
        else if (mode == ESM_fromStackTop)
        {
            dO = resultStack.top();
            if (dO == this)
                l_error = searchSubFromHere(tag, resultStack, searchIntoSub);
            else
            {   // continue directly in sub-tree
                l_error = dO->search(tag, resultStack, mode, searchIntoSub);
// The next two lines destroy the stack, delete them
//                if (l_error.bad())          // raeumt nur die oberste Stackebene
//                    resultStack.pop();      // ab; der Rest ist unveraendert
            }
        }
        else if (mode == ESM_afterStackTop && searchIntoSub)
        {
            // resultStack enthaelt Zielinformationen:
            // - stelle Zustand der letzen Suche in den einzelnen Suchroutinen
            //   wieder her
            // - finde Position von dO in Baum-Struktur
            //   1. suche eigenen Stack-Eintrag
            //      - bei Fehlschlag Suche beenden
            //   2. nehme naechsthoeheren Eintrag dnO
            //   3. stelle eigene Liste auf Position von dnO
            //   4. starte Suche ab dnO

            unsigned long i = resultStack.card();
            while (i > 0 && (dO = resultStack.elem(i-1)) != this)
            {
                i--;
            }
            if (dO != this && resultStack.card() > 0)
            {                            // highest level is never in resultStack
                i = resultStack.card()+1;// now points to highest level +1
                dO = this;               // match in highest level
            }
            if (dO == this)
            {
                if (i == 1)                 // resultStack.top() found
                    l_error = EC_TagNotFound; // don't mark as match, see above
                else
                {
                    E_SearchMode submode = mode;
                    OFBool searchNode = OFTrue;
                    DcmObject *dnO;
                    dnO = resultStack.elem(i-2); // node of next level
                    itemList->seek(ELP_first);
                    do {
                        dO = itemList->get();
                        searchNode = searchNode ? (dO != dnO) : OFFalse;
                        if (!searchNode)
                        {                               // continue search
                            if (submode == ESM_fromStackTop)
                                resultStack.push(dO);   // update stack
                            if (submode == ESM_fromStackTop && tag == dO->getTag())
                                l_error = EC_Normal;
                            else
                                l_error = dO->search(tag, resultStack, submode, OFTrue);
                            if (l_error.bad())
                                resultStack.pop();
                            else
                                break;
                            submode = ESM_fromStackTop; // normal search from here
                        }
                    } while (itemList->seek(ELP_next));
                }
            } else                              // probably never reached
                l_error = EC_IllegalCall;
        } // (mode == ESM_afterStackTop
        else
            l_error = EC_IllegalCall;
    }
    return l_error;
}


// ********************************


OFCondition DcmSequenceOfItems::searchErrors(DcmStack &resultStack)
{
    OFCondition l_error = errorFlag;
    DcmObject *dO = (DcmObject*)NULL;
    if (errorFlag.bad())
        resultStack.push(this);
    if (!itemList->empty())
    {
        itemList->seek(ELP_first);
        do {
            OFCondition err = EC_Normal;
            dO = itemList->get();
            if ((err = dO->searchErrors(resultStack)).bad())
                l_error = err;
        } while (itemList->seek(ELP_next));
    }
    return l_error;
}


// ********************************


OFCondition DcmSequenceOfItems::loadAllDataIntoMemory()
{
    OFCondition l_error = EC_Normal;
    if (!itemList->empty())
    {
        itemList->seek(ELP_first);
        do {
            OFCondition err = EC_Normal;
            DcmObject *dO = itemList->get();
            if ((err = dO->loadAllDataIntoMemory()).bad())
                l_error = err;
        } while (itemList->seek(ELP_next));
    }
    return l_error;
}


// ********************************


OFBool DcmSequenceOfItems::isSignable() const
{
    // a sequence is signable if the tag and VR doesn't prevent signing
    // and if none of the items contains a UN element
    return (DcmElement::isSignable() && (! containsUnknownVR()));
}


OFBool DcmSequenceOfItems::containsUnknownVR() const
{
    if (!itemList->empty())
    {
        itemList->seek(ELP_first);
        do {
            if (itemList->get()->containsUnknownVR())
                return OFTrue;
        } while (itemList->seek(ELP_next));
    }
    return OFFalse;
}
