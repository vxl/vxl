#ifndef vipl_gradient_mag_txx_
#define vipl_gradient_mag_txx_

#include "vipl_gradient_mag.h"
#include <vcl_cmath.h> // for sqrt()

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_gradient_mag <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop()
{
  const ImgIn &in = this->in_data(0);
  ImgOut &out = *this->out_data_ptr();

  register double dx, dy;
  int startx = start(this->X_Axis());
  int starty = start(this->Y_Axis());
  int stopx = stop(this->X_Axis());
  int stopy = stop(this->Y_Axis());
  for (int j = starty; j < stopy; ++j)
    for (int i = startx; i < stopx; ++i) {
      dx = fgetpixel(in, i, j, DataIn(0)) - getpixel(in, i-1, j, DataIn(0));
      dy = fgetpixel(in, i, j, DataIn(0)) - getpixel(in, i, j-1, DataIn(0));
      dx = (vcl_sqrt( dx*dx + dy*dy ) + shift()) * scale();
      fsetpixel(out, i, j, DataOut(dx));
    }
  return true;
}

#endif // vipl_gradient_mag_txx_
