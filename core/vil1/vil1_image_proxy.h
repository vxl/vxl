#ifndef vil_image_proxy_h_
#define vil_image_proxy_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vil/vil_image_proxy.h

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
