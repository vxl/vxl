//:
// \file
// \brief Construct histogram from pixels in given image.
// \author Tim Cootes

#include "vil3d_histogram.h"

//: Construct histogram from pixels in given image of bytes
//  Resulting histogram has 256 bins
void vil3d_histogram_byte(const vil3d_image_view<vxl_byte>& image,
                          vcl_vector<double>& histo)
{
  histo.resize(256);
  vcl_fill(histo.begin(),histo.end(),0.0);

  const vxl_byte* plane = image.origin_ptr();
  unsigned ni = image.ni(),nj = image.nj(),nk = image.nk(),np = image.nplanes();
  vcl_ptrdiff_t istep=image.istep(),jstep=image.jstep(),kstep=image.kstep(),pstep = image.planestep();
  for (unsigned p=0;p<np;++p,plane += pstep)
  {
    const vxl_byte* slice = plane;
    for (unsigned k=0;k<nk;++k,slice += kstep)
    {
      const vxl_byte* row = slice;
      for (unsigned j=0;j<nj;++j,row += jstep)
      {
        const vxl_byte* pixel = row;
        for (unsigned i=0;i<ni;++i,pixel+=istep) histo[*pixel]+=1;
      }
    }
  }
}
