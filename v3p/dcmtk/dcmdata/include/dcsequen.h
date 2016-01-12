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
 *  Purpose: Interface of class DcmSequenceOfItems
 *
 */

#ifndef DCSEQUEN_H
#define DCSEQUEN_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "ofconsol.h"
#include "dcerror.h"
#include "dctypes.h"
#include "dcobject.h"
#include "dcitem.h"
#include "dctag.h"
#include "dclist.h"
#include "dcstack.h"


//
// CLASS DcmSequenceOfItems
// A sequence has no explicit value. Therefore, it should be derived from
// DcmObject. Since a sequence is created in an (pseudo)-item and items collect
// sequences of elements the sequence Tag is derived from element.

class DcmSequenceOfItems : public DcmElement
{
protected:
    DcmList *itemList;
    OFBool lastItemComplete;
    Uint32 fStartPosition;

    virtual OFCondition readTagAndLength(DcmInputStream & inStream,   // inout
                                         const E_TransferSyntax xfer,  // in
                                         DcmTag &tag,                  // out
                                         Uint32 & length );        // out

    virtual OFCondition makeSubObject(DcmObject * & subObject,
                                      const DcmTag & mewTag,
                                      const Uint32 newLength);

    OFCondition readSubItem(DcmInputStream & inStream,               // inout
                            const DcmTag &newTag,               // in
                            const Uint32 newLength,             // in
                            const E_TransferSyntax xfer,        // in
                            const E_GrpLenEncoding glenc,       // in
                            const Uint32 maxReadLength          // in
                            = DCM_MaxReadLength);

    virtual OFCondition searchSubFromHere(const DcmTagKey &tag,          // in
                                          DcmStack &resultStack,      // inout
                                          const OFBool searchIntoSub ); // in

public:
    DcmSequenceOfItems(const DcmTag &tag, const Uint32 len = 0);
    DcmSequenceOfItems( const DcmSequenceOfItems& oldSeq );
    virtual ~DcmSequenceOfItems();

    DcmSequenceOfItems &operator=(const DcmSequenceOfItems &obj);

    virtual DcmEVR ident() const { return EVR_SQ; }
    virtual OFBool isLeaf(void) const { return OFFalse; }

    virtual void print(ostream &out,
                       const size_t flags = 0,
                       const int level = 0,
                       const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL);

    virtual unsigned long getVM() { return 1L; }

    virtual OFCondition computeGroupLengthAndPadding
                            (const E_GrpLenEncoding glenc,
                             const E_PaddingEncoding padenc = EPD_noChange,
                             const E_TransferSyntax xfer = EXS_Unknown,
                             const E_EncodingType enctype = EET_ExplicitLength,
                             const Uint32 padlen = 0,
                             const Uint32 subPadlen = 0,
                             Uint32 instanceLength = 0);

    virtual Uint32 calcElementLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype);

    virtual Uint32 getLength(const E_TransferSyntax xfer
                             = EXS_LittleEndianImplicit,
                             const E_EncodingType enctype
                             = EET_UndefinedLength );

    virtual void transferInit(void);
    virtual void transferEnd(void);

    virtual OFBool canWriteXfer(const E_TransferSyntax oldXfer,
                              const E_TransferSyntax newXfer);

    virtual OFCondition read(DcmInputStream & inStream,
                             const E_TransferSyntax xfer,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength = DCM_MaxReadLength);

    virtual OFCondition write(DcmOutputStream & outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType enctype = EET_UndefinedLength);

    /** write object in XML format
     *  @param out output stream to which the XML document is written
     *  @param flags optional flag used to customize the output (see DCMTypes::XF_xxx)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(ostream &out,
                                 const size_t flags = 0);

    /** special write method for creation of digital signatures
     */
    virtual OFCondition writeSignatureFormat(DcmOutputStream & outStream,
                                         const E_TransferSyntax oxfer,
                                         const E_EncodingType enctype = EET_UndefinedLength);

    /** returns true if the current object may be included in a digital signature
     *  @return true if signable, false otherwise
     */
    virtual OFBool isSignable() const;

    /** returns true if the object contains an element with Unknown VR at any nesting level
     *  @return true if the object contains an element with Unknown VR, false otherwise
     */
    virtual OFBool containsUnknownVR() const;

    virtual unsigned long card();

    virtual OFCondition prepend(DcmItem* item);
    virtual OFCondition insert(DcmItem* item,
                               unsigned long where = DCM_EndOfListIndex,
                               OFBool before = OFFalse );
    virtual OFCondition append(DcmItem* item);

    virtual DcmItem* getItem(const unsigned long num);
    virtual OFCondition nextObject(DcmStack & stack, const OFBool intoSub);
    virtual DcmObject * nextInContainer(const DcmObject * obj);
    virtual DcmItem* remove(const unsigned long num);
    virtual DcmItem* remove(DcmItem* item);
    virtual OFCondition clear();
    virtual OFCondition verify(const OFBool autocorrect = OFFalse);
    virtual OFCondition search(const DcmTagKey &xtag,          // in
                               DcmStack &resultStack,          // inout
                               E_SearchMode mode = ESM_fromHere,  // in
                               OFBool searchIntoSub = OFTrue );   // in
    virtual OFCondition searchErrors( DcmStack &resultStack );    // inout
    virtual OFCondition loadAllDataIntoMemory(void);

private:

  /* static helper method used in writeSignatureFormat().
   * This function resembles DcmObject::writeTagAndLength()
   * but only writes the tag, VR and reserved field.
   * @param outStream stream to write to
   * @param tag attribute tag
   * @param vr attribute VR as reported by getVR
   * @param oxfer output transfer syntax
   * @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition writeTagAndVR(
    DcmOutputStream & outStream,
    const DcmTag & tag,
    DcmEVR vr,
    const E_TransferSyntax oxfer);

};



#endif // DCSEQUEN_H
