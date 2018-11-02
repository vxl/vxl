#ifndef vaip_orientation_histogram_hxx_
#define vaip_orientation_histogram_hxx_
//:
// \file
// \brief Functions to compute histogram of orientations (HOGs)
// \author Tim Cootes

#include <iostream>
#include <cmath>
#include "mipa_orientation_histogram.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_math.h> // for pi
#include <cassert>

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
                                bool full360,
                                bool bilin_interp)
{
  assert(src.nplanes()==1);

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
    sA=n_angles/vnl_math::pi/2;
  }
  else
  {
    // Allow for wrap at 180
    sA=n_angles/vnl_math::pi;
  }
  double binwid=1.0/sA;
  double dA = vnl_math::twopi - 0.5/sA;
  const double wrapNeg = full360 ? vnl_math::twopi : vnl_math::pi;
  // Process each block
  const srcT* s_row = &src(1,1);
  std::ptrdiff_t si_step = src.istep();
  std::ptrdiff_t sj_step = src.jstep();

  sumT *h_row=&hog_image(0,0);
  std::ptrdiff_t hi_step = hog_image.istep();
  std::ptrdiff_t hj_step = hog_image.jstep();
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
          double theta=std::atan2(gj,gi);
          double magwt=std::sqrt(gi*gi+gj*gj);
          if (!bilin_interp)
          {
            unsigned A = unsigned((theta+dA)*sA);
            // Update histo bin with gradient magnitude:
            h[A%n_angles]+=sumT(magwt);
          }
          else
          {
            //interpolate theta over the two neighbouring bins
            //Note in this scheme we don't worry about the wrap around factor
            //as twopi-epsilon gets evenly(ish) interpolated between the last and zero bins
            if (theta<0.0) //convert to [0,2pi) or [0,pi)
            {
              theta += wrapNeg; //add twopi (or pi if signs don't matter)
            }
            int iA = int(theta*sA);

            const double centre0 = 0.5*binwid;
            double b0 = centre0+double(iA)*binwid; //bin centres
            double d0 = theta-b0; //distances to bin centres
            bool nextUp=(d0>0.0) ? true : false;
            double w0 = 1.0-std::fabs(d0)*sA; //interpolation weights
            double w1 = 1.0-w0;

            h[iA%n_angles]+=sumT(magwt*w0);
            int iAnext=((nextUp) ? iA+1 : iA -1); //NB can be -1 for twopi-epsilon
            h[(iAnext)%n_angles]+=sumT(magwt*w1); //NB -1 remapped to n-1 by mod n
          }
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
                                         bool full360, \
                                         bool bilin_intrep)

#endif // vaip_orientation_histogram_hxx_
