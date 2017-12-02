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
 *  Purpose: Interface of class DcmOverlayData
 *
 */

#ifndef DCOVLAY_H
#define DCOVLAY_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvrpobw.h"


class DcmOverlayData : public DcmPolymorphOBOW
{
public:
    DcmOverlayData(
        const DcmTag & tag,
        const Uint32 len = 0)
        : DcmPolymorphOBOW(tag, len) {}
    DcmOverlayData(
        const DcmOverlayData & oldObj)
        : DcmPolymorphOBOW(oldObj) {}
    virtual ~DcmOverlayData() {}

    DcmOverlayData &operator=(const DcmOverlayData &obj) { DcmPolymorphOBOW::operator=(obj); return *this; }

    virtual DcmEVR ident() const { return EVR_OverlayData; }
};

#endif
