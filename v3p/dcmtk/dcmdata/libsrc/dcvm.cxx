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
 *  Author:  Andreas Barth
 *
 *  Purpose: functions to derive VM from string
 *
 */

#include "osconfig.h"
#include "dcvm.h"

#define INCLUDE_CSTRING
#include "ofstdinc.h"

// get the number of values stored in string
unsigned long getVMFromString(const char * val)
{
    unsigned long vm = 1;
    if (val == NULL)
        vm = 0;
    else
    {
        for (const char * s = strchr(val, '\\'); s; s = strchr(++s, '\\'))
            vm++;
    }
    return vm;
}

// get first value stored in string, set the parameter to beginning of the
// next value
char * getFirstValueFromString(const char * & s)
{
    if (s)
    {
        const char * p = strchr(s, '\\');
        const char * backup = s;
        size_t len = 0;
        if (p)
        {
            len = p-s;
            s += (len+1);
        }
        else
        {
            len = strlen(s);
            s = NULL;
        }

        char * result = new char[len+1];
        strncpy(result, backup, len);
        result[len] = '\0';
        return result;
    }
    else
        return NULL;
}
