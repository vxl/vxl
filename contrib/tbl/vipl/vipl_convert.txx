#include "vipl_convert.h"

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_convert <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop(){
  const ImgIn &in = in_data();
  ImgOut &out = out_data();
  DataOut nil_obj;

  for(int j = start(Y_Axis()); j < stop(Y_Axis()); ++j)
    for(int i = start(X_Axis(),j); i < stop(X_Axis(),j); ++i)
      fsetpixel(out, i, j, (DataOut)fgetpixel(in,i,j,nil_obj));
  return true;
}
