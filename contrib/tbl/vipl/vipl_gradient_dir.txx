#ifndef vipl_gradient_dir_txx_
#define vipl_gradient_dir_txx_

#include "vipl_gradient_dir.h"
#include <vcl_cmath.h> // for atan2()

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_gradient_dir <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop()
{
  const ImgIn &in = in_data(0);
  ImgOut &out = *out_data_ptr();

  int startx = start(X_Axis());
  int starty = start(Y_Axis());
  int stopx = stop(X_Axis());
  int stopy = stop(Y_Axis());
  for (int j = starty; j < stopy; ++j)
    for (int i = startx; i < stopx; ++i) {
      register double dx = fgetpixel(in, i, j, DataIn(0)) - getpixel(in, i-1, j, DataIn(0));
      register double dy = fgetpixel(in, i, j, DataIn(0)) - getpixel(in, i, j-1, DataIn(0));
      if (dx==0 && dy==0) dx=1.0; // to avoid an atan2() domain error
      dx = (vcl_atan2( dy, dx ) + shift()) * scale();
      fsetpixel(out, i, j, DataOut(dx));
    }
  return true;
}

#endif // vipl_gradient_dir_txx_
