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
 *  Purpose: Interface of class DcmElement
 *
 */


#ifndef DCELEM_H
#define DCELEM_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcerror.h"
#include "dctypes.h"
#include "dcobject.h"


// forward declarations
class OFString;
class DcmInputStreamFactory;


/** base class for all DICOM elements
 */
class DcmElement
  : public DcmObject
{

  public:

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmElement(const DcmTag &tag,
               const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmElement(const DcmElement &old);

    /** destructor
     */
    virtual ~DcmElement();

    /** assignment operator
     *  @param obj element to be assigned/copied
     *  @return reference to this object
     */
    DcmElement &operator=(const DcmElement &obj);

    // returns length of element with tag, vr, ...
    virtual Uint32 calcElementLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype);

    // returns length of value
    virtual Uint32 getLength(const E_TransferSyntax /*xfer*/ = EXS_LittleEndianImplicit,
                             const E_EncodingType /*enctype*/ = EET_UndefinedLength)
    {
        return Length;
    }

    virtual OFBool isLeaf() const { return OFTrue; }
    inline OFBool valueLoaded() { return fValue != NULL || Length == 0; }

    virtual void transferInit();

    virtual OFBool canWriteXfer(const E_TransferSyntax newXfer,
                                const E_TransferSyntax oldXfer);

    /** This function reads the data value of an attribute which is
     *  captured in the input stream and captures this information
     *  in this. If not all information for an attribute could be
     *  read from the stream, the function returns EC_StreamNotifyClient.
     *  Note that if certain conditions are met, this function does
     *  not actually load the data value but creates and stores an object
     *  that enables us to load this information later.
     *  @param inStream      The stream which contains the information.
     *  @param ixfer         The transfer syntax which was used to encode
     *                       the information in inStream.
     *  @param glenc         Encoding type for group length; specifies what
     *                       will be done with group length tags.
     *  @param maxReadLength Maximum read length for reading an attribute value.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition read(DcmInputStream &inStream,
                             const E_TransferSyntax ixfer,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength = DCM_MaxReadLength);

    /** This function writes this element's value to the outstream which was
     *  passed. When writing information, the byte ordering (little or big endian)
     *  of the transfer syntax which was passed will be accounted for. In case the
     *  outstream does not provide enough space for all bytes of the current
     *  element's value, only a certain part of the value will be written to the
     *  stream. This element's transfer state indicates if the all bytes of value
     *  have already been written to the stream (ERW_ready), if the writing is
     *  still in progress and more bytes need to be written to the stream
     *  (ERW_inWork) or if the writing of the bytes of this element's value has not
     *  even begun yet (ERW_init). The member variable fTransferredBytes indicates
     *  how many bytes (starting from byte 0) of this element's value have already
     *  been written to the stream. This function will return EC_Normal, if the
     *  entire value of this element has been written to the stream, it will return
     *  EC_StreamNotifyClient, if there is no more space in the buffer and _not_ all
     *  bytes of this element's value have been written, and it will return some
     *  other (error) value if there was an error.
     *  @param outStream The stream the information will be written to.
     *  @param oxfer The transfer syntax which shall be used.
     *  @param enctype encoding types (undefined or explicit length) (actually unused)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition write(DcmOutputStream &outStream,
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
     *  @param outStream DICOM output stream
     *  @param oxfer output transfer syntax
     *  @param enctype encoding types (undefined or explicit length)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition writeSignatureFormat(DcmOutputStream &outStream,
                                                                 const E_TransferSyntax oxfer,
                                                                 const E_EncodingType enctype = EET_UndefinedLength);

    virtual OFCondition clear();

    virtual OFCondition loadAllDataIntoMemory();

        // GET-Operations

    // get copies of individual components
    virtual OFCondition getUint8(Uint8 &val, const unsigned long pos = 0);
    virtual OFCondition getSint16(Sint16 &val, const unsigned long pos = 0);
    virtual OFCondition getUint16(Uint16 &val, const unsigned long pos = 0);
    virtual OFCondition getSint32(Sint32 &val, const unsigned long pos = 0);
    virtual OFCondition getUint32(Uint32 &val, const unsigned long pos = 0);
    virtual OFCondition getFloat32(Float32 &val, const unsigned long pos = 0);
    virtual OFCondition getFloat64(Float64 &val, const unsigned long pos = 0);
    virtual OFCondition getTagVal(DcmTagKey &val, const unsigned long pos = 0);

    // Gets a copy of one string value component.  For multi-valued
    // string attributes (i.e those using \ separators),
    // this method extracts the pos component (counting from zero base).
    virtual OFCondition getOFString(OFString &str,
                                    const unsigned long pos,
                                    OFBool normalize = OFTrue);

    /** get entire element value as a character string.
     *  In case of VM > 1 the single values are separated by a backslash ('\').
     *  This method implements a general approach by concatenating the results of
     *  getOFString() for each value component. Derived class may implement more
     *  sophisticated methods.
     *  @param value variable in which the result value is stored
     *  @param normalize normalize each element value prior to concatenation
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFStringArray(OFString &value,
                                         OFBool normalize = OFTrue);

    // The following get operations do not copy,
    // they return a reference of the element value
    // The element value remains under control of the element
    // and is only valid until the next put.., read, or write
    // operation.
    virtual OFCondition getString(char *&val);        // for strings
    virtual OFCondition getUint8Array(Uint8 *&val);   // for bytes
    virtual OFCondition getSint16Array(Sint16 *&val);
    virtual OFCondition getUint16Array(Uint16 *&val);
    virtual OFCondition getSint32Array(Sint32 *&val);
    virtual OFCondition getUint32Array(Uint32 *&val);
    virtual OFCondition getFloat32Array(Float32 *&val);
    virtual OFCondition getFloat64Array(Float64 *&val);

    // detachValueField detaches the value field from the
    // DICOM element. After detaching the calling part of the
    // application has total control over the element value, especially
    // the value must be deleted from the heap after use.
    // The DICOM element remains a copy of the value if the copy
    // parameter is OFTrue else the value is erased in the DICOM
    // element.
    OFCondition detachValueField(OFBool copy = OFFalse);


// PUT-Operations
// Put operations copy the value.

    // Sets the value of a complete (possibly multi-valued) string attribute.
    virtual OFCondition putOFStringArray(const OFString &stringValue);

    // One Value
    virtual OFCondition putString(const char *val);

    // One Value at a position pos
    virtual OFCondition putSint16(const Sint16 val, const unsigned long pos = 0);
    virtual OFCondition putUint16(const Uint16 val, const unsigned long pos = 0);
    virtual OFCondition putSint32(const Sint32 val, const unsigned long pos = 0);
    virtual OFCondition putUint32(const Uint32 val, const unsigned long pos = 0);
    virtual OFCondition putFloat32(const Float32 val, const unsigned long pos = 0);
    virtual OFCondition putFloat64(const Float64 val, const unsigned long pos = 0);
    virtual OFCondition putTagVal(const DcmTagKey &attrTag, const unsigned long pos = 0);

    // num Values
    virtual OFCondition putUint8Array(const Uint8 *vals, const unsigned long num);
    virtual OFCondition putSint16Array(const Sint16 *vals, const unsigned long num);
    virtual OFCondition putUint16Array(const Uint16 *vals, const unsigned long num);
    virtual OFCondition putSint32Array(const Sint32 *vals, const unsigned long num);
    virtual OFCondition putUint32Array(const Uint32 *vals, const unsigned long num);
    virtual OFCondition putFloat32Array(const Float32 *vals, const unsigned long num);
    virtual OFCondition putFloat64Array(const Float64 *vals, const unsigned long num);


  protected:

    E_ByteOrder fByteOrder;

    /** This function returns this element's value. The returned value
     *  corresponds to the byte ordering (little or big endian) that
     *  was passed.
     *  @param newByteOrder The byte ordering that shall be accounted
     *                      for (little or big endian).
     */
    void *getValue(const E_ByteOrder newByteOrder = gLocalByteOrder);


    OFCondition changeValue(const void *value,      // new Value
                            const Uint32 position,  // position in value array
                            const Uint32 num);      // number of new value bytes

    OFCondition putValue(const void *value,     // new value
                         const Uint32 length);  // number of new value bytes

    OFCondition createEmptyValue(const Uint32 length); // number of new value bytes




    /** This function reads the data value of an attribute and stores the
     *  information which was read in this. The information is either read
     *  from the inStream or (if inStream is NULL) from a different stream
     *  which was created earlier and which is accessible through the fLoadValue
     *  member variable. Note that if not all information for an attribute
     *  could be read from the stream, the function returns EC_StreamNotifyClient.
     *  @param inStream The stream which contains the information.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition loadValue(DcmInputStream *inStream = NULL);

    virtual void postLoadValue();

    /** This function creates a byte array of Length bytes and returns this
     *  array. In case Length is odd, an array of Length+1 bytes will be
     *  created and Length will be increased by 1.
     */
    virtual Uint8 *newValueField();

    void swapValueField(size_t valueWidth);

    /** write element start tag in XML format
     *  @param out output stream to which the XML start tag is written
     *  @param flags flag used to customize the output (not yet used)
     *  @param attrText extra attribute text to be added to the element tag
     */
    virtual void writeXMLStartTag(ostream &out,
                                  const size_t flags,
                                  const char *attrText = NULL);

    /** write element end tag in XML format
     *  @param out output stream to which the XML end tag is written
     *  @param flags flag used to customize the output (not yet used)
     */
    virtual void writeXMLEndTag(ostream &out,
                                const size_t flags);


  private:

    /// required information to load value later
    DcmInputStreamFactory *fLoadValue;

    /// value of the element
    Uint8 *fValue;
};


#endif // DCELEM_H
