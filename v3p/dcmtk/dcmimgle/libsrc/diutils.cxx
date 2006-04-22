/*
 *
 *  Copyright (C) 1996-2001, OFFIS
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
 *  Module:  dcmimgle
 *
 *  Author:  Joerg Riesmeier
 *
 *  Purpose: Utilities (Source)
 *
 */


#include "osconfig.h"
#include "dctypes.h"
#include "ofconsol.h"

#include "diutils.h"

#include "ofstream.h"


/*-------------------*
 *  initializations  *
 *-------------------*/

const int DicomImageClass::DL_NoMessages     = 0x0;
const int DicomImageClass::DL_Errors         = 0x1;
const int DicomImageClass::DL_Warnings       = 0x2;
const int DicomImageClass::DL_Informationals = 0x4;
const int DicomImageClass::DL_DebugMessages  = 0x8;

#ifdef DEBUG
 OFGlobal<int> DicomImageClass::DebugLevel(DicomImageClass::DL_DebugMessages);
#else
 OFGlobal<int> DicomImageClass::DebugLevel(DicomImageClass::DL_NoMessages);
#endif


/*------------------------*
 *  function definitions  *
 *------------------------*/

EP_Representation DicomImageClass::determineRepresentation(double minvalue,
                                                           double maxvalue)
{
    if (minvalue > maxvalue)                        /* assertion: min < max ! */
    {
        const double temp = minvalue;
        minvalue = maxvalue;
        maxvalue = temp;
    }
    if (minvalue < 0)                               /* signed */
    {
        if ((-minvalue <= maxval(7, 0)) && (maxvalue <= maxval(7)))
            return EPR_Sint8;
        if ((-minvalue <= maxval(15, 0)) && (maxvalue <= maxval(15)))
            return EPR_Sint16;
#ifdef DEBUG
        if (-minvalue > maxval(MAX_BITS - 1, 0))
        {
            if (checkDebugLevel(DL_Warnings))
            {
                ofConsole.lockCerr() << "WARNING: minimum pixel value (" << minvalue << ") exceeds signed " << MAX_BITS
                                     << " bit " << "representation after modality transformation !" << endl;
                ofConsole.unlockCerr();
            }
        }
        if (maxvalue > maxval(MAX_BITS - 1))
        {
            if (checkDebugLevel(DL_Warnings))
            {
                ofConsole.lockCerr() << "WARNING: maximum pixel value (" << maxvalue << ") exceeds signed " << MAX_BITS
                                     << " bit " << "representation after modality transformation !" << endl;
                ofConsole.unlockCerr();
            }
        }
#endif
        return EPR_Sint32;
    }
    if (maxvalue <= maxval(8))
        return EPR_Uint8;
    if (maxvalue <= maxval(16))
        return EPR_Uint16;
#ifdef DEBUG
    if (maxvalue > maxval(MAX_BITS))
    {
        if (checkDebugLevel(DL_Warnings))
        {
            ofConsole.lockCerr() << "WARNING: maximum pixel value (" << maxvalue << ") exceeds unsigned " << MAX_BITS
                                 << " bit " << "representation after modality transformation !" << endl;
            ofConsole.unlockCerr();
        }
    }
#endif
    return EPR_Uint32;
}
