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
 *  Purpose: DicomMonoOutputPixel (Header)
 *
 */


#ifndef __DIMOOPX_H
#define __DIMOOPX_H

#include "osconfig.h"
#include "diutils.h"

#define INCLUDE_CSTDLIB
#define INCLUDE_CSTDIO
#include "ofstdinc.h"

#include "ofstream.h"

/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiMonoPixel;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Abstract base class to create monochrome output data
 */
class DiMonoOutputPixel
{
 public:

    /** constructor
     *
     ** @param  pixel  pointer to intermediate pixel representation
     *  @param  size   number of pixel per frame
     *  @param  frame  frame to be rendered
     *  @param  max    maximum output value
     */
    DiMonoOutputPixel(const DiMonoPixel *pixel,
                      const unsigned long size,
                      const unsigned long frame,
                      const unsigned long max);

    /** destructor
     */
    virtual ~DiMonoOutputPixel();

    /** get integer representation (abstract)
     *
     ** @return integer representation
     */
    virtual EP_Representation getRepresentation() const = 0;

    /** get pointer to output pixel data (abstract)
     *
     ** @return pointer to pixel data if sucessful, NULL otherwise
     */
    virtual void *getData() const = 0;

    /** remove reference to (internally handled) pixel data (abstract)
     */
    virtual void removeDataReference() = 0;

    /** get size of one pixel / item in the pixel array (abstract)
     *
     ** @return item size
     */
    virtual size_t getItemSize() const = 0;

    /** write pixel data of selected frame to PPM/ASCII file (abstract)
     *
     ** @param  stream  open C++ output stream
     *
     ** @return status, true if successful, false otherwise
     */
    virtual int writePPM(ostream &stream) const = 0;

    /** write pixel data of selected frame to PPM/ASCII file (abstract)
     *
     ** @param  stream  open C file stream
     *
     ** @return status, true if successful, false otherwise
     */
    virtual int writePPM(FILE *stream) const = 0;

    /** get number of pixel per frame
     *
     ** @return number of pixel per frame
     */
    inline unsigned long getCount() const
    {
        return FrameSize;
    }

    /** check whether specified pixel value is used in the image.
     *  Pixel array is examined when this method is called for the first time
     *  (just-in-time creation / create on demand of the supporting table).
     *
     ** @param  value  pixel value to be checked
     *
     ** @return true if value is used, false otherwise
     */
    int isUnused(const unsigned long value);


 protected:

    /** examine which pixel values are actually used (abstract)
     */
    virtual void determineUsedValues() = 0;


    /// number of pixels per frame (intermediate representation)
    /*const*/ unsigned long Count;
    /// number of pixels per frame (memory buffer size)
    const unsigned long FrameSize;

    /// array of used pixel values
    Uint8 *UsedValues;
    /// maximum output value
    const unsigned long MaxValue;


 private:

 // --- declarations to avoid compiler warnings

    DiMonoOutputPixel(const DiMonoOutputPixel &);
    DiMonoOutputPixel &operator=(const DiMonoOutputPixel &);
};


#endif
