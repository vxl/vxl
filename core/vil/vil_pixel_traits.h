// This is mul/vil2/vil2_pixel_traits.h
#ifndef vil2_pixel_traits_h_
#define vil2_pixel_traits_h_
//:
// \file
// \author Ian Scott.

#include <vil/vil_rgb.h>
#include <vxl_config.h>

template <class T>
class vil2_pixel_traits
{
public:
  typedef T component_type;
};

template <class T>
class vil2_pixel_traits<vil_rgb<T> >
{
public:
  typedef T component_type;
};


#endif // vil2_pixel_traits
