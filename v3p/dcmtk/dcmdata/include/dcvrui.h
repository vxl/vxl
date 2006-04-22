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
 *  Purpose: Interface of class DcmUniqueIdentifier
 *
 */

#ifndef DCVRUI_H
#define DCVRUI_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcbytstr.h"


/** a class representing the DICOM value representation 'Unique Identifier' (UI)
 */
class DcmUniqueIdentifier
  : public DcmByteString
{

  public:

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmUniqueIdentifier(const DcmTag &tag,
                        const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmUniqueIdentifier(const DcmUniqueIdentifier &old);

    /** destructor
     */
    virtual ~DcmUniqueIdentifier();

    /** assignment operator
     *  @param obj element to be assigned/copied
     *  @return reference to this object
     */
    DcmUniqueIdentifier &operator=(const DcmUniqueIdentifier &obj);

    /** get element type identifier
     *  @return type identifier of this class (EVR_UI)
     */
    virtual DcmEVR ident() const;

    /** print element to a stream.
     *  The output format of the value is a backslash separated sequence of string
     *  components. In case of a single component the UID number is mapped to the
     *  corresponding UID name (using "dcmFindNameOfUID()") if available. A "=" is
     *  used as a prefix to distinguish the UID name from the UID number.
     *  NB: this mapping of UID names only works for single-valued strings.
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

    /** set element value from the given character string.
     *  If the string starts with a "=" the subsequent characters are interpreted as a
     *  UID name and mapped to the corresponding UID number (using "dcmFindUIDFromName()")
     *  if possible. Otherwise the leading "=" is removed.
     *  NB: this mapping of UID names only works for single-valued input strings.
     *  @param stringVal input character string (possibly multi-valued)
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition putString(const char *stringVal);


  protected:

    /** convert currently stored string value to internal representation.
     *  It removes any leading, embedded and trailing space character and recomputes
     *  the string length. This manipulation attempts to correct problems with
     *  incorrectly encoded UIDs which have been observed in some images.
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition makeMachineByteString();
};


#endif // DCVRUI_H
