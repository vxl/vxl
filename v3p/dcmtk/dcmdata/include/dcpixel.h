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
 *  Author:  Andreas Barth
 *
 *  Purpose: Interface of class DcmPixelData
 *
 */

#ifndef DCPIXEL_H
#define DCPIXEL_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "ofconsol.h"
#include "dcvrpobw.h"
#include "oflist.h"

class DcmCodec;
class DcmCodecList;
class DcmStack;
class DcmPixelSequence;
class DcmPixelData;
class DcmRepresentationEntry;

class DcmRepresentationParameter
{
public:
    /// default constructor
    DcmRepresentationParameter() {}

    /// copy constructor
    DcmRepresentationParameter(const DcmRepresentationParameter&) {}

    /// destructor
    virtual ~DcmRepresentationParameter() {}

    /** this methods creates a copy of type DcmRepresentationParameter *
     *  it must be overweritten in every subclass.
     *  @return copy of this object
     */
    virtual DcmRepresentationParameter *clone() const = 0;

    /** returns the class name as string.
     *  can be used in operator== as poor man's RTTI replacement.
     */
    virtual const char *className() const = 0;

    /** compares an object to another DcmRepresentationParameter.
     *  Implementation must make sure that classes are comparable.
     *  @param arg representation parameter to compare with
     *  @return true if equal, false otherwise.
     */
    virtual OFBool operator==(const DcmRepresentationParameter &arg) const = 0;
};

typedef OFList<DcmRepresentationEntry *> DcmRepresentationList;
typedef OFListIterator(DcmRepresentationEntry *) DcmRepresentationListIterator;

// The class DcmPixelData stores different pixel representations identified by
// a type (the transfer syntax) and some representation parameters
// The three unencapsulated transfer syntaxes belong to the same pixel
// representation.
// A type (or transfer syntax) conforms to a representation if
// the type and the representation type are equal or both are unencapsulated.
// If this is valid for the representation read or set by chooseRepresentation
// then this representation is the conforming representation.
// else a representation with the default parameter set defined in the
// codec is the conforming representation.

class DcmPixelData : public DcmPolymorphOBOW
{
private:
    friend class DcmRepresentationEntry;

    /// List of representations of pixel data
    DcmRepresentationList repList;

    /// Iterator to the last dummy element in representation lis
    DcmRepresentationListIterator repListEnd;

    /// Iterator to the original representation. if an uncompressed
    /// representation is used the iterator points to repList.end()
    DcmRepresentationListIterator original;

    /// current list element for some operations
    DcmRepresentationListIterator current;

    /// shows if an unencapsulated representation is stored
    OFBool existUnencapsulated;

    /// value representation of unencapsulated data
    DcmEVR unencapsulatedVR;

    /// in write function: pointer to current pixel sequence
    DcmPixelSequence * pixelSeqForWrite;

    /** This function removes all pixel representations from the list
     *  of pixel representations except the one which was passed. Note
     *  that if parameter leaveInList equals repListEnd, all representations
     *  will be removed from the list.
     *  @param leaveInList Iterator to a representation which shall not
     *                     be removed from the list of representations.
     */
    void clearRepresentationList(
        DcmRepresentationListIterator leaveInList);

    /** find a conforming representation in the list of
     *  encapsulated representations
     */
    OFCondition findConformingEncapsulatedRepresentation(
        const DcmXfer & repType,
        const DcmRepresentationParameter * repParam,
        DcmRepresentationListIterator & result);


    /** find a representation entry and return an iterator to the found entry
     *  or the next element in the list. The condition returned can be EC_Normal
     *  if such an entry is found or EC_RepresentationNotFound. The pixSeq
     *  attribute in findEntry can be NULL, it is not needed for the find
     *  operation!
     */
    OFCondition findRepresentationEntry(
        const DcmRepresentationEntry & findEntry,
        DcmRepresentationListIterator & result);

    /** insert or replace a representation entry in the list
     */
    DcmRepresentationListIterator insertRepresentationEntry(
        DcmRepresentationEntry * repEntry);

    /** decode representation to unencapsulated format
     */
    OFCondition decode(
        const DcmXfer & fromType,
        const DcmRepresentationParameter * fromParam,
        DcmPixelSequence * fromPixSeq,
        DcmStack & pixelStack);

    /** encode to encapsulated format
     */
    OFCondition encode(
        const DcmXfer & fromType,
        const DcmRepresentationParameter * fromParam,
        DcmPixelSequence * fromPixSeq,
        const DcmXfer & toType,
        const DcmRepresentationParameter *toParam,
        DcmStack & pixelStack);

    void recalcVR()
    {
        if (current == repList.end()) Tag.setVR(unencapsulatedVR);
        else Tag.setVR(EVR_OB);
    }

public:
    DcmPixelData(const DcmTag & tag, const Uint32 len = 0);
    DcmPixelData(const DcmPixelData & pixelData);
    virtual ~DcmPixelData();

    DcmPixelData &operator=(const DcmPixelData &obj);

    virtual OFCondition setVR(DcmEVR vr);
    virtual DcmEVR ident() const { return EVR_PixelData; }

    virtual void print(ostream &out,
                       const size_t flags = 0,
                       const int level = 0,
                       const char *pixelFileName = NULL,
                       size_t *pixelCounter = NULL);

    /** tests if it is possible to write a specific representation
     *  Only existing representations are considered, since this
     *  method does not create a representation.
     */
    virtual OFBool canWriteXfer(const E_TransferSyntax newXfer,
                                const E_TransferSyntax oldXfer);

    /** returns length of representation conforming to the
     *  transfer syntax with tag, vr, ... It does not create a
     *  representation. If no conforming representation exists an
     *  error code is set and 0 returned.
     */
    virtual Uint32 calcElementLength(const E_TransferSyntax xfer,
                                     const E_EncodingType enctype);

    /** returns length of representation value field conforming to
     *  given transfer syntax. It does not create a representation.
     *  If no conforming representation exists, an error code is set
     *  and 0 returned.
     */
    virtual Uint32 getLength(
        const E_TransferSyntax xfer  = EXS_LittleEndianImplicit,
        const E_EncodingType enctype = EET_UndefinedLength);

    /** Initialize a streaming operation (read, write)
     */
    virtual void transferInit();

    /** This function reads the data value of a pixel data attribute
     *  which is captured in the input stream and captures this
     *  information in this. This function takes into account, if the
     *  pixel data is captured in native (uncompressed) or encapsulated
     *  (compressed) format.
     *  @param inStream      The stream which contains the information.
     *  @param ixfer         The transfer syntax which was used to encode the
     *                       information in inStream.
     *  @param glenc         Encoding type for group length; specifies what
     *                       will be done with group length tags.
     *  @param maxReadLength Maximum read length for reading an attribute value.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition read(DcmInputStream & inStream,
                             const E_TransferSyntax ixfer,
                             const E_GrpLenEncoding glenc = EGL_noChange,
                             const Uint32 maxReadLength = DCM_MaxReadLength);

    /** writes a representation conforming to the transfer syntax
     *  It does not create a representation. If no conforming
     *  representation exists,  an error code is returned.
     *  The written representation is the new current representation
     */
    virtual OFCondition write(
        DcmOutputStream & outStream,
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
    virtual OFCondition writeSignatureFormat(
        DcmOutputStream & outStream,
        const E_TransferSyntax oxfer,
        const E_EncodingType enctype = EET_UndefinedLength);

    virtual OFCondition loadAllDataIntoMemory(void);

    /** Finalize a streaming operation (read, write)
     */
    virtual void transferEnd();

    /** test if it is possible to choose the representation in the parameters
     */
    OFBool canChooseRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam);

    /** choose a specific representation. if representation does not exist
     *  it is created (if possible).
     *  if repParam is zero, a representation is chosen or created that
     *  is equal to the default representation parameters
     */
    OFCondition chooseRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam,
        DcmStack & stack);

    /** Inserts an original encapsulated representation. current and original
     *  representations are changed, all old representations are deleted
     */
    void putOriginalRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam,
        DcmPixelSequence * pixSeq);

    /** The following two put-methods insert an original unencapsulated
     *  representation. current and original representations are changed,
     *  all old representations are deleted
     */
    virtual OFCondition putUint16Array(
        const Uint16 * wordValue,
        const unsigned long length);

    virtual OFCondition putUint8Array(
        const Uint8 * byteValue,
        const unsigned long length);


    /** get a specific exisiting Representation, creates no representation
     *  if repParam is NULL, then the representation conforming to the default
     *  presentationParameters (defined with the codec) is returned.
     */
    OFCondition getEncapsulatedRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam,
        DcmPixelSequence * & pixSeq);

    /** returns the representation identification (key) consisting of
     *  representation type and parameter of the original representation
     */
    void getOriginalRepresentationKey(
        E_TransferSyntax & repType,
        const DcmRepresentationParameter * & repParam);

    /** returns the representation identification (key) consisting of
     *  representation type and parameter of the current representation
     */
    void getCurrentRepresentationKey(
        E_TransferSyntax & repType,
        const DcmRepresentationParameter * & repParam);

    /** sets the representation identification parameter of the current
     *  representation
     */
    OFCondition setCurrentRepresentationParameter(
        const DcmRepresentationParameter * repParam);

    /** returns if a specific conforming  representation exists.
     */
    OFBool hasRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam = NULL);

    /** delete a representation. It is not possible to delete the
     *  original representation with this method
     */
    OFCondition removeRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam);

    /** removes all but the original representation
     */
    void removeAllButOriginalRepresentations();

    /** removes all but the current representation
     *  Makes the current representation original
     */
    void removeAllButCurrentRepresentations();

    /** delete original representation and set new original representation.
     *  If the new representation does not exist, the original one is not
     *  deleted and an error code returns
     */
    OFCondition removeOriginalRepresentation(
        const E_TransferSyntax repType,
        const DcmRepresentationParameter * repParam);
};

#endif
