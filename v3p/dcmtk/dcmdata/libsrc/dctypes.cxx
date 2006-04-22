/*
 *
 *  Copyright (C) 2002, OFFIS
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
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: global type and constant definitions
 *
 */


#include "osconfig.h"

#include "dctypes.h"


/* print flags */
const size_t DCMTypes::PF_shortenLongTagValues = 1;
const size_t DCMTypes::PF_showTreeStructure    = 2;
const size_t DCMTypes::PF_lastEntry            = 4;

/* writeXML flags */
const size_t DCMTypes::XF_addDocumentType = 1;
const size_t DCMTypes::XF_writeBinaryData = 2;
const size_t DCMTypes::XF_encodeBase64    = 4;
