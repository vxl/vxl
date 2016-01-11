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
 *  Purpose: DicomMonochromeFlipTemplate (Header)
 *
 */


#ifndef __DIMOFLT_H
#define __DIMOFLT_H

#include "osconfig.h"
#include "dctypes.h"

#include "dimopxt.h"
#include "diflipt.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to flip monochrome images (on pixel data level)
 *  horizontally and vertically
 */
template<class T>
class DiMonoFlipTemplate
  : public DiMonoPixelTemplate<T>,
    protected DiFlipTemplate<T>
{
 public:

    /** constructor
     *
     ** @param  pixel    pointer to intermediate pixel representation
     *  @param  columns  number of columns
     *  @param  rows     number of rows
     *  @param  frames   number of frames
     *  @param  horz     flip horizontally if true
     *  @param  vert     flip vertically if true
     */
    DiMonoFlipTemplate(const DiMonoPixel *pixel,
                       const Uint16 columns,
                       const Uint16 rows,
                       const Uint32 frames,
                       const int horz,
                       const int vert)
      : DiMonoPixelTemplate<T>(pixel, (unsigned long)columns * (unsigned long)rows * frames),
        DiFlipTemplate<T>(1, columns, rows, frames)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
        {
            if (pixel->getCount() == (unsigned long)columns * (unsigned long)rows * frames)
                flip((const T *)pixel->getData(), horz, vert);
            else {
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                {
                   ofConsole.lockCerr() << "WARNING: could not flip image ... corrupted data." << endl;
                   ofConsole.unlockCerr();
                }
            }
        }
    }

    /** destructor
     */
    ~DiMonoFlipTemplate()
    {
    }


 private:

    /** choose flipping algorithm depending on given parameters
     *
     ** @param  pixel  pointer to pixel data which should be flipped
     *  @param  horz   flip horizontally if true
     *  @param  vert   flip vertically if true
     */
    inline void flip(const T *pixel,
                     const int horz,
                     const int vert)
    {
        if (pixel != NULL)
        {
            this->Data = new T[this->getCount()];
            if (this->Data != NULL)
            {
                if (horz && vert)
                    DiFlipTemplate<T>::flipHorzVert(&pixel, &this->Data);
                else if (horz)
                    DiFlipTemplate<T>::flipHorz(&pixel, &this->Data);
                else if (vert)
                    DiFlipTemplate<T>::flipVert(&pixel, &this->Data);
            }
        }
    }
};


#endif
