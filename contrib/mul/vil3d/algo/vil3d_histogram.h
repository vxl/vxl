#ifndef vil3d_histogram_h_
#define vil3d_histogram_h_
//:
// \file
// \brief Construct histogram from pixels in given image.
// \author Tim Cootes

#include <vcl_vector.h>
#include <vxl_config.h>
#include <vcl_algorithm.h>
#include <vil3d/vil3d_image_view.h>

//: Construct histogram from pixels in given image
template<class T>
inline
void vil3d_histogram(const vil3d_image_view<T>& image,
                     vcl_vector<double>& histo,
                     double min, double max, unsigned n_bins)
{
  histo.resize(n_bins);
  vcl_fill(histo.begin(),histo.end(),0.0);
  double x0 = double(min);
  double s = double(n_bins-1)/(double(max)-x0);

  const T* plane = image.origin_ptr();
  unsigned ni = image.ni(),nj = image.nj(),nk = image.nk(),np = image.nplanes();
  vcl_ptrdiff_t istep=image.istep(),jstep=image.jstep(),kstep=image.kstep(),pstep = image.planestep();
  for (unsigned p=0;p<np;++p,plane += pstep)
  {
    const T* slice = plane;
    for (unsigned k=0;k<nk;++k,slice += kstep)
    {
      const T* row = slice;
      for (unsigned j=0;j<nj;++j,row += jstep)
      {
        const T* pixel = row;
        for (unsigned i=0;i<ni;++i,pixel+=istep)
        {
          int index = int(0.5+s*(double(*pixel) - x0));
          if (index>=0 && (unsigned)index<n_bins) histo[index]+=1;
        }
      }
    }
  }
}

//: Construct histogram from pixels in given image of bytes
//  Resulting histogram has 256 bins
void vil3d_histogram_byte(const vil3d_image_view<vxl_byte>& image,
                          vcl_vector<double>& histo);

#endif // vil3d_histogram_h_
