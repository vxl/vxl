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
 *  Purpose: DicomGSDFLUT (Source)
 *
 */


#include "osconfig.h"

#include "ofconsol.h"
#include "digsdlut.h"
#include "displint.h"

#define INCLUDE_CMATH
#include "ofstdinc.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiGSDFLUT::DiGSDFLUT(const unsigned long count,
                     const Uint16 max,
                     const Uint16 *ddl_tab,
                     const double *val_tab,
                     const unsigned long ddl_cnt,
                     const double *gsdf_tab,
                     const double *gsdf_spl,
                     const unsigned int gsdf_cnt,
                     const double jnd_min,
                     const double jnd_max,
                     const double amb,
                     const double illum,
                     const OFBool inverse,
                     ostream *stream,
                     const OFBool printMode)
  : DiDisplayLUT(count, max, amb, illum)
{
    if ((Count > 0) && (Bits > 0))
    {
#ifdef DEBUG
        if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Informationals))
        {
            ofConsole.lockCerr() << "INFO: new GSDF LUT with " << Bits << " bits output and "
                                 << Count << " entries created !" << endl;
            ofConsole.unlockCerr();
        }
#endif
        Valid = createLUT(ddl_tab, val_tab, ddl_cnt, gsdf_tab, gsdf_spl, gsdf_cnt, jnd_min, jnd_max,
                          inverse, stream, printMode);
    }
}


/*--------------*
 *  destructor  *
 *--------------*/

DiGSDFLUT::~DiGSDFLUT()
{
}


/********************************************************************/


int DiGSDFLUT::createLUT(const Uint16 *ddl_tab,
                         const double *val_tab,
                         const unsigned long ddl_cnt,
                         const double *gsdf_tab,
                         const double *gsdf_spl,
                         const unsigned int gsdf_cnt,
                         const double jnd_min,
                         const double jnd_max,
                         const OFBool inverse,
                         ostream *stream,
                         const OFBool printMode)
{
    if ((ddl_tab != NULL) && (val_tab != NULL) && (ddl_cnt > 0) && (gsdf_tab != NULL) && (gsdf_spl != NULL) && (gsdf_cnt > 0))
    {
        int status = 0;
        const unsigned long gin_ctn = (inverse) ? ddl_cnt : Count;      // number of points to be interpolated
        double *jidx = new double[gin_ctn];
        if (jidx != NULL)
        {
            const double dist = (jnd_max - jnd_min) / (gin_ctn - 1);    // distance between two entries
            register unsigned long i;
            register double *s = jidx;
            register double value = jnd_min;                            // first value is fixed !
            for (i = gin_ctn; i > 1; i--)                               // initialize scaled JND index array
            {
                *(s++) = value;
                value += dist;                                          // add step by step ...
            }
            *s = jnd_max;                                               // last value is fixed !
            double *jnd_idx = new double[gsdf_cnt];
            if (jnd_idx != NULL)
            {
                s = jnd_idx;
                for (i = 0; i < gsdf_cnt; i++)                          // initialize JND index array
                    *(s++) = i + 1;
                double *gsdf = new double[gin_ctn];                     // interpolated GSDF
                if (gsdf != NULL)
                {
                    if (DiCubicSpline<double, double>::Interpolation(jnd_idx, gsdf_tab, gsdf_spl, gsdf_cnt, jidx, gsdf, (unsigned int)gin_ctn))
                    {
                        DataBuffer = new Uint16[Count];
                        if (DataBuffer != NULL)
                        {
                            const double amb = getAmbientLightValue();
                            register Uint16 *q = DataBuffer;
                            register unsigned long j = 0;
                            if (inverse)
                            {
                                register double v;
                                const double factor = (double)(ddl_cnt - 1) / (double)(Count - 1);
                                /* convert DDL to P-Value */
                                for (i = 0; i < Count; i++)
                                {
                                    v = val_tab[(int)(i * factor)] + amb;                 // need to scale index to range of value table
                                    while ((j + 1 < ddl_cnt) && (gsdf[j] < v))            // search for closest index, assuming monotony
                                        j++;
                                    if ((j > 0) && (fabs(gsdf[j - 1] - v) < fabs(gsdf[j] - v)))
                                        j--;
                                    *(q++) = ddl_tab[j];
                                }
                            } else {
                                register const double *r = gsdf;
                                /* convert P-Value to DDL */
                                for (i = Count; i != 0; i--, r++)
                                {
                                    while ((j + 1 < ddl_cnt) && (val_tab[j] + amb < *r))  // search for closest index, assuming monotony
                                        j++;
                                    if ((j > 0) && (fabs(val_tab[j - 1] + amb - *r) < fabs(val_tab[j] + amb - *r)))
                                        j--;
                                    *(q++) = ddl_tab[j];
                                }
                            }
                            Data = DataBuffer;
                            if (stream != NULL)                         // write curve data to file
                            {
                                if (Count == ddl_cnt)                   // check whether GSDF LUT fits exactly to DISPLAY file
                                {
                                    for (i = 0; i < ddl_cnt; i++)
                                    {
                                        (*stream) << ddl_tab[i];                               // DDL
                                        stream->setf(ios::fixed, ios::floatfield);
                                        if (printMode)
                                            (*stream) << "\t" << val_tab[i] + amb;             // CC
                                        (*stream) << "\t" << gsdf[i];                          // GSDF
                                        if (printMode)
                                        {
                                            if (inverse)
                                                (*stream) << "\t" << gsdf[Data[i]];            // PSC'
                                            else
                                                (*stream) << "\t" << val_tab[Data[i]] + amb;   // PSC
                                        }
                                        (*stream) << endl;
                                    }
                                } else {
                                    if (DicomImageClass::checkDebugLevel(DicomImageClass::DL_Warnings))
                                    {
                                        ofConsole.lockCerr() << "WARNING: can't write curve data, "
                                                             << "wrong DISPLAY file or GSDF LUT !" << endl;
                                        ofConsole.unlockCerr();
                                    }
                                }
                            }
                            status = 1;
                        }
                    }
                }
                delete[] gsdf;
            }
            delete[] jnd_idx;
        }
        delete[] jidx;
        return status;
    }
    return 0;
}
