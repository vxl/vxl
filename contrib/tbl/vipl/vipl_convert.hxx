#ifndef vipl_convert_hxx_
#define vipl_convert_hxx_

#include "vipl_convert.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#ifdef _MSC_VER
# pragma warning(disable:4244) // disable conversion warning for explicit cast at line 23
#endif

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_convert <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop()
{
  const ImgIn &in = this->in_data(0);
  ImgOut &out = this->out_data(0);

  int startx = vipl_filter<ImgIn,ImgOut,DataIn,DataOut,2,PixelItr>::start(this->X_Axis());
  int starty = vipl_filter<ImgIn,ImgOut,DataIn,DataOut,2,PixelItr>::start(this->Y_Axis());
  int stopx  = vipl_filter<ImgIn,ImgOut,DataIn,DataOut,2,PixelItr>::stop(this->X_Axis());
  int stopy  = vipl_filter<ImgIn,ImgOut,DataIn,DataOut,2,PixelItr>::stop(this->Y_Axis());
  for (int j = starty; j < stopy; ++j)
    for (int i = startx; i < stopx; ++i)
      fsetpixel(out, i, j, (DataOut)fgetpixel(in,i,j,DataIn(0)));
  return true;
}

#endif // vipl_convert_hxx_
