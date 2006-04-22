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
 *  Purpose: DicomMonochromeScaleTemplate (Header)
 *
 */


#ifndef __DIMOSCT_H
#define __DIMOSCT_H

#include "osconfig.h"
#include "dctypes.h"

#include "dimopxt.h"
#include "discalet.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to scale monochrome images (on pixel data level).
 */
template<class T>
class DiMonoScaleTemplate
  : public DiMonoPixelTemplate<T>,
    protected DiScaleTemplate<T>
{
 public:

    /** constructor
     *
     ** @param  pixel        pointer to intermediate pixel representation
     *  @param  columns      width of source image
     *  @param  rows         height of source image
     *  @param  left_pos     left coordinate of clipping area
     *  @param  top_pos      top coordinate of clipping area
     *  @param  src_cols     width of clipping area
     *  @param  src_rows     height of clipping area
     *  @param  dest_cols    width of destination image (scaled image)
     *  @param  dest_rows    height of destination image
     *  @param  frames       number of frames
     *  @param  interpolate  use of interpolation when scaling
     *  @param  pvalue       value possibly used for regions outside the image boundaries
     */
    DiMonoScaleTemplate(const DiMonoPixel *pixel,
                        const Uint16 columns,
                        const Uint16 rows,
                        const signed long left_pos,
                        const signed long top_pos,
                        const Uint16 src_cols,
                        const Uint16 src_rows,
                        const Uint16 dest_cols,
                        const Uint16 dest_rows,
                        const Uint32 frames,
                        const int interpolate,
                        const Uint16 pvalue)
      : DiMonoPixelTemplate<T>(pixel, (unsigned long)dest_cols * (unsigned long)dest_rows * frames),
        DiScaleTemplate<T>(1, columns, rows, left_pos, top_pos, src_cols, src_rows, dest_cols, dest_rows, frames)
    {
        if ((pixel != NULL) && (pixel->getCount() > 0))
        {
            if (pixel->getCount() == (unsigned long)columns * (unsigned long)rows * frames)
            {
                scale((const T *)pixel->getData(), pixel->getBits(), interpolate, pvalue);
                this->determineMinMax();
            } else {
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                {
                   ofConsole.lockCerr() << "WARNING: could not scale image ... corrupted data." << endl;
                   ofConsole.unlockCerr();
                }
            }
        }
    }

    /** destructor
     */
    virtual ~DiMonoScaleTemplate()
    {
    }


 private:

    /** scale pixel data
     *
     ** @param  pixel        pointer to pixel data to be scaled
     *  @param  bits         bit depth of pixel data
     *  @param  interpolate  use of interpolation when scaling
     *  @param  pvalue       value possibly used for regions outside the image boundaries
     */
    inline void scale(const T *pixel,
                      const unsigned int bits,
                      const int interpolate,
                      const Uint16 pvalue)
    {
        if (pixel != NULL)
        {
            this->Data = new T[this->getCount()];
            if (this->Data != NULL)
            {
                const T value = (T)((double)DicomImageClass::maxval(bits) * (double)pvalue /
                    (double)DicomImageClass::maxval(WIDTH_OF_PVALUES));
                scaleData(&pixel, &this->Data, interpolate, value);
             }
        }
    }
};


#endif
