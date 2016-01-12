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
 *  Purpose: Interface of class DcmPixelItem
 *
 */

#ifndef DCPXITEM_H
#define DCPXITEM_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofconsol.h"
#include "dctypes.h"
#include "dcvrobow.h"
#include "dcofsetl.h"    /* for class DcmOffsetList */

// CLASS DcmPixelItem
// This is a pseudo item, that has a value with representation OB
// and has no sub elements. Since a DcmOtherByteOtherWord is defined as a
// Dicom structure with a value of representation OW/OB it is better to
// derive this class from DcmOtherByteOtherWord.

class DcmPixelItem : public DcmOtherByteOtherWord

{
  protected:
    virtual OFCondition writeTagAndLength(DcmOutputStream & outStream,
                                          const E_TransferSyntax oxfer,
                                          Uint32 & writtenBytes) const;
  public:
    DcmPixelItem(const DcmTag &tag, const Uint32 len = 0);
    DcmPixelItem(const DcmPixelItem &old);
    virtual ~DcmPixelItem();

    DcmPixelItem &operator=(const DcmPixelItem &obj) { DcmOtherByteOtherWord::operator=(obj); return *this; }

    virtual DcmEVR ident(void) const { return EVR_pixelItem; }

    virtual void print(ostream &out,
                       const size_t flags = 0,
                       const int level = 0,
                       const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL);

    /** creates in this object an offset table for a compressed pixel sequence.
     *  @param offsetList list of size entries for each individual encoded frame
     *    provided by the compression codec
     *  @return EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition createOffsetTable(const DcmOffsetList& offsetList);

    /** write object in XML format
     *  @param out output stream to which the XML document is written
     *  @param flags optional flag used to customize the output (see DCMTypes::XF_xxx)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(ostream &out,
                                 const size_t flags = 0);

};


#endif // DCPXITEM_H
