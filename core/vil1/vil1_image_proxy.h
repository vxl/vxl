// This is core/vil1/vil1_image_proxy.h
#ifndef vil1_image_proxy_h_
#define vil1_image_proxy_h_
//:
// \file
// \author fsm

#include <vil1/vil1_image.h>

//: Proxy for an image
struct vil1_image_proxy : public vil1_image
{
  vil1_image_proxy(char const *file);
  ~vil1_image_proxy() = default;

 private:
  vil1_image_proxy(vil1_image const &) {}
  vil1_image_proxy(vil1_image_proxy const& i)  = default;
};

#endif // vil1_image_proxy_h_
