#include "vipl_dyadic.h"

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_dyadic <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop(){
  DataIn dummy1 = DataIn(); // dummy initialization to avoid compiler warning
  DataOut dummy2 = DataOut(); // dummy initialization to avoid compiler warning
  const ImgIn &in = in_data();
  ImgOut &out = out_data();

  for(int j = start(Y_Axis()); j < stop(Y_Axis()); ++j)
    for(int i = start(X_Axis(),j); i < stop(X_Axis(),j); ++i) {
      DataIn p = fgetpixel(in, i, j, dummy1);
      DataOut q = fgetpixel(out, i, j, dummy2);
      func()(q,p);
      fsetpixel(out, i, j, q);
    }
  return true;
}
