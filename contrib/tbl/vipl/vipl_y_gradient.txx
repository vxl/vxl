#ifndef vipl_y_gradient_txx_
#define vipl_y_gradient_txx_

#include "vipl_y_gradient.h"

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_y_gradient <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop()
{
  const ImgIn &in = this->in_data(0);
  ImgOut &out = *this->out_data_ptr();

  int startx = start(this->X_Axis());
  int starty = start(this->Y_Axis());
  int stopx = stop(this->X_Axis());
  int stopy = stop(this->Y_Axis());
  for (register int j = starty+1; j < stopy; ++j)
    for (register int i = startx; i < stopx; ++i) {
      DataIn w = fgetpixel(in, i, j,   DataIn(0))
               - fgetpixel(in, i, j-1, DataIn(0)) /* + shift() */;
#if 0
      if (scale() != 1) w *= scale();
#endif
      fsetpixel(out, i, j, DataOut(w));
    }
  return true;
}

#endif // vipl_y_gradient_txx_
