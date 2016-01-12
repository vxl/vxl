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
 *  Author:  Gerd Ehlers, Andreas Barth
 *
 *  Purpose: Interface of class DcmPixelSequence
 *
 */

#ifndef DCPIXSEQ_H
#define DCPIXSEQ_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "ofconsol.h"
#include "dctypes.h"
#include "dcsequen.h"
#include "dcerror.h"
#include "dcofsetl.h"    /* for class DcmOffsetList */

class DcmPixelItem;

class DcmPixelSequence : public DcmSequenceOfItems
{
private:
    E_TransferSyntax Xfer;


// These methods are not sensible for a pix-sequence

    virtual OFCondition insert(DcmItem* /*item*/,
                               unsigned long /*where*/ = DCM_EndOfListIndex,
                               OFBool /*before*/ = OFFalse)
    {
        return EC_IllegalCall;
    }
    virtual DcmItem* getItem(const unsigned long /*num*/)
    {
        return NULL;
    }
    virtual DcmItem* remove(const unsigned long /*num*/)
    {
        return NULL;
    }
    virtual DcmItem* remove(DcmItem* /*item*/)
    {
        return NULL;
    }

protected:
    virtual OFCondition makeSubObject(DcmObject * & newObject, // out
                                      const DcmTag & newTag,
                                      const Uint32 newLength);  // in

public:
    DcmPixelSequence(const DcmTag &tag, const Uint32 len = 0);
    DcmPixelSequence(const DcmPixelSequence &old);
    virtual ~DcmPixelSequence();

    DcmPixelSequence &operator=(const DcmPixelSequence &obj);

    virtual DcmEVR ident(void) const { return EVR_pixelSQ; }

    virtual void print(ostream &out,
                       const size_t flags = 0,
                       const int level = 0,
                       const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL);

    virtual Uint32 calcElementLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype);

    virtual OFCondition insert(DcmPixelItem* item,
                               unsigned long where = DCM_EndOfListIndex);

    virtual OFCondition getItem(DcmPixelItem * & item, const unsigned long num);
    virtual OFCondition remove(DcmPixelItem * & item, const unsigned long num);
    virtual OFCondition remove(DcmPixelItem* item);


    OFCondition changeXfer(const E_TransferSyntax newXfer);

    virtual OFBool canWriteXfer(const E_TransferSyntax newXfer,
                              const E_TransferSyntax oldXfer);

    virtual OFCondition read(DcmInputStream & inStream,
                             const E_TransferSyntax ixfer,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength
                             = DCM_MaxReadLength);

    virtual OFCondition write(DcmOutputStream & outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType /*enctype*/);

    /** special write method for creation of digital signatures
     */
    virtual OFCondition writeSignatureFormat(DcmOutputStream & outStream,
                                         const E_TransferSyntax oxfer,
                                         const E_EncodingType enctype
                                         = EET_UndefinedLength);

  /** appends a single compressed frame to this DICOM pixel sequence
   *  @param offsetList list containing offset table entries.
   *    Upon success, an entry is appended to the list
   *  @param compressedData pointer to compressed image data, must not be NULL
   *  @param compressedLen number of bytes of compressed image data
   *  @param fragmentSize maximum fragment size (in kbytes) for compression, 0 for unlimited.
   *  @return EC_Normal if successful, an error code otherwise
   */
  virtual OFCondition storeCompressedFrame(
        DcmOffsetList& offsetList,
        Uint8 *compressedData,
        Uint32 compressedLen,
        Uint32 fragmentSize);

};

#endif // DCPIXSEQ_H
