#ifndef vil_clamp_h_
#define vil_clamp_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME    vil_clamp - Templated vil_clamping functions to ensure value is in range 0-255
// .HEADER  Basics Package
// .LIBRARY vil
// .INCLUDE vil/vil_clamp.h
// .FILE    vil/vil_clamp.cxx
//
// .SECTION Author
//    Andrew Fitzgibbon and David Capel 

#include <vcl/vcl_compiler.h>

//:
// Default behaviour just returns value. Clamping of double to return byte is
// also defined here. Other clamps, such as vil_rgb<double> to vil_rgb<byte>
// may be defined in the appropriate places.
//
#if !defined(VCL_SGI_CC)
template <class V, class U>
inline
U vil_clamp(const V& b, U*)
{
   return U(b);
}
#endif

inline
unsigned char vil_clamp(const float& d, unsigned char *)
{
  if (d > 255.0)
    return 255; 
  else if (d < 0.0)
    return 0; 
  else
    return (unsigned char)d; 
}

inline
unsigned char vil_clamp(const double& d, unsigned char *)
{
  if (d > 255.0)
    return 255; 
  else if (d < 0.0)
    return 0; 
  else
    return (unsigned char)d; 
}

#endif // vil_clamp_h_
