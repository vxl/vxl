// This is vxl/vil/vil_image_proxy.h
#ifndef vil_image_proxy_h_
#define vil_image_proxy_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

#include <vil/vil_image.h>

//: Proxy for an image
struct vil_image_proxy : public vil_image
{
  vil_image_proxy(char const *file);
  ~vil_image_proxy() { }

 private:
  vil_image_proxy(vil_image const &) { }
  vil_image_proxy(vil_image_proxy const &) { }
};

#endif // vil_image_proxy_h_
