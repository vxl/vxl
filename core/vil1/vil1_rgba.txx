//-*- c++ -*-------------------------------------------------------------------
//
// Class: vil_rgba
// Author: Philip C. Pritchett, RRG, University of Oxford
// Created: 12 Nov 99
// Modifications:
//   
// See vil_rgba.h
//
//-----------------------------------------------------------------------------

#include "vil_rgba.h"

#undef VBL_RGBA_INSTANTIATE
#define VBL_RGBA_INSTANTIATE(T)\
template struct vil_rgba<T >;
