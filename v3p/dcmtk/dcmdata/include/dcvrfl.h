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
 *  Purpose: Interface of class DcmFloatingPointSingle
 *
 */


#ifndef DCVRFL_H
#define DCVRFL_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcelem.h"


/** a class representing the DICOM value representation 'Floating Point Single' (FL)
 */
class DcmFloatingPointSingle
  : public DcmElement
{

  public:

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmFloatingPointSingle(const DcmTag &tag,
                           const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmFloatingPointSingle(const DcmFloatingPointSingle &old);

    /** destructor
     */
    virtual ~DcmFloatingPointSingle();

    /** assignment operator
     *  @param obj element to be assigned/copied
     *  @return reference to this object
     */
    DcmFloatingPointSingle &operator=(const DcmFloatingPointSingle &obj);

    /** get element type identifier
     *  @return type identifier of this class (EVR_FL)
     */
    virtual DcmEVR ident(void) const;

    /** get value multiplicity
     *  @return number of currently stored values
     */
    virtual unsigned long getVM();

    /** print element to a stream.
     *  The output format of the value is a backslash separated sequence of numbers.
     *  This function uses a variable number of digits for the floating point values
     *  as created by the OFStandard::ftoa() function by default.
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

    /** get particular float value
     *  @param floatVal reference to result variable (cleared in case of error)
     *  @param pos index of the value to be retrieved (0..vm-1)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getFloat32(Float32 &floatVal,
                                   const unsigned long pos = 0);

    /** get reference to stored float data
     *  @param floatVals reference to result variable
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getFloat32Array(Float32 *&floatVals);

    /** get a particular value as a character string.
     *  The resulting string contains a variable number of digits as created by
     *  the OFStandard::ftoa() function by default.
     *  @param stringVal variable in which the result value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param normalize not used
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFString(OFString &stringVal,
                                    const unsigned long pos,
                                    OFBool normalize = OFTrue);

    /** set particular element value to given float
     *  @param floatVal floating point value to be set
     *  @param pos index of the value to be set (0 = first position)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition putFloat32(const Float32 floatVal,
                                                   const unsigned long pos = 0);

    /** set element value to given float array data
     *  @param floatVals floating point data to be set
     *  @param numFloats number of floating point values to be set
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition putFloat32Array(const Float32 *floatVals,
                                                            const unsigned long numFloats);

    /** set element value from the given character string.
     *  The input string is expected to be a backslash separated sequence of
     *  numeric characters, e.g. "12.3456\1\-123.456\1234.0".
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


#endif // DCVRFL_H
