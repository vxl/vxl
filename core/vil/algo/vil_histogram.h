#ifndef vil_histogram_h_
#define vil_histogram_h_
//:
// \file
// \brief Construct histogram from pixels in given image.
// \author Tim Cootes

#include <vil/vil_image_view.h>
#include <vcl_vector.h>
#include <vxl_config.h>
#include <vcl_algorithm.h>

//: Construct histogram from pixels in given image
template<class T>
inline
void vil_histogram(const vil_image_view<T>& image,
                   vcl_vector<double>& histo,
                   double min, double max, unsigned n_bins)
{
  histo.resize(n_bins);
  vcl_fill(histo.begin(),histo.end(),0.0);
  double x0 = double(min);
  double s = double(n_bins-1)/(double(max)-x0);

  unsigned ni = image.ni(),nj = image.nj(),np = image.nplanes();
  vcl_ptrdiff_t istep=image.istep(),jstep=image.jstep(),pstep = image.planestep();
  const T* plane = image.top_left_ptr();
  for (unsigned p=0;p<np;++p,plane += pstep)
  {
    const T* row = plane;
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

//: Construct histogram from pixels in given image of bytes
//  Resulting histogram has 256 bins
void vil_histogram_byte(const vil_image_view<vxl_byte>& image,
                        vcl_vector<double>& histo);

//: Instantiation macro for other types
#define VIL_HISTOGRAM_INSTANTIATE(T) \
template void vil_histogram(const vil_image_view<T>& image, \
                   vcl_vector<double>& histo, \
                   double min, double max, unsigned n_bins)

#endif // vil_histogram_h_
