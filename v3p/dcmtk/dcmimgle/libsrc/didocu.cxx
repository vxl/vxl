/*
 *
 *  Copyright (C) 1996-2002, OFFIS
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
 *  Purpose: DicomDocument (Source)
 *
 */


#include "osconfig.h"
#include "dctk.h"
#include "ofstring.h"

#include "didocu.h"
#include "diutils.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiDocument::DiDocument(const char *filename,
                       const unsigned long flags,
                       const unsigned long fstart,
                       const unsigned long fcount)
  : Object(NULL),
    FileFormat(new DcmFileFormat()),
    Xfer(EXS_Unknown),
    FrameStart(fstart),
    FrameCount(fcount),
    Flags(flags)
{
    if (FileFormat)
    {
        if (FileFormat->loadFile(filename).bad())
        {
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
            {
                ofConsole.lockCerr() << "ERROR: can't read file '" << filename << "' !" << endl;
                ofConsole.unlockCerr();
            }
            delete FileFormat;
            FileFormat = NULL;
        } else {
            Object = FileFormat->getDataset();
            if (Object != NULL)
            {
                Xfer = ((DcmDataset *)Object)->getOriginalXfer();
                convertPixelData();
            }
        }
    }
}


DiDocument::DiDocument(DcmObject *object,
                       const E_TransferSyntax xfer,
                       const unsigned long flags,
                       const unsigned long fstart,
                       const unsigned long fcount)
  : Object(NULL),
    FileFormat(NULL),
    Xfer(xfer),
    FrameStart(fstart),
    FrameCount(fcount),
    Flags(flags)
{
    if (object != NULL)
    {
        if (object->ident() == EVR_fileFormat)
        {
            /* store reference to DICOM file format to be deleted on object destruction */
            if (Flags & CIF_TakeOverExternalDataset)
                FileFormat = (DcmFileFormat *)object;
            Object = ((DcmFileFormat *)object)->getDataset();
        } else
            Object = object;
        if (Object != NULL)
        {
            if (Xfer == EXS_Unknown)
                Xfer = ((DcmDataset *)Object)->getOriginalXfer();
            convertPixelData();
        }
    }
}


void DiDocument::convertPixelData()
{
    DcmStack pstack;
    // convert pixel data to uncompressed format if required
    if (search(DCM_PixelData, pstack))
    {
        DcmPixelData *pixel = (DcmPixelData *)pstack.top();
        pstack.clear();
        // push reference to DICOM dataset on the stack (required for decompression process)
        pstack.push(Object);
        pstack.push(pixel);                         // dummy stack entry
        if ((pixel != NULL) && pixel->chooseRepresentation(EXS_LittleEndianExplicit, NULL, pstack).good())
        {
            // set transfer syntax to unencapsulated/uncompressed
            if (DcmXfer(Xfer).isEncapsulated())
                Xfer = EXS_LittleEndianExplicit;
        } else {
            if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
            {
                ofConsole.lockCerr() << "ERROR: cannot change to unencapsulated representation for pixel data !" << endl;
                ofConsole.unlockCerr();
            }
        }
    } else {
        if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Errors))
        {
            ofConsole.lockCerr() << "ERROR: no pixel data found in DICOM dataset !" << endl;
            ofConsole.unlockCerr();
        }
    }
}


/*--------------*
 *  destructor  *
 *--------------*/

DiDocument::~DiDocument()
{
    /* DICOM image loaded from file: delete file format (and data set) */
    if (FileFormat != NULL)
        delete FileFormat;
    /* DICOM image loaded from external data set: only delete if flag is set */
    else if (Flags & CIF_TakeOverExternalDataset)
        delete Object;
}


/********************************************************************/

DcmElement *DiDocument::search(const DcmTagKey &tag,
                               DcmObject *obj) const
{
    DcmStack stack;
    if (obj == NULL)
        obj = Object;
    if ((obj != NULL) && (obj->search(tag, stack, ESM_fromHere, OFFalse /* searchIntoSub */) == EC_Normal) &&
        (stack.top()->getLength(Xfer) > 0))
            return (DcmElement *)stack.top();
    return NULL;
}


/********************************************************************/

int DiDocument::search(const DcmTagKey &tag,
                       DcmStack &pstack) const
{
    if (pstack.empty())
        pstack.push(Object);
    DcmObject *obj = pstack.top();
    if ((obj != NULL) && (obj->search(tag, pstack, ESM_fromHere, OFFalse /* searchIntoSub */) == EC_Normal) &&
        (pstack.top()->getLength(Xfer) > 0))
            return 1;
    return 0;
}


/********************************************************************/


unsigned long DiDocument::getVM(const DcmTagKey &tag) const
{
    DcmElement *elem = search(tag);
    if (elem != NULL)
        return elem->getVM();
    return 0;
}


unsigned long DiDocument::getValue(const DcmTagKey &tag,
                                   Uint16 &returnVal,
                                   const unsigned long pos,
                                   DcmObject *item) const
{
    return getElemValue(search(tag, item), returnVal, pos);
}


unsigned long DiDocument::getValue(const DcmTagKey &tag,
                                   Sint16 &returnVal,
                                   const unsigned long pos) const
{
    DcmElement *elem = search(tag);
    if (elem != NULL)
    {
        elem->getSint16(returnVal, pos);
        return elem->getVM();
    }
    return 0;
}


unsigned long DiDocument::getValue(const DcmTagKey &tag,
                                   Uint32 &returnVal,
                                   const unsigned long pos) const
{
    DcmElement *elem = search(tag);
    if (elem != NULL)
    {
        elem->getUint32(returnVal, pos);
        return elem->getVM();
    }
    return 0;
}


unsigned long DiDocument::getValue(const DcmTagKey &tag,
                                   Sint32 &returnVal,
                                   const unsigned long pos) const
{
    DcmElement *elem = search(tag);
    if (elem != NULL)
    {
        elem->getSint32(returnVal, pos);
        return elem->getVM();
    }
    return 0;
}


unsigned long DiDocument::getValue(const DcmTagKey &tag,
                                   double &returnVal,
                                   const unsigned long pos) const
{
    DcmElement *elem = search(tag);
    if (elem != NULL)
    {
        elem->getFloat64(returnVal, pos);
        return elem->getVM();
    }
    return 0;
}


unsigned long DiDocument::getValue(const DcmTagKey &tag,
                                   const Uint16 *&returnVal,
                                   DcmObject *item) const
{
    DcmElement *elem = search(tag, item);
    if (elem != NULL)
    {
        Uint16 *val;
        elem->getUint16Array(val);
        returnVal = val;
        if (elem->getVR() == EVR_OW)
            return elem->getLength(Xfer) / sizeof(Uint16);
        return elem->getVM();
    }
    return 0;
}


unsigned long DiDocument::getValue(const DcmTagKey &tag,
                                   const char *&returnVal,
                                   DcmObject *item) const
{
    return getElemValue(search(tag, item), returnVal);
}


unsigned long DiDocument::getValue(const DcmTagKey &tag,
                                   OFString &returnVal,
                                   const unsigned long pos,
                                   DcmObject *item) const
{
    return getElemValue(search(tag, item), returnVal, pos);
}


unsigned long DiDocument::getSequence(const DcmTagKey &tag,
                                      DcmSequenceOfItems *&seq) const
{
    DcmElement *elem = search(tag);
    if ((elem != NULL) && (elem->ident() == EVR_SQ))
        return (seq =(DcmSequenceOfItems *)elem)->card();
    return 0;
}


unsigned long DiDocument::getElemValue(const DcmElement *elem,
                                       Uint16 &returnVal,
                                       const unsigned long pos)
{
    if (elem != NULL)
    {
        ((DcmElement *)elem)->getUint16(returnVal, pos);   // remove 'const' to use non-const methods
        return ((DcmElement *)elem)->getVM();
    }
    return 0;
}


unsigned long DiDocument::getElemValue(const DcmElement *elem,
                                       const Uint16 *&returnVal)
{
    if (elem != NULL)
    {
        Uint16 *val;                                    // parameter has no 'const' qualifier
        ((DcmElement *)elem)->getUint16Array(val);      // remove 'const' to use non-const methods
        returnVal = val;
        if (((DcmElement *)elem)->getVR() == EVR_OW)
            return ((DcmElement *)elem)->getLength(/*Xfer*/) / sizeof(Uint16);
        return ((DcmElement *)elem)->getVM();
    }
    return 0;
}


unsigned long DiDocument::getElemValue(const DcmElement *elem,
                                       const char *&returnVal)
{
    if (elem != NULL)
    {
        char *val;                                        // parameter has no 'const' qualifier
        ((DcmElement *)elem)->getString(val);             // remove 'const' to use non-const methods
        returnVal = val;
        return ((DcmElement *)elem)->getVM();
    }
    return 0;
}


unsigned long DiDocument::getElemValue(const DcmElement *elem,
                                       OFString &returnVal,
                                       const unsigned long pos)
{
    if (elem != NULL)
    {
        ((DcmElement *)elem)->getOFString(returnVal, pos);      // remove 'const' to use non-const methods
        return ((DcmElement *)elem)->getVM();
    }
    return 0;
}
