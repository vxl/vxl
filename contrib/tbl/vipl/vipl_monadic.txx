#ifndef vipl_monadic_txx_
#define vipl_monadic_txx_

#include "vipl_monadic.h"

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_monadic <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop(){
  const ImgIn &in = in_data();
  ImgOut &out = out_data();
  const DataIn dummy = DataIn(); // dummy initialization to avoid compiler warning

  int startx = start(X_Axis());
  int starty = start(Y_Axis());
  int stopx = stop(X_Axis());
  int stopy = stop(Y_Axis());
  for (int j = starty; j < stopy; ++j)
    for (int i = startx; i < stopx; ++i) {
      DataIn p = fgetpixel(in, i, j, dummy);
      fsetpixel(out, i, j, func()(p));
    }
  return true;
}

#endif // vipl_monadic_txx_
