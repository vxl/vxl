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
 *  Purpose: Interface of class DcmAttributeTag
 *
 */


#ifndef DCVRAT_H
#define DCVRAT_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcelem.h"


/** a class representing the DICOM value representation 'Attribute Tag' (AT)
 */
class DcmAttributeTag
  : public DcmElement
{

  public:

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmAttributeTag(const DcmTag &tag,
                    const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmAttributeTag(const DcmAttributeTag &old);

    /** destructor
     */
    virtual ~DcmAttributeTag();

    /** assignment operator
     *  @param obj element to be assigned/copied
     *  @return reference to this object
     */
    DcmAttributeTag &operator=(const DcmAttributeTag &obj);

    /** get element type identifier
     *  @return type identifier of this class (EVR_AT)
     */
    virtual DcmEVR ident() const;

    /** get value multiplicity
     *  @return number of tag value pairs (group,element)
     */
    virtual unsigned long getVM();

    /** print element to a stream.
     *  The output format of the value is a backslash separated sequence of group and
     *  element value pairs, e.g. "(0008,0020)\(0008,0030)"
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

    /** get particular tag value
     *  @param tagVal reference to result variable (cleared in case of error)
     *  @param pos index of the value to be retrieved (0..vm-1)
     *  @return status status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getTagVal(DcmTagKey &tagVal,
                                  const unsigned long pos = 0);

    /** get reference to stored integer data.
     *  The array entries with an even-numbered index contain the group numbers
     *  and the odd entries contain the element numbers (see "putUint16Array()").
     *  The number of entries is twice as large as the return value of "getVM()".
     *  @param uintVals reference to result variable
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getUint16Array(Uint16 *&uintVals);

    /** get specified value as a character string.
     *  The output format is "(gggg,eeee)" where "gggg" is the hexa-decimal group
     *  number and "eeee" the hexa-decimal element number of the attribute tag.
     *  @param value variable in which the result value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param normalize not used
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFString(OFString &stringVal,
                                    const unsigned long pos,
                                    OFBool normalize = OFTrue);

    /** set particular tag value
     *  @param tagVal tag value to be set
     *  @param pos index of the value to be set (0 = first position)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition putTagVal(const DcmTagKey &tagVal,
                                  const unsigned long pos = 0);

    /** set element value to given integer array data.
     *  The array entries with an even-numbered index are expected to contain the
     *  group numbers and the odd entries to contain the element numbers, e.g.
     *  {0x0008, 0x0020, 0x0008, 0x0030}. This function uses the same format as
     *  "getUint16Array()".
     *  @param uintVals unsigned integer data to be set
     *  @param numUints number of integer values to be set (should be even)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition putUint16Array(const Uint16 *uintVals,
                                       const unsigned long numUints);

    /** set element value from the given character string.
     *  The input string is expected to be a backslash separated sequence of
     *  attribute tags, e.g. "(0008,0020)\(0008,0030)". This is the same format
     *  as used by "print()".
     *  @param stringVal input character string
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition putString(const char *stringVal);

    /** check the currently stored element value
     *  @param autocorrect correct value length if OFTrue
     *  @return status, EC_Normal if value length is correct, an error code otherwise
     */
    virtual OFCondition verify(const OFBool autocorrect = OFFalse);
};


#endif // DCVRAT_H
