#ifndef vipl_erode_disk_hxx_
#define vipl_erode_disk_hxx_

#include <iostream>
#include <algorithm>
#include "vipl_erode_disk.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_erode_disk <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop()
{
#ifdef DEBUG
  std::cout << "Starting vipl_erode_disk::section_applyop() ...";
#endif
  const ImgIn &in = this->in_data(0);
  ImgOut &out = *this->out_data_ptr();
  int size = (radius() < 0) ? 0 : int(radius());

  // circular mask was generated in preop(), we just use it here

  // apply filter:
#ifdef DEBUG
  std::cout << " set start & stop ...";
#endif
  int startx = vipl_filter<ImgIn,ImgOut,DataIn,DataOut,2,PixelItr>::start(this->X_Axis());
  int starty = 0; // = vipl_filter<ImgIn,ImgOut,DataIn,DataOut,2,PixelItr>::start(this->Y_Axis());
  int stopx  = vipl_filter<ImgIn,ImgOut,DataIn,DataOut,2,PixelItr>::stop(this->X_Axis());
  int stopy  = vipl_filter<ImgIn,ImgOut,DataIn,DataOut,2,PixelItr>::stop(this->Y_Axis()); // = height(out);
#ifdef DEBUG
  std::cout << " (" << startx << ':' << stopx << ',' << starty << ':' << stopy << ')';
  std::cout << " run over image ...";
#endif
  for (int j = starty; j < stopy; ++j)
    for (int i = startx; i < stopx; ++i)
    {
      DataIn v = fgetpixel(in, i, j, DataIn(0)); // set v to min of surrounding pixels:
      for (int x=0; x<=size; ++x)
      for (int y=0; y<=size; ++y)
        if (mask()[x][y]) {
          v = std::min(v, getpixel(in, i+x, j+y, DataIn(0)));
          v = std::min(v, getpixel(in, i-x, j+y, DataIn(0)));
          v = std::min(v, getpixel(in, i+x, j-y, DataIn(0)));
          v = std::min(v, getpixel(in, i-x, j-y, DataIn(0)));
        }
      fsetpixel(out, i, j, DataOut(v));
    }
#ifdef DEBUG
  std::cout << " done\n";
#endif
  return true;
}

// it is important that the mask be computed in preop, if it was done in
// section_applyop then on a large image it would be computed many times.
template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_erode_disk <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: preop()
{
#ifdef DEBUG
  std::cout << "Starting vipl_erode_disk::preop() ...";
#endif
  // create circular mask:
  int size = (radius() < 0) ? 0 : int(radius());
  float rs = (radius() < 0) ? 0 : radius() * radius();
  typedef bool* boolptr;
  if (mask() == nullptr) {
#ifdef DEBUG
    std::cout << " allocate mask ...";
#endif
    ref_mask() = new boolptr[1+size];
  }
  else {
#ifdef DEBUG
    std::cout << " re-allocate mask ...";
#endif
    for (int x=0; x<=size; ++x)
      if (mask()[x]) delete[] ref_mask()[x];
    delete[] ref_mask();
    ref_mask() = new boolptr[1+size];
  }
#ifdef DEBUG
  std::cout << " write mask ...";
#endif
  for (int x=0; x<=size; ++x) {
    ref_mask()[x] = new bool[size+1];
    for (int y=0; y<=size; ++y)
      ref_mask()[x][y] = (x*x + y*y <= rs);
  }
#ifdef DEBUG
  std::cout << " done\n";
#endif
  return true;
}

// Since we will know if radius changes between calls to filter, we
// destroy the mask in postop, after we are all done filtering
template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_erode_disk <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: postop()
{
#ifdef DEBUG
  std::cout << "Starting vipl_erode_disk::postop() ...";
#endif
  int size = (radius() < 0) ? 0 : int(radius());
  if (mask()) {
#ifdef DEBUG
    std::cout << " de-allocate mask ...";
#endif
    for (int x=0; x<=size; ++x)
      if (mask()[x]) delete[] ref_mask()[x];
    delete[] ref_mask();
    ref_mask()=nullptr;
  }
#ifdef DEBUG
  std::cout << " done\n";
#endif
  return true;
}

#endif // vipl_erode_disk_hxx_
