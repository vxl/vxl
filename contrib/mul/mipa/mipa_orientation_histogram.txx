#ifndef vaip_orientation_histogram_txx_
#define vaip_orientation_histogram_txx_
//:
// \file
// \brief Functions to compute histogram of orientations (HOGs)
// \author Tim Cootes

#include "mipa_orientation_histogram.h"
#include <vcl_cmath.h>

//: Generate an image containing histograms of oriented gradients (HOG)
//  At each pixel in src, compute angle and quantise into n_angles.
//  If full360, then angle range is 0-360, else it is 0-180.
//  hog_image is set to have n_angles planes.
//  hog_image(i,j,k) gives the weighted sum of pixels with angle k
//  in cell (i,j), corresponding to the i,j-th cell_size square block.
//
//  The corner of cell(0,0) is at src(1,1), to ignore border pixels.
//
//  Number of cells (size of hog_image) chosen so every cell entirely
//  within src.  Thus hog_image.ni()=(src.ni()-2)/cell_size.
template<class srcT, class sumT>
void mipa_orientation_histogram(const vil_image_view<srcT>& src,
                               vil_image_view<sumT>& hog_image,
                               unsigned n_angles,
                               unsigned cell_size,
                               bool full360)
{
  assert(src.nplanes()==1);

  unsigned ni=src.ni(), nj=src.nj();
  unsigned h_ni=(src.ni()-2)/cell_size;
  unsigned h_nj=(src.nj()-2)/cell_size;

  // Create image with correct size of memory, with interleaved planes
  vil_image_view<sumT> h_im0(h_ni,h_nj,1,n_angles); 
  h_im0.fill(0);

  // This forces hog_image to have interleaved planes.
  hog_image = h_im0;

  // Compute size of angle bins (1/sA) and an offset to ensure 0rad is
  // in centre of bin 0
  double sA;
  if (full360)
  {
    sA=n_angles/6.2831853;
  }
  else
  {
    // Allow for wrap at 180
    sA=n_angles/3.1415927;
  }
  double dA=6.2831853 - 0.5/sA;

  // Process each block
  const srcT* s_row = &src(1,1);
  vcl_ptrdiff_t si_step = src.istep();
  vcl_ptrdiff_t sj_step = src.jstep();

  sumT *h_row=&hog_image(0,0);
  vcl_ptrdiff_t hi_step = hog_image.istep();
  vcl_ptrdiff_t hj_step = hog_image.jstep();
  const sumT* h_row_end = h_row+h_nj*hj_step;
  for (;h_row!=h_row_end;h_row+=hj_step)
  {
    const srcT* s_row_end = s_row + cell_size*sj_step;
    for (;s_row!=s_row_end;s_row+=sj_step)
    {
      // Process row of h_ni cells
      sumT* h_end=h_row+hi_step*h_ni;
      const srcT* s=s_row;
      for (sumT* h=h_row;h!=h_end;h+=hi_step)
      {
        // Process cell_size pixels in current cell (histo h[A])
        const srcT* s_end=s+si_step*cell_size;
        for (;s!=s_end;s+=si_step)
        {
          // Compute angle from gradient
          sumT gi = sumT(s[si_step]-s[-si_step]);
          sumT gj = sumT(s[sj_step]-s[-sj_step]);
          unsigned A = unsigned((vcl_atan2(gj,gi)+dA)*sA);
            // Update histo bin with gradient magnitude:
          h[A%n_angles]+=sumT(vcl_sqrt(gi*gi+gj*gj));
        }
      }
    }
  }
}

#undef MIPA_ORIENTATION_HISTOGRAM_INSTANTIATE
#define MIPA_ORIENTATION_HISTOGRAM_INSTANTIATE(srcT, sumT) \
template void mipa_orientation_histogram(const vil_image_view<srcT >& src, \
                               vil_image_view<sumT >& hog_image, \
                               unsigned n_angles, \
                               unsigned cell_size, \
                               bool full360);



#endif // vaip_orientation_histogram_txx_

