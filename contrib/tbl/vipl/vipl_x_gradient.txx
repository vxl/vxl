#ifndef vipl_x_gradient_txx_
#define vipl_x_gradient_txx_

#include "vipl_x_gradient.h"

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_x_gradient <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop(){
  const ImgIn &in = in_data(0);
  ImgOut &out = *out_data_ptr();

  const DataIn dummy = DataIn(0); // dummy initialization to avoid compiler warning
  int startx = start(X_Axis());
  int starty = start(Y_Axis());
  int stopx = stop(X_Axis());
  int stopy = stop(Y_Axis());
  for (int j = starty; j < stopy; ++j)
    for (int i = startx+1; i < stopx; ++i)
    {
      DataOut w = fgetpixel(in, i, j, dummy) - fgetpixel(in, i-1, j, dummy) /* + shift() */;
//    if (scale() != 1) w = w * scale();
      fsetpixel(out, i, j, w);
    }
  return true;
}

#endif // vipl_x_gradient_txx_
