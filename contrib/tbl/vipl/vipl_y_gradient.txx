#ifndef vipl_y_gradient_txx_
#define vipl_y_gradient_txx_

#include "vipl_y_gradient.h"

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_y_gradient <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop(){
  const ImgIn &in = in_data(0);
  ImgOut &out = *out_data_ptr();

  const DataIn dummy = DataIn(0); // dummy initialization to avoid compiler warning
  int startx = start(X_Axis());
  int starty = start(Y_Axis());
  int stopx = stop(X_Axis());
  int stopy = stop(Y_Axis());
  for (register int j = starty+1; j < stopy; ++j)
    for (register int i = startx; i < stopx; ++i) {
      DataOut w = fgetpixel(in, i, j, dummy) - fgetpixel(in, i, j-1, dummy);
      fsetpixel(out, i, j, w);
    }
  return true;
}

#endif // vipl_y_gradient_txx_
