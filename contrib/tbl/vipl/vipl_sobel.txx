#include "vipl_sobel.h"

// template<class DataIn> inline DataIn max(DataIn a,DataIn b){ return (a<b)?b:a; }

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_sobel <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop(){
  const ImgIn &in = in_data(0);
  ImgOut &out = out_data();
  // apply filter:
  register DataIn v;
  for(int j = start(Y_Axis()), ej =  stop(Y_Axis()); j < ej  ; ++j)
    for(int i = start(X_Axis(),j), ei = stop(X_Axis(),j); i < ei ; ++i) {
      v = fgetpixel(in, i, j, v); v *= 4;
      v -= getpixel(in, i+1, j+1, v);
      v -= getpixel(in, i-1, j+1, v);
      v -= getpixel(in, i+1, j-1, v);
      v -= getpixel(in, i-1, j-1, v);
      fsetpixel(out, i, j, (DataOut)v);
    }
  return true;
}
