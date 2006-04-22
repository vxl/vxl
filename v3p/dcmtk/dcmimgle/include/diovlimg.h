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
 *  Purpose: DicomOverlayImage (Header)
 *
 */


#ifndef __DIOVLIMG_H
#define __DIOVLIMG_H

#include "osconfig.h"

#include "dimo2img.h"


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Class for standalone overlay images
 */
class DiOverlayImage
  : public DiMono2Image
{
 public:

    /** constructor
     *
     ** @param  docu    pointer to dataset (encapsulated)
     *  @param  status  current image status
     */
    DiOverlayImage(const DiDocument *docu,
                   const EI_Status status);

    /** destructor
     */
    virtual ~DiOverlayImage();
};


#endif
