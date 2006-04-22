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
 *  Purpose: DicomRegisterBase (Header)
 *
 */


#ifndef __DIREGBAS_H
#define __DIREGBAS_H

#include "osconfig.h"

#include "diutils.h"


/*------------------------*
 *  forward declarations  *
 *------------------------*/

class DiImage;
class DiColorImage;
class DiMonoPixel;
class DiDocument;
class DiRegisterBase;


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Abstract base class to register additional libraries
 */
class DiRegisterBase
{
 public:

    /** constructor, default
     */
    DiRegisterBase()
    {
    }

    /** destructor
     */
    virtual ~DiRegisterBase()
    {
    }

    /** create color image representation (abstract)
     *
     ** @param  docu    pointer to dataset (encapsulated)
     *  @param  status  image status
     *  @param  photo   photometric interpretation
     *
     ** @return pointer to new DiImage object (NULL if an error occurred)
     */
    virtual DiImage *createImage(const DiDocument *docu,
                                 const EI_Status status,
                                 const EP_Interpretation photo) = 0;

    /** create monochrome pixel data of color image (abstract)
     *
     ** @param  image  pointer to color image
     *  @param  red    coefficient by which the red component is weighted
     *  @param  green  coefficient by which the green component is weighted
     *  @param  blue   coefficient by which the blue component is weighted
     *
     ** @return pointer to new DiImage object (NULL if an error occurred)
     */
    virtual DiMonoPixel *createMonoImageData(const DiColorImage *image,
                                             const double red,
                                             const double green,
                                             const double blue) = 0;

    /// global pointer to registered 'dcmimage' library
    static DiRegisterBase *Pointer;
};


#endif
