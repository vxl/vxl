// This is core/vil1/vil1_rgba.txx
// -*- c++ -*-
#ifndef vil1_rgba_txx_
#define vil1_rgba_txx_

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

#endif // vil1_rgba_txx_
