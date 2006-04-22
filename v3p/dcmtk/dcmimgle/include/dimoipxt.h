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
 *  Purpose: DicomMonochromeInputPixelTemplate (Header)
 *
 */


#ifndef __DIMOIPXT_H
#define __DIMOIPXT_H

#include "osconfig.h"
#include "ofconsol.h"
#include "ofbmanip.h"

#include "dimopxt.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to convert monochrome pixel data to intermediate representation
 */
template<class T1, class T2, class T3>
class DiMonoInputPixelTemplate
  : public DiMonoPixelTemplate<T3>
{
 public:

    /** constructor
     *
     ** @param  pixel     pointer to input pixel representation
     *  @param  modality  pointer to modality transform object
     */
    DiMonoInputPixelTemplate(DiInputPixel *pixel,
                             DiMonoModality *modality)
      : DiMonoPixelTemplate<T3>(pixel, modality)
    {
        /* erase empty part of the buffer (=blacken the background) */
        if ((this->Data != NULL) && (this->InputCount < this->Count))
            OFBitmanipTemplate<T3>::zeroMem(this->Data + this->InputCount, this->Count - this->InputCount);
        if ((pixel != NULL) && (this->Count > 0))
        {
            // check whether to apply any modality transform
            if ((this->Modality != NULL) && this->Modality->hasLookupTable() && (bitsof(T1) <= MAX_TABLE_ENTRY_SIZE))
            {
                modlut(pixel);
                // ignore modality LUT min/max values since the image does not necessarily have to use all LUT entries
                this->determineMinMax();
            }
            else if ((this->Modality != NULL) && this->Modality->hasRescaling())
            {
                rescale(pixel, this->Modality->getRescaleSlope(), this->Modality->getRescaleIntercept());
                this->determineMinMax((T3)this->Modality->getMinValue(), (T3)this->Modality->getMaxValue());
            } else {
                rescale(pixel);                     // "copy" or reference pixel data
                this->determineMinMax((T3)this->Modality->getMinValue(), (T3)this->Modality->getMaxValue());
            }
        }
    }

    /** destructor
     */
    virtual ~DiMonoInputPixelTemplate()
    {
    }


 private:

    /** initialize optimization LUT
     *
     ** @param  lut   reference to storage area for lookup table
     *  @param  ocnt  number of LUT entries (will be check as optimization criteria)
     *
     ** @return status, true if successful (LUT has been created), false otherwise
     */
    inline int initOptimizationLUT(T3 *&lut,
                                   const unsigned long ocnt)
    {
        int result = 0;
        if ((sizeof(T1) <= 2) && (this->InputCount > 3 * ocnt))                     // optimization criteria
        {                                                                     // use LUT for optimization
            lut = new T3[ocnt];
            if (lut != NULL)
            {
#ifdef DEBUG
                if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Informationals))
                {
                    ofConsole.lockCerr() << "INFO: using optimized routine with additional LUT" << endl;
                    ofConsole.unlockCerr();
                }
#endif
                result = 1;
            }
        }
        return result;
    }

    /** perform modality LUT transform
     *
     ** @param  input  pointer to input pixel representation
     */
    void modlut(DiInputPixel *input)
    {
        const T1 *pixel = (const T1 *)input->getData();
        if ((pixel != NULL) && (this->Modality != NULL))
        {
            const DiLookupTable *mlut = this->Modality->getTableData();
            if (mlut != NULL)
            {
                const int useInputBuffer = (sizeof(T1) == sizeof(T3)) && (this->Count <= input->getCount());
                if (useInputBuffer)                            // do not copy pixel data, reference them!
                {
                    this->Data = (T3 *)input->getData();
                    input->removeDataReference();              // avoid double deletion
                } else
                    this->Data = new T3[this->Count];
                if (this->Data != NULL)
                {
#ifdef DEBUG
                    if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Informationals))
                    {
                        ofConsole.lockCerr() << "INFO: using modality routine 'modlut()'" << endl;
                        ofConsole.unlockCerr();
                    }
#endif
                    register T2 value = 0;
                    const T2 firstentry = mlut->getFirstEntry(value);                     // choose signed/unsigned method
                    const T2 lastentry = mlut->getLastEntry(value);
                    const T3 firstvalue = (T3)mlut->getFirstValue();
                    const T3 lastvalue = (T3)mlut->getLastValue();
                    register const T1 *p = pixel + input->getPixelStart();
                    register T3 *q = this->Data;
                    register unsigned long i;
                    T3 *lut = NULL;
                    const unsigned long ocnt = (unsigned long)input->getAbsMaxRange();    // number of LUT entries
                    if (initOptimizationLUT(lut, ocnt))
                    {                                                                     // use LUT for optimization
                        const T2 absmin = (T2)input->getAbsMinimum();
                        q = lut;
                        for (i = 0; i < ocnt; i++)                                        // calculating LUT entries
                        {
                            value = (T2)i + absmin;
                            if (value <= firstentry)
                                *(q++) = firstvalue;
                            else if (value >= lastentry)
                                *(q++) = lastvalue;
                            else
                                *(q++) = (T3)mlut->getValue(value);
                        }
                        const T3 *lut0 = lut - (T2)absmin;                                // points to 'zero' entry
                        q = this->Data;
                        for (i = this->InputCount; i != 0; i--)                                 // apply LUT
                            *(q++) = *(lut0 + (*(p++)));
                    }
                    if (lut == NULL)                                                      // use "normal" transformation
                    {
                        for (i = this->InputCount; i != 0; i--)
                        {
                            value = (T2)(*(p++));
                            if (value <= firstentry)
                                *(q++) = firstvalue;
                            else if (value >= lastentry)
                                *(q++) = lastvalue;
                            else
                                *(q++) = (T3)mlut->getValue(value);
                        }
                    }
                    delete[] lut;
                }
            }
        }
    }

    /** perform rescale slope/intercept transform
     *
     ** @param  input      pointer to input pixel representation
     *  @param  slope      rescale slope value (optional)
     *  @param  intercept  rescale intercept value (optional)
     */
    void rescale(DiInputPixel *input,
                 const double slope = 1.0,
                 const double intercept = 0.0)
    {
        const T1 *pixel = (const T1 *)input->getData();
        if (pixel != NULL)
        {
            const int useInputBuffer = (sizeof(T1) == sizeof(T3)) && (this->Count <= input->getCount());
            if (useInputBuffer)
            {                                              // do not copy pixel data, reference them!
                this->Data = (T3 *)input->getData();
                input->removeDataReference();              // avoid double deletion
            } else
                this->Data = new T3[this->Count];
            if (this->Data != NULL)
            {
                register T3 *q = this->Data;
                register unsigned long i;
                if ((slope == 1.0) && (intercept == 0.0))
                {
                    if (!useInputBuffer)
                    {
                        register const T1 *p = pixel + input->getPixelStart();
                        for (i = this->InputCount; i != 0; i--)   // copy pixel data: can't use copyMem because T1 isn't always equal to T3
                            *(q++) = (T3)*(p++);
                    }
                }
                else
                {
#ifdef DEBUG
                    if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Informationals))
                    {
                        ofConsole.lockCerr() << "INFO: using modality routine 'rescale()'" << endl;
                        ofConsole.unlockCerr();
                    }
#endif
                    T3 *lut = NULL;
                    register const T1 *p = pixel + input->getPixelStart();
                    const unsigned long ocnt = (unsigned long)input->getAbsMaxRange();    // number of LUT entries
                    if (initOptimizationLUT(lut, ocnt))
                    {                                                                     // use LUT for optimization
                        const double absmin = input->getAbsMinimum();
                        q = lut;
                        if (slope == 1.0)
                        {
                            for (i = 0; i < ocnt; i++)                                    // calculating LUT entries
                                *(q++) = (T3)((double)i + absmin + intercept);
                        } else {
                            if (intercept == 0.0)
                            {
                                for (i = 0; i < ocnt; i++)
                                    *(q++) = (T3)(((double)i + absmin) * slope);
                            } else {
                                for (i = 0; i < ocnt; i++)
                                    *(q++) = (T3)(((double)i + absmin) * slope + intercept);
                            }
                        }
                        const T3 *lut0 = lut - (T2)absmin;                                // points to 'zero' entry
                        q = this->Data;
                        for (i = this->InputCount; i != 0; i--)                                 // apply LUT
                            *(q++) = *(lut0 + (*(p++)));
                    }
                    if (lut == NULL)                                                      // use "normal" transformation
                    {
                        if (slope == 1.0)
                        {
                            for (i = this->Count; i != 0; i--)
                                *(q++) = (T3)((double)*(p++) + intercept);
                        } else {
                            if (intercept == 0.0)
                            {
                                for (i = this->InputCount; i != 0; i--)
                                    *(q++) = (T3)((double)*(p++) * slope);
                            } else {
                                for (i = this->InputCount; i != 0; i--)
                                    *(q++) = (T3)((double)*(p++) * slope + intercept);
                            }
                        }
                    }
                    delete[] lut;
                }
            }
        }
    }
};


#endif
