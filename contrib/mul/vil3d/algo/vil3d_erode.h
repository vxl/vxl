#ifndef vil3d_erode_h_
#define vil3d_erode_h_
//:
// \file
// \brief Perform greyscale erosion on 3D images
// \author Tim Cootes Ian Scott

#include <vil3d/vil3d_image_view.h>
#include <vil3d/vil3d_crop.h>
#include <vil3d/algo/vil3d_structuring_element.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Return minimum of im[offset[k]], k=0..n;
template <class T>
inline T vil3d_erode(const T* im, const std::ptrdiff_t* offset, unsigned n)
{
  T min_v = im[offset[0]];
  for (unsigned i=1;i<n;++i) if (im[offset[i]] < min_v) min_v=im[offset[i]];
  return min_v;
}

//: Erodes src_image to produce dest_image (assumed single plane)
// \relatesalso vil3d_image_view
// \relatesalso vil3d_structuring_element
template <class T>
void vil3d_erode(const vil3d_image_view<T>& src_image,
                 vil3d_image_view<T>& dest_image,
                 const vil3d_structuring_element& element)
{
  assert(src_image.nplanes()==1);
  unsigned ni = src_image.ni(); assert(ni>0);
  unsigned nj = src_image.nj(); assert(nj>0);
  unsigned nk = src_image.nk(); assert(nk>0);
  dest_image.set_size(ni,nj,nk,1);

  std::ptrdiff_t s_istep = src_image.istep(),  s_jstep = src_image.jstep();
  std::ptrdiff_t s_kstep = src_image.kstep();
  std::ptrdiff_t d_istep = dest_image.istep();

  std::vector<std::ptrdiff_t> offset;
  vil3d_compute_offsets(offset,element,s_istep,s_jstep,s_kstep);

  // Define box in which all element will be valid
  int ilo = -element.min_i();
  int ihi = ni-1-element.max_i();
  int jlo = -element.min_j();
  int jhi = nj-1-element.max_j();
  int klo = -element.min_k();
  int khi = nk-1-element.max_k();

  // Zero low and high i sides
  if (ilo>0)    vil3d_crop(dest_image,0,ilo,0,nj,0,nk).fill(0);
  if (ihi<int(ni)-1) vil3d_crop(dest_image,ihi+1,ni-ihi-1,0,nj,0,nk).fill(0);

  // Zero low and high j sides
  if (jlo>0)    vil3d_crop(dest_image,0,ni,0,jlo,0,nk).fill(0);
  if (jhi<int(nj)-1) vil3d_crop(dest_image,0,ni,jhi+1,nj-jhi-1,0,nk).fill(0);

  // Zero low and high k sides
  if (klo>0)    vil3d_crop(dest_image,0,ni,0,nj,0,klo).fill(0);
  if (khi<int(nk)-1) vil3d_crop(dest_image,0,ni,0,nj,khi+1,nk-khi-1).fill(0);

  for (int k=klo;k<=khi;++k)
    for (int j=jlo;j<=jhi;++j)
    {
      const T* src_p = &src_image(ilo,j,k);
      T* dest_p = &dest_image(ilo,j,k);

      for (int i=ilo;i<=ihi;++i,src_p+=s_istep,dest_p+=d_istep)
        *dest_p=vil3d_erode(src_p,&offset[0],offset.size());
    }
}

#endif // vil3d_erode_h_
