#include "vipl_gradient_mag.h"
#include <vcl/vcl_cmath.h> // for sqrt()

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_gradient_mag <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop(){
  const ImgIn &in = in_data(0);
  ImgOut &out = *out_data_ptr();

  DataIn dummy; register double dx, dy;
  for(int j = start(Y_Axis()); j < stop(Y_Axis()); ++j)
    for(int i = start(X_Axis(),j); i < stop(X_Axis(),j); ++i) {
      dx = fgetpixel(in, i, j, dummy) - getpixel(in, i-1, j, dummy);
      dy = fgetpixel(in, i, j, dummy) - getpixel(in, i, j-1, dummy);
      dx = (sqrt( dx*dx + dy*dy ) + shift()) * scale();
      fsetpixel(out, i, j, (DataOut)dx);
    }
  return true;
}
