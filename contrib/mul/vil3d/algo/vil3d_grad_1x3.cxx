//:
// \file
// \brief Apply gradient filter to single plane 3D image
// \author Tim Cootes

#include "vil3d_grad_1x3.h"
#include <vil3d/vil3d_slice.h>
#include <vcl_cassert.h>

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
void vil3d_grad_1x3_1plane(const vil3d_image_view<vxl_byte>& src_im,
                           vil3d_image_view<float>& grad_i,
                           vil3d_image_view<float>& grad_j,
                           vil3d_image_view<float>& grad_k)
{
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  unsigned nk = src_im.nk();

  // Not exhaustive
  assert(grad_i.ni()==ni);
  assert(grad_j.nj()==nj);
  assert(grad_k.nk()==nk);

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

  const vcl_ptrdiff_t src_istep = src_im.istep();
  const vcl_ptrdiff_t gri_istep = grad_i.istep();
  const vcl_ptrdiff_t grj_istep = grad_j.istep();
  const vcl_ptrdiff_t grk_istep = grad_j.istep();
  
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
        //      to ensure conversion to float before subtraction
        *pgi = 0.5f*s[di1] - 0.5f*s[di2];
        // Compute gradient in j
        *pgj = 0.5f*s[dj1] - 0.5f*s[dj2];
        // Compute gradient in k
        *pgk = 0.5f*s[dk1] - 0.5f*s[dk2];

        s   += src_istep;
        pgi += gri_istep;
        pgj += grj_istep;
        pgk += grk_istep;
      }
    }
  }

  vil3d_zero_border_1plane(grad_i);
  vil3d_zero_border_1plane(grad_j);
  vil3d_zero_border_1plane(grad_k);
}

//: Compute gradients of single plane of 3D data using 1x3 grad filters
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


//: Compute gradients of single plane of 3D data using 1x3 grad filters
void vil3d_grad_1x3_1plane(const vil3d_image_view<vxl_int_32>& src_im,
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
      const vxl_int_32* s = &src_im(1,j,k);
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

//: Compute square dgradient magnitude of single plane of 3D data
//  Use (-0.5,0,+0.5) filters in i,j,k
void vil3d_grad_1x3_mag_sq_1plane(const vil3d_image_view<vxl_byte>& src_im,
                                  vil3d_image_view<float>& grad_mag2)
{
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  unsigned nk = src_im.nk();

  assert(grad_mag2.ni()==ni);
  assert(grad_mag2.nj()==nj);
  assert(grad_mag2.nk()==nk);

  if (ni<=2 || nj<=2 || nk<=2)
  {
    grad_mag2.fill(0.0f);
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
      float* pg = &grad_mag2(1,j,k);

      for (unsigned i=1;i<ni1;++i)
      {
        // Compute gradient in i
        // Note: Multiply each element individually
        //      to ensure conversion to float before subtraction
        float dx = 0.5f*s[di1] - 0.5f*s[di2];
        // Compute gradient in j
        float dy = 0.5f*s[dj1] - 0.5f*s[dj2];
        // Compute gradient in k
        float dz = 0.5f*s[dk1] - 0.5f*s[dk2];

        *pg = dx*dx + dy*dy + dz*dz;

        s   += src_im.istep();
        pg += grad_mag2.istep();
      }
    }
  }

  vil3d_zero_border_1plane(grad_mag2);
}

//: Compute square gradient magnitude of single plane of 3D data
//  Use (-0.5,0,+0.5) filters in i,j,k
void vil3d_grad_1x3_mag_sq_1plane(const vil3d_image_view<float>& src_im,
                                  vil3d_image_view<float>& grad_mag2)
{
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  unsigned nk = src_im.nk();

  assert(grad_mag2.ni()==ni);
  assert(grad_mag2.nj()==nj);
  assert(grad_mag2.nk()==nk);

  if (ni<=2 || nj<=2 || nk<=2)
  {
    grad_mag2.fill(0.0f);
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
      float* pg = &grad_mag2(1,j,k);

      for (unsigned i=1;i<ni1;++i)
      {
        // Compute gradient in i
        float dx = s[di1] - s[di2];
        // Compute gradient in j
        float dy = s[dj1] - s[dj2];
        // Compute gradient in k
        float dz = s[dk1] - s[dk2];

        *pg = 0.25f*(dx*dx + dy*dy + dz*dz);

        s   += src_im.istep();
        pg += grad_mag2.istep();
      }
    }
  }

  vil3d_zero_border_1plane(grad_mag2);
}


//: Compute square gradient magnitude of single plane of 3D data
//  Use (-0.5,0,+0.5) filters in i,j,k
void vil3d_grad_1x3_mag_sq_1plane(const vil3d_image_view<vxl_int_32>& src_im,
                                  vil3d_image_view<float>& grad_mag2)
{
  unsigned ni = src_im.ni();
  unsigned nj = src_im.nj();
  unsigned nk = src_im.nk();

  assert(grad_mag2.ni()==ni);
  assert(grad_mag2.nj()==nj);
  assert(grad_mag2.nk()==nk);

  if (ni<=2 || nj<=2 || nk<=2)
  {
    grad_mag2.fill(0.0f);
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
      const vxl_int_32* s = &src_im(1,j,k);
      float* pg = &grad_mag2(1,j,k);

      for (unsigned i=1;i<ni1;++i)
      {
        // Compute gradient in i
        float dx = static_cast<float>(s[di1]) - static_cast<float>(s[di2]);
        // Compute gradient in j
        float dy = static_cast<float>(s[dj1]) - static_cast<float>(s[dj2]);
        // Compute gradient in k
        float dz = static_cast<float>(s[dk1]) - static_cast<float>(s[dk2]);

        *pg = 0.25f*(dx*dx + dy*dy + dz*dz);

        s   += src_im.istep();
        pg += grad_mag2.istep();
      }
    }
  }

  vil3d_zero_border_1plane(grad_mag2);
}

