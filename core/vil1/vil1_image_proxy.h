#ifndef vil_image_proxy_h_
#define vil_image_proxy_h_
#ifdef __GNUC__
#pragma interface "vil_image_proxy"
#endif
/*
  fsm@robots.ox.ac.uk
*/

#include <vil/vil_image.h>

struct vil_image_proxy_impl;

struct vil_image_proxy : public vil_image
{
  vil_image_proxy(char const *file);
  ~vil_image_proxy() { }
  
private:
  vil_image_proxy(vil_image const &) { }
  vil_image_proxy(vil_image_proxy const &) { }
};

#endif
