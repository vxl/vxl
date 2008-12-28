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
 *  Purpose: DicomPixelRepresentationTemplate (Header)
 *
 */


#ifndef __DIPXREPT_H
#define __DIPXREPT_H

#include "osconfig.h"

#include "diutils.h"

#ifdef HAVE_EXPLICIT_TEMPLATE_SPECIALIZATION
#define EXPLICIT_SPECIALIZATION template<>
#else
#define EXPLICIT_SPECIALIZATION
#endif


/*---------------------*
 *  class declaration  *
 *---------------------*/

/** Template class to determine pixel representation.
 *  Size of basic structure, sign-extension
 */
template<class T>
class DiPixelRepresentationTemplate
{
 public:
    virtual ~DiPixelRepresentationTemplate() {}

    /** check whether template type T is signed or not
     *
     ** @return true if signed, false otherwise
     */
    inline int isSigned() const;

    /** determine integer representation for template type T
     *
     ** @return integer representation
     */
    virtual EP_Representation getRepresentation() const;
};


/********************************************************************/


EXPLICIT_SPECIALIZATION
inline EP_Representation DiPixelRepresentationTemplate<Uint8>::getRepresentation() const
{
    return EPR_Uint8;
}


EXPLICIT_SPECIALIZATION
inline EP_Representation DiPixelRepresentationTemplate<Sint8>::getRepresentation() const
{
    return EPR_Sint8;
}


EXPLICIT_SPECIALIZATION
inline EP_Representation DiPixelRepresentationTemplate<Uint16>::getRepresentation() const
{
    return EPR_Uint16;
}


EXPLICIT_SPECIALIZATION
inline EP_Representation DiPixelRepresentationTemplate<Sint16>::getRepresentation() const
{
    return EPR_Sint16;
}


EXPLICIT_SPECIALIZATION
inline EP_Representation DiPixelRepresentationTemplate<Uint32>::getRepresentation() const
{
    return EPR_Uint32;
}


EXPLICIT_SPECIALIZATION
inline EP_Representation DiPixelRepresentationTemplate<Sint32>::getRepresentation() const
{
    return EPR_Sint32;
}


EXPLICIT_SPECIALIZATION
inline int DiPixelRepresentationTemplate<Uint8>::isSigned() const
{
    return 0;
}


EXPLICIT_SPECIALIZATION
inline int DiPixelRepresentationTemplate<Uint16>::isSigned() const
{
    return 0;
}


EXPLICIT_SPECIALIZATION
inline int DiPixelRepresentationTemplate<Uint32>::isSigned() const
{
    return 0;
}


EXPLICIT_SPECIALIZATION
inline int DiPixelRepresentationTemplate<Sint8>::isSigned() const
{
    return 1;
}


EXPLICIT_SPECIALIZATION
inline int DiPixelRepresentationTemplate<Sint16>::isSigned() const
{
    return 1;
}


EXPLICIT_SPECIALIZATION
inline int DiPixelRepresentationTemplate<Sint32>::isSigned() const
{
    return 1;
}


#endif // __DIPXREPT_H
