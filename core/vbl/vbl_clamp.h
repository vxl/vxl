#ifndef vbl_clamp_h_
#define vbl_clamp_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME    vbl_clamp - Templated vbl_clamping functions to ensure value is in range 0-255
// .HEADER  Basics Package
// .LIBRARY vbl
// .INCLUDE vbl/vbl_clamp.h
// .FILE    vbl/vbl_clamp.cxx
//
// .SECTION Author
//    Andrew Fitzgibbon and David Capel 

#include <vcl/vcl_compiler.h>

//:
// Default behaviour just returns value. Clamping of double to return byte is
// also defined here. Other clamps, such as vbl_rgb<double> to vbl_rgb<byte>
// may be defined in the appropriate places.
//
#if defined(VCL_SGI_CC)
// why not do this for all compilers, and rely on function overloading
// instead of template specialization?
extern unsigned char vbl_clamp(float  const &, unsigned char *);
extern unsigned char vbl_clamp(double const &, unsigned char *);
#else // not native SGI
template <class V, class U>
inline
U vbl_clamp(const V& b, U*)
{
   return U(b);
}
#endif

// vbl_clamp.cxx defines specializations for float/double to byte
// VCL_DECxLARE_SPECIALIZATION(unsigned char vbl_clamp(const float & d, unsigned char *))
// VCL_DECxLARE_SPECIALIZATION(unsigned char vbl_clamp(const double& d, unsigned char *))

#endif // vbl_clamp_h_
