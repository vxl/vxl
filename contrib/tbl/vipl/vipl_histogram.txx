#ifndef vipl_histogram_txx_
#define vipl_histogram_txx_

#include "vipl_histogram.h"

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_histogram <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop(){
  const ImgIn &in = in_data(0);
  DataIn dummy = DataIn(); // dummy initialization to avoid compiler warning
  ImgOut &out = *out_data_ptr();
  int index = indexout();
//if (index < 0) index = 0;
//if (index > out.y_size()) index = out.y_size();
  if (checkrange() == 1)  { // check range is slow, we always keep the divide...
//  int maxval = out.x_size()-1;
    for (Yiterator j = start(Y_Axis()), ej = stop(Y_Axis()) ; j < ej ; ++j)
      for (Xiterator i = start(X_Axis(),j), ei = stop(X_Axis(),j) ; i < ei ; ++i) {
        long bin = int(0.5 + (shiftin()+getpixel(in,i,j,dummy))/scalein());
//      if (bin < 0) bin = 0;
//      if (bin >= maxval) bin = maxval;
        // not fsetpixel !!! cannot assume `bin' will lie inside output image section
        DataOut bs /* quell gcc warning : */ = DataOut(); bs=getpixel(out,bin,index,bs);
        setpixel(out, bin, index, scaleout()+bs);
      }
  }  // else we want speed, skip safety check, check special cases
  else  if (scalein() == 1 && scaleout() == 1 && shiftin() == 0) {
    for (Yiterator j = start(Y_Axis()), ej = stop(Y_Axis()) ; j < ej ; ++j)
      for (Xiterator i = start(X_Axis(),j), ei = stop(X_Axis(),j) ; i < ei ; ++i) {
        long bin = int(0.5 + (getpixel(in,i,j,dummy)));
        DataOut bs/* quell gcc warning : */ = DataOut(); bs=getpixel(out,bin,index,bs);
        setpixel(out, bin, index, 1+bs);
      }
  } else  if (scalein() == 1)  {
    for (Yiterator j = start(Y_Axis()), ej = stop(Y_Axis()) ; j < ej ; ++j)
      for (Xiterator i = start(X_Axis(),j), ei = stop(X_Axis(),j) ; i < ei ; ++i) {
        long bin = int(0.5 + (shiftin()+getpixel(in,i,j,dummy)));
        DataOut bs/* quell gcc warning : */ = DataOut(); bs=getpixel(out,bin,index,bs);
        setpixel(out, bin, index, scaleout()+bs);
      }
  } else { // all all mods
    for (Yiterator j = start(Y_Axis()), ej = stop(Y_Axis()) ; j < ej ; ++j)
      for (Xiterator i = start(X_Axis(),j), ei = stop(X_Axis(),j) ; i < ei ; ++i) {
        long bin = int(0.5 + (shiftin()+getpixel(in,i,j,dummy))/scalein());
        DataOut bs/* quell gcc warning : */ = DataOut(); bs=getpixel(out,bin,index,bs);
        setpixel(out, bin, index, scaleout()+bs);
      }
  }
  return true;
}


#endif // vipl_histogram_txx_
