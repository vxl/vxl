#ifdef __GNUC__
#pragma implementation "vbl_clamp.h"
#endif
//
// Class: vbl_clamp
// Author: Andrew W. Fitzgibbon, Oxford RRG
// Created: 14 Aug 98
// Modifications:
//
//-----------------------------------------------------------------------------

#include <vbl/vbl_clamp.h>

VCL_DEFINE_SPECIALIZATION
unsigned char vbl_clamp(const float& d, unsigned char *)
{
  if (d > 255.0)
    return 255; 
  else if (d < 0.0)
    return 0; 
  else
    return (unsigned char)d; 
}

VCL_DEFINE_SPECIALIZATION
unsigned char vbl_clamp(const double& d, unsigned char *)
{
  if (d > 255.0)
    return 255; 
  else if (d < 0.0)
    return 0; 
  else
    return (unsigned char)d; 
}

