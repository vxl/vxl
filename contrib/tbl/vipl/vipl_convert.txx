#ifndef vipl_convert_txx_
#define vipl_convert_txx_

#include "vipl_convert.h"

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_convert <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop()
{
  const ImgIn &in = in_data(0);
  ImgOut &out = out_data(0);
  const DataIn nil_obj = DataIn(0);

  int startx = start(X_Axis());
  int starty = start(Y_Axis());
  int stopx = stop(X_Axis());
  int stopy = stop(Y_Axis());
  for (int j = starty; j < stopy; ++j)
    for (int i = startx; i < stopx; ++i)
      fsetpixel(out, i, j, (DataOut)fgetpixel(in,i,j,nil_obj));
  return true;
}

#endif // vipl_convert_txx_
