//:
// \file
// \brief Construct histogram from pixels in given image.
// \author Tim Cootes

#include "vil_histogram.h"

//: Construct histogram from pixels in given image of bytes
//  Resulting histogram has 256 bins
void vil_histogram_byte(const vil_image_view<vxl_byte>& image,
                        vcl_vector<double>& histo)
{
  histo.resize(256);
  vcl_fill(histo.begin(),histo.end(),0.0);

  unsigned ni = image.ni(),nj = image.nj(),np = image.nplanes();
  vcl_ptrdiff_t istep=image.istep(),jstep=image.jstep(),pstep = image.planestep();
  const vxl_byte* plane = image.top_left_ptr();
  for (unsigned p=0;p<np;++p,plane += pstep)
  {
    const vxl_byte* row = plane;
    for (unsigned j=0;j<nj;++j,row += jstep)
    {
      const vxl_byte* pixel = row;
      for (unsigned i=0;i<ni;++i,pixel+=istep) histo[*pixel]+=1;
    }
  }
}
