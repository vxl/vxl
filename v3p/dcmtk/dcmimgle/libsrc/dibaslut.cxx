/*
 *
 *  Copyright (C) 1996-2001, OFFIS
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
 *  Purpose: DicomBaseLUT (Source)
 *
 */


#include "osconfig.h"

#include "dibaslut.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiBaseLUT::DiBaseLUT(const Uint32 count,
                     const Uint16 bits)
  : Count(count),
    FirstEntry(0),
    Bits(bits),
    MinValue(0),
    MaxValue(0),
    Valid(0),
    Explanation(),
    Data(NULL),
    DataBuffer(NULL)
{
}


DiBaseLUT::DiBaseLUT(Uint16 *buffer,
                     const Uint32 count,
                     const Uint16 bits)
  : Count(count),
    FirstEntry(0),
    Bits(bits),
    MinValue(0),
    MaxValue(0),
    Valid(0),
    Explanation(),
    Data(buffer),
    DataBuffer(buffer)
{
}


/*--------------*
 *  destructor  *
 *--------------*/

DiBaseLUT::~DiBaseLUT()
{
    delete[] DataBuffer;
}


/********************************************************************/


OFBool DiBaseLUT::operator==(const DiBaseLUT &lut)
{
    return (compare(&lut) == 0);
}


int DiBaseLUT::compare(const DiBaseLUT *lut)
{
    int result = 1;                                     // invalid LUT (1)
    if (Valid && (lut != NULL) && lut->isValid())
    {
        result = 2;                                     // descriptor differs (2)
        if ((Count == lut->getCount()) && (FirstEntry == lut->getFirstEntry()) && (Bits == lut->getBits()))
        {                                               // special case: if Count == 0 LUT data is equal
            register Uint32 i = Count;                  // ... but normally not Valid !
            if ((MinValue == lut->getMinValue()) && (MaxValue == lut->getMaxValue()))
            {                                           // additional check for better performance
                register const Uint16 *p = Data;
                register const Uint16 *q = lut->getData();
                while ((i != 0) && (*(p++) == *(q++)))
                    i--;
            }
            result = (i != 0) ? 3 : 0;                  // check whether data is equal (0)
        }
    }
    return result;
}
