// This is core/vil1/vil1_rgba.hxx
#ifndef vil1_rgba_hxx_
#define vil1_rgba_hxx_

//-*- c++ -*-------------------------------------------------------------------
//
// vil1_rgba
// Author: Philip C. Pritchett, RRG, University of Oxford
// Created: 12 Nov 99
//
//-----------------------------------------------------------------------------

#include "vil1_rgba.h"

#undef VIL1_RGBA_INSTANTIATE
#define VIL1_RGBA_INSTANTIATE(T) \
template struct vil1_rgba<T >

#endif // vil1_rgba_hxx_
