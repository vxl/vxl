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
 *  Purpose: Interface of class DcmFileFormat
 *
 */


#ifndef DCFILEFO_H
#define DCFILEFO_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "ofconsol.h"
#include "dctypes.h"
#include "dcerror.h"
#include "dcsequen.h"
#include "dcdatset.h"


// forward declarations
class DcmMetaInfo;
class DcmInputStream;
class DcmOutputStream;
class DcmRepresentationParameter;


/** a class handling the DICOM file format (with meta header)
 */
class DcmFileFormat
  : public DcmSequenceOfItems
{

 public:

    /** default constructor
     */
    DcmFileFormat();

    DcmFileFormat(DcmDataset *dataset);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmFileFormat(const DcmFileFormat &old);

    /** destructor
     */
    virtual ~DcmFileFormat();

    /** assignment operator
     *  @param obj fileformat to be assigned/copied
     *  @return reference to this object
     */
    DcmFileFormat &operator=(const DcmFileFormat &obj);

    /** get type identifier
     *  @return type identifier of this class (EVR_fileFormat)
     */
    virtual DcmEVR ident() const;

    /** print meta header and dataset to a stream
     *  @param out output stream
     *  @param flags optional flag used to customize the output (see DCMTypes::PF_xxx)
     *  @param level current level of nested items. Used for indentation.
     *  @param pixelFileName not used
     *  @param pixelCounter not used
     */
    virtual void print(ostream &out,
                       const size_t flags = 0,
                       const int level = 0,
                       const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL);

    virtual OFCondition validateMetaInfo(E_TransferSyntax oxfer);

    DcmMetaInfo *getMetaInfo();
    DcmDataset  *getDataset();
    DcmDataset  *getAndRemoveDataset();

    virtual Uint32 calcElementLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype);

    virtual OFBool canWriteXfer(const E_TransferSyntax newXfer,
                                const E_TransferSyntax oldXfer = EXS_Unknown);

    virtual OFCondition read(DcmInputStream &inStream,
                             const E_TransferSyntax xfer = EXS_Unknown,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength = DCM_MaxReadLength);

    /** write fileformat to a stream
     *  @param outStream DICOM output stream
     *  @param oxfer output transfer syntax
     *  @param enctype encoding types (undefined or explicit length)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition write(DcmOutputStream &outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType enctype = EET_UndefinedLength);

    virtual OFCondition write(DcmOutputStream &outStream,
                              const E_TransferSyntax oxfer,
                              const E_EncodingType enctype,
                              const E_GrpLenEncoding glenc,
                              const E_PaddingEncoding padenc = EPD_noChange,
                              const Uint32 padlen = 0,
                              const Uint32 subPadlen = 0,
                              Uint32 instanceLength = 0);

    /** write object in XML format.
     *  The XML declaration (e.g. <?xml version="1.0"?>) is not written by this function.
     *  @param out output stream to which the XML document is written
     *  @param flags optional flag used to customize the output (see DCMTypes::XF_xxx)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeXML(ostream &out,
                                 const size_t flags = 0);

    /** load object from a DICOM file.
     *  This method supports DICOM objects stored as a file (with meta header) or as a
     *  dataset (without meta header). Presence of a meta header is detected automatically.
     *  @param fileName name of the file to load
     *  @param readXfer transfer syntax used to read the data (auto detection if EXS_Unknown)
     *  @param groupLength flag, specifying how to handle the group length tags
     *  @param maxReadLength maximum number of bytes to be read for an element value.
     *    Element values with a larger size are not loaded until their value is retrieved
     *    (with getXXX()) or loadAllDataElements() is called.
     *  @param isDataset if true, meta header detection is disabled and loading of a
     *    dataset without meta header is forced.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition loadFile(const char *fileName,
                                 const E_TransferSyntax readXfer = EXS_Unknown,
                                 const E_GrpLenEncoding groupLength = EGL_noChange,
                                 const Uint32 maxReadLength = DCM_MaxReadLength,
                                 OFBool isDataset = OFFalse);

    /** save object to a DICOM file.
     *  @param fileName name of the file to save
     *  @param writeXfer transfer syntax used to write the data (EXS_Unknown means use current)
     *  @param encodingType flag, specifying the encoding with undefined or explicit length
     *  @param groupLength flag, specifying how to handle the group length tags
     *  @param padEncoding flag, specifying how to handle the padding tags
     *  @param padLength number of bytes used for the dataset padding (has to be an even number)
     *  @param subPadLength number of bytes used for the item padding (has to be an even number)
     *  @param isDataset if true, file is stored without meta header, i.e. as pure dataset
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition saveFile(const char *fileName,
                                 const E_TransferSyntax writeXfer = EXS_Unknown,
                                 const E_EncodingType encodingType = EET_UndefinedLength,
                                 const E_GrpLenEncoding groupLength = EGL_recalcGL,
                                 const E_PaddingEncoding padEncoding = EPD_noChange,
                                 const Uint32 padLength = 0,
                                 const Uint32 subPadLength = 0,
                                 OFBool isDataset = OFFalse);

    // methods for different pixel representations

    // choose Representation changes the representation of
    // PixelData Elements in the data set to the given representation
    // If the representation does not exists it creates one.
    OFCondition chooseRepresentation(const E_TransferSyntax repType,
                                     const DcmRepresentationParameter *repParam)
    {
        return getDataset()->chooseRepresentation(repType, repParam);
    }

    // checks if all PixelData elements have a conforming representation
    // (for definition of conforming representation see dcpixel.h).
    // if one PixelData element has no conforming representation
    // OFFalse is returned.
    OFBool hasRepresentation(const E_TransferSyntax repType,
                             const DcmRepresentationParameter *repParam)
    {
        return getDataset()->hasRepresentation(repType, repParam);
    }

    /** removes all but the original representation in all pixel data elements
     */
    void removeAllButOriginalRepresentations()
    {
        getDataset()->removeAllButOriginalRepresentations();
    }

    /** removes all but the current representation in all pixel data elements.
     *  Makes the current representation original.
     */
    void removeAllButCurrentRepresentations()
    {
        getDataset()->removeAllButCurrentRepresentations();
    }


// The following methods have no meaning in DcmFileFormat and shall not be
// called. Since it is not possible to delete inherited methods from a class
// stubs are defined that create an error.

    virtual OFCondition insertItem(DcmItem *item,
                                   const unsigned long where = DCM_EndOfListIndex);
    virtual DcmItem *remove(const unsigned long num);
    virtual DcmItem *remove(DcmItem *item);
    virtual OFCondition clear();


  private:

    OFCondition checkValue(DcmMetaInfo *metainfo,
                           DcmDataset *dataset,
                           const DcmTagKey &atagkey,
                           DcmObject *obj,
                           const E_TransferSyntax oxfer);

    E_TransferSyntax lookForXfer(DcmMetaInfo *metainfo);
};


#endif // DCFILEFO_H
