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
 *  Purpose: DicomMonochromePixel (Source)
 *
 */


#include "osconfig.h"

#include "dimopx.h"
#include "dimomod.h"
#include "diinpx.h"
#include "dimoopx.h"


/*----------------*
 *  constructors  *
 *----------------*/

DiMonoPixel::DiMonoPixel(const unsigned long count)
  : DiPixel(count),
    Modality(NULL)
{
}


DiMonoPixel::DiMonoPixel(const DiInputPixel *pixel,
                         DiMonoModality *modality)
  : DiPixel((pixel != NULL) ? pixel->getComputedCount() : 0,
            (pixel != NULL) ? pixel->getPixelCount() : 0),
    Modality(modality)
{
}


DiMonoPixel::DiMonoPixel(const DiPixel *pixel,
                         DiMonoModality *modality)
  : DiPixel(pixel->getCount(), pixel->getInputCount()),
    Modality(modality)
{
}


DiMonoPixel::DiMonoPixel(const DiMonoPixel *pixel,
                         const unsigned long count)
  : DiPixel(count, pixel->getInputCount()),
    Modality(pixel->Modality)
{
    if (Modality != NULL)
        Modality->addReference();
}


DiMonoPixel::DiMonoPixel(const DiMonoOutputPixel *pixel,
                         DiMonoModality *modality)
  : DiPixel(pixel->getCount()),
    Modality(modality)
{
}


/*--------------*
 *  destructor  *
 *--------------*/

DiMonoPixel::~DiMonoPixel()
{
    if (Modality != NULL)
        Modality->removeReference();
}
