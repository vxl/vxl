#ifndef vipl_y_gradient_txx_
#define vipl_y_gradient_txx_

#include "vipl_y_gradient.h"

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_y_gradient <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop(){
  const ImgIn &in = in_data(0);
  ImgOut &out = *out_data_ptr();

  register DataIn v; register DataOut w;
  for(int j = start(Y_Axis())+1; j < stop(Y_Axis()); ++j)
    for(int i = start(X_Axis(),j); i < stop(X_Axis(),j); ++i) {
      w = fgetpixel(in, i, j, v) - fgetpixel(in, i, j-1, v);
      fsetpixel(out, i, j, w);
    }
  return true;
}

#endif // vipl_y_gradient_txx_
