// This is mul/vil3d/vil3d_resample_trilinear.txx
#ifndef vil3d_resample_trilinear_txx_
#define vil3d_resample_trilinear_txx_


//:
// \file
// \brief Resample a 3D image by an integer factor in each dimension
// \author Kevin de Souza


#include <vil3d/vil3d_resample_trilinear.h>
#include <vil3d/vil3d_trilin_interp.h>


// Resample a 3D image by a different factor in each dimension.
template <class T >
void vil3d_resample_trilinear(const vil3d_image_view< T >& src_image,
                              vil3d_image_view< T >& dst_image,
                              const double dx,
                              const double dy,
                              const double dz)
{
  assert (dx > 0.0 && dy > 0.0 && dz > 0.0);

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
  
  const unsigned dni = static_cast<unsigned>(sni*dx);
  const unsigned dnj = static_cast<unsigned>(snj*dy);
  const unsigned dnk = static_cast<unsigned>(snk*dz);
  dst_image.set_size(dni, dnj, dnk, np);
  const vcl_ptrdiff_t d_istep = dst_image.istep();
  const vcl_ptrdiff_t d_jstep = dst_image.jstep();
  const vcl_ptrdiff_t d_kstep = dst_image.kstep();
  const vcl_ptrdiff_t d_pstep = dst_image.planestep();
  T* d_plane = dst_image.origin_ptr();

  // Loop over all voxels in the destination image
  // and sample from the corresponding point in the source image
  // (except near upper boundaries).
  for (unsigned p=0; p<np; ++p, s_plane+=s_pstep, d_plane+=d_pstep)
  {
    T* d_slice = d_plane;
    for (unsigned k=0; k<static_cast<unsigned>(dnk-dz); ++k, d_slice+=d_kstep)
    {
      T* d_row = d_slice;
      for (unsigned j=0; j<static_cast<unsigned>(dnj-dy); ++j, d_row+=d_jstep)
      {
        T* d_pix = d_row;
        for (unsigned i=0; i<static_cast<unsigned>(dni-dx); ++i, d_pix+=d_istep)
        {
          *d_pix = static_cast<T>(vil3d_trilin_interp_raw(i/dx, j/dy, k/dz, 
                                                          s_plane,
                                                          s_istep, s_jstep, s_kstep));
        }
        // Process the pixels near the upper i boundary - safe_extend interpolation
        for (unsigned i=static_cast<unsigned>(dni-dx); i<dni; ++i, d_pix+=d_istep)
        {
          *d_pix = static_cast<T>(vil3d_trilin_interp_safe_extend(i/dx, j/dy, k/dz, 
                                                                  s_plane,
                                                                  sni, snj, snk,
                                                                  s_istep, s_jstep, s_kstep));
        }
      }

      // Process the pixels near the upper j boundary - safe_extend interpolation
      for (unsigned j=static_cast<unsigned>(dnj-dy); j<dnj; ++j, d_row+=d_jstep)
      {
        T* d_pix = d_row;
        for (unsigned i=0; i<dni; ++i, d_pix+=d_istep)
        {
          *d_pix = static_cast<T>(vil3d_trilin_interp_safe_extend(i/dx, j/dy, k/dz, 
                                                                  s_plane,
                                                                  sni, snj, snk,
                                                                  s_istep, s_jstep, s_kstep));
        }
      }      
    }

    // Process the pixels near the upper k boundary - safe_extend interpolation
    for (unsigned k=static_cast<unsigned>(dnk-dz); k<dnk; ++k, d_slice+=d_kstep)
    {
      T* d_row = d_slice;
      for (unsigned j=0; j<dnj; ++j, d_row+=d_jstep)
      {
        T* d_pix = d_row;
        for (unsigned i=0; i<dni; ++i, d_pix+=d_istep)
        {
          *d_pix = static_cast<T>(vil3d_trilin_interp_safe_extend(i/dx, j/dy, k/dz, 
                                                                  s_plane,
                                                                  sni, snj, snk,
                                                                  s_istep, s_jstep, s_kstep));
        }
      }      
    }

  }

}


#define VIL3D_RESAMPLE_TRILINEAR_INSTANTIATE(T) \
template void vil3d_resample_trilinear(const vil3d_image_view<T >& src_image, \
                                       vil3d_image_view<T >& dst_image, \
                                       const double dx, \
                                       const double dy, \
                                       const double dz)


#endif // vil3d_resample_trilinear_txx_
