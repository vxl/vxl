// This is core/vil/algo/vil_suppress_non_max_edges.hxx
#ifndef vil_suppress_non_max_edges_hxx_
#define vil_suppress_non_max_edges_hxx_
//:
// \file
// \brief Given gradient image, compute magnitude and zero any non-maximal values
// \author Tim Cootes

#include <cmath>
#include "vil_suppress_non_max_edges.h"
#include <vil/vil_bilin_interp.h>
#include <vil/vil_fill.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//: Given gradient images, computes magnitude image containing maximal edges
//  Computes magnitude image.  Zeros any below a threshold.
//  Points with magnitude above a threshold are tested against gradient
//  along normal to the edge and retained only if they are higher than
//  their neighbours.
//
//  Note: Currently assumes single plane only.
//  2 pixel border around output set to zero
template<class srcT, class destT>
void vil_suppress_non_max_edges(const vil_image_view<srcT>& grad_i,
                                const vil_image_view<srcT>& grad_j,
                                double grad_mag_threshold,
                                vil_image_view<destT>& grad_mag)
{
  assert(grad_i.nplanes()==grad_j.nplanes());
  assert(grad_i.nplanes()==1);
  unsigned ni = grad_i.ni(), nj = grad_i.nj();
  assert(ni>2 && nj>2);
  assert(grad_j.ni()==ni && grad_j.nj()==nj);
  grad_mag.set_size(ni,nj,1);

  // Fill 2 pixel border with zero
  vil_fill_col(grad_mag,0,destT(0));
  vil_fill_col(grad_mag,1,destT(0));
  vil_fill_col(grad_mag,ni-1,destT(0));
  vil_fill_col(grad_mag,ni-2,destT(0));
  vil_fill_row(grad_mag,0,destT(0));
  vil_fill_row(grad_mag,1,destT(0));
  vil_fill_row(grad_mag,nj-1,destT(0));
  vil_fill_row(grad_mag,nj-2,destT(0));

  const std::ptrdiff_t gi_istep = grad_i.istep(), gi_jstep = grad_i.jstep();
  const std::ptrdiff_t gj_istep = grad_j.istep(), gj_jstep = grad_j.jstep();
  const std::ptrdiff_t gm_istep = grad_mag.istep(), gm_jstep = grad_mag.jstep();

  const srcT * gi_data = &grad_i(0,0);
  const srcT * gj_data = &grad_j(0,0);
  const srcT * gi_row = &grad_i(2,2);
  const srcT * gj_row = &grad_j(2,2);
  destT * gm_row = &grad_mag(2,2);
  unsigned ihi=ni-3;
  unsigned jhi=nj-3;

  for (unsigned j=2; j<=jhi; ++j, gi_row+=gi_jstep, gj_row+=gj_jstep,
                                  gm_row+=gm_jstep)
  {
    const srcT* pgi = gi_row;
    const srcT* pgj = gj_row;
    destT *pgm = gm_row;
    for (unsigned i=2; i<=ihi; ++i, pgi+=gi_istep, pgj+=gj_istep,
                                    pgm+=gm_istep)
    {
      double gmag=std::sqrt(double(pgi[0]*pgi[0] + pgj[0]*pgj[0]));
      if (gmag<grad_mag_threshold) *pgm=0;
      else
      {
        double dx=pgi[0]/gmag;
        double dy=pgj[0]/gmag;
        // Evaluate gradient along direction (dx,dy) at point (i+dx,j+dy)
        double gx1=vil_bilin_interp_unsafe(i+dx,j+dy,gi_data,gi_istep,gi_jstep);
        double gy1=vil_bilin_interp_unsafe(i+dx,j+dy,gj_data,gj_istep,gj_jstep);
        if (dx*gx1+dy*gy1>gmag) *pgm=0;
        else
        {
          // Evaluate gradient along direction (dx,dy) at point (i-dx,j-dy)
          double gx2=vil_bilin_interp_unsafe(i-dx,j-dy,gi_data,gi_istep,gi_jstep);
          double gy2=vil_bilin_interp_unsafe(i-dx,j-dy,gj_data,gj_istep,gj_jstep);
          if (dx*gx2+dy*gy2>gmag) *pgm=0;
          else
            *pgm = destT(gmag);  // This is a maximal edge!
        }
      }
    }
  }
}

namespace {
  //: Parabolic interpolation of 3 points \p y_0, \p y_1, \p y_2
  //  \returns the peak value by reference in \p y_peak
  //  \returns the peak location offset from the x of \p y_0
  double interpolate_parabola(double y_1, double y_0, double y_2,
                              double& y_peak)
  {
    y_peak = y_0;                       // initial peak
    double diff1 = y_2 - y_1;           // first derivative
    double diff2 = 2 * y_0 - y_1 - y_2; // second derivative
    // handle special case of zero offset
    if (diff2 == 0.0)
      return 0.0;
    y_peak += diff1 * diff1 / (8 * diff2);  // interpolate y as max/min
    return diff1 / (2 * diff2);             // interpolate x offset
  }
}


//: Given gradient images, computes a subpixel edgemap with magnitudes and orientations
//  Computes magnitude image.  Zeros any below a threshold.
//  Points with magnitude above a threshold are tested against gradient
//  along normal to the edge and retained only if they are higher than
//  their neighbours.  The magnitude of retained points is revised using
//  parabolic interpolation in the normal direction.  The same interpolation
//  provides a subpixel offset from the integral pixel location.
//
//  This algorithm returns a 3-plane image where the planes are:
//  - 0 - The interpolated peak magnitude
//  - 1 - The orientation (in radians)
//  - 2 - The offset to the subpixel peak in the gradient direction
//  All non-maximal edge pixel have the value zero in all three planes.
//  \sa vil_orientations_at_edges
//
//  The subpixel location for pixel (i,j) is computed as
//  \code
//    double theta = grad_mag_orient_offset(i,j,1);
//    double offset = grad_mag_orient_offset(i,j,2);
//    double x = i + std::cos(theta)*offset;
//    double y = j + std::sin(theta)*offset;
//  \endcode
//
//  Note: Currently assumes single plane only.
//  2 pixel border around output set to zero.
//  If two neighbouring edges have exactly the same strength, it retains
//  both (ie an edge is eliminated if it is strictly lower than a neighbour,
//  but not if it is the same as two neighbours).
template<class srcT, class destT>
void vil_suppress_non_max_edges_subpixel(const vil_image_view<srcT>& grad_i,
                                         const vil_image_view<srcT>& grad_j,
                                         double grad_mag_threshold,
                                         vil_image_view<destT>& grad_moo)
{
  assert(grad_i.nplanes()==grad_j.nplanes());
  assert(grad_i.nplanes()==1);
  unsigned ni = grad_i.ni(), nj = grad_i.nj();
  assert(ni>2 && nj>2);
  assert(grad_j.ni()==ni && grad_j.nj()==nj);
  grad_moo.set_size(ni,nj,3);

  // Fill 2 pixel border with zero
  vil_fill_col(grad_moo,0,destT(0));
  vil_fill_col(grad_moo,1,destT(0));
  vil_fill_col(grad_moo,ni-1,destT(0));
  vil_fill_col(grad_moo,ni-2,destT(0));
  vil_fill_row(grad_moo,0,destT(0));
  vil_fill_row(grad_moo,1,destT(0));
  vil_fill_row(grad_moo,nj-1,destT(0));
  vil_fill_row(grad_moo,nj-2,destT(0));

  const std::ptrdiff_t gi_istep = grad_i.istep(), gi_jstep = grad_i.jstep();
  const std::ptrdiff_t gj_istep = grad_j.istep(), gj_jstep = grad_j.jstep();
  const std::ptrdiff_t gm_istep = grad_moo.istep(), gm_jstep = grad_moo.jstep();
  const std::ptrdiff_t gm_pstep = grad_moo.planestep();

  const srcT * gi_data = &grad_i(0,0);
  const srcT * gj_data = &grad_j(0,0);
  const srcT * gi_row = &grad_i(2,2);
  const srcT * gj_row = &grad_j(2,2);
  destT * gm_row = &grad_moo(2,2);
  unsigned ihi=ni-3;
  unsigned jhi=nj-3;

  for (unsigned j=2; j<=jhi; ++j, gi_row+=gi_jstep, gj_row+=gj_jstep,
                                  gm_row+=gm_jstep)
  {
    const srcT* pgi = gi_row;
    const srcT* pgj = gj_row;
    destT *pgm = gm_row;
    for (unsigned i=2; i<=ihi; ++i, pgi+=gi_istep, pgj+=gj_istep,
                                    pgm+=gm_istep)
    {
      double gmag=std::sqrt(double(pgi[0]*pgi[0] + pgj[0]*pgj[0]));
      if (gmag<grad_mag_threshold){
        *pgm=0;
        *(pgm+gm_pstep)=0;
        *(pgm+2*gm_pstep)=0;
      }
      else
      {
        double dx=pgi[0]/gmag;
        double dy=pgj[0]/gmag;
        // Evaluate gradient along direction (dx,dy) at point (i+dx,j+dy)
        double gx1=vil_bilin_interp_unsafe(i+dx,j+dy,gi_data,gi_istep,gi_jstep);
        double gy1=vil_bilin_interp_unsafe(i+dx,j+dy,gj_data,gj_istep,gj_jstep);
        double g1mag = dx*gx1+dy*gy1;
        if (g1mag>gmag){
          *pgm=0;
          *(pgm+gm_pstep)=0;
          *(pgm+2*gm_pstep)=0;
        }
        else
        {
          // Evaluate gradient along direction (dx,dy) at point (i-dx,j-dy)
          double gx2=vil_bilin_interp_unsafe(i-dx,j-dy,gi_data,gi_istep,gi_jstep);
          double gy2=vil_bilin_interp_unsafe(i-dx,j-dy,gj_data,gj_istep,gj_jstep);
          double g2mag = dx*gx2+dy*gy2;
          if (g2mag>gmag){
            *pgm=0;
            *(pgm+gm_pstep)=0;
            *(pgm+2*gm_pstep)=0;
          }
          else
          {
            // This is a maximal edge!
            double peak;
            double offset = interpolate_parabola(g2mag, gmag, g1mag, peak);
            *pgm = destT(peak);
            *(pgm+gm_pstep) = destT(std::atan2(dy,dx));
            *(pgm+2*gm_pstep) = destT(offset);
          }
        }
      }
    }
  }
}

#undef VIL_SUPPRESS_NON_MAX_EDGES_INSTANTIATE
#define VIL_SUPPRESS_NON_MAX_EDGES_INSTANTIATE(srcT, destT) \
template void vil_suppress_non_max_edges(const vil_image_view<srcT >& grad_i,\
                                         const vil_image_view<srcT >& grad_j,\
                                         double grad_mag_threshold,\
                                         vil_image_view<destT >& grad_mag);\
template void vil_suppress_non_max_edges_subpixel(const vil_image_view<srcT >& grad_i,\
                                                  const vil_image_view<srcT >& grad_j,\
                                                  double grad_mag_threshold,\
                                                  vil_image_view<destT >& grad_moo)

#endif // vil_suppress_non_max_edges_hxx_
