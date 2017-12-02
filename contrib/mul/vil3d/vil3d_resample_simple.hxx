// This is mul/vil3d/vil3d_resample_simple.hxx
#ifndef vil3d_resample_simple_hxx_
#define vil3d_resample_simple_hxx_
//:
// \file
// \brief Resample a 3D image by a different factor in each dimension
// \author Kevin de Souza

#include "vil3d_resample_simple.h"

//: Resample a 3D image by a different factor in each dimension.
//  dst_image resized by factors dx, dy, dz.
//  dst_image(i, j, k, p) is sampled from src_image(i/dx, j/dy, k/dz, p).
//  No interpolation is performed.
template <class T >
void vil3d_resample_simple(const vil3d_image_view< T >& src_image,
                           vil3d_image_view< T >& dst_image,
                           const double dx,
                           const double dy,
                           const double dz)
{
  // Assume planes are the same for both images
  const unsigned np = src_image.nplanes();

  const unsigned sni = src_image.ni();
  const unsigned snj = src_image.nj();
  const unsigned snk = src_image.nk();

  const unsigned dni = static_cast<unsigned>(sni*dx);
  const unsigned dnj = static_cast<unsigned>(snj*dy);
  const unsigned dnk = static_cast<unsigned>(snk*dz);

  dst_image.set_size(dni, dnj, dnk, np);
  const std::ptrdiff_t d_istep = dst_image.istep();
  const std::ptrdiff_t d_jstep = dst_image.jstep();
  const std::ptrdiff_t d_kstep = dst_image.kstep();
  const std::ptrdiff_t d_pstep = dst_image.planestep();
  T* d_plane = dst_image.origin_ptr();

  // Loop over all voxels in the destination image and
  // sample from the corresponding point in the source image
  for (unsigned p=0; p<np; ++p, d_plane+=d_pstep)
  {
    T* d_slice = d_plane;
    for (unsigned k=0; k<dnk; ++k, d_slice+=d_kstep)
    {
      T* d_row = d_slice;
      for (unsigned j=0; j<dnj; ++j, d_row+=d_jstep)
      {
        T* d_pix = d_row;
        for (unsigned i=0; i<dni; ++i, d_pix+=d_istep)
        {
          *d_pix = src_image(static_cast<unsigned>(i/dx),
                             static_cast<unsigned>(j/dy),
                             static_cast<unsigned>(k/dz),
                             p);
        }
      }
    }
  }
}


#define VIL3D_RESAMPLE_SIMPLE_INSTANTIATE( T ) \
template void vil3d_resample_simple(const vil3d_image_view< T >& src_image, \
                                    vil3d_image_view< T >& dst_image, \
                                    const double dx, \
                                    const double dy, \
                                    const double dz)


#endif // vil3d_resample_simple_hxx_
