// This is core/vil/algo/vil_suppress_non_max_edges.txx
#ifndef vil_suppress_non_max_edges_txx_
#define vil_suppress_non_max_edges_txx_
//:
// \file
// \brief Given gradient image, compute magnitude and zero any non-maximal values
// \author Tim Cootes

#include "vil_suppress_non_max_edges.h"
#include <vil/vil_bilin_interp.h>
#include <vil/vil_fill.h>

//: Given gradient images, computes magnitude image containing maximal edges
//  Computes magnitude image.  Zeros any below a threshold.
//  Points with magnitude above a threshold are tested against gradient
//  along normal to the edge and retained only if they are higher than
//  their neighbours.
//
//  Note: Currently assumes single plane only.
//  1 pixel border around output set to zero
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

  // Fill border with zero
  vil_fill_col(grad_mag,0,destT(0));
  vil_fill_col(grad_mag,ni-1,destT(0));
  vil_fill_row(grad_mag,0,destT(0));
  vil_fill_row(grad_mag,nj-1,destT(0));

  const vcl_ptrdiff_t gi_istep = grad_i.istep(), gi_jstep = grad_i.jstep();
  const vcl_ptrdiff_t gj_istep = grad_j.istep(), gj_jstep = grad_j.jstep();
  const vcl_ptrdiff_t gm_istep = grad_mag.istep(), gm_jstep = grad_mag.jstep();

  const srcT * gi_data = &grad_i(0,0);
  const srcT * gj_data = &grad_j(0,0);
  const srcT * gi_row = &grad_i(1,1);
  const srcT * gj_row = &grad_j(1,1);
  destT * gm_row = &grad_mag(1,1);
  unsigned ni1=ni-1;
  unsigned nj1=nj-1;

  for (unsigned j=1;j<nj1;++j, gi_row+=gi_jstep, gj_row+=gj_jstep,
                              gm_row+=gm_jstep)
  {
    const srcT* pgi = gi_row;
    const srcT* pgj = gj_row;
    destT *pgm = gm_row;
    for (unsigned i=1;i<ni1;++i, pgi+=gi_istep, pgj+=gj_istep,
                                pgm+=gm_istep)
    {
      double gmag=vcl_sqrt(pgi[0]*pgi[0] + pgj[0]*pgj[0]);
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

#undef VIL_SUPPRESS_NON_MAX_EDGES_INSTANTIATE
#define VIL_SUPPRESS_NON_MAX_EDGES_INSTANTIATE(srcT, destT) \
template void vil_suppress_non_max_edges(const vil_image_view<srcT>& grad_i,\
                                const vil_image_view<srcT>& grad_j,\
                                double grad_mag_threshold,\
                                vil_image_view<destT>& grad_mag);

#endif // vil_sobel_3x3_txx_
