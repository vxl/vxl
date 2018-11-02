// This is mul/vil3d/algo/vil3d_world_gradients.hxx
#ifndef vil3d_world_gradients_hxx_
#define vil3d_world_gradients_hxx_
//:
// \file
// \brief Given image gradients compute world gradients and gradient magnitude
// \author Tim Cootes

#include <iostream>
#include <cmath>
#include "vil3d_world_gradients.h"
#include <vil3d/algo/vil3d_fill_border.h>
#include <vil3d/vil3d_transform.h>
#include <vil3d/vil3d_plane.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//: Functor class to scale by s
class vil3d_math_scale_functor
{
 private:
  double s_;
 public:
  vil3d_math_scale_functor(double s) : s_(s) {}
  float operator()(vxl_byte x) const { return float(s_*x); }
  float operator()(unsigned x) const { return float(s_*x); }
  float operator()(short x) const { return float(s_*x); }
  float operator()(int x) const { return float(s_*x); }
  float operator()(float x) const { return float(s_*x); }
  float operator()(double x) const { return float(s_*x); }
};

//: Given image gradients compute world gradients and gradient magnitude
//  Input gradient images are assumed to be un-normalised pixel gradients
//  (ie no scaling has been done to take account of world pixel widths).
//  Divides each by corresponding pixel dimension to give gradient in world units
//  (ie intensity change per unit world length) in world_grad (3 plane image)
//  The gradient magnitude output is in units of intensity change per world length
//  (ie it does take account of voxel sizes).
//
//  Note: Currently assumes single plane only.
//  1 pixel border around output set to zero.
//
// \relatesalso vil3d_image_view
template<class srcT, class destT>
void vil3d_world_gradients(const vil3d_image_view<srcT>& grad_i,
                           const vil3d_image_view<srcT>& grad_j,
                           const vil3d_image_view<srcT>& grad_k,
                           double voxel_width_i,
                           double voxel_width_j,
                           double voxel_width_k,
                           vil3d_image_view<destT>& world_grad,
                           vil3d_image_view<destT>& grad_mag)
{
  assert(grad_i.nplanes()==grad_j.nplanes());
  assert(grad_i.nplanes()==grad_k.nplanes());
  assert(grad_i.nplanes()==1);
  unsigned ni = grad_i.ni(), nj = grad_i.nj(), nk = grad_i.nk();
  assert(ni>2 && nj>2 && nk>2);
  assert(grad_j.ni()==ni && grad_j.nj()==nj && grad_j.nk()==nk);
  assert(grad_k.ni()==ni && grad_k.nj()==nj && grad_k.nk()==nk);
  world_grad.set_size(ni,nj,nk,3);
  grad_mag.set_size(ni,nj,nk,1);

  vil3d_image_view<destT> w_grad_i=vil3d_plane(world_grad,0);
  vil3d_image_view<destT> w_grad_j=vil3d_plane(world_grad,1);
  vil3d_image_view<destT> w_grad_k=vil3d_plane(world_grad,2);

  vil3d_transform(grad_i,w_grad_i,vil3d_math_scale_functor(1.0/voxel_width_i));
  vil3d_transform(grad_j,w_grad_j,vil3d_math_scale_functor(1.0/voxel_width_j));
  vil3d_transform(grad_k,w_grad_k,vil3d_math_scale_functor(1.0/voxel_width_k));

  // Fill 1 voxel border with zero
  vil3d_fill_border(grad_mag,1,1,1,destT(0));

  const std::ptrdiff_t gi_istep = w_grad_i.istep(), gi_jstep = w_grad_i.jstep(),
                      gi_kstep = w_grad_i.kstep();
  const std::ptrdiff_t gj_istep = w_grad_j.istep(), gj_jstep = w_grad_j.jstep(),
                      gj_kstep = w_grad_j.kstep();
  const std::ptrdiff_t gk_istep = w_grad_k.istep(), gk_jstep = w_grad_k.jstep(),
                      gk_kstep = w_grad_k.kstep();
  const std::ptrdiff_t gm_istep = grad_mag.istep(), gm_jstep = grad_mag.jstep(),
                      gm_kstep = grad_mag.kstep();

  unsigned ihi=ni-2;
  unsigned jhi=nj-2;
  unsigned khi=nk-2;

  // Scaling to allow for relative size of voxels
  double c2i=1.0/(voxel_width_i*voxel_width_i);
  double c2j=1.0/(voxel_width_j*voxel_width_j);
  double c2k=1.0/(voxel_width_k*voxel_width_k);

  // Compute gradient magnitude at every point
  const srcT * gi_slice = &w_grad_i(1,1,1);
  const srcT * gj_slice = &w_grad_j(1,1,1);
  const srcT * gk_slice = &w_grad_k(1,1,1);
  destT * gm_slice = &grad_mag(1,1,1);

  for (unsigned k=1; k<=khi; ++k, gi_slice+=gi_kstep, gj_slice+=gj_kstep,
                                  gk_slice+=gk_kstep, gm_slice+=gm_kstep)
  {
    const srcT* gi_row=gi_slice;
    const srcT* gj_row=gj_slice;
    const srcT* gk_row=gk_slice;
    destT *gm_row = gm_slice;

    for (unsigned j=2; j<=jhi; ++j, gi_row+=gi_jstep, gj_row+=gj_jstep,
                                    gk_row+=gk_jstep, gm_row+=gm_jstep)
    {
      const srcT* pgi = gi_row;
      const srcT* pgj = gj_row;
      const srcT* pgk = gk_row;
      destT *pgm = gm_row;
      for (unsigned i=2; i<=ihi; ++i, pgi+=gi_istep, pgj+=gj_istep,
                                      pgk+=gk_istep, pgm+=gm_istep)
      {
        *pgm=destT(std::sqrt(double(c2i*pgi[0]*pgi[0] + c2j*pgj[0]*pgj[0] + c2k*pgk[0]*pgk[0])));
      }
    }
  }
}

#undef VIL3D_WORLD_GRADIENTS_INSTANTIATE
#define VIL3D_WORLD_GRADIENTS_INSTANTIATE(srcT, destT) \
template void vil3d_world_gradients(const vil3d_image_view<srcT >& grad_i,\
                                    const vil3d_image_view<srcT >& grad_j,\
                                    const vil3d_image_view<srcT >& grad_k,\
                                    double voxel_width_i,\
                                    double voxel_width_j,\
                                    double voxel_width_k,\
                                    vil3d_image_view<destT >& world_grad,\
                                    vil3d_image_view<destT >& grad_mag)

#endif // vil3d_world_gradients_hxx_
