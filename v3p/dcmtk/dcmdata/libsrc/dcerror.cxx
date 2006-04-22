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
 *  Author:  Andrew Hewett
 *
 *  Purpose: Error handling, codes and strings
 *
 */

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dcerror.h"

const OFConditionConst ECC_InvalidTag(                 OFM_dcmdata,  1, OF_error, "Invalid Tag"                            );
const OFConditionConst ECC_TagNotFound(                OFM_dcmdata,  2, OF_error, "Tag Not Found"                          );
const OFConditionConst ECC_InvalidVR(                  OFM_dcmdata,  3, OF_error, "Invalid VR"                             );
const OFConditionConst ECC_InvalidStream(              OFM_dcmdata,  4, OF_error, "Invalid Stream"                         );
const OFConditionConst ECC_EndOfStream(                OFM_dcmdata,  5, OF_error, "End Of Stream"                          );
const OFConditionConst ECC_CorruptedData(              OFM_dcmdata,  6, OF_error, "Corrupted Data"                         );
const OFConditionConst ECC_IllegalCall(                OFM_dcmdata,  7, OF_error, "Illegal Call, perhaps wrong parameters" );
const OFConditionConst ECC_SequEnd(                    OFM_dcmdata,  8, OF_error, "Sequence End"                           );
const OFConditionConst ECC_DoubledTag(                 OFM_dcmdata,  9, OF_error, "Doubled Tag"                            );
const OFConditionConst ECC_StreamNotifyClient(         OFM_dcmdata, 10, OF_error, "I/O suspension or premature end of stream" );
const OFConditionConst ECC_WrongStreamMode(            OFM_dcmdata, 11, OF_error, "Mode (R/W, random/sequence) is wrong"   );
const OFConditionConst ECC_ItemEnd(                    OFM_dcmdata, 12, OF_error, "Item End"                               );
const OFConditionConst ECC_RepresentationNotFound(     OFM_dcmdata, 13, OF_error, "Pixel representation not found"         );
const OFConditionConst ECC_CannotChangeRepresentation( OFM_dcmdata, 14, OF_error, "Pixel representation cannot be changed" );
const OFConditionConst ECC_UnsupportedEncoding(        OFM_dcmdata, 15, OF_error, "Unsupported compression or encryption"  );
// error code 16 is reserved for zlib-related error messages
const OFConditionConst ECC_PutbackFailed(              OFM_dcmdata, 17, OF_error, "Parser failure: Putback operation failed" );
// error code 18 is reserved for file read error messages
// error code 19 is reserved for file write error messages
const OFConditionConst ECC_DoubleCompressionFilters(   OFM_dcmdata, 20, OF_error, "Too many compression filters"           );
const OFConditionConst ECC_ApplicationProfileViolated( OFM_dcmdata, 21, OF_error, "Storage media application profile violated" );


const OFCondition EC_InvalidTag(                 ECC_InvalidTag);
const OFCondition EC_TagNotFound(                ECC_TagNotFound);
const OFCondition EC_InvalidVR(                  ECC_InvalidVR);
const OFCondition EC_InvalidStream(              ECC_InvalidStream);
const OFCondition EC_EndOfStream(                ECC_EndOfStream);
const OFCondition EC_CorruptedData(              ECC_CorruptedData);
const OFCondition EC_IllegalCall(                ECC_IllegalCall);
const OFCondition EC_SequEnd(                    ECC_SequEnd);
const OFCondition EC_DoubledTag(                 ECC_DoubledTag);
const OFCondition EC_StreamNotifyClient(         ECC_StreamNotifyClient);
const OFCondition EC_WrongStreamMode(            ECC_WrongStreamMode);
const OFCondition EC_ItemEnd(                    ECC_ItemEnd);
const OFCondition EC_RepresentationNotFound(     ECC_RepresentationNotFound);
const OFCondition EC_CannotChangeRepresentation( ECC_CannotChangeRepresentation);
const OFCondition EC_UnsupportedEncoding(        ECC_UnsupportedEncoding);
const OFCondition EC_PutbackFailed(              ECC_PutbackFailed);
const OFCondition EC_DoubleCompressionFilters(   ECC_DoubleCompressionFilters);
const OFCondition EC_ApplicationProfileViolated( ECC_ApplicationProfileViolated);

const char *dcmErrorConditionToString(OFCondition cond)
{
  return cond.text();
}
