#ifndef vipl_erode_disk_txx_
#define vipl_erode_disk_txx_

#include <vcl_algorithm.h>
#include <vcl_iostream.h>

#include "vipl_erode_disk.h"

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_erode_disk <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop(){
  vcl_cout << "Starting vipl_erode_disk::section_applyop() ...";
  const ImgIn &in = in_data(0);
  ImgOut &out = *out_data_ptr();
  int size = (radius() < 0) ? 0 : int(radius());

  // circular mask was generated in preop(), we just use it here

  // apply filter:
  vcl_cout << " set start & stop ...";
  int startx = start(X_Axis());
  int starty = start(Y_Axis());
  int stopx = stop(X_Axis());
  int stopy = stop(Y_Axis());
  vcl_cout << " (" << startx << ':' << stopx << ',' << starty << ':' << stopy << ')';
  vcl_cout << " run over image ...";
  for(int j = starty; j < stopy; ++j)
    for(int i = startx; i < stopx; ++i) {
      register DataIn
      v = fgetpixel(in, i, j, DataIn()); // set v to min of surrounding pixels:
      for (int x=0; x<=size; ++x) for (int y=0; y<=size; ++y) if (mask()[x][y]) {
        v = vcl_min(v, getpixel(in, i+x, j+y, DataIn()));
        v = vcl_min(v, getpixel(in, i-x, j+y, DataIn()));
        v = vcl_min(v, getpixel(in, i+x, j-y, DataIn()));
        v = vcl_min(v, getpixel(in, i-x, j-y, DataIn()));
      }
      fsetpixel(out, i, j, (DataOut)v);
    }
  vcl_cout << " done\n";
  return true;
}

// it is important that the mask be computed in preop, if it was done in
// section_applyop then on a large image it would be computed many times.
template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_erode_disk <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: preop(){
  vcl_cout << "Starting vipl_erode_disk::preop() ...";
  // create circular mask:
  int size = (radius() < 0) ? 0 : int(radius());
  float rs = (radius() < 0) ? 0 : radius() * radius();
  typedef bool* boolptr;
  if(mask() == 0) {
    vcl_cout << " allocate mask ...";
    ref_mask() = new boolptr[1+size];
  }
  else {
    vcl_cout << " re-allocate mask ...";
    for (int x=0; x<=size; ++x)
      if(mask()[x]) delete[] ref_mask()[x];
    delete[] ref_mask();
    ref_mask() = new boolptr[1+size];
  }
  vcl_cout << " write mask ...";
  for (int x=0; x<=size; ++x) {
    ref_mask()[x] = new bool[size+1];
    for (int y=0; y<=size; ++y)
      ref_mask()[x][y] = (x*x + y*y <= rs);
  }
  vcl_cout << " done\n";
  return true;
}

// Since we will know if radius changes between calls to filter, we
// destroy the mask in postop, after we are all done filtering
template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_erode_disk <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: postop(){
  vcl_cout << "Starting vipl_erode_disk::postop() ...";
  int size = (radius() < 0) ? 0 : int(radius());
  if(mask()) {
    vcl_cout << " de-allocate mask ...";
    for (int x=0; x<=size; ++x)
      if(mask()[x]) delete[] ref_mask()[x];
    delete[] ref_mask();
    ref_mask()=0;
  }
  vcl_cout << " done\n";
  return true;
}

#endif // vipl_erode_disk_txx_
