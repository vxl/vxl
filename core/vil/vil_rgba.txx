// This is core/vil2/vil2_rgba.txx
#ifndef vil2_rgba_txx_
#define vil2_rgba_txx_

//-*- c++ -*-------------------------------------------------------------------
//
// vil2_rgba
// Author: Philip C. Pritchett, RRG, University of Oxford
// Created: 12 Nov 99
//
//-----------------------------------------------------------------------------

#include "vil2_rgba.h"

#undef VIL2_RGBA_INSTANTIATE
#define VIL2_RGBA_INSTANTIATE(T) \
template struct vil2_rgba<T >

#endif // vil2_rgba_txx_
