// This is core/vil1/vil1_scale_intensities_image.h
#ifndef vil1_scale_intensities_image_h_
#define vil1_scale_intensities_image_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#  pragma interface
#endif
// :
// \file
// \brief Support function for vil1_scale_intensities_image_impl

#include <vcl_compiler.h>

class vil1_image;

#define vil1_decl(T) \
  bool vil1_scale_intensities_image(vil1_image const & base, double scale, double shift, \
                                    T * buf, int x0, int y0, int w, int h)

template <class T>
vil1_decl(T);

#undef vil1_decl

#endif // vil1_scale_intensities_image_h_
