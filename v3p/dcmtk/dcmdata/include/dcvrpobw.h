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
 *  Author:  Andreas Barth
 *
 *  Purpose:
 *  Interface of class DcmPolymorphOBOW for Tags that can change their VR
 *  between OB and OW (e.g. Tag PixelData, OverlayData). This class shall
 *  not be used directly in applications. No identification exists.
 *
 */

#ifndef DCVRPOBW_H
#define DCVRPOBW_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcvrobow.h"

class DcmPolymorphOBOW : public DcmOtherByteOtherWord
{
private:
    OFBool changeVR;
    DcmEVR currentVR;    // current VR of value field (can change)

public:
    DcmPolymorphOBOW(
        const DcmTag & tag,
        const Uint32 len = 0);

    DcmPolymorphOBOW(
        const DcmPolymorphOBOW & old);

    virtual ~DcmPolymorphOBOW();

    DcmPolymorphOBOW &operator=(
        const DcmPolymorphOBOW &obj);

    virtual OFCondition read(
        DcmInputStream & inStream,
        const E_TransferSyntax ixfer,
        const E_GrpLenEncoding glenc,
        const Uint32 maxReadLength);

    virtual OFCondition write(
        DcmOutputStream & outStream,
        const E_TransferSyntax oxfer,
        const E_EncodingType enctype = EET_UndefinedLength);

    /** special write method for creation of digital signatures
     */
    virtual OFCondition writeSignatureFormat(
        DcmOutputStream & outStream,
        const E_TransferSyntax oxfer,
        const E_EncodingType enctype = EET_UndefinedLength);

    virtual void transferInit();
    virtual void transferEnd();

    // get data as Uint8 Array
    virtual OFCondition getUint8Array(
        Uint8 * & bytes);

    // get data as Uint16 Array
    virtual OFCondition getUint16Array(
        Uint16 * & words);

    // put an Unit8 array. It is converted to OW if VR == OW
    virtual OFCondition putUint8Array(
        const Uint8 * byteValue,
        const unsigned long length);

    // put an Unit16 array. It is converted to OB if VR == OB
    virtual OFCondition putUint16Array(
        const Uint16 * wordValue,
        const unsigned long length );

    // create an empty Uint8 array of given number of words and set it
    virtual OFCondition createUint8Array(
        const Uint32 numBytes,
        Uint8 * & bytes);

    // create an empty Uint16 array of given number of words and set it
    virtual OFCondition createUint16Array(
        const Uint32 numWords,
        Uint16 * & words);
};
#endif
