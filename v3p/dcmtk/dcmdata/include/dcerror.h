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

#ifndef DCERROR_H
#define DCERROR_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofcond.h"      /* for OFCondition */

/*
 *  dcmtk module numbers for modules which create their own error codes.
 *  Module numbers > 1023 are reserved for user code.
 */

const unsigned short OFM_dcmdata  =  1;
const unsigned short OFM_ctndisp  =  2;
const unsigned short OFM_dcmimgle =  3;
const unsigned short OFM_dcmimage =  4;
const unsigned short OFM_dcmjpeg  =  5;
const unsigned short OFM_dcmnet   =  6;
const unsigned short OFM_dcmprint =  7;
const unsigned short OFM_dcmpstat =  8;
const unsigned short OFM_dcmsign  =  9;
const unsigned short OFM_dcmsr    = 10;
const unsigned short OFM_dcmtls   = 11;
const unsigned short OFM_imagectn = 12;
const unsigned short OFM_wlistctn = 13;
const unsigned short OFM_dcmwlm   = 14;
const unsigned short OFM_dcmpps   = 15;
const unsigned short OFM_dcmdbsup = 16;
const unsigned short OFM_dcmppswm = 17;


// condition constants
extern const OFCondition EC_InvalidTag;
extern const OFCondition EC_TagNotFound;
extern const OFCondition EC_InvalidVR;
extern const OFCondition EC_InvalidStream;
extern const OFCondition EC_EndOfStream;
extern const OFCondition EC_CorruptedData;
extern const OFCondition EC_IllegalCall;
extern const OFCondition EC_SequEnd;
extern const OFCondition EC_DoubledTag;
extern const OFCondition EC_StreamNotifyClient;
extern const OFCondition EC_WrongStreamMode;
extern const OFCondition EC_ItemEnd;
extern const OFCondition EC_RepresentationNotFound;
extern const OFCondition EC_CannotChangeRepresentation;
extern const OFCondition EC_UnsupportedEncoding;
extern const OFCondition EC_PutbackFailed;
extern const OFCondition EC_DoubleCompressionFilters;
extern const OFCondition EC_ApplicationProfileViolated;


#ifndef OFCONDITION_STRICT_MODE

// for backward compatibility with existing software
typedef OFCondition E_Condition;

/** Return a pointer to a char array describing the error condition.
 *  For backward compatibility with old software; deprecated.
 */
extern const char *dcmErrorConditionToString(OFCondition cond);

#endif


#endif /* !DCERROR_H */
