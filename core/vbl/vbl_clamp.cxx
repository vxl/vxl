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

unsigned char vbl_clamp(const float& d, unsigned char *)
{
  if (d > 255.0)
    return 255; 
  else if (d < 0.0)
    return 0; 
  else
    return (unsigned char)d; 
}

unsigned char vbl_clamp(const double& d, unsigned char *)
{
  if (d > 255.0)
    return 255; 
  else if (d < 0.0)
    return 0; 
  else
    return (unsigned char)d; 
}

