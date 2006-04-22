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
 *  Purpose: Implementation of class DcmMetaInfo
 *
 */


#include "osconfig.h"    /* make sure OS specific configuration is included first */

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTRING
#include "ofstdinc.h"

#include "ofstream.h"
#include "ofstd.h"

#include "dcmetinf.h"
#include "dcitem.h"
#include "dcxfer.h"
#include "dcvrul.h"
#include "dcdebug.h"
#include "dcdeftag.h"
#include "dcdefine.h"
#include "dcistrma.h"    /* for class DcmInputStream */
#include "dcostrma.h"    /* for class DcmOutputStream */


const Uint32 DCM_GroupLengthElementLength = 12;


// ********************************


DcmMetaInfo::DcmMetaInfo()
  : DcmItem(ItemTag),
    preambleUsed(OFFalse),
    fPreambleTransferState(ERW_init),
    Xfer(META_HEADER_DEFAULT_TRANSFERSYNTAX)
{
    setPreamble();
}


DcmMetaInfo::DcmMetaInfo(const DcmMetaInfo &old)
  : DcmItem(old),
    preambleUsed(OFFalse),
    fPreambleTransferState(ERW_init),
    Xfer(META_HEADER_DEFAULT_TRANSFERSYNTAX)
{
    if (old.ident() == EVR_metainfo)
    {
        Xfer = old.Xfer;
        preambleUsed = old.preambleUsed;
        memcpy(filePreamble, old.filePreamble, 128);
    } else {
        // wrong use of copy constructor
        setPreamble();
    }
}


DcmMetaInfo::~DcmMetaInfo()
{
}


// ********************************


DcmEVR DcmMetaInfo::ident() const
{
    return EVR_metainfo;
}


E_TransferSyntax DcmMetaInfo::getOriginalXfer() const
{
    return Xfer;
}


// ********************************


void DcmMetaInfo::print(ostream &out,
                        const size_t flags,
                        const int level,
                        const char *pixelFileName,
                        size_t *pixelCounter)
{
    out << endl;
    printNestingLevel(out, flags, level);
    out << "# Dicom-Meta-Information-Header" << endl;
    printNestingLevel(out, flags, level);
    out << "# Used TransferSyntax: " << DcmXfer(Xfer).getXferName();
    out << endl;
    if (!elementList->empty())
    {
        DcmObject *dO;
        elementList->seek(ELP_first);
        do {
            dO = elementList->get();
            dO->print(out, flags, level + 1, pixelFileName, pixelCounter);
        } while (elementList->seek(ELP_next));
    }
}


// ********************************


OFCondition DcmMetaInfo::writeXML(ostream &out,
                                  const size_t flags)
{
    OFString xmlString;
    DcmXfer xfer(Xfer);
    /* XML start tag for "meta-header" */
    out << "<meta-header xfer=\"" << xfer.getXferID() << "\"";
    out << " name=\"" << OFStandard::convertToMarkupString(xfer.getXferName(), xmlString) << "\">" << endl;
    if (!elementList->empty())
    {
        /* write content of all children */
        DcmObject *dO;
        elementList->seek(ELP_first);
        do
        {
            dO = elementList->get();
            dO->writeXML(out, flags);
        } while (elementList->seek(ELP_next));
    }
    /* XML end tag for "meta-header" */
    out << "</meta-header>" << endl;
    /* always report success */
    return EC_Normal;
}


// ********************************


void DcmMetaInfo::setPreamble()
{
    memzero(filePreamble, sizeof(filePreamble));
    preambleUsed = OFFalse;
}


// ********************************


OFBool DcmMetaInfo::checkAndReadPreamble(DcmInputStream &inStream,
                                         E_TransferSyntax &newxfer)
{
    if (fPreambleTransferState == ERW_init)
    {
        inStream.mark();
        fPreambleTransferState = ERW_inWork;
    }
    OFBool retval = OFFalse;
    if (fPreambleTransferState == ERW_inWork)
    {
        const Uint32 preambleLen = DCM_PreambleLen + DCM_MagicLen;
        const Uint32 readLen = preambleLen-fTransferredBytes;
        if (readLen > 0)
            fTransferredBytes += inStream.read(&filePreamble[fTransferredBytes], readLen);
        if (inStream.eos() && fTransferredBytes != preambleLen)
        {   // file too short, no preamble
            inStream.putback();
            debug(4, ("DcmMetaInfo::checkAndReadPreamble() No Preamble available: File too short (%d) < %d bytes",
                preambleLen, DCM_PreambleLen + DCM_MagicLen));
            retval = OFFalse;
            this -> setPreamble();
            fPreambleTransferState = ERW_ready;
        }
        else if (fTransferredBytes == preambleLen)    // check Preamble and Dicom Prefix
        {   // set prefix to appropriate position
            char *prefix = filePreamble + DCM_PreambleLen;
            if (memcmp(prefix, DCM_Magic, DCM_MagicLen) == 0)
            {
                retval = OFTrue;  // Preamble present
                // inStream.UnsetPutbackMark(); // not needed anymore with new stream architecture
            } else {              // no Preamble
                retval = OFFalse;
                this -> setPreamble();
                inStream.putback();
            }
            fPreambleTransferState = ERW_ready;
        } else
            errorFlag = EC_StreamNotifyClient;
    }

    if (fPreambleTransferState == ERW_ready)
    {
        E_TransferSyntax tmpxfer = checkTransferSyntax(inStream);
        DcmXfer tmpxferSyn(tmpxfer);
        DcmXfer xferSyn(newxfer);
        if ((tmpxferSyn.isExplicitVR() && xferSyn.isImplicitVR()) ||
            (tmpxferSyn.isImplicitVR() && xferSyn.isExplicitVR()) ||
            xferSyn.getXfer() == EXS_Unknown)
        {
            newxfer = tmpxferSyn.getXfer();   // use determined xfer
            if (xferSyn.getXfer() != EXS_Unknown)
            {
                ofConsole.lockCerr() << "Info: DcmMetaInfo::checkAndReadPreamble(): "
                    "TransferSyntax of MetaInfo is different from "
                    "passed parameter newxfer: ignoring newxfer!" << endl;
                ofConsole.unlockCerr();
            }
        } else
            newxfer = xferSyn.getXfer();
    }
    Cdebug(4, retval==OFTrue, ("DcmMetaInfo::checkAndReadPreamble() found Preamble=[0x%8.8x]", (Uint32)(*filePreamble)));
    Cdebug(4, retval==OFFalse, ("DcmMetaInfo::checkAndReadPreamble() No Preambel found!"));
    debug(4, ("DcmMetaInfo::checkAndReadPreamble() TransferSyntax = %s", DcmXfer(newxfer).getXferName()));
    return retval;
} // DcmMetaInfo::checkAndReadPreamble


// ********************************


OFBool DcmMetaInfo::nextTagIsMeta(DcmInputStream &inStream)
{
    char testbytes[2];
    inStream.mark();
    inStream.read(testbytes, 2);
    inStream.putback();
    // check for group 0x0002 only
    return (testbytes[0] == 0x02 && testbytes[1] == 0x00) || (testbytes[0] == 0x00 && testbytes[1] == 0x02);
}


// ********************************


Uint32 DcmMetaInfo::calcElementLength(const E_TransferSyntax /*xfer*/,
                                      const E_EncodingType enctype)
{
    Uint32 metaLength = DcmItem::getLength(META_HEADER_DEFAULT_TRANSFERSYNTAX, enctype);
    metaLength += DCM_PreambleLen + DCM_MagicLen;
    return metaLength;
}


// ********************************


OFCondition DcmMetaInfo::readGroupLength(DcmInputStream &inStream,
                                         const E_TransferSyntax xfer,
                                         const DcmTagKey &xtag,
                                         const E_GrpLenEncoding glenc,
                                         Uint32 &headerLen,
                                         Uint32 &bytesRead,
                                         const Uint32 maxReadLength)
{
    OFCondition l_error = EC_TagNotFound;
    E_TransferSyntax newxfer = xfer;
    bytesRead = 0;
    headerLen = 0;
    if (nextTagIsMeta(inStream))
    {
        DcmTag newTag;
        Uint32 newValueLength = 0;
        Uint32 bytes_tagAndLen = 0;
        l_error = DcmItem::readTagAndLength(inStream, newxfer, newTag, newValueLength, bytes_tagAndLen);
        bytesRead += bytes_tagAndLen;
        if (l_error.good() && !inStream.good())
            l_error = inStream.status();
        if (l_error.good())
        {
            l_error = DcmItem::readSubElement(inStream, newTag, newValueLength, newxfer, glenc, maxReadLength);
            bytesRead += newValueLength;
            if (l_error.good() && newTag.getXTag() == xtag && elementList->get() != NULL && newValueLength > 0)
            {
                l_error = ((DcmUnsignedLong*)(elementList->get()))->getUint32(headerLen);
                debug(4, ("DcmMetaInfo::readGroupLength() Group Length of File Meta Header=%d", headerLen+bytesRead));
            } else {
                l_error = EC_CorruptedData;
                ofConsole.lockCerr() << "Warning: DcmMetaInfo::readGroupLength(): No Group"
                    " Length available in Meta Information Header" << endl;
                ofConsole.unlockCerr();
            }
        }
    }
    debug(4, ("DcmMetaInfo::readGroupLength() returns error = %s", l_error.text()));
    return l_error;
}


// ********************************


OFCondition DcmMetaInfo::read(DcmInputStream &inStream,
                              const E_TransferSyntax xfer,
                              const E_GrpLenEncoding glenc,
                              const Uint32 maxReadLength)
{
    if (fPreambleTransferState == ERW_notInitialized || fTransferState == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        Xfer = xfer;
        E_TransferSyntax newxfer = xfer;
        /* figure out if the stream reported an error */
        errorFlag = inStream.status();
        if (errorFlag.good() && inStream.eos())
            errorFlag = EC_EndOfStream;
        else if (errorFlag.good() && fTransferState != ERW_ready)
        {
            if (fTransferState == ERW_init && fPreambleTransferState != ERW_ready)
            {
                if (xfer == EXS_Unknown)
                    preambleUsed = checkAndReadPreamble(inStream, newxfer);
                else
                    newxfer = xfer;
                if (fPreambleTransferState == ERW_ready)
                {
                    Xfer = newxfer;   // store parameter transfer syntax
                    fTransferState = ERW_inWork;
                    fTransferredBytes = 0;
                    fStartPosition = inStream.tell();
                    Length = 0;
                }
            }
            if (fTransferState == ERW_inWork && Length == 0)
            {
                if (inStream.avail() < DCM_GroupLengthElementLength)
                    errorFlag = EC_StreamNotifyClient;
                else
                {
                    Uint32 headerLength = 0;
                    errorFlag = readGroupLength(inStream, newxfer, DCM_MetaElementGroupLength, glenc,
                        headerLength, fTransferredBytes, maxReadLength);
                    if (errorFlag.good())
                        Length = headerLength + fTransferredBytes;
                    else
                        Length = DCM_UndefinedLength;
                }
            }
            if (fTransferState == ERW_inWork && Length != 0 && errorFlag.good())
            {
                while (inStream.good() && !inStream.eos() &&
                       ((Length < DCM_UndefinedLength && fTransferredBytes < Length) ||
                        (Length == DCM_UndefinedLength && nextTagIsMeta(inStream)) ||
                        !lastElementComplete))
                {
                    DcmTag newTag;
                    Uint32 newValueLength = 0;
                    Uint32 bytes_tagAndLen = 0;
                    if (lastElementComplete)
                    {
                        errorFlag = DcmItem::readTagAndLength(inStream, newxfer, newTag, newValueLength, bytes_tagAndLen);
                        fTransferredBytes += bytes_tagAndLen;
                        if (errorFlag != EC_Normal)
                            break;                         // terminate while loop
                        lastElementComplete = OFFalse;
                        errorFlag = DcmItem::readSubElement(inStream, newTag, newValueLength, newxfer, glenc, maxReadLength);
                        if (errorFlag.good())
                            lastElementComplete = OFTrue;
                    } else {
                        errorFlag = elementList->get()->read(inStream, xfer, glenc, maxReadLength);
                        if (errorFlag.good())
                            lastElementComplete = OFTrue;
                    }
                    fTransferredBytes = inStream.tell() - fStartPosition;
                    if (errorFlag.bad())
                        break;                      // terminate while loop
                } //while
            }
            if (errorFlag == EC_TagNotFound || errorFlag == EC_EndOfStream)
            {
                errorFlag = EC_Normal;      // there is no meta header
                Xfer = EXS_Unknown;
            } else if (errorFlag == EC_ItemEnd)
                errorFlag = EC_Normal;
            if (errorFlag.good())
            {
                if (Length != DCM_UndefinedLength && fTransferredBytes != Length)
                {
                    ofConsole.lockCerr() << "Warning: DcmMetaInfo::read(): "
                        "Group Length of Meta"
                        " Information Header has incorrect Value!" << endl;
                    ofConsole.unlockCerr();
                }
                fTransferState = ERW_ready;          // MetaInfo ist komplett
            }
        }
    }
    return errorFlag;
} // DcmMetaInfo::read()


// ********************************


void DcmMetaInfo::transferInit()
{
    DcmItem::transferInit();
    fPreambleTransferState = ERW_init;
}


// ********************************


void DcmMetaInfo::transferEnd()
{
    DcmItem::transferEnd();
    fPreambleTransferState = ERW_notInitialized;
}


// ********************************


OFCondition DcmMetaInfo::write(DcmOutputStream &outStream,
                               const E_TransferSyntax /*oxfer*/,
                               const E_EncodingType enctype)
    /*
     * This function writes all data elements which make up the meta header to the stream.
     * For a specification of the elements that make up the meta header see DICOM standard
     * (year 2000) part 10, section 7.1)) or the corresponding section in a later version of
     * the standard).
     *
     * Parameters:
     *   outStream - [inout] The stream that the information will be written to.
     *   oxfer     - [in] The transfer syntax which shall be used. (is not necessary since the meta header
     *                    shall always be encoded in the explicit VR little endian transfer syntax)
     *   enctype   - [in] Encoding type for sequences. Specifies how sequences will be handled.
     */
{
    /* if the transfer state of this is not initialized, this is an illegal call */
    if (fTransferState == ERW_notInitialized)
        errorFlag = EC_IllegalCall;
    else
    {
        /* if this is not an illegal call, do something */

        /* determine the (default) transfer syntax which shall be used (always explicit VR little endian) */
        E_TransferSyntax outxfer = META_HEADER_DEFAULT_TRANSFERSYNTAX;
        /* check if the stream reported an error so far */
        errorFlag = outStream.status();
        /* if the stream did not report any error and the transfer state is ERW_ready, */
        /* go ahead and write the meta header information to the out stream */
        if (errorFlag.good() && fTransferState != ERW_ready)
        {
            /* if some particular conditions are met we need to write the file preamble (128 byte wide) and */
            /* the DICOM prefix "DICM" to the stream. Always check if there is enough space in the stream and */
            /* set the transfer state of certain elements to indicate that they have already been written. */
            if (fTransferState == ERW_init)
            {
                if (preambleUsed || !elementList->empty())
                {
                    if (fPreambleTransferState == ERW_init)
                    {
                        fTransferredBytes += outStream.write(&filePreamble[fTransferredBytes],
                            DCM_PreambleLen - fTransferredBytes);
                        if (fTransferredBytes != DCM_PreambleLen)
                            errorFlag = EC_StreamNotifyClient;
                        else
                            fPreambleTransferState = ERW_inWork;
                    }
                    if (fPreambleTransferState == ERW_inWork && outStream.avail() >= 4)
                    {
                        outStream.write(DCM_Magic, 4);
                        fPreambleTransferState = ERW_ready;
                        fTransferState = ERW_inWork;
                        elementList->seek(ELP_first);
                    } else
                        errorFlag = EC_StreamNotifyClient;
                }
            }
            /* if the file premable and the DICOM prefix have been written, go */
            /* ahead and write the meta header's data elements to the stream. */
            /* (note that at this point elementList->get() should never be NULL, */
            /* but lets play the game safe here...) */
            if (!elementList->empty() && (fTransferState == ERW_inWork) && (elementList->get() != NULL))
            {
                DcmObject *dO;
                /* iterate over the list of data elements and write them to the stream */
                do {
                    dO = elementList->get();
                    errorFlag = dO->write(outStream, outxfer, enctype);
                } while (errorFlag.good() && elementList->seek(ELP_next));
            }
            /* if the error flag equals ok and the transfer state equals ERW_inWork, all data elements of the meta */
            /* header have been written to the stream. Indicate this by setting the transfer state to ERW_ready */
            if (errorFlag.good() && fTransferState == ERW_inWork)
                fTransferState = ERW_ready;
        }
    }
    /* return result value */
    return errorFlag;
}
