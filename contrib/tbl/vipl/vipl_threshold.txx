#include "vipl_threshold.h"

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_threshold <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop(){
  DataIn dummy /* quell gcc warning : */ = DataIn();
  const ImgIn &in = in_data();
  ImgOut &out = out_data();

  for(int j = start(Y_Axis()); j < stop(Y_Axis()); ++j)
    for(int i = start(X_Axis(),j); i < stop(X_Axis(),j); ++i) {
      DataIn p = fgetpixel(in, i, j, dummy);
      if(p <= threshold()) {fsetpixel(out, i, j, (DataOut)below());}
      else if (aboveset()) {fsetpixel(out, i, j, (DataOut)above());}
      else {fsetpixel(out, i, j, p);}
    }
  return true;
}
