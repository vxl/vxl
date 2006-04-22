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
 *  Author:  Gerd Ehlers
 *
 *  Purpose: Interface of class DcmPersonName
 *
 */


#ifndef DCVRPN_H
#define DCVRPN_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dcchrstr.h"


/** a class representing the DICOM value representation 'Person Name' (PN)
 */
class DcmPersonName
  : public DcmCharString
{

  public:

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmPersonName(const DcmTag &tag,
                  const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmPersonName(const DcmPersonName &old);

    /** destructor
     */
    virtual ~DcmPersonName();

    /** assignment operator
     *  @param obj element to be assigned/copied
     *  @return reference to this object
     */
    DcmPersonName &operator=(const DcmPersonName &obj);

    /** get element type identifier
     *  @return type identifier of this class (EVR_PN)
     */
    virtual DcmEVR ident() const;

    /** get a copy of a particular string component
     *  @param stringVal variable in which the result value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param normalize delete leading and trailing spaces if OFTrue
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFString(OFString &stringVal,
                                    const unsigned long pos,
                                    OFBool normalize = OFTrue);

    /** get name components from the element value.
     *  The DICOM PN consists of up to three component groups separated by a "=". The
     *  supported format is "[CG0][=CG1][=CG2]" where the brackets enclose optional
     *  parts and CG0 is a single-byte character representation, CG1 an ideographic
     *  representation, and CG2 a phonetic representation of the name.
     *  Each component group may consist of up to five components separated by a "^".
     *  The format is "[lastName[^firstName[^middleName[^namePrefix[^nameSuffix]]]]";
     *  each component might be empty.
     *  If this function fails the result variables are cleared automatically. If the
     *  format is valid but does not comply with the above described scheme ("=" and "^")
     *  the full person name is returned in the 'lastName' variable.
     *  @param lastName reference to string variable where the "last name" is stored
     *  @param firstName reference to string variable where the "first name" is stored
     *  @param middleName reference to string variable where the "middle name" is stored
     *  @param namePrefix reference to string variable where the "name prefix" is stored
     *  @param nameSuffix reference to string variable where the "name suffix" is stored
     *  @param pos index of the element component in case of value multiplicity (0..vm-1)
     *  @param componentGroup index of the component group (0..2) to be used, see above
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition getNameComponents(OFString &lastName,
                                  OFString &firstName,
                                  OFString &middleName,
                                  OFString &namePrefix,
                                  OFString &nameSuffix,
                                  const unsigned long pos = 0,
                                  const unsigned int componentGroup = 0);

    /** get current element value as a formatted/readable name.
     *  The current element value is expected to be in DICOM PN format as described above.
     *  The output format is "[namePrefix][ firstName][ middleName][ lastName][, nameSuffix]";
     *  the delimiters (" " and ", ") are only inserted if required.
     *  If this function fails the result variable 'formattedName' is cleared automatically.
     *  @param formattedName reference to string variable where the result is stored
     *  @param pos index of the element component in case of value multiplicity (0..vm-1)
     *  @param componentGroup index of the component group (0..2) to be used, see above
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition getFormattedName(OFString &formattedName,
                                 const unsigned long pos = 0,
                                 const unsigned int componentGroup = 0);


    /* --- static helper functions --- */

    /** get name components from specified DICOM person name.
     *  The DICOM PN consists of up to three component groups separated by a "=". The
     *  supported format is "[CG0][=CG1][=CG2]" where the brackets enclose optional
     *  parts and CG0 is a single-byte character representation, CG1 an ideographic
     *  representation, and CG2 a phonetic representation of the name.
     *  Each component group may consist of up to five components separated by a "^".
     *  The format is "[lastName[^firstName[^middleName[^namePrefix[^nameSuffix]]]]";
     *  each component might be empty.
     *  If this function fails the result variables are cleared automatically. If the
     *  format is valid but does not comply with the above described scheme ("=" and "^")
     *  the full person name is returned in the 'lastName' variable.
     *  @param dicomName string value in DICOM PN format to be split into components
     *  @param lastName reference to string variable where the "last name" is stored
     *  @param firstName reference to string variable where the "first name" is stored
     *  @param middleName reference to string variable where the "middle name" is stored
     *  @param namePrefix reference to string variable where the "name prefix" is stored
     *  @param nameSuffix reference to string variable where the "name suffix" is stored
     *  @param componentGroup index of the component group (0..2) to be used, see above
     *  @return EC_Normal upon success, an error code otherwise
     */
    static OFCondition getNameComponentsFromString(const OFString &dicomName,
                                                   OFString &lastName,
                                                   OFString &firstName,
                                                   OFString &middleName,
                                                   OFString &namePrefix,
                                                   OFString &nameSuffix,
                                                   const unsigned int componentGroup = 0);

    /** get specified DICOM person name as a formatted/readable name.
     *  The specified 'dicomName' is expected to be in DICOM PN format as described above.
     *  The output format is "[namePrefix][ firstName][ middleName][ lastName][, nameSuffix]";
     *  the delimiters (" " and ", ") are only inserted if required.
     *  If this function fails the result variable 'formattedName' is cleared automatically.
     *  @param dicomName string value in DICOM PN format to be converted to readable format
     *  @param formattedName reference to string variable where the result is stored
     *  @param componentGroup index of the component group (0..2) to be used, see above
     *  @return EC_Normal upon success, an error code otherwise
     */
    static OFCondition getFormattedNameFromString(const OFString &dicomName,
                                                  OFString &formattedName,
                                                  const unsigned int componentGroup = 0);

    /** get formatted/readable name from specified name components.
     *  The output format is "[namePrefix][ firstName][ middleName][ lastName][, nameSuffix]";
     *  the delimiters (" " and ", ") are only inserted if required.
     *  If this function fails the result variable 'formattedName' is cleared automatically.
     *  @param lastName reference to string variable where the "last name" is stored
     *  @param firstName reference to string variable where the "first name" is stored
     *  @param middleName reference to string variable where the "middle name" is stored
     *  @param namePrefix reference to string variable where the "name prefix" is stored
     *  @param nameSuffix reference to string variable where the "name suffix" is stored
     *  @param formattedName reference to string variable where the result is stored
     *  @return always returns EC_Normal
     */
    static OFCondition getFormattedNameFromComponents(const OFString &lastName,
                                                      const OFString &firstName,
                                                      const OFString &middleName,
                                                      const OFString &namePrefix,
                                                      const OFString &nameSuffix,
                                                      OFString &formattedName);
};


#endif // DCVRPN_H
