#ifndef vipl_erode_disk_txx_
#define vipl_erode_disk_txx_

#include "vipl_erode_disk.h"
#include <vcl_algorithm.h>
#include <vcl_iostream.h>

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_erode_disk <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop(){
#ifdef DEBUG
  vcl_cout << "Starting vipl_erode_disk::section_applyop() ...";
#endif
  const ImgIn &in = in_data(0);
  ImgOut &out = *out_data_ptr();
  int size = (radius() < 0) ? 0 : int(radius());

  // circular mask was generated in preop(), we just use it here

  // apply filter:
#ifdef DEBUG
  vcl_cout << " set start & stop ...";
#endif
  int startx = start(X_Axis());
  int starty = 0 // = start(Y_Axis());
  int stopx = stop(X_Axis());
  int stopy = stop(Y_Axis()); // = height(out);
  const DataIn dummy = DataIn(0); // dummy initialisation to avoid compiler warning
#ifdef DEBUG
  vcl_cout << " (" << startx << ':' << stopx << ',' << starty << ':' << stopy << ')';
  vcl_cout << " run over image ...";
#endif
  for (register int j = starty; j < stopy; ++j)
    for (register int i = startx; i < stopx; ++i)
    {
      DataIn v = fgetpixel(in, i, j, dummy); // set v to min of surrounding pixels:
      for (register int x=0; x<=size; ++x)
      for (register int y=0; y<=size; ++y)
        if (mask()[x][y]) {
          v = vcl_min(v, getpixel(in, i+x, j+y, dummy));
          v = vcl_min(v, getpixel(in, i-x, j+y, dummy));
          v = vcl_min(v, getpixel(in, i+x, j-y, dummy));
          v = vcl_min(v, getpixel(in, i-x, j-y, dummy));
        }
      fsetpixel(out, i, j, (DataOut)v);
    }
#ifdef DEBUG
  vcl_cout << " done\n";
#endif
  return true;
}

// it is important that the mask be computed in preop, if it was done in
// section_applyop then on a large image it would be computed many times.
template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_erode_disk <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: preop(){
#ifdef DEBUG
  vcl_cout << "Starting vipl_erode_disk::preop() ...";
#endif
  // create circular mask:
  int size = (radius() < 0) ? 0 : int(radius());
  float rs = (radius() < 0) ? 0 : radius() * radius();
  typedef bool* boolptr;
  if (mask() == 0) {
#ifdef DEBUG
    vcl_cout << " allocate mask ...";
#endif
    ref_mask() = new boolptr[1+size];
  }
  else {
#ifdef DEBUG
    vcl_cout << " re-allocate mask ...";
#endif
    for (int x=0; x<=size; ++x)
      if (mask()[x]) delete[] ref_mask()[x];
    delete[] ref_mask();
    ref_mask() = new boolptr[1+size];
  }
#ifdef DEBUG
  vcl_cout << " write mask ...";
#endif
  for (int x=0; x<=size; ++x) {
    ref_mask()[x] = new bool[size+1];
    for (int y=0; y<=size; ++y)
      ref_mask()[x][y] = (x*x + y*y <= rs);
  }
#ifdef DEBUG
  vcl_cout << " done\n";
#endif
  return true;
}

// Since we will know if radius changes between calls to filter, we
// destroy the mask in postop, after we are all done filtering
template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_erode_disk <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: postop(){
#ifdef DEBUG
  vcl_cout << "Starting vipl_erode_disk::postop() ...";
#endif
  int size = (radius() < 0) ? 0 : int(radius());
  if (mask()) {
#ifdef DEBUG
    vcl_cout << " de-allocate mask ...";
#endif
    for (int x=0; x<=size; ++x)
      if (mask()[x]) delete[] ref_mask()[x];
    delete[] ref_mask();
    ref_mask()=0;
  }
#ifdef DEBUG
  vcl_cout << " done\n";
#endif
  return true;
}

#endif // vipl_erode_disk_txx_
