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
 *  Purpose: DicomMonochromeRotateTemplate (Header)
 *
 */


#ifndef __DIMOROT_H
#define __DIMOROT_H

#include "osconfig.h"
#include "dctypes.h"

#include "dimopxt.h"
#include "dirotat.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to rotate monochrome images (on pixel data level).
 *  by steps of 90 degrees
 */
template<class T>
class DiMonoRotateTemplate
  : public DiMonoPixelTemplate<T>,
    protected DiRotateTemplate<T>
{
 public:

    /** constructor
     *
     ** @param  pixel      pointer to intermediate pixel representation
     *  @param  src_cols   width of clipping area
     *  @param  src_rows   height of clipping area
     *  @param  dest_cols  width of destination image (scaled image)
     *  @param  dest_rows  height of destination image
     *  @param  frames     number of frames
     *  @param  degree     angle by which the pixel data should be rotated
     */
    DiMonoRotateTemplate(const DiMonoPixel *pixel,
                         const Uint16 src_cols,
                         const Uint16 src_rows,
                         const Uint16 dest_cols,
                         const Uint16 dest_rows,
                         const Uint32 frames,
                         const int degree)
      : DiMonoPixelTemplate<T>(pixel, (unsigned long)dest_cols * (unsigned long)dest_rows * frames),
        DiRotateTemplate<T>(1, src_cols, src_rows, dest_cols, dest_rows, frames)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
        {
            if (pixel->getCount() == (unsigned long)src_cols * (unsigned long)src_rows * frames)
                rotate((const T *)pixel->getData(), degree);
            else {
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                {
                   ofConsole.lockCerr() << "WARNING: could not rotate image ... corrupted data." << endl;
                   ofConsole.unlockCerr();
                }
            }
        }
    }

    /** destructor
     */
    ~DiMonoRotateTemplate()
    {
    }


 private:

    /** rotate pixel data
     *
     ** @param  pixel   pointer to pixel data to be rotated
     *  @param  degree  angle by which the pixel data should be rotated
     */
    inline void rotate(const T *pixel,
                       const int degree)
    {
        if (pixel != NULL)
        {
            this->Data = new T[this->getCount()];
            if (this->Data != NULL)
            {
                if (degree == 90)
                    rotateRight(&pixel, &this->Data);
                else if (degree == 180)
                    rotateTopDown(&pixel, &this->Data);
                else if (degree == 270)
                    rotateLeft(&pixel, &this->Data);
            }
        }
    }
};


#endif
