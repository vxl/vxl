#ifndef vipl_histogram_txx_
#define vipl_histogram_txx_

#include "vipl_histogram.h"
#include <vcl_algorithm.h>

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_histogram <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop(){
  const ImgIn &in = in_data(0);
  DataIn dummy = DataIn(); // dummy initialization to avoid compiler warning
  ImgOut &out = *out_data_ptr();
  int index = indexout();
//if (index < 0) index = 0;
  if (checkrange() == 1)  { // check range is slow, we always keep the divide...
    for (int j = start(Y_Axis()), ej = stop(Y_Axis()) ; j < ej ; ++j)
      for (int i = start(X_Axis(),j), ei = stop(X_Axis(),j) ; i < ei ; ++i) {
        long bin = long(0.5 + (shiftin()+getpixel(in,i,j,dummy))/scalein());
//      if (bin < 0) bin = 0;
        // not fsetpixel !!! cannot assume `bin' will lie inside output image section
        DataOut bs = getpixel(out,bin,index,DataOut());
        setpixel(out, bin, index, scaleout()+bs);
      }
  }  // else we want speed, skip safety check, check special cases
  else  if (scalein() == 1 && scaleout() == 1 && shiftin() == 0) {
    for (int j = start(Y_Axis()), ej = stop(Y_Axis()) ; j < ej ; ++j)
      for (int i = start(X_Axis(),j), ei = stop(X_Axis(),j) ; i < ei ; ++i) {
        long bin = long(0.5 + (getpixel(in,i,j,dummy)));
        DataOut bs = getpixel(out,bin,index,DataOut());
        setpixel(out, bin, index, 1+bs);
      }
  } else  if (scalein() == 1)  {
    for (int j = start(Y_Axis()), ej = stop(Y_Axis()) ; j < ej ; ++j)
      for (int i = start(X_Axis(),j), ei = stop(X_Axis(),j) ; i < ei ; ++i) {
        long bin = long(0.5 + (shiftin()+getpixel(in,i,j,dummy)));
        DataOut bs = getpixel(out,bin,index,DataOut());
        setpixel(out, bin, index, scaleout()+bs);
      }
  } else { // all modes
    for (int j = start(Y_Axis()), ej = stop(Y_Axis()) ; j < ej ; ++j)
      for (int i = start(X_Axis(),j), ei = stop(X_Axis(),j) ; i < ei ; ++i) {
        long bin = long(0.5 + (shiftin()+getpixel(in,i,j,dummy))/scalein());
        DataOut bs = getpixel(out,bin,index,DataOut());
        setpixel(out, bin, index, scaleout()+bs);
      }
  }
  return true;
}

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_histogram <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_preop(){
#if 0 // should not zero the output here - that is up to the user
  vcl_fill(out_data_ptr()->begin(), out_data_ptr()->end(), DataOut());
  const ImgIn &in = in_data(0);
  DataIn dummy = DataIn(); // dummy initialization to avoid compiler warning
  ImgOut &out = *out_data_ptr();
  int index = indexout();
//if (index < 0) index = 0;

  for (int i = start(X_Axis(),0), ei = stop(X_Axis(),0) ; i < ei ; ++i)
  {
    setpixel(out, bin, index, 0);
  }
#endif
  return true;
}


#endif // vipl_histogram_txx_
