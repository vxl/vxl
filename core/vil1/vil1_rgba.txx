//-*- c++ -*-------------------------------------------------------------------
//
// .NAME vil_rgba
// Author: Philip C. Pritchett, RRG, University of Oxford
// Created: 12 Nov 99
//
//-----------------------------------------------------------------------------

#include "vil_rgba.h"

#undef VIL_RGBA_INSTANTIATE
#define VIL_RGBA_INSTANTIATE(T)\
template struct vil_rgba<T >;
