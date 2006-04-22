/*
 *
 *  Copyright (C) 1994-2001, OFFIS
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
 *  Author:  Andreas Barth
 *
 *  Purpose:
 *  class DcmPolymorphOBOW for Tags that can change their VR
 *  between OB and OW (e.g. Tag PixelData, OverlayData). This class shall
 *  not be used directly in applications. No identification exists.
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcvrpobw.h"

DcmPolymorphOBOW::DcmPolymorphOBOW(
    const DcmTag & tag,
    const Uint32 len)
  : DcmOtherByteOtherWord(tag, len),
    changeVR(OFFalse),
    currentVR(EVR_OW)
{
    if (Tag.getEVR() == EVR_ox) Tag.setVR(EVR_OW);
}

DcmPolymorphOBOW::DcmPolymorphOBOW(const DcmPolymorphOBOW & oldObj)
: DcmOtherByteOtherWord(oldObj)
, changeVR(oldObj.changeVR)
, currentVR(oldObj.currentVR)
{
}

DcmPolymorphOBOW::~DcmPolymorphOBOW()
{
}

DcmPolymorphOBOW &DcmPolymorphOBOW::operator=(const DcmPolymorphOBOW & obj)
{
  DcmOtherByteOtherWord::operator=(obj);
  changeVR = obj.changeVR;
  currentVR = obj.currentVR;
  return *this;
}

OFCondition
DcmPolymorphOBOW::getUint8Array(
    Uint8 * & bytes)
{
    errorFlag = EC_Normal;
    OFBool bchangeVR = OFFalse;
    if (currentVR == EVR_OW)
    {
        if (fByteOrder == EBO_BigEndian)
        {
            swapValueField(sizeof(Uint16));
            fByteOrder = EBO_LittleEndian;
        }
        if (Tag.getEVR() == EVR_OW)
        {
            bchangeVR = OFTrue;
            Tag.setVR(EVR_OB);
            currentVR = EVR_OB;
        }
    }
    bytes = (Uint8 *)this -> getValue();
    if (bchangeVR)
        Tag.setVR(EVR_OW);

    return errorFlag;
}


OFCondition
DcmPolymorphOBOW::getUint16Array(
    Uint16 * & words)
{
    errorFlag = EC_Normal;
    OFBool bchangeVR = OFFalse;
    if (currentVR == EVR_OB)
    {
        fByteOrder = EBO_LittleEndian;
        currentVR = EVR_OW;
        if (Tag.getEVR() == EVR_OB)
        {
            Tag.setVR(EVR_OW);
            bchangeVR = OFTrue;
        }
    }
    words = (Uint16 *)this -> getValue();
    if (bchangeVR)
        Tag.setVR(EVR_OB);

    return errorFlag;
}

OFCondition
DcmPolymorphOBOW::createUint8Array(
    const Uint32 numBytes,
    Uint8 * & bytes)
{
    currentVR = EVR_OB;
    Tag.setVR(EVR_OB);
    errorFlag = createEmptyValue(sizeof(Uint8) * Uint32(numBytes));
    fByteOrder = gLocalByteOrder;
    if (EC_Normal == errorFlag)
        bytes = (Uint8 *)this->getValue();
    else
        bytes = NULL;
    return errorFlag;
}


OFCondition
DcmPolymorphOBOW::createUint16Array(
    const Uint32 numWords,
    Uint16 * & words)
{
    currentVR = EVR_OW;
    Tag.setVR(EVR_OW);
    errorFlag = createEmptyValue(sizeof(Uint16) * Uint32(numWords));
    fByteOrder = gLocalByteOrder;
    if (EC_Normal == errorFlag)
        words = (Uint16 *)this->getValue();
    else
        words = NULL;
    return errorFlag;
}


OFCondition
DcmPolymorphOBOW::putUint8Array(
    const Uint8 * byteValue,
    const unsigned long numBytes)
{
    errorFlag = EC_Normal;
    currentVR = Tag.getEVR();
    if (numBytes)
    {
        if (byteValue)
        {
            errorFlag = putValue(byteValue, sizeof(Uint8)*Uint32(numBytes));
            if (errorFlag == EC_Normal)
            {
                if (Tag.getEVR() == EVR_OW && fByteOrder == EBO_BigEndian)
                    fByteOrder = EBO_LittleEndian;
                this -> alignValue();
            }
        }
        else
            errorFlag = EC_CorruptedData;
    }
    else
        this -> putValue(NULL, 0);

    return errorFlag;
}


OFCondition
DcmPolymorphOBOW::putUint16Array(
    const Uint16 * wordValue,
    const unsigned long numWords)
{
    errorFlag = EC_Normal;
    currentVR = Tag.getEVR();
    if (numWords)
    {
        if (wordValue)
        {
            errorFlag = putValue(wordValue, sizeof(Uint16)*Uint32(numWords));
            if (errorFlag == EC_Normal &&
                Tag.getEVR() == EVR_OB && fByteOrder == EBO_BigEndian)
            {
                swapValueField(sizeof(Uint16));
                fByteOrder = EBO_LittleEndian;
            }
        }
        else
            errorFlag = EC_CorruptedData;
    }
    else
        errorFlag = this -> putValue(NULL, 0);

    return errorFlag;
}


OFCondition
DcmPolymorphOBOW::read(
    DcmInputStream & inStream,
    const E_TransferSyntax ixfer,
    const E_GrpLenEncoding glenc,
    const Uint32 maxReadLength)
{
    OFCondition l_error =
        DcmOtherByteOtherWord::read(inStream, ixfer, glenc, maxReadLength);

    if (fTransferState == ERW_ready)
        currentVR = Tag.getEVR();

    return l_error;
}

void
DcmPolymorphOBOW::transferEnd()
{
    changeVR = OFFalse;
    DcmOtherByteOtherWord::transferEnd();
}

void
DcmPolymorphOBOW::transferInit()
{
    changeVR = OFFalse;
    DcmOtherByteOtherWord::transferInit();
}

OFCondition DcmPolymorphOBOW::write(
    DcmOutputStream & outStream,
    const E_TransferSyntax oxfer,
    const E_EncodingType enctype)
{
    DcmXfer oXferSyn(oxfer);
    if (fTransferState == ERW_init)
    {
        if (Tag.getEVR() == EVR_OB && oXferSyn.isImplicitVR() &&  fByteOrder == EBO_BigEndian)
        {
            // VR is OB and it will be written as OW in LittleEndianImplicit.
            Tag.setVR(EVR_OW);
            if (currentVR == EVR_OB) fByteOrder = EBO_LittleEndian;
            currentVR = EVR_OB;
            changeVR = OFTrue;
        }
        else if (Tag.getEVR() == EVR_OW && currentVR == EVR_OB)
        {
            fByteOrder = EBO_LittleEndian;
            currentVR = EVR_OW;
        }
    }
    errorFlag = DcmOtherByteOtherWord::write(outStream, oxfer, enctype);
    if (fTransferState == ERW_ready && changeVR)
    {
        // VR must be OB again. No Swapping is needed since the written
        // transfer syntax was LittleEndianImplicit and so no swapping
        // took place.
        Tag.setVR(EVR_OB);
    }
    return errorFlag;
}

OFCondition DcmPolymorphOBOW::writeSignatureFormat(
    DcmOutputStream & outStream,
    const E_TransferSyntax oxfer,
    const E_EncodingType enctype)
{
    DcmXfer oXferSyn(oxfer);
    if (fTransferState == ERW_init)
    {
        if (Tag.getEVR() == EVR_OB && oXferSyn.isImplicitVR() &&  fByteOrder == EBO_BigEndian)
        {
            // VR is OB and it will be written as OW in LittleEndianImplicit.
            Tag.setVR(EVR_OW);
            if (currentVR == EVR_OB) fByteOrder = EBO_LittleEndian;
            currentVR = EVR_OB;
            changeVR = OFTrue;
        }
        else if (Tag.getEVR() == EVR_OW && currentVR == EVR_OB)
        {
            fByteOrder = EBO_LittleEndian;
            currentVR = EVR_OW;
        }
    }
    errorFlag = DcmOtherByteOtherWord::writeSignatureFormat(outStream, oxfer, enctype);
    if (fTransferState == ERW_ready && changeVR)
    {
        // VR must be OB again. No Swapping is needed since the written
        // transfer syntax was LittleEndianImplicit and so no swapping
        // took place.
        Tag.setVR(EVR_OB);
    }
    return errorFlag;
}
