//-*- c++ -*-------------------------------------------------------------------
//
// Class: vbl_rgba
// Author: Philip C. Pritchett, RRG, University of Oxford
// Created: 12 Nov 99
// Modifications:
//   
// See vbl_rgba.h
//
//-----------------------------------------------------------------------------

#include "vbl_rgba.h"

#undef VBL_RGBA_INSTANTIATE
#define VBL_RGBA_INSTANTIATE(T)\
template class vbl_rgba<T >;
