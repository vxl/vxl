#ifndef vipl_convert_txx_
#define vipl_convert_txx_

#include "vipl_convert.h"

#include <vcl_compiler.h>
#ifdef VCL_VC
# pragma warning(disable:4244) // disable conversion warning for explicit cast at line 23
#endif

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_convert <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop()
{
  const ImgIn &in = this->in_data(0);
  ImgOut &out = this->out_data(0);

  int startx = start(this->X_Axis());
  int starty = start(this->Y_Axis());
  int stopx = stop(this->X_Axis());
  int stopy = stop(this->Y_Axis());
  for (int j = starty; j < stopy; ++j)
    for (int i = startx; i < stopx; ++i)
      fsetpixel(out, i, j, (DataOut)fgetpixel(in,i,j,DataIn(0)));
  return true;
}

#endif // vipl_convert_txx_
