//:
//  \file
//  \brief Apply gradient filter to single plane 3D image
//  \author Tim Cootes

#include "vil3d_grad_1x3.h"
#include <vil3d/vil3d_slice.h>

//: Fill 1 pixel border in image with zeros
void vil3d_zero_border_1plane(vil3d_image_view<float>& image)
{
  unsigned ni = image.ni();
  unsigned nj = image.nj();
  unsigned nk = image.nk();
  vil3d_slice_ij(image,0).fill(0.0f);
  vil3d_slice_ij(image,nk-1).fill(0.0f);
  vil3d_slice_jk(image,0).fill(0.0f);
  vil3d_slice_jk(image,ni-1).fill(0.0f);
  vil3d_slice_ik(image,0).fill(0.0f);
  vil3d_slice_ik(image,nj-1).fill(0.0f);
}

//: Compute gradients of single plane of 2D data using 1x3 grad filters
//  Computes both i and j gradients of an ni x nj plane of data
void vil3d_grad_1x3_1plane(const vil3d_image_view<vxl_byte>& src_im,
                    vil3d_image_view<float>& grad_i,
                    vil3d_image_view<float>& grad_j,
                    vil3d_image_view<float>& grad_k)
{
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  unsigned nk = src_im.nk();

  if (ni<=2 || nj<=2 || nk<=2)
  {
      // Zero the elements in the column
    grad_i.fill(0.0f);
    grad_j.fill(0.0f);
    grad_k.fill(0.0f);
    return;
  }

  // Compute relative sampling positions
  const vcl_ptrdiff_t di1 = src_im.istep(), di2= -src_im.istep();
  const vcl_ptrdiff_t dj1 = src_im.jstep(), dj2= -src_im.jstep();
  const vcl_ptrdiff_t dk1 = src_im.kstep(), dk2= -src_im.kstep();

  const unsigned ni1 = ni-1;
  const unsigned nj1 = nj-1;
  const unsigned nk1 = nk-1;

  for (unsigned k=1;k<nk1;++k)
  {
    for (unsigned j=1;j<nj1;++j)
    {
      const unsigned char* s = &src_im(1,j,k);
      float* pgi = &grad_i(1,j,k);
      float* pgj = &grad_j(1,j,k);
      float* pgk = &grad_k(1,j,k);

      for (unsigned i=1;i<ni1;++i)
      {
        // Compute gradient in i
        // Note: Multiply each element individually
        //      to ensure conversion to float before addition
        *pgi = 0.5f*s[di1] - 0.5f*s[di2];
        // Compute gradient in j
        *pgj = 0.5f*s[dj1] - 0.5f*s[dj2];
        // Compute gradient in k
        *pgk = 0.5f*s[dk1] - 0.5f*s[dk2];

        s   += src_im.istep();
        pgi += grad_i.istep();
        pgj += grad_j.istep();
        pgk += grad_k.istep();
      }
    }
  }

  vil3d_zero_border_1plane(grad_i);
  vil3d_zero_border_1plane(grad_j);
  vil3d_zero_border_1plane(grad_k);
}

//: Compute gradients of single plane of 2D data using 1x3 grad filters
//  Computes both i and j gradients of an ni x nj plane of data
void vil3d_grad_1x3_1plane(const vil3d_image_view<float>& src_im,
                    vil3d_image_view<float>& grad_i,
                    vil3d_image_view<float>& grad_j,
                    vil3d_image_view<float>& grad_k)
{
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  unsigned nk = src_im.nk();

  if (ni<=2 || nj<=2 || nk<=2)
  {
      // Zero the elements in the column
    grad_i.fill(0.0f);
    grad_j.fill(0.0f);
    grad_k.fill(0.0f);
    return;
  }

  // Compute relative sampling positions
  const vcl_ptrdiff_t di1 = src_im.istep(), di2= -src_im.istep();
  const vcl_ptrdiff_t dj1 = src_im.jstep(), dj2= -src_im.jstep();
  const vcl_ptrdiff_t dk1 = src_im.kstep(), dk2= -src_im.kstep();

  const unsigned ni1 = ni-1;
  const unsigned nj1 = nj-1;
  const unsigned nk1 = nk-1;

  for (unsigned k=1;k<nk1;++k)
  {
    for (unsigned j=1;j<nj1;++j)
    {
      const float* s = &src_im(1,j,k);
      float* pgi = &grad_i(1,j,k);
      float* pgj = &grad_j(1,j,k);
      float* pgk = &grad_k(1,j,k);

      for (unsigned i=1;i<ni1;++i)
      {
        // Compute gradient in i
        *pgi = 0.5f*(s[di1] - s[di2]);
        // Compute gradient in j
        *pgj = 0.5f*(s[dj1] - s[dj2]);
        // Compute gradient in k
        *pgk = 0.5f*(s[dk1] - s[dk2]);

        s   += src_im.istep();
        pgi += grad_i.istep();
        pgj += grad_j.istep();
        pgk += grad_k.istep();
      }
    }
  }

  vil3d_zero_border_1plane(grad_i);
  vil3d_zero_border_1plane(grad_j);
  vil3d_zero_border_1plane(grad_k);
}

/*
//: Compute gradients of single plane of 2D data using 1x3 grad filters
//  Computes both x and j gradients of an nx x nj plane of data
void vil3d_grad_1x3_1plane(const float* src,
                           vcl_ptrdiff_t s_istep, vcl_ptrdiff_t s_jstep,
                           float* gi, vcl_ptrdiff_t gi_istep, vcl_ptrdiff_t gi_jstep,
                           float* gj, vcl_ptrdiff_t gj_istep, vcl_ptrdiff_t gj_jstep,
                           unsigned ni, unsigned nj)
{
  const float* s_data = src;
  float *gi_data = gi;
  float *gj_data = gj;

  if (ni==0 || nj==0) return;
  if (ni==1)
  {
      // Zero the elements in the column
    for (unsigned j=0;j<nj;++j)
    {
      *gi_data = 0;
      *gj_data = 0;
      gi_data += gi_jstep;
      gj_data += gj_jstep;
    }
    return;
  }
  if (nj==1)
  {
      // Zero the elements in the column
    for (unsigned i=0;i<ni;++i)
    {
      *gi_data = 0;
      *gj_data = 0;
      gi_data += gi_istep;
      gj_data += gj_istep;
    }
    return;
  }

  // Compute relative grid positions
  //     o2
  //  o4    o5
  //     o7
  const int o2 = s_jstep;
  const int o4 = -s_istep;
  const int o5 = s_istep;
  const int o7 = -s_jstep;

  const unsigned ni1 = ni-1;
  const unsigned nj1 = nj-1;

  s_data += s_istep + s_jstep;
  gi_data += gi_jstep;
  gj_data += gj_jstep;

  for (unsigned j=1;j<nj1;++j)
  {
    const float* s = s_data;
    float* pgi = gi_data;
    float* pgj = gj_data;

    // Zero the first elements in the rows
    *pgi = 0; pgi+=gi_istep;
    *pgj = 0; pgj+=gj_istep;

    for (unsigned i=1;i<ni1;++i)
    {
    // Compute gradient in i
      *pgi = 0.5f*(s[o5]-s[o4]);
    // Compute gradient in j
      *pgj = 0.5f*(s[o2]-s[o7]);

      s+=s_istep;
      pgi += gi_istep;
      pgj += gj_istep;
    }

    // Zero the last elements in the rows
    *pgi = 0;
    *pgj = 0;

    // Move to next row
    s_data  += s_jstep;
    gi_data += gi_jstep;
    gj_data += gj_jstep;
  }

  // Zero the first and last rows
  for (unsigned i=0;i<ni;++i)
  {
    *gi=0; gi+=gi_istep;
    *gj=0; gj+=gj_istep;
    *gi_data = 0; gi_data+=gi_istep;
    *gj_data = 0; gj_data+=gj_istep;
  }
}
*/

