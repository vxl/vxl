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
 *  Purpose: Interface of abstract class DcmCodec and the class DcmCodecStruct
 *
 */

#ifndef DCCODEC_H
#define DCCODEC_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */
#include "dctypes.h"
#include "dcerror.h"
#include "dcxfer.h"
#include "oflist.h"

class DcmStack;
class DcmRepresentationParameter;
class DcmPixelSequence;
class DcmPolymorphOBOW;
class DcmItem;
class DcmTagKey;

/** abstract base class for a codec parameter object that
 *  describes the settings (modes of operations) for one
 *  particular codec (DcmCodec) object.
 */
class DcmCodecParameter
{
public:
    /// default constructor
    DcmCodecParameter() {}

    /// copy constructor
    DcmCodecParameter(const DcmCodecParameter&) {}

    /// destructor
    virtual ~DcmCodecParameter() {}

    /** this methods creates a copy of type DcmCodecParameter *
     *  it must be overweritten in every subclass.
     *  @return copy of this object
     */
    virtual DcmCodecParameter *clone() const = 0;

    /** returns the class name as string.
     *  can be used as poor man's RTTI replacement.
     */
    virtual const char *className() const = 0;

};


/** abstract base class for a codec object that can be registered
 *  in dcmdata and performs transfer syntax transformation (i.e.
 *  compressing, decompressing or transcoding between different
 *  compressed transfer syntaxes).
 *  When dcmdata is requested to write a transfer syntax that differs
 *  from the current one (i.e. the one in which the object was read),
 *  dcmdata dynamically searches for a DcmCodec object that is able
 *  to create the desired transfer syntax.  If no suitable codec
 *  is found, the write operation fails.
 */
class DcmCodec
{
public:
  /// default constructor
  DcmCodec() {}

  /** decompresses the given pixel sequence and
   *  stores the result in the given uncompressedPixelData element.
   *  @param fromRepParam current representation parameter of compressed data, may be NULL
   *  @param pixSeq compressed pixel sequence
   *  @param uncompressedPixelData uncompressed pixel data stored in this element
   *  @param cp codec parameters for this codec
   *  @param objStack stack pointing to the location of the pixel data
   *    element in the current dataset.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  virtual OFCondition decode(
      const DcmRepresentationParameter * fromRepParam,
      DcmPixelSequence * pixSeq,
      DcmPolymorphOBOW& uncompressedPixelData,
      const DcmCodecParameter * cp,
      const DcmStack& objStack) const = 0;

  /** compresses the given uncompressed DICOM image and stores
   *  the result in the given pixSeq element.
   *  @param pixelData pointer to the uncompressed image data in OW format
   *    and local byte order
   *  @param length of the pixel data field in bytes
   *  @param toRepParam representation parameter describing the desired
   *    compressed representation (e.g. JPEG quality)
   *  @param pixSeq compressed pixel sequence (pointer to new DcmPixelSequence object
   *    allocated on heap) returned in this parameter upon success.
   *  @param cp codec parameters for this codec
   *  @param objStack stack pointing to the location of the pixel data
   *    element in the current dataset.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  virtual OFCondition encode(
      const Uint16 * pixelData,
      const Uint32 length,
      const DcmRepresentationParameter * toRepParam,
      DcmPixelSequence * & pixSeq,
      const DcmCodecParameter *cp,
      DcmStack & objStack) const = 0;

  /** transcodes (re-compresses) the given compressed DICOM image and stores
   *  the result in the given toPixSeq element.
   *  @param fromRepType current transfer syntax of the compressed image
   *  @param fromRepParam current representation parameter of compressed data, may be NULL
   *  @param fromPixSeq compressed pixel sequence
   *  @param toRepParam representation parameter describing the desired
   *    new compressed representation (e.g. JPEG quality)
   *  @param toPixSeq compressed pixel sequence (pointer to new DcmPixelSequence object
   *    allocated on heap) returned in this parameter upon success.
   *  @param cp codec parameters for this codec
   *  @param objStack stack pointing to the location of the pixel data
   *    element in the current dataset.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  virtual OFCondition encode(
      const E_TransferSyntax fromRepType,
      const DcmRepresentationParameter * fromRepParam,
      DcmPixelSequence * fromPixSeq,
      const DcmRepresentationParameter * toRepParam,
      DcmPixelSequence * & toPixSeq,
      const DcmCodecParameter * cp,
      DcmStack & objStack) const = 0;

  /** checks if this codec is able to convert from the
   *  given current transfer syntax to the given new
   *  transfer syntax
   *  @param oldRepType current transfer syntax
   *  @param newRepType desired new transfer syntax
   *  @return true if transformation is supported by this codec, false otherwise.
   */
  virtual OFBool canChangeCoding(
      const E_TransferSyntax oldRepType,
      const E_TransferSyntax newRepType) const = 0;


  // static helper methods that have proven useful in codec classes derived from DcmCodec

  /** helper function that inserts a string attribute with a given value into a dataset
   *  if missing in the dataset.
   *  @param dataset dataset to insert to, must not be NULL.
   *  @param tag tag key of attribute to check/insert
   *  @param val string value, may be NULL.
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition insertStringIfMissing(DcmItem *dataset, const DcmTagKey& tag, const char *val);

  /** helper function that converts a dataset containing a DICOM image
   *  into a valid (standard extended) Secondary Capture object
   *  by inserting all attributes that are type 1/2 in Secondary Capture
   *  and missing in the source dataset.  Replaces SOP Class UID
   *  by Secondary Capture. It does not, however, change an existing SOP Instance UID.
   *  @param dataset dataset to insert to, must not be NULL.
   *  @param tag tag key of attribute to check/insert
   *  @param val string value, may be NULL.
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition convertToSecondaryCapture(DcmItem *dataset);

  /** create new SOP instance UID and Source Image Sequence
   *  referencing the old SOP instance (if present)
   *  @param dataset dataset to be modified
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition newInstance(DcmItem *dataset);

  /** set first two values of Image Type to DERIVED\SECONDARY.
   *  @param dataset dataset to be modified
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition updateImageType(DcmItem *dataset);

};


/** singleton that keeps list of registered codecs for dcmdata.
 *  All operations on the list are protected by a read/write lock
 *  and, therefore, are safe for multi-thread applications.
 */
class DcmCodecList
{

public:

  /// destructor
  virtual ~DcmCodecList();

  /** registers a codec object in the global list of codecs consulted by dcmdata
   *  whenever conversion to/from compressed transfer syntaxes is requested.
   *  This function must not be called before main() is started, e.g. from
   *  a constructor of a global object.
   *  This call is safe in multi-thread operations.
   *  @param aCodec pointer to codec object.
   *    Must remain unmodified and valid until the codec has been deregistered.
   *  @param aDefaultRepParam default representation parameter.
   *    Must remain unmodified and valid until the codec has been deregistered.
   *  @param aCodecParameter codec parameter.
   *    Must remain unmodified and valid until the codec has been deregistered or the
   *    parameter has been replaced by a call to updateCodecParameter()
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition registerCodec(
    const DcmCodec *aCodec,
    const DcmRepresentationParameter *aDefaultRepParam,
    const DcmCodecParameter *aCodecParameter);

  /** deregisters a codec and it's parameter objects from the global list.
   *  This call is safe in multi-thread operations.
   *  @param aCodec pointer to registered codec
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition deregisterCodec(const DcmCodec *aCodec);

  /** updates the codec parameters object for a codec that has been registered before.
   *  This call is safe in multi-thread operations.
   *  @param aCodec pointer to codec object that has been registered before
   *  @param aCodecParameter codec parameter.
   *    Must remain unmodified and valid until the codec has been deregistered or the
   *    parameter has been replaced by another call to updateCodecParameter()
   *  @return EC_Normal if successful, an error code otherwise
   */
  static OFCondition updateCodecParameter(
    const DcmCodec *aCodec,
    const DcmCodecParameter *aCodecParameter);

  /** looks for a codec that is able to decode from the given transfer syntax
   *  and calls the decode() method of the codec.  A read lock on the list of
   *  codecs is acquired until this method returns.
   *  @param fromType transfer syntax to decode from
   *  @param fromParam representation parameter of current compressed
   *    representation, may be NULL.
   *  @param fromPixSeq compressed pixel sequence
   *  @param uncompressedPixelData uncompressed pixel data stored in this element
   *  @param pixelStack stack pointing to the location of the pixel data
   *    element in the current dataset.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  static OFCondition decode(
    const DcmXfer & fromType,
    const DcmRepresentationParameter * fromParam,
    DcmPixelSequence * fromPixSeq,
    DcmPolymorphOBOW& uncompressedPixelData,
    DcmStack & pixelStack);

  /** looks for a codec that is able to encode from the given transfer syntax
   *  and calls the encode() method of the codec.  A read lock on the list of
   *  codecs is acquired until this method returns.
   *  @param fromRepType transfer syntax to encode from
   *  @param pixelData pointer to the uncompressed image data in OW format
   *    and local byte order
   *  @param length of the pixel data field in bytes
   *  @param toRepType transfer syntax to compress to
   *  @param toRepParam representation parameter describing the desired
   *    compressed representation (e.g. JPEG quality)
   *  @param pixSeq compressed pixel sequence (pointer to new DcmPixelSequence object
   *    allocated on heap) returned in this parameter upon success.
   *  @param pixelStack stack pointing to the location of the pixel data
   *    element in the current dataset.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  static OFCondition encode(
    const E_TransferSyntax fromRepType,
    const Uint16 * pixelData,
    const Uint32 length,
    const E_TransferSyntax toRepType,
    const DcmRepresentationParameter * toRepParam,
    DcmPixelSequence * & toPixSeq,
    DcmStack & pixelStack);

  /** looks for a codec that is able to transcode (re-compresses)
   *  from the given transfer syntax to the given transfer syntax
   *  and calls the encode() method of the codec.
   *  A read lock on the list of
   *  codecs is acquired until this method returns.
   *  @param fromRepType current transfer syntax of the compressed image
   *  @param fromParam current representation parameter of compressed data, may be NULL
   *  @param fromPixSeq compressed pixel sequence
   *  @param toRepType transfer syntax to compress to
   *  @param toRepParam representation parameter describing the desired
   *    new compressed representation (e.g. JPEG quality)
   *  @param toPixSeq compressed pixel sequence (pointer to new DcmPixelSequence object
   *    allocated on heap) returned in this parameter upon success.
   *  @param pixelStack stack pointing to the location of the pixel data
   *    element in the current dataset.
   *  @return EC_Normal if successful, an error code otherwise.
   */
  static OFCondition encode(
    const E_TransferSyntax fromRepType,
    const DcmRepresentationParameter * fromParam,
    DcmPixelSequence * fromPixSeq,
    const E_TransferSyntax toRepType,
    const DcmRepresentationParameter * toRepParam,
    DcmPixelSequence * & toPixSeq,
    DcmStack & pixelStack);


  /** looks for a codec that claims to be able to convert
   *  between the given transfer syntaxes.
   *  A read lock on the list of
   *  codecs is acquired until this method returns.
   *  @param fromRepType current transfer syntax
   *  @param toRepType desired new transfer syntax
   *  @return true if transformation is supported by a codec, false otherwise.
   */
  static OFBool canChangeCoding(
    const E_TransferSyntax fromRepType,
    const E_TransferSyntax toRepType);

private:

  /** constructor
   *  @param aCodec pointer to codec object.
   *  @param aDefaultRepParam default representation parameter.
   *  @param aCodecParameter codec parameter.
   */
  DcmCodecList(
    const DcmCodec *aCodec,
    const DcmRepresentationParameter *aDefaultRepParam,
    const DcmCodecParameter *aCodecParameter);

  /// private undefined copy constructor
  DcmCodecList(const DcmCodecList &);

  /// private undefined copy assignment operator
  DcmCodecList &operator=(const DcmCodecList &);

  /// pointer to codec object
  const DcmCodec * codec;

  /// pointer to default representation parameter
  const DcmRepresentationParameter * defaultRepParam;

  /// pointer to codec parameter set
  const DcmCodecParameter * codecParameter;

  /// singleton list of registered codecs
  static OFList<DcmCodecList *> registeredCodecs;

#ifdef _REENTRANT
  /// read/write lock guarding access to singleton list
  static OFReadWriteLock codecLock;
#endif

  // dummy friend declaration to prevent gcc from complaining
  // that this class only defines private constructors and has no friends.
  friend class DcmCodecListDummyFriend;
};


#endif
