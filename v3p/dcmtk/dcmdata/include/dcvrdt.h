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
 *  Author:  Gerd Ehlers, Joerg Riesmeier
 *
 *  Purpose: Interface of class DcmDateTime
 *
 */

#ifndef DCVRDT_H
#define DCVRDT_H

#include "osconfig.h"    /* make sure OS specific configuration is included first */

#include "dctypes.h"
#include "dcbytstr.h"
#include "ofdatime.h"


/** a class representing the DICOM value representation 'Date Time' (DT)
 */
class DcmDateTime
  : public DcmByteString
{

  public:

    /** constructor.
     *  Create new element from given tag and length.
     *  @param tag DICOM tag for the new element
     *  @param len value length for the new element
     */
    DcmDateTime(const DcmTag &tag,
                const Uint32 len = 0);

    /** copy constructor
     *  @param old element to be copied
     */
    DcmDateTime(const DcmDateTime &old);

    /** destructor
     */
    virtual ~DcmDateTime();

    /** assignment operator
     *  @param obj element to be assigned/copied
     *  @return reference to this object
     */
    DcmDateTime &operator=(const DcmDateTime &obj);

    /** get element type identifier
     *  @return type identifier of this class (EVR_DT)
     */
    virtual DcmEVR ident() const;

    /** get a copy of a particular string component
     *  @param stringValue variable in which the result value is stored
     *  @param pos index of the value in case of multi-valued elements (0..vm-1)
     *  @param normalize delete trailing spaces if OFTrue
     *  @return status, EC_Normal if successful, an error code otherwise
     */
    virtual OFCondition getOFString(OFString &stringValue,
                                        const unsigned long pos,
                                        OFBool normalize = OFTrue);

        /** set the element value to the current system date and time.
         *  The DICOM DT format supported by this function is "YYYYMMDDHHMM[SS[.FFFFFF]][&ZZZZ]"
         *  where the brackets enclose optional parts. If the current system date/time or parts
         *  of it are unavailable the corresponding values are set to "0" and an error code is
         *  returned - in fact, the date is set to "19000101" if unavailable.
         *  @param seconds add optional seconds ("SS") if OFTrue
         *  @param fraction add optional fractional part of a second (".FFFFFF") if OFTrue
         *   (requires parameter 'seconds' to be also OFTrue)
         *  @param timeZone add optional time zone ("&ZZZZ" where "&" is "+" or "-") if OFTrue.
         *   The time zone is given as the offset (hours and minutes) from Coordinated Universal
         *   Time (UTC).
         *  @return EC_Normal upon success, an error code otherwise
         */
    OFCondition setCurrentDateTime(const OFBool seconds = OFTrue,
                                   const OFBool fraction = OFFalse,
                                   const OFBool timeZone = OFFalse);

    /** set the element value to the given date and time
     *  @param dateTimeValue date to be set (should be a valid date and time)
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition setOFDateTime(const OFDateTime &dateTimeValue);

    /** get the current element value in OFDateTime format.
     *  Please note that the element value is expected to be in valid DICOM DT format
     *  ("YYYYMMDD[HH[MM[SS[.FFFFFF]]]][&ZZZZ]"). If the optional time zone ("&ZZZZ") is
     *  missing the local time zone is used.
     *  If this function fails the result variable 'dateTimeValue' is cleared automatically.
     *  @param dateTimeValue reference to OFDateTime variable where the result is stored
     *  @param pos index of the element component in case of value multiplicity (0..vm-1)
     *  @return EC_Normal upon success, an error code otherwise
     */
    OFCondition getOFDateTime(OFDateTime &dateTimeValue,
                              const unsigned long pos = 0);

        /** get the current element value in ISO date/time format.
         *  The ISO date/time format supported by this function is "YYYY-MM-DD HH:MM[:SS[.FFFFFF]]
         *  [&HH:MM]" where the brackets enclose optional parts. Please note that the element value
         *  is expected to be in valid DICOM DT format ("YYYYMMDD[HH[MM[SS[.FFFFFF]]]][&ZZZZ]"). If
         *  this function fails the result variable 'formattedDateTime' is cleared automatically.
         *  @param formattedDateTime reference to string variable where the result is stored
         *  @param pos index of the element component in case of value multiplicity (0..vm-1)
         *  @param seconds add optional seconds (":SS") if OFTrue
         *  @param fraction add optional fractional part of a second (".FFFFFF") if OFTrue
         *   (requires parameter 'seconds' to be also OFTrue)
         *  @param timeZone add optional time zone ("&HH:MM" where "&" is "+" or "-") if OFTrue.
         *   The time zone is given as the offset (hours and minutes) from Coordinated Universal
         *   Time (UTC). Please note that the formatted time output is not adapted to the local
         *   time if the time zone is omitted.
         *  @param createMissingPart if OFTrue create optional parts (seconds, fractional part of
         *   a seconds and/or time zone) if absent in the element value
         *  @return EC_Normal upon success, an error code otherwise
         */
    OFCondition getISOFormattedDateTime(OFString &formattedDateTime,
                                        const unsigned long pos = 0,
                                        const OFBool seconds = OFTrue,
                                        const OFBool fraction = OFFalse,
                                        const OFBool timeZone = OFTrue,
                                        const OFBool createMissingPart = OFFalse);

    /* --- static helper functions --- */

        /** get the current system date and time.
         *  The DICOM DT format supported by this function is "YYYYMMDDHHMM[SS[.FFFFFF]][&ZZZZ]"
         *  where the brackets enclose optional parts. If the current system date/time or parts
         *  of it are unavailable the corresponding values are set to "0" and an error code is
         *  returned - in fact, the date is set to "19000101" if unavailable.
         *  @param dicomDateTime reference to string variable where the result is stored
         *  @param seconds add optional seconds ("SS") if OFTrue
         *  @param fraction add optional fractional part of a second (".FFFFFF") if OFTrue
         *   (requires parameter 'seconds' to be also OFTrue)
         *  @param timeZone add optional time zone ("&ZZZZ" where "&" is "+" or "-") if OFTrue.
         *   The time zone is given as the offset (hours and minutes) from Coordinated Universal
         *   Time (UTC).
         *  @return EC_Normal upon success, an error code otherwise
         */
    static OFCondition getCurrentDateTime(OFString &dicomDateTime,
                                          const OFBool seconds = OFTrue,
                                          const OFBool fraction = OFFalse,
                                          const OFBool timeZone = OFFalse);

    /** get the specified OFDateTime value in DICOM format.
     *  The DICOM DT format supported by this function is "YYYYMMDDHHMM[SS[.FFFFFF]][&ZZZZ]"
     *  where the brackets enclose optional parts. If the current system date/time or parts
     *  of it are unavailable the corresponding values are set to "0" and an error code is
     *  returned - in fact, the date is set to "19000101" if unavailable.
     *  @param dateTimeValue date and time to be converted to DICOM format
     *  @param dicomDateTime reference to string variable where the result is stored
     *  @param seconds add optional seconds ("SS") if OFTrue
     *  @param fraction add optional fractional part of a second (".FFFFFF") if OFTrue
     *   (requires parameter 'seconds' to be also OFTrue)
         *  @param timeZone add optional time zone ("&ZZZZ" where "&" is "+" or "-") if OFTrue.
         *   The time zone is given as the offset (hours and minutes) from Coordinated Universal
         *   Time (UTC).
     *  @return EC_Normal upon success, an error code otherwise
     */
    static OFCondition getDicomDateTimeFromOFDateTime(const OFDateTime &dateTimeValue,
                                                      OFString &dicomDateTime,
                                                      const OFBool seconds = OFTrue,
                                                      const OFBool fraction = OFFalse,
                                                      const OFBool timeZone = OFFalse);

    /** get the specified DICOM date and time value in OFDateTime format.
     *  Please note that the element value is expected to be in valid DICOM DT format
     *  ("YYYYMMDD[HH[MM[SS[.FFFFFF]]]][&ZZZZ]"). If the optional time zone ("&ZZZZ") is
     *  missing the local time zone is used.
     *  If this function fails the result variable 'dateTimeValue' is cleared automatically.
     *  @param dicomDateTime string value in DICOM DT format to be converted to ISO format
     *  @param dateTimeValue reference to OFDateTime variable where the result is stored
     *  @return EC_Normal upon success, an error code otherwise
     */
    static OFCondition getOFDateTimeFromString(const OFString &dicomDateTime,
                                               OFDateTime &dateTimeValue);

        /** get the specified DICOM date/time value in ISO format.
         *  The ISO date/time format supported by this function is "YYYY-MM-DD HH:MM[:SS[.FFFFFF]]
         *  [&HH:MM]" where the brackets enclose optional parts. Please note that the specified
         *  value is expected to be in valid DICOM DT format ("YYYYMMDD[HH[MM[SS[.FFFFFF]]]][&ZZZZ]").
     *  If this function fails the result variable 'formattedDateTime' is cleared automatically.
         *  @param dicomDateTime string value in DICOM DT format to be converted to ISO format
         *  @param formattedDateTime reference to string variable where the result is stored
         *  @param seconds add optional seconds (":SS") if OFTrue
         *  @param fraction add optional fractional part of a second (".FFFFFF") if OFTrue
         *   (requires parameter 'seconds' to be also OFTrue)
         *  @param timeZone add optional time zone ("&HH:MM" where "&" is "+" or "-") if OFTrue.
         *   The time zone is given as the offset (hours and minutes) from the Coordinated Universal
         *   Time (UTC). Please note that the formatted time output is not adapted to the local time
         *   if the time zone is omitted.
         *  @param createMissingPart if OFTrue create optional parts (seconds, fractional part of
         *   a seconds and/or time zone) if absent in the element value
         *  @return EC_Normal upon success, an error code otherwise
         */
    static OFCondition getISOFormattedDateTimeFromString(const OFString &dicomDateTime,
                                                         OFString &formattedDateTime,
                                                         const OFBool seconds = OFTrue,
                                                         const OFBool fraction = OFFalse,
                                                         const OFBool timeZone = OFTrue,
                                                         const OFBool createMissingPart = OFFalse);
};


#endif // DCVRDT_H
