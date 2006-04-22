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
 *  Purpose: Interface of class DcmByteString
 *
 */


#ifndef DCBYTSTR_H
#define DCBYTSTR_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "ofconsol.h"
#include "dcerror.h"
#include "dctypes.h"
#include "dcelem.h"


// forward declaration
class OFString;


/** base class for all DICOM value representations storing a character string
 */
class DcmByteString
  : public DcmElement
{

    // internal type used to specify the current string representation
    enum E_StringMode
    {
        // string has internal representation (no padding)
        DCM_MachineString,
        // string has DICOM representation (even length)
        DCM_DicomString,
        // string has unknown representation (maybe multiple padding chars?)
        DCM_UnknownString
    };


 public:

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmByteString(const DcmTag &tag,
                  const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmByteString(const DcmByteString &old);

    /** destructor
     */
    virtual ~DcmByteString();

    /** assignment operator
     *  @param obj element to be assigned/copied
     *  @return reference to this object
     */
    DcmByteString& operator=(const DcmByteString& obj);

    /** get element type identifier
     *  @return type identifier of this class (EVR_UNKNOWN)
     */
    virtual DcmEVR ident() const;

    /** clear the currently stored value
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition clear();

    /** get value multiplicity
     *  @return number of string components (separated by a backslash)
     */
    virtual unsigned long getVM();

    /** get length of the stored value.
     *  Trailing spaces (padding characters) are ignored for the "real" length.
     *  @return number of characters stored for the string value
     */
    Uint32 getRealLength();

    /** get DICOM length of the stored value.
     *  The string value is padded if required.  Therefore, the returned length
     *  always has an even value.
     *  @param xfer not used
     *  @param enctype not used
     *  @return number of characters stored in DICOM representation
     */
    virtual Uint32 getLength(const E_TransferSyntax xfer = EXS_LittleEndianImplicit,
                             const E_EncodingType enctype = EET_UndefinedLength);

    /** print element to a stream.
     *  The output format of the value is a backslash separated sequence of string
     *  components (if any).
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

    /** write data element to a stream
     *  @param outStream output stream
     *  @param writeXfer transfer syntax used to write the data
     *  @param encodingType flag, specifying the encoding with undefined or explicit length
     */
    virtual OFCondition write(DcmOutputStream &outStream,
                              const E_TransferSyntax writeXfer,
                              const E_EncodingType encodingType = EET_UndefinedLength);

    /** write data element to a stream as required for the creation of digital signatures
     *  @param outStream output stream
     *  @param writeXfer transfer syntax used to write the data
     *  @param encodingType flag, specifying the encoding with undefined or explicit length
     */
    virtual OFCondition writeSignatureFormat(DcmOutputStream &outStream,
                                             const E_TransferSyntax writeXfer,
                                             const E_EncodingType encodingType = EET_UndefinedLength);

    /** get a copy of a particular string component
     *  @param stringVal variable in which the result value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param normalize not used since string normalization depends on value representation
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFString(OFString &stringVal,
                                    const unsigned long pos,
                                    OFBool normalize = OFTrue);

    /** get a pointer to the current string value.
     *  This includes all string components and separators. NB: this method does
     *  not copy the stored value.
     *  @param stringVal reference to the pointer variable
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getString(char *&stringVal);

    /** set element value from the given character string
     *  @param stringVal input character string (possibly multi-valued)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition putString(const char *stringVal);

    /** set element value from the given character string.
     *  @param stringVal input character string (possibly multi-valued)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition putOFStringArray(const OFString &stringVal);

    /** check the currently stored string value.
     *  Checks every string component for the maximum length specified for the particular
     *  value representation.
     *  @param autocorrect correct value and value component length if OFTrue
     *  @return status, EC_Normal if value length is correct, an error code otherwise
     */
    virtual OFCondition verify(const OFBool autocorrect = OFFalse);


 protected:

    /** create a new value field (string buffer) of the previously defined size
     *  (member variable 'Length'). Also handles odd value length by allocating
     *  extra space for the padding character.
     *  This method is used by derived classes only.
     *  @return pointer to the newly created value field
     */
    virtual Uint8 *newValueField();

    /** method is called after the element value has been loaded.
     *  Can be used to correct the value before it is used for the first time.
     */
    virtual void postLoadValue();

    /** convert currently stored string value to internal representation.
     *  It removes any trailing space character and recomputes the string length.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition makeMachineByteString();

    /** convert currently stored string value to DICOM representation.
     *  It removes trailing spaces apart from a possibly required single padding
     *  character (in case of odd string length).
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition makeDicomByteString();

    /** get a copy of the current string value.
     *  This includes all string components and separators.
     *  @param stringVal variable in which the result is stored
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    OFCondition getStringValue(OFString &stringVal);

    /// padding character used to adjust odd value length (space)
    char paddingChar;
    /// maximum number of characters for each string component
    Uint32 maxLength;


 private:

    /// number of characters of the internal string representation
    Uint32 realLength;
    /// current representation of the string value
    E_StringMode fStringMode;
};


/** @name string normalization flags.
 *  These flags can be used with normalizeString() to specify the extent of normalization.
 */
//@{

/// delete trailing spaces
const OFBool DELETE_TRAILING = OFTrue;
/// delete leading spaces
const OFBool DELETE_LEADING = OFTrue;
/// handle string as multi-valued (components separated by a backslash)
const OFBool MULTIPART = OFTrue;

//@}


/* Function to get part out of a String for VM > 1 */

/** extract particular component from a string value.
 *  String components are expected to be separated by a backslash character ('\').
 *  @param result reference to the result variable
 *  @param orgStr input string value
 *  @param pos index of the string component to be extracted (0..vm-1)
 *  @return status, EC_Normal if successful, an error code otherwise
 */
OFCondition getStringPart(OFString &result,
                          const char *orgStr,
                          const unsigned long pos);


/** normalize the given string value, i.e. remove leading and/or trailing spaces
 *  @param string input and output string value to be normalized
 *  @param multiPart handle string as multi-valued if OFTrue
 *  @param leading delete leading spaces if OFTrue
 *  @param trailing delete trailing spaces if OFTrue
 */
void normalizeString(OFString &string,
                     const OFBool multiPart,
                     const OFBool leading,
                     const OFBool trailing);


#endif // DCBYTSTR_H
