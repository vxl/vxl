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
 *  Purpose: Implementation of class DcmUnsignedLongOffset
 *
 */

#include "osconfig.h"
#include "dcvrulup.h"

#define INCLUDE_CSTDIO
#define INCLUDE_CSTRING
#include "ofstdinc.h"


// ********************************


DcmUnsignedLongOffset::DcmUnsignedLongOffset(const DcmTag &tag,
                                             const Uint32 len)
  : DcmUnsignedLong(tag, len),
    nextRecord(NULL)
{
}


DcmUnsignedLongOffset::DcmUnsignedLongOffset(const DcmUnsignedLongOffset &old)
  : DcmUnsignedLong(old),
    nextRecord(old.nextRecord)
{
}


DcmUnsignedLongOffset::~DcmUnsignedLongOffset()
{
}


// ********************************


DcmEVR DcmUnsignedLongOffset::ident() const
{
    /* internal type identifier */
    return EVR_up;
}


OFCondition DcmUnsignedLongOffset::clear()
{
    /* call inherited method */
    errorFlag = DcmUnsignedLong::clear();
    /* remove reference to object */
    nextRecord = NULL;
    return errorFlag;
}


// ********************************


DcmObject* DcmUnsignedLongOffset::getNextRecord()
{
    errorFlag = EC_Normal;
    /* return pointer to currently stored object reference */
    return nextRecord;
}


DcmObject *DcmUnsignedLongOffset::setNextRecord(DcmObject *record)
{
    errorFlag = EC_Normal;
    /* store new object reference */
    nextRecord = record;
    return record;
}


// ********************************


OFCondition DcmUnsignedLongOffset::verify(const OFBool autocorrect)
{
    /* call inherited method */
    errorFlag = DcmUnsignedLong::verify(autocorrect);
    /* perform additional checks on the stored value */
    Uint32 *uintVals;
    errorFlag = getUint32Array(uintVals);
    if (errorFlag.good() && (Length > 0) && (uintVals != NULL) && (*uintVals != 0) && (nextRecord == NULL))
        errorFlag = EC_CorruptedData;
    return errorFlag;
}
