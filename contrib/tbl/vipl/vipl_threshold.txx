#ifndef vipl_threshold_txx_
#define vipl_threshold_txx_

#include "vipl_threshold.h"

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_threshold <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop()
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
      if (p <= threshold()) { fsetpixel(out, i, j, DataOut(below())); }
      else  if (aboveset()) { fsetpixel(out, i, j, DataOut(above())); }
      else                  { fsetpixel(out, i, j, DataOut(p)); }
    }
  return true;
}

#endif // vipl_threshold_txx_
