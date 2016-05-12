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
 *  Author:  Gerd Ehlers, Andrew Hewett
 *
 *  Purpose: global type and constant definitions
 *
 */


#ifndef DCTYPES_H
#define DCTYPES_H 1

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "oftypes.h"

#define INCLUDE_CSTDLIB
#include "ofstdinc.h"


typedef Uint8                    BYTE;
typedef Sint8                    SBYTE;


/*
** Enumerated Types
*/

typedef enum {
    EET_ExplicitLength = 0,
    EET_UndefinedLength = 1
} E_EncodingType;


typedef enum {
    EGL_noChange = 0,       // no change of GL values, WARNING: DO NOT USE FOR WRITE
    EGL_withoutGL = 1,      // remove group length tags
    EGL_withGL = 2,         // add group length tags for every group
    EGL_recalcGL = 3        // recalculate values for existing group length tags
} E_GrpLenEncoding;

typedef enum {
    EPD_noChange = 0,       // no change of padding tags
    EPD_withoutPadding = 1, // remove all padding tags
    EPD_withPadding = 2     // add padding tags
} E_PaddingEncoding;


typedef enum {
    ESM_fromHere = 0,
    ESM_fromStackTop = 1,
    ESM_afterStackTop = 2
} E_SearchMode;


typedef enum {
    ERW_init = 0,
    ERW_ready = 1,
    ERW_inWork = 2,
    ERW_notInitialized = 3
} E_TransferState;



/** General purpose class hiding constants from the global namespace.
 */
struct DCMTypes
{
  public:

    /** @name print() flags.
     *  These flags can be combined and passed to the print() methods.
     */
    //@{

    /// shorten long tag values (e.g. long texts, pixel data)
    static const size_t PF_shortenLongTagValues;

    /// show hierarchical tree structure of the dataset
    static const size_t PF_showTreeStructure;

    /// internal: current entry is the last one on the level
    static const size_t PF_lastEntry;

    //@}


    /** @name writeXML() flags.
     *  These flags can be combined and passed to the writeXML() methods.
     */
    //@{

    /// add document type declaration
    static const size_t XF_addDocumentType;

    /// write binary data to XML output file
    static const size_t XF_writeBinaryData;

    /// encode binary data as Base64 (MIME)
    static const size_t XF_encodeBase64;
    //@}
};


// Undefined Length Identifier
const Uint32 DCM_UndefinedLength = 0xffffffff;


#endif /* !DCTYPES_H */
