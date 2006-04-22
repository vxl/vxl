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
 *  Purpose: Implementation of class DcmPersonName
 *
 */


#include "dcvrpn.h"


// ********************************


DcmPersonName::DcmPersonName(const DcmTag &tag,
                             const Uint32 len)
  : DcmCharString(tag, len)
{
    maxLength = 64;     // not correct: max length of PN is 3*64+2 = 194 characters (not bytes!)
}


DcmPersonName::DcmPersonName(const DcmPersonName& old)
  : DcmCharString(old)
{
}


DcmPersonName::~DcmPersonName()
{
}


DcmPersonName &DcmPersonName::operator=(const DcmPersonName &obj)
{
    DcmCharString::operator=(obj);
    return *this;
}


// ********************************


DcmEVR DcmPersonName::ident() const
{
    return EVR_PN;
}


// ********************************


OFCondition DcmPersonName::getOFString(OFString &stringVal,
                                       const unsigned long pos,
                                       OFBool normalize)
{
    OFCondition l_error = DcmCharString::getOFString(stringVal, pos, normalize);
    if (l_error.good() && normalize)
        normalizeString(stringVal, !MULTIPART, !DELETE_LEADING, DELETE_TRAILING);
    return l_error;
}


// ********************************


OFCondition DcmPersonName::getNameComponents(OFString &lastName,
                                             OFString &firstName,
                                             OFString &middleName,
                                             OFString &namePrefix,
                                             OFString &nameSuffix,
                                             const unsigned long pos,
                                             const unsigned int componentGroup)
{
    OFString dicomName;
    OFCondition l_error = getOFString(dicomName, pos);
    if (l_error.good())
        l_error = getNameComponentsFromString(dicomName, lastName, firstName, middleName, namePrefix, nameSuffix, componentGroup);
    else
    {
        lastName.clear();
        firstName.clear();
        middleName.clear();
        namePrefix.clear();
        nameSuffix.clear();
    }
    return l_error;
}


OFCondition DcmPersonName::getNameComponentsFromString(const OFString &dicomName,
                                                       OFString &lastName,
                                                       OFString &firstName,
                                                       OFString &middleName,
                                                       OFString &namePrefix,
                                                       OFString &nameSuffix,
                                                       const unsigned int componentGroup)
{
    OFCondition l_error = EC_Normal;
    /* initialize all name components */
    lastName.clear();
    firstName.clear();
    middleName.clear();
    namePrefix.clear();
    nameSuffix.clear();
    if (dicomName.length() > 0)
    {
        /* Excerpt from DICOM part 5:
           "For the purpose of writing names in ideographic characters and in
            phonetic characters, up to 3 groups of components may be used."
        */
        if (componentGroup < 3)
        {
            OFString name;
            // find component group (0..2)
            const size_t posA = dicomName.find('=');
            if (posA != OFString_npos)
            {
                if (componentGroup > 0)
                {
                    const size_t posB = dicomName.find('=', posA + 1);
                    if (posB != OFString_npos)
                    {
                        if (componentGroup == 1)
                            name = dicomName.substr(posA + 1, posB - posA - 1);
                        else /* componentGroup == 2 */
                            name = dicomName.substr(posB + 1);
                    } else if (componentGroup == 1)
                        name = dicomName.substr(posA + 1);
                } else /* componentGroup == 0 */
                    name = dicomName.substr(0, posA);
            } else if (componentGroup == 0)
                name = dicomName;
            /* check whether component group is valid (= non-empty) */
            if (name.length() > 0)
            {
                /* find caret separators */
                /* (tbd: add more sophisticated heuristics for comma and space separated names) */
                const size_t pos1 = name.find('^');
                if (pos1 != OFString_npos)
                {
                    const size_t pos2 = name.find('^', pos1 + 1);
                    lastName = name.substr(0, pos1);
                    if (pos2 != OFString_npos)
                    {
                        const size_t pos3 = name.find('^', pos2 + 1);
                        firstName = name.substr(pos1 + 1, pos2 - pos1 - 1);
                        if (pos3 != OFString_npos)
                        {
                            const size_t pos4 = name.find('^', pos3 + 1);
                            middleName = name.substr(pos2 + 1, pos3 - pos2 - 1);
                            if (pos4 != OFString_npos)
                            {
                                namePrefix = name.substr(pos3 + 1, pos4 - pos3 - 1);
                                nameSuffix = name.substr(pos4 + 1);
                            } else
                                namePrefix = name.substr(pos3 + 1);
                        } else
                            middleName = name.substr(pos2 + 1);
                    } else
                        firstName = name.substr(pos1 + 1);
                } else
                    lastName = name;
            }
        } else
            l_error = EC_IllegalParameter;
    }
    return l_error;
}


// ********************************


OFCondition DcmPersonName::getFormattedName(OFString &formattedName,
                                            const unsigned long pos,
                                            const unsigned int componentGroup)
{
    OFString dicomName;
    OFCondition l_error = getOFString(dicomName, pos);
    if (l_error.good())
        l_error = getFormattedNameFromString(dicomName, formattedName, componentGroup);
    else
        formattedName.clear();
    return l_error;
}


OFCondition DcmPersonName::getFormattedNameFromString(const OFString &dicomName,
                                                      OFString &formattedName,
                                                      const unsigned int componentGroup)
{
    OFString lastName, firstName, middleName, namePrefix, nameSuffix;
    OFCondition l_error = getNameComponentsFromString(dicomName, lastName, firstName, middleName, namePrefix, nameSuffix, componentGroup);
    if (l_error.good())
        l_error = getFormattedNameFromComponents(lastName, firstName, middleName, namePrefix, nameSuffix, formattedName);
    else
        formattedName.clear();
    return l_error;
}


OFCondition DcmPersonName::getFormattedNameFromComponents(const OFString &lastName,
                                                          const OFString &firstName,
                                                          const OFString &middleName,
                                                          const OFString &namePrefix,
                                                          const OFString &nameSuffix,
                                                          OFString &formattedName)
{
    formattedName.clear();
    /* concatenate name components */
    if (namePrefix.length() > 0)
        formattedName += namePrefix;
    if (firstName.length() > 0)
    {
        if (formattedName.length() > 0)
            formattedName += ' ';
        formattedName += firstName;
    }
    if (middleName.length() > 0)
    {
        if (formattedName.length() > 0)
            formattedName += ' ';
        formattedName += middleName;
    }
    if (lastName.length() > 0)
    {
        if (formattedName.length() > 0)
            formattedName += ' ';
        formattedName += lastName;
    }
    if (nameSuffix.length() > 0)
    {
        if (formattedName.length() > 0)
            formattedName += ", ";
        formattedName += nameSuffix;
    }
    return EC_Normal;
}
