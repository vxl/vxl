// This is oxl/oxp/MedianReduceImage.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
//  \file

#include "MedianReduceImage.h"

#include <vcl_vector.h>
#include <vbl/vbl_qsort.h>

typedef unsigned char byte;

//:
// Make an image which is a SCALE * SCALE subsampling of \argfont{in},
// where each output pixel O(x,y) is the median of values in the SCALE * SCALE
// window with top left corner at I(x*SCALE, y*SCALE).
MedianReduceImage::MedianReduceImage(vil1_memory_image_of<unsigned char> const& in, int SCALE):
  vil1_memory_image_of<unsigned char>(in.width() / SCALE, in.height() / SCALE)
{
  int w= in.width();
  int h= in.height();

  int ow = w / SCALE;
  int oh = h / SCALE;
  vil1_memory_image_of<byte>& out = *this;
//vil1_memory_image_of<byte> hists(ow * oh, SCALE * SCALE);
  vcl_vector<int> intensities(SCALE*SCALE);
  for (int ox = 0; ox < ow; ++ox)
    for (int oy = 0; oy < oh; ++oy) {
      int k = 0;
      for (int ix = ox*SCALE; ix < ox*SCALE+SCALE; ++ix)
        for (int iy = oy*SCALE; iy < oy*SCALE+SCALE; ++iy)
          intensities[k++] = in(ix,iy);
      vbl_qsort_ascending(&intensities[0], k);
      out(ox,oy) = intensities[k/2];
    }
}

#if defined(MAKE_EXAMPLE)
#include <vil1/vil1_file_image.h>
int main(int argc, char ** argv)
{
  vil1_file_image fim(argv[1]);
  vil1_memory_image_of<byte> in(fim);
  MedianReduceImage out(in, (argc < 2) ? 4 : vcl_atoi(argv[2]));
  out.save_pnm(vcl_cout);
}
#endif
