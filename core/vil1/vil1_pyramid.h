#ifndef vil_pyramid_h_
#define vil_pyramid_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vil_pyramid
// .INCLUDE vil/vil_pyramid.h
// .FILE vil_pyramid.cxx
//
// .SECTION Description
// vil_pyramid is *not* an image. It is like an array of images, with
// the 0th element as the given image and the ith element a subsampled
// version of the (i-1)th element. The subsampled images are computed
// on demand and cached for later use.
//
// \author fsm@robots.ox.ac.uk

#include <vil/vil_image.h>
#include <vcl/vcl_vector.h>

struct vil_pyramid 
{
  enum cache_strategy {
    none = 0,
    memory = 1,
    blocked = 2 // not implemented. don't try to use it.
  };
  
  vil_pyramid(vil_image const &, cache_strategy = memory);
  ~vil_pyramid();
  
  unsigned size() const { return levels.size(); }
  vil_image operator[](unsigned i);
  
private:
  cache_strategy cs;
  vcl_vector<vil_image> levels;
};

#endif
