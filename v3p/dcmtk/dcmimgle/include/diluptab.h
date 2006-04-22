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
 *  Purpose: DicomLookupTable (Header)
 *
 */


#ifndef __DILUPTAB_H
#define __DILUPTAB_H

#include "osconfig.h"
#include "dctagkey.h"

#include "dibaslut.h"
#include "diobjcou.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DcmObject;
class DcmUnsignedShort;
class DcmLongString;
class DiDocument;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class to handle color palette look-up tables
 */
class DiLookupTable
  : public DiBaseLUT,
    public DiObjectCounter
{
 public:

    /** constructor
     *
     ** @param  docu         pointer to dataset (encapsulated)
     *  @param  descriptor   tag key containing the LUT descriptor
     *  @param  data         tag key containing the LUT data
     *  @param  explanation  tag key containing the LUT explanation
     *  @param  status       pointer to image status variable (optional)
     */
    DiLookupTable(const DiDocument *docu,
                  const DcmTagKey &descriptor,
                  const DcmTagKey &data,
                  const DcmTagKey &explanation,
                  EI_Status *status = NULL);

    /** constructor
     *
     ** @param  docu         pointer to dataset (encapsulated)
     *  @param  sequence     tag key containing the LUT sequence
     *  @param  descriptor   tag key containing the LUT descriptor
     *  @param  data         tag key containing the LUT data
     *  @param  explanation  tag key containing the LUT explanation
     *  @param  pos          position in the LUT sequence which should be used (optional)
     *  @param  card         pointer to storage area where the number of sequence entries
     *                       should be stored (optional)
     */
    DiLookupTable(const DiDocument *docu,
                  const DcmTagKey &sequence,
                  const DcmTagKey &decriptor,
                  const DcmTagKey &data,
                  const DcmTagKey &explanation,
                  const unsigned long pos = 0,
                  unsigned long *card = NULL);

    /** constructor
     *
     ** @param  data         element containing the LUT data
     *  @param  descriptor   element containing the LUT descriptor
     *  @param  explanation  element containing the LUT explanation (optional)
     *  @param  first        expected value for "first input value mapped" (optional)
     *  @param  status       pointer to image status variable (optional)
     */
    DiLookupTable(const DcmUnsignedShort &data,
                  const DcmUnsignedShort &descriptor,
                  const DcmLongString *explanation = NULL,
                  const signed long first = -1,
                  EI_Status *status = NULL);

    /** constructor
     *
     ** @param  buffer  pointer to array with LUT entries
     *  @param  count   number of LUT entries
     *  @param  bits    number of bits per entry
     */
    DiLookupTable(Uint16 *buffer,
                  const Uint32 count,
                  const Uint16 bits);

    /** destructor
     */
    virtual ~DiLookupTable();

    /** invert all LUT values.
     *  (new_value = max_value - old_value, e.g. used for presentation LUTs)
     *
     ** @param  flag  if bit 0 is set (0x1, default) the internal copy of the LUT data is modified
     *                   or a copy of the original data is created (if not already existing),
     *                if bit 1 is set (0x2) the original LUT data is modified,
     *                a combination of both modes is also supported.
     *
     ** @return status true if successful (0x1 if internal data modified, 0x2 if original data, 0x3 if both),
     *                 false otherwise
     */
    int invertTable(const int flag = 0x1);

    /** mirror the order of all LUT entries.
    *   i.e. the last one becomes the first etc.
     *  (value[i] = value[last - i], e.g. used for presentation LUTs)
     *
     ** @param  flag  if bit 0 is set (0x1, default) the internal copy of the LUT data is modified
     *                   or a copy of the original data is created (if not already existing),
     *                if bit 1 is set (0x2) the original LUT data is modified,
     *                a combination of both modes is also supported.
     *
     ** @return status true if successful (0x1 if internal data modified, 0x2 if original data, 0x3 if both),
     *                 false otherwise
     */
    int mirrorTable(const int flag = 0x1);

    /** create an inverse copy of the current LUT.
     *  (input values become output values and vice versa)
     *  This function is used for DICOM print (mainly 8<->12 bit).
     *
     ** @return pointer to inverse LUT
     */
    DiLookupTable *createInverseLUT() const;

    /** compares current LUT with specified LUT
     *
     ** @param  data        element containing the LUT data
     *  @param  descriptor  element containing the LUT descriptor
     *
     ** @return true if LUTs are not equal (1 = invalid LUT / memory error,
     *                                      2 = descriptor differs,
     *                                      3 = data differs)
     *          false (0) otherwise
     */
    int compareLUT(const DcmUnsignedShort &data,
                   const DcmUnsignedShort &descriptor);


    /** compares current LUT with specified LUT
     *
     ** @param  lut  LUT to be compared with the current one
     *
     ** @return OFTrue if LUTs are equal, OFFalse otherwise
     */
    virtual OFBool operator==(const DiBaseLUT &lut);

    /** compares current LUT with specified LUT
     *
     ** @param  lut  LUT to be compared with the current one
     *
     ** @return OFTrue if LUTs are equal, OFFalse otherwise
     */
    virtual OFBool operator==(const DiLookupTable &lut);


 protected:

    /** initialize lookup table
     *
     ** @param  docu         pointer to dataset (encapsulated)
     *  @param  obj          pointer to item in dataset where to start (maybe NULL)
     *  @param  descriptor   tag key containing the LUT descriptor
     *  @param  data         tag key containing the LUT data
     *  @param  explanation  tag key containing the LUT explanation
     *  @param  status       pointer to image status variable (optional)
     */
    void Init(const DiDocument *docu,
              DcmObject *obj,
              const DcmTagKey &descriptor,
              const DcmTagKey &data,
              const DcmTagKey &explanation,
              EI_Status *status = NULL);

    /** check (and possibly correct) lookup table for consistency
     *
     ** @param  count   number of LUT entries
     *  @param  bits    bits per LUT entry
     *  @param  status  pointer to image status variable (optional)
     */
    void checkTable(unsigned long count,
                    Uint16 bits,
                    EI_Status *status = NULL);

    /** check (and possibly correct) value for bits per LUT entry.
     *  Be very lenient with the value stored in the LUT descriptor and
     *  therefore perform some addition (heuristical tests) to assume the
     *  most probable value.
     *
     ** @param  bits       actual value for bits per entry
     *  @param  rightBits  right value (8 or 16)
     *  @param  wrongBits  wrong value (8 or 16)
     */
    void checkBits(const Uint16 bits,
                   const Uint16 rightBits,
                   const Uint16 wrongBits = 0);

 private:

    /// number of bits originally allocated for a LUT entry
    int OriginalBitsAllocated;
    /// pointer to original data buffer (where the LUT data is created from)
    void *OriginalData;

 // --- declarations to avoid compiler warnings

    DiLookupTable(const DiLookupTable &);
    DiLookupTable &operator=(const DiLookupTable &);
};


#endif
