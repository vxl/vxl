#include "vipl_x_gradient.h"

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_x_gradient <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop(){
  const ImgIn &in = in_data(0);
  ImgOut &out = *out_data_ptr();

  DataIn dummy; register DataOut w;
  for(int j = start(Y_Axis()); j < stop(Y_Axis()); ++j)
    for(int i = start(X_Axis(),j)+1; i < stop(X_Axis(),j); ++i) {
      w = fgetpixel(in, i, j, dummy) - fgetpixel(in, i-1, j, dummy) /* + shift() */;
//    if (scale() != 1) w = w * scale();
      fsetpixel(out, i, j, w);
    }
  return true;
}
