#ifndef vipl_sobel_txx_
#define vipl_sobel_txx_

#include "vipl_sobel.h"

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_sobel <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop(){
  const ImgIn &in = in_data(0);
  ImgOut &out = out_data();
  // apply filter:
  int startx = start(X_Axis());
  int starty = start(Y_Axis());
  int stopx = stop(X_Axis());
  int stopy = stop(Y_Axis());
  for (int j = starty, ej =  stopy; j < ej  ; ++j)
    for (int i = startx, ei = stopx; i < ei ; ++i)
      fsetpixel(out, i, j, (DataOut)(
                4*fgetpixel(in, i, j, (DataIn)0)
              -    getpixel(in, i+1, j+1, (DataIn)0)
              -    getpixel(in, i-1, j+1, (DataIn)0)
              -    getpixel(in, i+1, j-1, (DataIn)0)
              -    getpixel(in, i-1, j-1, (DataIn)0)));
  return true;
}

#endif // vipl_sobel_txx_
