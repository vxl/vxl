#ifndef vipl_add_random_noise_txx_
#define vipl_add_random_noise_txx_

#include "vipl_add_random_noise.h"
#include <vnl/vnl_numeric_traits.h>

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_add_random_noise <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop()
{
  int startx = start(X_Axis());
  int starty = start(Y_Axis());
  int stopx = stop(X_Axis());
  int stopy = stop(Y_Axis());
  for (register int j = starty; j < stopy; ++j)
    for (register int i = startx; i < stopx; ++i)
    {
      double rnd = type_ == UNIFORM_NOISE ? vnl_sample_uniform(-maxdev_,maxdev_)
                 : type_ == GAUSSIAN_NOISE ? vnl_sample_normal(0,maxdev_)
                 : vnl_sample_uniform(-maxdev_,maxdev_); // TODO - implement exponential noise
      DataOut p = fgetpixel(in_data(),i,j,DataIn(0)), q = DataOut(rnd);
      if (!clipping_ && rnd+p < 0)
        setpixel(out_data(), i, j, (DataOut)0);
      else if (!clipping_ && rnd+p > vnl_numeric_traits<DataOut>::maxval)
        setpixel(out_data(), i, j, vnl_numeric_traits<DataOut>::maxval);
      else
        setpixel(out_data(), i, j, DataOut(p+q));
  }
  return true;
}

#endif // vipl_add_random_noise_txx_
