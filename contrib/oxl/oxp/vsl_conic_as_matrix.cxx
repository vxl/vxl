//-*- c++ -*-------------------------------------------------------------------
#ifdef __GNUC__
#pragma implementation "vsl_conic_as_matrix.h"
#endif
//
// Class: vsl_conic_as_matrix
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 12 Nov 99
// Modifications:
//   991112 AWF Initial version.
//
//-----------------------------------------------------------------------------

#include "vsl_conic_as_matrix.h"

#include <oxp/vsl_conic_as_params.h>


vsl_conic_as_matrix::vsl_conic_as_matrix(double cx, double cy, double rx, double ry, double theta)
{
  vsl_conic_as_params a(cx, cy, rx, ry, theta);
  A_(0,0) =           a[0];  
  A_(0,1) = A_(1,0) = a[1] / 2;
  A_(0,2) = A_(2,0) = a[3] / 2;
  A_(1,1) =           a[2];  
  A_(1,2) = A_(2,1) = a[4] / 2;
  A_(2,2) =           a[5];
}
