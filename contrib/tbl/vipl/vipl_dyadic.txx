#ifndef vipl_dyadic_txx_
#define vipl_dyadic_txx_

#include "vipl_dyadic.h"

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_dyadic <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop(){
  DataIn dummy1 = DataIn(0); // dummy initialization to avoid compiler warning
  DataOut dummy2 = DataOut(0); // dummy initialization to avoid compiler warning
  const ImgIn &in = in_data();
  ImgOut &out = out_data();

  int startx = start(X_Axis());
  int starty = start(Y_Axis());
  int stopx = stop(X_Axis());
  int stopy = stop(Y_Axis());
  for (int j = starty; j < stopy; ++j)
    for (int i = startx; i < stopx; ++i) {
      DataIn p = fgetpixel(in, i, j, dummy1);
      DataOut q = fgetpixel(out, i, j, dummy2);
      func()(q,p);
      fsetpixel(out, i, j, q);
    }
  return true;
}

#endif // vipl_dyadic_txx_
