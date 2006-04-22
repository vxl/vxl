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
 *  Purpose: DicomPixel (Header)
 *
 */


#ifndef __DIPIXEL_H
#define __DIPIXEL_H

#include "osconfig.h"
#include "dctypes.h"

#include "diutils.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Abstract base class to handle pixel data
 */
class DiPixel
{
 public:

    /** constructor
     *
     ** @param  count       number of pixels stored in the buffer
     *  @param  inputCount  number of pixels read from 'PixelData' attribute
     */
    DiPixel(const unsigned long count,
            const unsigned long inputCount = 0)
      : Count(count),
        InputCount(inputCount)
    {
    }

    /** destructor
     */
    virtual ~DiPixel()
    {
    }

    /** get integer representation (abstract)
     *
     ** @return integer representation
     */
    virtual EP_Representation getRepresentation() const = 0;

    /** get number of planes (abstract)
     *
     ** @return number of planes
     */
    virtual int getPlanes() const = 0;

    /** get pointer to pixel data (abstract)
     *
     ** @return pointer to pixel data
     */
    virtual void *getData() const = 0;

    /** get reference to pointer to pixel data (abstract)
     *
     ** @return reference to pointer to pixel data
     */
    virtual void *getDataPtr() = 0;

    /** get number of pixels
     *
     ** @return number of pixels
     */
    inline unsigned long getCount() const
    {
        return Count;
    }

    /** get number of pixels stored in the 'PixelData' element
     *
     ** @return number pixels read from the input buffer
     */
    inline unsigned long getInputCount() const
    {
        return InputCount;
    }


 protected:

    /// number of pixels
    /*const*/ unsigned long Count;

    /// number of pixels in the input buffer
    unsigned long InputCount;
};


#endif
