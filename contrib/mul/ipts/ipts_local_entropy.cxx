// This is mul/ipts/ipts_local_entropy.cxx
#include "ipts_local_entropy.h"
//:
// \file
// \brief Compute entropy in region around each image pixel
// \author Tim Cootes

#include <vcl_vector.h>
#include <vcl_cmath.h>
#include <vcl_cassert.h>

inline double histo_entropy_sum(const vcl_vector<int>& histo,
                                unsigned min_v, unsigned max_v)
{
  double sum = 0.0;
  for (unsigned k=min_v;k<=max_v;++k)
    if (histo[k]>0) sum+=histo[k]*vcl_log(double(histo[k]));
  return sum;
}

//: Compute local entropy in square region around each pixel in image
//  For each pixel in image, compute entropy in region (2h+1)x(2h+1)
//  centred on the pixel.  Result put in entropy image, which is of
//  size (image.ni()-2h) x (image.nj()-2h). Thus entropy(i,j)
//  corresponds to the value in the box around image point (i+h,j+h).
//
//  Values in image are assumed to lie in the range [min_v,max_v].
//  Any values outside that range will be ignored in the entropy calculation.
void ipts_local_entropy(const vil_image_view<vxl_byte>& image,
                        vil_image_view<float>& entropy,
                        unsigned h, unsigned min_v, unsigned max_v)
{
  vcl_vector<int> histo(256);
  const unsigned ni=image.ni(),nj=image.nj();
  assert(image.nplanes()==1 && ni>2*h && nj>2*h);
  unsigned eni = ni-2*h, enj=nj-2*h;
  entropy.set_size(eni,enj);

  unsigned h2 = 2*h+1;
  unsigned n = h2*h2;
  double logn = vcl_log(double(n));

  const vcl_ptrdiff_t istep=image.istep(),   jstep=image.jstep();
//const vcl_ptrdiff_t eistep=entropy.istep(),ejstep=entropy.jstep();

  for (unsigned i=0;i<eni;++i)
  {
    const vxl_byte* im = &image(i,0);

    // Create histogram from h2 x h2 region starting at (i,0)
    for (unsigned k=min_v;k<=max_v;++k) histo[k]=0;
    for (unsigned j1=0;j1<h2;++j1, im+=jstep)
    {
      const vxl_byte* p = im;
      for (unsigned i1=0;i1<h2;++i1,p+=istep) histo[*p]++;
    }
      // Compute (negative) entropy of histogram (sum plog(p))
    entropy(i,0) = float(logn-histo_entropy_sum(histo,min_v,max_v)/n);

    for (unsigned j=1;j<enj;++j)
    {
      // Update the histogram
      // Remove previous line
      const vxl_byte* p = &image(i,j-1);
      for (unsigned i1=0;i1<h2;++i1,p+=istep) histo[*p]--;
      // Add line at end
      p = &image(i,j+h2-1);
      for (unsigned i1=0;i1<h2;++i1,p+=istep) histo[*p]++;

       // Compute entropy of histogram (sum plog(p))
      entropy(i,j) = float(logn-histo_entropy_sum(histo,min_v,max_v)/n);  // Negative entropy
    }
  }
}
