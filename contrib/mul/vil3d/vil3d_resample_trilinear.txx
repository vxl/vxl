// This is contrib/mul/vil3d/vil3d_resample_trilinear.txx


#ifndef vil3d_resample_trilinear_txx_
#define vil3d_resample_trilinear_txx_


//:
// \file
// \brief Resample a 3D image by an integer factor in each dimension
// \author Kevin de Souza


#include <vil3d/vil3d_resample_trilinear.h>
#include <vil3d/vil3d_trilin_interp.h>


//: Resample a 3D image by an integer factor in each dimension.
//  dst_image resized by factors dx, dy, dz.
//  dst_image(i, j, k, p) is sampled from src_image(i/dx, j/dy, k/dz, p).
//  Trilinear interpolation is performed on all voxels except ones near
//  the upper boundaries.
template <class T >
void vil3d_resample_trilinear(const vil3d_image_view< T >& src_image,
                              vil3d_image_view< T >& dst_image,
                              const unsigned dx,
                              const unsigned dy,
                              const unsigned dz)
{
  // Assume planes are the same for both images
  const unsigned np = src_image.nplanes();

  const unsigned sni = src_image.ni();
  const unsigned snj = src_image.nj();
  const unsigned snk = src_image.nk();
  const vcl_ptrdiff_t s_istep = src_image.istep();
  const vcl_ptrdiff_t s_jstep = src_image.jstep();
  const vcl_ptrdiff_t s_kstep = src_image.kstep();
  const vcl_ptrdiff_t s_pstep = src_image.planestep();
  const T* s_plane = src_image.origin_ptr();

  const unsigned dni = sni*dx;
  const unsigned dnj = snj*dy;
  const unsigned dnk = snk*dz;
  dst_image.set_size(dni, dnj, dnk, np);
  const vcl_ptrdiff_t d_istep = dst_image.istep();
  const vcl_ptrdiff_t d_jstep = dst_image.jstep();
  const vcl_ptrdiff_t d_kstep = dst_image.kstep();
  const vcl_ptrdiff_t d_pstep = dst_image.planestep();
  T* d_plane = dst_image.origin_ptr();

  // Loop over all voxels in the destination image (except near upper boundaries)
  // and sample from the corresponding point in the source image.
  for (unsigned p=0; p<np; ++p, s_plane+=s_pstep, d_plane+=d_pstep)
  {
    T* d_slice = d_plane;
    for (unsigned k=0; k<dnk-dz-1; ++k, d_slice+=d_kstep)
    {
      T* d_row = d_slice;
      for (unsigned j=0; j<dnj-dy-1; ++j, d_row+=d_jstep)
      {
        T* d_pix = d_row;
        for (unsigned i=0; i<dni-dx-1; ++i, d_pix+=d_istep)
        {
          *d_pix = vil3d_trilin_interp_raw(i/dx, j/dy, k/dz, s_plane, 
                                           s_istep, s_jstep, s_kstep);
        }
      }
    }
  }

  // Now process the pixels near the upper boundaries - no interpolation,
  // just give them the same value as the source image.
  d_plane = dst_image.origin_ptr();
  for (unsigned p=0; p<np; ++p, s_plane+=s_pstep, d_plane+=d_pstep)
  {
    T* d_slice = d_plane;
    for (unsigned k=dnk-dz-1; k<dnk; ++k, d_slice+=d_kstep)
    {
      T* d_row = d_slice;
      for (unsigned j=dnj-dy-1; j<dnj; ++j, d_row+=d_jstep)
      {
        T* d_pix = d_row;
        for (unsigned i=dni-dx-1; i<dni; ++i, d_pix+=d_istep)
        {
          *d_pix = src_image(i/dx, j/dy, k/dz);
        }
      }
    }
  }
}


#define VIL3D_RESAMPLE_TRILINEAR_INSTANTIATE( T ) \
template void vil3d_resample_trilinear(const vil3d_image_view< T >& src_image, \
                                       vil3d_image_view< T >& dst_image, \
                                       const unsigned dx, \
                                       const unsigned dy, \
                                       const unsigned dz)


#endif // vil3d_resample_trilinear_txx_
