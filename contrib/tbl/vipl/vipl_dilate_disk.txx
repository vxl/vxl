#ifndef vipl_dilate_disk_txx_
#define vipl_dilate_disk_txx_

#include "vipl_dilate_disk.h"
#include <vcl_algorithm.h>

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_dilate_disk <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop()
{
  const ImgIn &in = this->in_data(0);
  ImgOut &out = this->out_data();
  int size = (radius() < 0) ? 0 : int(radius());
  // mask is filled in preop function
  // apply filter:
  int startx = start(this->X_Axis());
  int starty = start(this->Y_Axis());
  int stopx = stop(this->X_Axis());
  int stopy = stop(this->Y_Axis());
  for (register int j = starty, ej =  stopy; j < ej  ; ++j)
    for (register int i = startx, ei = stopx; i < ei ; ++i)
    {
      DataIn v = fgetpixel(in, i, j, DataIn(0)); // set v to max of surrounding pixels:
      for (register int x=0; x<=size; ++x)
      for (register int y=0; y<=size; ++y)
        if (mask()[x][y]) {
          v = vcl_max(v, getpixel(in, i+x, j+y, DataIn(0)));
          v = vcl_max(v, getpixel(in, i-x, j+y, DataIn(0)));
          v = vcl_max(v, getpixel(in, i+x, j-y, DataIn(0)));
          v = vcl_max(v, getpixel(in, i-x, j-y, DataIn(0)));
        }
      fsetpixel(out, i, j, DataOut(v));
    }
  return true;
}

// it is important that the mask be computed in preop, if it was done in
// section_applyop then on a large image it would be computed many times.
template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_dilate_disk <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: preop()
{
  // create circular mask:
  int size = (radius() < 0) ? 0 : int(radius());
  float rs = (radius() < 0) ? 0 : radius() * radius();
  typedef bool* boolptr;
  if (mask() == 0)
         ref_mask() = new boolptr[1+size];
  else {
    for (int x=0; x<=size; ++x)
      if (mask()[x]) delete[] ref_mask()[x];
    delete[] ref_mask();
    ref_mask() = new boolptr[1+size];
  }
  for (int x=0; x<=size; ++x) {
    ref_mask()[x] = new bool[size+1];
    for (int y=0; y<=size; ++y)
      ref_mask()[x][y] = (x*x + y*y <= rs);
  }
  return true;
}

// Since we will know if radius changes between calls to filter, we
// destroy the mask in postop, after we are all done filtering
template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_dilate_disk <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: postop()
{
  int size = (radius() < 0) ? 0 : int(radius());
  if (mask()) {
    for (int x=0; x<=size; ++x)
      if (mask()[x]) delete[] ref_mask()[x];
    delete[] ref_mask();
    ref_mask()=0;
  }
  return true;
}

#endif // vipl_dilate_disk_txx_
