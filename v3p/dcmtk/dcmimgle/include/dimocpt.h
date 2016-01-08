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
 *  Purpose: DicomMonochromeCopyTemplate (Header)
 *
 */


#ifndef __DIMOCPT_H
#define __DIMOCPT_H

#include "osconfig.h"
#include "dctypes.h"
#include "ofbmanip.h"

#include "dimopxt.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to copy monochrome pixel data
 */
template<class T>
class DiMonoCopyTemplate
  : public DiMonoPixelTemplate<T>
{
 public:

    /** constructor
     *
     ** @param  pixel   pointer to monochrome intermediate representation of pixel data
     *  @param  fstart  first frame to be copied
     *  @param  fcount  number of frames to be copied
     *  @param  fsize   size of one frame (in bytes)
     */
    DiMonoCopyTemplate(const DiMonoPixel *pixel,
                       const unsigned long fstart,
                       const unsigned long fcount,
                       const unsigned long fsize)
      : DiMonoPixelTemplate<T>(pixel, fcount * fsize)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
        {
            if ((pixel->getCount() > fstart * fsize) && (pixel->getCount() >= (fstart + fcount) * fsize))
                copy((const T *)pixel->getData() + fstart * fsize);
        }
    }

    /** destructor
     */
    ~DiMonoCopyTemplate()
    {
    }


 private:

    /** copy specified amount of pixel data
     *
     ** @param  pixel  array of pixel data to be copied
     */
    inline void copy(const T *pixel)
    {
        if (pixel != NULL)
        {
            this->Data = new T[this->getCount()];
            if (this->Data != NULL)
                OFBitmanipTemplate<T>::copyMem(pixel, this->Data, this->getCount());
        }
    }
};


#endif
