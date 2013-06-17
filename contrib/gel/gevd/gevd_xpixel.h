#ifndef xpixel_h_
#define xpixel_h_

//:
// \file

#include <vnl/vnl_vector.h>
#include "gevd_bufferxy.h"

//: Get reference to pixel as a pointer to a vnl_vector<float>, at indexes (x, y).

inline vnl_vector<float>*&
fvectorPixel(gevd_bufferxy& buf, int x, int y)
{
  return (*((vnl_vector<float>**) buf.GetElementAddr(x,y)));
}

inline vnl_vector<float>*
fvectorPixel(const gevd_bufferxy& buf, int x, int y)
{
  return (*((vnl_vector<float>*const *) buf.GetElementAddr(x,y)));
}

inline void freeFVectors(gevd_bufferxy& buf)
{
  for (int x = 0; x < buf.GetSizeX(); x++)
    for (int y = 0; y < buf.GetSizeY(); y++)
      delete fvectorPixel(buf, x, y);
}

#endif // xpixel_h_
