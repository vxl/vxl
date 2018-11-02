// This is mul/vil3d/algo/vil3d_suppress_non_max_edges.hxx
#ifndef vil3d_suppress_non_max_edges_hxx_
#define vil3d_suppress_non_max_edges_hxx_
//:
// \file
// \brief Given gradient image, compute magnitude and zero any non-maximal values
// \author Tim Cootes

#include <iostream>
#include <cmath>
#include "vil3d_suppress_non_max_edges.h"
#include <vil3d/algo/vil3d_fill_border.h>
#include <vil3d/vil3d_trilin_interp.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>


//: Given gradient images, computes magnitude image containing maximal edges
//  Points with magnitude above a threshold are tested against gradient
//  along normal to the edge and retained only if they are higher than
//  their neighbours.
//
//  Gradient images are assumed to be in units of (intensity change) per world unit.
//  (ie the output of vil3d_world_gradients)
//
//  Note: Currently assumes single plane only.
//  2 pixel border around output set to zero.
//  If two neighbouring edges have exactly the same strength, it retains
//  both (ie an edge is eliminated if it is strictly lower than a neighbour,
//  but not if it is the same as two neighbours).
//
// \relatesalso vil3d_image_view
template<class srcT>
void vil3d_suppress_non_max_edges(const vil3d_image_view<srcT>& world_grad,
                                  const vil3d_image_view<srcT>& grad_mag,
                                  double voxel_width_i,
                                  double voxel_width_j,
                                  double voxel_width_k,
                                  srcT grad_mag_threshold,
                                  vil3d_image_view<srcT>& max_grad_mag)
{
  assert(world_grad.nplanes()==3);
  assert(grad_mag.nplanes()==1);
  unsigned ni = world_grad.ni(), nj = world_grad.nj(), nk = world_grad.nk();
  assert(ni>2 && nj>2 && nk>2);
  assert(grad_mag.ni()==ni && grad_mag.nj()==nj && grad_mag.nk()==nk);
  max_grad_mag.set_size(ni,nj,nk,1);

  // Fill 2 voxel border with zero
  vil3d_fill_border(max_grad_mag,2,2,2,srcT(0));

  const std::ptrdiff_t g_istep = world_grad.istep(), g_jstep = world_grad.jstep(),
                      g_kstep = world_grad.kstep();
  const std::ptrdiff_t gm_istep = grad_mag.istep(), gm_jstep = grad_mag.jstep(),
                      gm_kstep = grad_mag.kstep();
  const std::ptrdiff_t d_istep = max_grad_mag.istep(), d_jstep = max_grad_mag.jstep(),
                      d_kstep = max_grad_mag.kstep();
  const std::ptrdiff_t pstep = world_grad.planestep();
  const std::ptrdiff_t pstep2 = 2*pstep;

  unsigned ihi=ni-3;
  unsigned jhi=nj-3;
  unsigned khi=nk-3;

  double step_size = std::sqrt((voxel_width_i*voxel_width_i +
                               voxel_width_j*voxel_width_j +
                               voxel_width_k*voxel_width_k   )/3.0);

  const srcT * gm_data = &grad_mag(0,0,0);
  const srcT * wg_slice = &world_grad(2,2,2);
  const srcT * gm_slice = &grad_mag(2,2,2);
  srcT * d_slice = &max_grad_mag(2,2,2);

  for (unsigned k=2; k<=khi; ++k, wg_slice+=g_kstep, gm_slice+=gm_kstep,
                                  d_slice+=d_kstep)
  {
    const srcT* wg_row = wg_slice;
    const srcT* gm_row = gm_slice;
    srcT *d_row = d_slice;

    for (unsigned j=2; j<=jhi; ++j, wg_row+=g_jstep, gm_row+=gm_jstep,
                                    d_row+=d_jstep)
    {
      const srcT* vg = wg_row;
      const srcT* vgm = gm_row;
      srcT *v_new_gm = d_row;
      for (unsigned i=2; i<=ihi; ++i, vg+=g_istep, vgm+=gm_istep,
                                      v_new_gm+=d_istep)
      {
        srcT gmag=*vgm;
        if (gmag<grad_mag_threshold)
        {
          *v_new_gm = 0; continue;
        }
        // Unit vector in world co-ords would be pgi/(gmag)
        // Multiply by step_size
        // Divide by voxel width to get back to pixel co-ords
        double dx=step_size*vg[0]/(gmag*voxel_width_i);
        double dy=step_size*vg[pstep]/(gmag*voxel_width_j);
        double dz=step_size*vg[pstep2]/(gmag*voxel_width_k);

        // Check that step isn't larger than 1 pixel in any direction
        double a= std::fabs(dx); if (a>=1.0) { dx/=a; dy/=a; dz/=a; }
        a= std::fabs(dy); if (a>=1.0) { dx/=a; dy/=a; dz/=a; }
        a= std::fabs(dz); if (a>=1.0) { dx/=a; dy/=a; dz/=a; }

        // Evaluate gradient at point (i+dx,j+dy,k+dz)
        double gm1=vil3d_trilin_interp_raw(i+dx,j+dy,k+dz,gm_data,gm_istep,gm_jstep,gm_kstep);
        if (gm1>gmag) { *v_new_gm=0; continue; }
        double gm2=vil3d_trilin_interp_raw(i-dx,j-dy,k-dz,gm_data,gm_istep,gm_jstep,gm_kstep);
        if (gm2>gmag) *v_new_gm=0;
        else          *v_new_gm=gmag;
      }
    }
  }
}

#undef VIL3D_SUPPRESS_NON_MAX_EDGES_INSTANTIATE
#define VIL3D_SUPPRESS_NON_MAX_EDGES_INSTANTIATE(srcT) \
template void vil3d_suppress_non_max_edges(const vil3d_image_view<srcT >& world_grad,\
                                           const vil3d_image_view<srcT >& grad_mag,\
                                           double voxel_width_i,\
                                           double voxel_width_j,\
                                           double voxel_width_k,\
                                           srcT grad_mag_threshold,\
                                           vil3d_image_view<srcT >& max_grad_mag)

#endif // vil3d_suppress_non_max_edges_hxx_
