#ifndef vipl_dyadic_txx_
#define vipl_dyadic_txx_

#include "vipl_dyadic.h"

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_dyadic <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop(){
  const ImgIn &in = in_data();
  ImgOut &out = out_data();
  const DataIn d_in = DataIn(0); // dummy initialization to avoid compiler warning
  const DataOut d_out = DataOut(0);

  int startx = start(X_Axis());
  int starty = start(Y_Axis());
  int stopx = stop(X_Axis());
  int stopy = stop(Y_Axis());
  for (int j = starty; j < stopy; ++j)
    for (int i = startx; i < stopx; ++i) {
      DataIn p = fgetpixel(in, i, j, d_in);
      DataOut q = fgetpixel(out, i, j, d_out);
      func()(q,p);
      fsetpixel(out, i, j, q);
    }
  return true;
}

#endif // vipl_dyadic_txx_
