// This is core/vil1/vil1_clamp_image.h
#ifndef vil1_clamp_image_h_
#define vil1_clamp_image_h_
//:
// \file
// \brief Support function for vil1_clamp_image_impl

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class vil1_image;

#define vil1_decl(T) \
bool vil1_clamp_image(vil1_image const &base, double low, double high, \
                      T *buf, int x0, int y0, int w, int h)

template <class T> vil1_decl(T);

#undef vil1_decl

#endif // vil1_clamp_image_h_
