//:
//  \file
//  \brief Apply histogram equalisation to given image
//  \author Tim Cootes

#include <vil3d/algo/vil3d_histogram_equalise.h>
#include <vil3d/algo/vil3d_histogram.h>


//: Apply histogram equalisation to given image
void vil3d_histogram_equalise(vil3d_image_view<vxl_byte>& image)
{
  vcl_vector<double> histo(256);
  vil3d_histogram_byte(image,histo);

  // Create cumulative frequency curve
  double sum=0.0;
  for (unsigned i=0;i<256;++i) { sum+=histo[i]; histo[i]=sum; }

  // Parameters of mapping
  int lo = 0;
  // Find smallest value in image
  while (histo[lo]==0) lo++;
  double x0 = histo[lo];
  double s =255.1/(sum-x0);  // Smallest values get mapped to zero

  vcl_vector<vxl_byte> lookup(256);
  vxl_byte* lup = &lookup[0];
  for (unsigned i=0;i<256;++i) { lup[i]= vxl_byte(s*(histo[i]-x0)); }

  unsigned ni = image.ni(),nj = image.nj(),nk = image.nk(),np = image.nplanes();
  vcl_ptrdiff_t istep=image.istep(),jstep=image.jstep(),kstep=image.kstep(),pstep = image.planestep();
  vxl_byte* plane = image.origin_ptr();
  for (unsigned p=0;p<np;++p,plane += pstep)
  {
    vxl_byte* slice = plane;
    for (unsigned k=0;k<nk;++k,slice += kstep)
    {
      vxl_byte* row = slice;
      for (unsigned j=0;j<nj;++j,row += jstep)
      {
        vxl_byte* pixel = row;
        for (unsigned i=0;i<ni;++i,pixel+=istep) *pixel = lup[*pixel];
      }
    }
  }

}
