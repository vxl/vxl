#ifndef vil_clamp_image_h_
#define vil_clamp_image_h_
#ifdef __GNUC__
#pragma interface
#endif

#include <vcl/vcl_compiler.h>

// .NAME vil_clamp_image - Support function for vil_clamp_image_impl
// .INCLUDE vil/vil_clamp_image.h
// .FILE vil_clamp_image.cxx

class vil_image;

#define vil_decl(T) \
bool vil_clamp_image(vil_image const &base, double low, double high, \
		     T *buf, int x0, int y0, int w, int h)

template <class T> vil_decl(T);

#ifdef VCL_KAI
template <typename T> struct vil_rgb;
template <> vil_decl(vil_rgb<unsigned char>);
template <> vil_decl(vil_rgb<float>);
template <> vil_decl(vil_rgb<double>);
#endif

#undef vil_decl

#endif // vil_clamp_image_h_
