//:
// \file
// \brief Functions to compute orientations and gradient magnitude
// \author Tim Cootes

#include "vil_orientations.h"
#include <vcl_cassert.h>
#include <vcl_cmath.h>

//: Compute orientation (in radians) and gradient magnitude at each pixel
void vil_orientations(const vil_image_view<float>& grad_i,
                      const vil_image_view<float>& grad_j,
                      vil_image_view<float>& orient_im,
                      vil_image_view<float>& grad_mag)
{
  assert(grad_i.nplanes()==1 && grad_j.nplanes()==1);
  unsigned ni = grad_i.ni(), nj = grad_j.nj();
  assert(grad_j.ni()==ni && grad_j.nj()==nj);
  orient_im.set_size(ni,nj,1);
  grad_mag.set_size(ni,nj,1);

  const vcl_ptrdiff_t gi_istep = grad_i.istep(), gi_jstep = grad_i.jstep();
  const vcl_ptrdiff_t gj_istep = grad_j.istep(), gj_jstep = grad_j.jstep();
  const vcl_ptrdiff_t o_istep = orient_im.istep(), o_jstep = orient_im.jstep();
  const vcl_ptrdiff_t gm_istep = grad_mag.istep(), gm_jstep = grad_mag.jstep();

  const float * gi_row = &grad_i(0,0);
  const float * gj_row = &grad_j(0,0);
  float * o_row  = &orient_im(0,0);
  float * gm_row = &grad_mag(0,0);

  for (unsigned j=0;j<nj;++j, gi_row+=gi_jstep, gj_row+=gj_jstep,
                              o_row+=o_jstep, gm_row+=gm_jstep)
  {
    const float* pgi = gi_row;
    const float* pgj = gj_row;
    float *po = o_row;
    float *pgm = gm_row;
    for (unsigned i=0;i<ni;++i, pgi+=gi_istep, pgj+=gj_istep,
                                po+=o_istep, pgm+=gm_istep)
    {
      *po  = vcl_atan2(*pgj,*pgi);
      *pgm = vcl_sqrt(pgi[0]*pgi[0] + pgj[0]*pgj[0]);
    }
  }
}

//: Compute discrete orientation and gradient magnitude at each pixel
//  Computes orientation at each pixel and scales to range [0,n_orientations-1].
//  Orientation of i corresponds to angles in range [(i-0.5)dA,(i+0.5)dA]
//  where dA=2*pi/n_orientations.
//  Images assumed to be single plane
void vil_orientations(const vil_image_view<float>& grad_i,
                      const vil_image_view<float>& grad_j,
                      vil_image_view<vxl_byte>& orient_im,
                      vil_image_view<float>& grad_mag,
                      unsigned n_orientations)
{
{
  assert(grad_i.nplanes()==1 && grad_j.nplanes()==1);
  assert(n_orientations<=256);
  unsigned ni = grad_i.ni(), nj = grad_j.nj();
  assert(grad_j.ni()==ni && grad_j.nj()==nj);
  orient_im.set_size(ni,nj,1);
  grad_mag.set_size(ni,nj,1);

  const vcl_ptrdiff_t gi_istep = grad_i.istep(), gi_jstep = grad_i.jstep();
  const vcl_ptrdiff_t gj_istep = grad_j.istep(), gj_jstep = grad_j.jstep();
  const vcl_ptrdiff_t o_istep = orient_im.istep(), o_jstep = orient_im.jstep();
  const vcl_ptrdiff_t gm_istep = grad_mag.istep(), gm_jstep = grad_mag.jstep();

  const float * gi_row = &grad_i(0,0);
  const float * gj_row = &grad_j(0,0);
  vxl_byte * o_row  = &orient_im(0,0);
  float * gm_row = &grad_mag(0,0);

  float scale = (2*n_orientations-1)/(6.28319f);

  for (unsigned j=0;j<nj;++j, gi_row+=gi_jstep, gj_row+=gj_jstep,
                              o_row+=o_jstep, gm_row+=gm_jstep)
  {
    const float* pgi = gi_row;
    const float* pgj = gj_row;
    vxl_byte *po = o_row;
    float *pgm = gm_row;
    for (unsigned i=0;i<ni;++i, pgi+=gi_istep, pgj+=gj_istep,
                                po+=o_istep, pgm+=gm_istep)
    {
      // In order to ensure bins are centred at k*2pi/n_orientation points,
      // compute position in twice angle range, then adjust.
      int A2 = ((vcl_atan2(*pgj,*pgi)+3.14159)*scale);
      *po  = vxl_byte(((A2+1)/2)%n_orientations);
      *pgm = vcl_sqrt(pgi[0]*pgi[0] + pgj[0]*pgj[0]);
    }
  }
}

}
