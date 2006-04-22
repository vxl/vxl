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
 *  Purpose: Implementation of class DcmUniqueIdentifier
 *
 */


#include "osconfig.h"
#include "ofstream.h"
#include "ofstring.h"
#include "ofstd.h"
#include "dcvrui.h"
#include "dcuid.h"

#define INCLUDE_CSTRING
#define INCLUDE_CCTYPE
#include "ofstdinc.h"


// ********************************


DcmUniqueIdentifier::DcmUniqueIdentifier(const DcmTag &tag,
                                         const Uint32 len)
  : DcmByteString(tag, len)
{
    /* padding character is NULL not a space! */
    paddingChar = '\0';
    maxLength = 64;
}


DcmUniqueIdentifier::DcmUniqueIdentifier(const DcmUniqueIdentifier &old)
  : DcmByteString(old)
{
}


DcmUniqueIdentifier::~DcmUniqueIdentifier()
{
}


DcmUniqueIdentifier &DcmUniqueIdentifier::operator=(const DcmUniqueIdentifier &obj)
{
    DcmByteString::operator=(obj);
    return *this;
}


// ********************************


DcmEVR DcmUniqueIdentifier::ident() const
{
    return EVR_UI;
}


// ********************************


void DcmUniqueIdentifier::print(ostream &out,
                                const size_t flags,
                                const int level,
                                const char * /*pixelFileName*/,
                                size_t * /*pixelCounter*/)
{
    if (valueLoaded())
    {
        /* get string data (possibly multi-valued) */
        char *string = NULL;
        getString(string);
        if (string != NULL)
        {
            /* check whether UID number can be mapped to a UID name */
            const char *symbol = dcmFindNameOfUID(string);
            if ((symbol != NULL) && (strlen(symbol) > 0))
            {
                const size_t bufSize = strlen(symbol) + 1 /* for "=" */ + 1;
                char *buffer = new char[bufSize];
                if (buffer != NULL)
                {
                    /* concatenate "=" and the UID name */
                    OFStandard::strlcpy(buffer, "=", bufSize);
                    OFStandard::strlcat(buffer, symbol, bufSize);
                    printInfoLine(out, flags, level, buffer);
                    /* delete temporary character buffer */
                    delete[] buffer;
                } else /* could not allocate buffer */
                    DcmByteString::print(out, flags, level);
            } else /* no symbol (UID name) found */
                DcmByteString::print(out, flags, level);
        } else
            printInfoLine(out, flags, level, "(no value available)" );
    } else
        printInfoLine(out, flags, level, "(not loaded)" );
}


// ********************************


OFCondition DcmUniqueIdentifier::putString(const char *stringVal)
{
    const char *uid = stringVal;
    /* check whether parameter contains a UID name instead of a UID number */
    if ((stringVal != NULL) && (stringVal[0] == '='))
        uid = dcmFindUIDFromName(stringVal + 1);
    /* call inherited method to set the UID string */
    return DcmByteString::putString(uid);
}


// ********************************


OFCondition DcmUniqueIdentifier::makeMachineByteString()
{
    /* get string data */
    char *value = (char *)getValue();
    /* check whether automatic input data correction is enabled */
    if ((value != NULL) && dcmEnableAutomaticInputDataCorrection.get())
    {
        const int len = strlen(value);
        /*
        ** Remove any leading, embedded, or trailing white space.
        ** This manipulation attempts to correct problems with
        ** incorrectly encoded UIDs which have been observed in
        ** some images.
        */
        int k = 0;
        for (int i = 0; i < len; i++)
        {
           if (!isspace(value[i]))
           {
              value[k] = value[i];
              k++;
           }
        }
        value[k] = '\0';
    }
    /* call inherited method: re-computes the string length, etc. */
    return DcmByteString::makeMachineByteString();
}
