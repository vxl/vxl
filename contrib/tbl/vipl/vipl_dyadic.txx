#ifndef vipl_dyadic_txx_
#define vipl_dyadic_txx_

#include "vipl_dyadic.h"

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_dyadic <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop()
{
  const ImgIn &in = this->in_data();
  ImgOut &out = this->out_data();

  int startx = start(this->X_Axis());
  int starty = start(this->Y_Axis());
  int stopx = stop(this->X_Axis());
  int stopy = stop(this->Y_Axis());
  for (int j = starty; j < stopy; ++j)
    for (int i = startx; i < stopx; ++i) {
      DataIn p = fgetpixel(in, i, j, DataIn(0));
      DataOut q = fgetpixel(out, i, j, DataOut(0));
      func()(q,p);
      fsetpixel(out, i, j, q);
    }
  return true;
}

#endif // vipl_dyadic_txx_
