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
 *  Purpose: Interface of class DcmUnsignedLongOffset
 *
 */


#ifndef DCVRULUP_H
#define DCVRULUP_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcvrul.h"


/** a class used for DICOMDIR byte offsets
 */
class DcmUnsignedLongOffset
  : public DcmUnsignedLong
{

  public:

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmUnsignedLongOffset(const DcmTag &tag,
                          const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmUnsignedLongOffset(const DcmUnsignedLongOffset &old);

    /** destructor
     */
    virtual ~DcmUnsignedLongOffset();

    /** get element type identifier
     *  @return type identifier of this class (internal type: EVR_up)
     */
    virtual DcmEVR ident() const;

    /** clear the currently stored value
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition clear();

    /** get stored object reference
     *  @return pointer to the currently referenced object (might be NULL)
     */
    virtual DcmObject *getNextRecord();

    /** set stored object reference
     *  @param record pointer to the object to be referenced
     *  @return pointer to the newly referenced object (might be NULL)
     */
    virtual DcmObject *setNextRecord(DcmObject *record);

    /** check the currently stored element value
     *  @param autocorrect correct value length if OFTrue
     *  @return status, EC_Normal if value length is correct, an error code otherwise
     */
    virtual OFCondition verify(const OFBool autocorrect = OFFalse);


  protected:

    /// pointer to the referenced object. NULL means that no object is referenced.
    DcmObject *nextRecord;


  private:

	/// private undefined copy assignment operator
    DcmUnsignedLongOffset &operator=(const DcmUnsignedLongOffset &);
};


#endif // DCVRUSUP_H
