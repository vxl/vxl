#include "vipl_monadic.h"

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_monadic <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop(){
  DataIn dummy;
  const ImgIn &in = in_data();
  ImgOut &out = out_data();

  for(int j = start(Y_Axis()); j < stop(Y_Axis()); ++j)
    for(int i = start(X_Axis(),j); i < stop(X_Axis(),j); ++i) {
      DataIn p = fgetpixel(in, i, j, dummy);
      fsetpixel(out, i, j, func()(p));
    }
  return true;
}
