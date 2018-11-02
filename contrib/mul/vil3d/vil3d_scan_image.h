// This is mul/vil3d/vil3d_scan_image.h
#ifndef vil3d_scan_image_h_
#define vil3d_scan_image_h_
//:
// \file
// \brief Read all voxels in an image (or pair) and feed into a functor
// \author Tim Cootes

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil3d/vil3d_image_view.h>

//: Call f(v) for every voxel in the image
template <class T, class Fn >
inline void vil3d_scan_image(const vil3d_image_view<T >& im, Fn& f)
{
  const T* plane = im.origin_ptr();

  unsigned ni = im.ni();
  unsigned nj = im.nj();
  unsigned nk = im.nk();
  unsigned np = im.nplanes();
  std::ptrdiff_t istep = im.istep(), jstep=im.jstep(), kstep=im.kstep();

  for (unsigned int p=0;p<np;++p, plane += im.planestep())
  {
    const T* slice = plane;
    for (unsigned int k=0;k<nk;++k, slice += kstep)
    {
      const T* row = slice;
      const T* row_end = row + nj*jstep;
      for (;row!=row_end;row += jstep)
      {
        const T* pixel = row;
        const T* pixel_end = row + ni*istep;
        for (;pixel!=pixel_end;pixel+=istep) f(*pixel);
      }
    }
  }
}

//: Call f(im1(i,j,k,p),im2(i,j,k,p)) for every voxel in the images
template <class T1, class T2, class Fn >
inline void vil3d_scan_image(const vil3d_image_view<T1 >& im1,
                             const vil3d_image_view<T2 >& im2, Fn& f)
{
  unsigned ni = im1.ni();
  unsigned nj = im1.nj();
  unsigned nk = im1.nk();
  unsigned np = im1.nplanes();
  assert(im1.ni()==ni && im2.nj()==nj && im2.nk()==nk && im2.nplanes()==np);

  const T1* plane1 = im1.origin_ptr();
  const T2* plane2 = im2.origin_ptr();

  std::ptrdiff_t istep1 = im1.istep(), jstep1=im1.jstep(), kstep1=im1.kstep();
  std::ptrdiff_t pstep1=im1.planestep();
  std::ptrdiff_t istep2 = im2.istep(), jstep2=im2.jstep(), kstep2=im2.kstep();
  std::ptrdiff_t pstep2=im2.planestep();

  for (unsigned int p=0;p<np;++p, plane1 += pstep1,plane2 += pstep2)
  {
    const T1* slice1 = plane1;
    const T2* slice2 = plane2;
    for (unsigned int k=0;k<nk;++k, slice1 += kstep1, slice2 += kstep2)
    {
      const T1* row1 = slice1;
      const T1* row1_end = row1 + nj*jstep1;
      const T2* row2 = slice2;
      for (;row1!=row1_end;row1 += jstep1,row2 += jstep2)
      {
        const T1* pixel1 = row1;
        const T1* pixel1_end = row1 + ni*istep1;
        const T2* pixel2 = row2;
        for (;pixel1!=pixel1_end;pixel1+=istep1,pixel2+=istep2)
          f(*pixel1,*pixel2);
      }
    }
  }

}

#endif
