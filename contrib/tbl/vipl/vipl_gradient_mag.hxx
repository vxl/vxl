#ifndef vipl_gradient_mag_hxx_
#define vipl_gradient_mag_hxx_

#include <iostream>
#include <cmath>
#include "vipl_gradient_mag.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_gradient_mag <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop()
{
  const ImgIn &in = this->in_data(0);
  ImgOut &out = *this->out_data_ptr();

  double dx, dy;
  int startx = vipl_filter<ImgIn,ImgOut,DataIn,DataOut,2,PixelItr>::start(this->X_Axis());
  int starty = vipl_filter<ImgIn,ImgOut,DataIn,DataOut,2,PixelItr>::start(this->Y_Axis());
  int stopx  = vipl_filter<ImgIn,ImgOut,DataIn,DataOut,2,PixelItr>::stop(this->X_Axis());
  int stopy  = vipl_filter<ImgIn,ImgOut,DataIn,DataOut,2,PixelItr>::stop(this->Y_Axis());
  for (int j = starty; j < stopy; ++j)
    for (int i = startx; i < stopx; ++i) {
      dx = fgetpixel(in, i, j, DataIn(0)) - getpixel(in, i-1, j, DataIn(0));
      dy = fgetpixel(in, i, j, DataIn(0)) - getpixel(in, i, j-1, DataIn(0));
      dx = (std::sqrt( dx*dx + dy*dy ) + shift()) * scale();
      fsetpixel(out, i, j, DataOut(dx));
    }
  return true;
}

#endif // vipl_gradient_mag_hxx_
