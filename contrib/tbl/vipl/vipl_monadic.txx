#ifndef vipl_monadic_txx_
#define vipl_monadic_txx_

#include "vipl_monadic.h"

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_monadic <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop()
{
  const ImgIn &in = this->in_data();
  ImgOut &out = this->out_data();

  int startx = start(this->X_Axis());
  int starty = start(this->Y_Axis());
  int stopx = stop(this->X_Axis());
  int stopy = stop(this->Y_Axis());
  for (int j = starty; j < stopy; ++j)
    for (int i = startx; i < stopx; ++i)
      fsetpixel(out, i, j, func()(fgetpixel(in, i, j, DataIn(0))));
  return true;
}

#endif // vipl_monadic_txx_
