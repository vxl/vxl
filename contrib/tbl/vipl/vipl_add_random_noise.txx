#ifndef vipl_add_random_noise_txx_
#define vipl_add_random_noise_txx_

#include "vipl_add_random_noise.h"

template <class ImgIn,class ImgOut,class DataIn,class DataOut,class PixelItr>
bool vipl_add_random_noise <ImgIn,ImgOut,DataIn,DataOut,PixelItr> :: section_applyop(){
  DataIn dummy = DataIn(); // dummy initialization to avoid compiler warning
  int startx = start(X_Axis());
  int starty = start(Y_Axis());
  int stopx = stop(X_Axis());
  int stopy = stop(Y_Axis());
  for (register int j = starty; j < stopy; ++j)
    for (register int i = startx; i < stopx; ++i) {
#ifdef STAT_LIB
      DataOut p = fgetpixel(in_data(),i,j,dummy) + (DataOut)(distrib_->Draw(&seed_));
#else
      seed_ *= 1366; seed_ += 150889; seed_ %= 714025;
      DataOut p = fgetpixel(in_data(),i,j,dummy) + (DataOut)(maxdev_*(seed_-357012)/357012);
#endif
      setpixel(out_data(), i, j, p);
  }
  return true;
}

#endif // vipl_add_random_noise_txx_
