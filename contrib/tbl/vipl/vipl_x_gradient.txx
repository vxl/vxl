#ifndef vipl_x_gradient_txx_
#define vipl_x_gradient_txx_

#include "vipl_x_gradient.h"

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_x_gradient <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop()
{
  const ImgIn &in = in_data(0);
  ImgOut &out = *out_data_ptr();

  int startx = start(X_Axis());
  int starty = start(Y_Axis());
  int stopx = stop(X_Axis());
  int stopy = stop(Y_Axis());
  for (register int j = starty; j < stopy; ++j)
    for (register int i = startx+1; i < stopx; ++i) {
      DataIn w = fgetpixel(in, i,  j, DataIn(0))
               - fgetpixel(in, i-1,j, DataIn(0)) /* + shift() */;
//    if (scale() != 1) w *= scale();
      fsetpixel(out, i, j, DataOut(w));
    }
  return true;
}

#endif // vipl_x_gradient_txx_
