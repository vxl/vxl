//:
// \file
// \brief Perform binary dilation on 3D images
// \author Tim Cootes

#include "vil3d_binary_dilate.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Dilates src_image to produce dest_image (assumed single plane)
void vil3d_binary_dilate(const vil3d_image_view<bool>& src_image,
                         vil3d_image_view<bool>& dest_image,
                         const vil3d_structuring_element& element)
{
  assert(src_image.nplanes()==1);
  unsigned ni = src_image.ni();
  unsigned nj = src_image.nj();
  unsigned nk = src_image.nk();
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

  // Deal with low i side
  for (int i=0;i<ilo;++i)
    for (unsigned int j=0;j<nj;++j)
      for (unsigned int k=0;k<nk;++k)
        dest_image(i,j,k,0)=vil3d_binary_dilate(src_image,0,element,i,j,k);
  // Deal with high i side
  for (unsigned int i=ihi+1;i<ni;++i)
    for (unsigned int j=0;j<nj;++j)
      for (unsigned int k=0;k<nk;++k)
        dest_image(i,j,k,0)=vil3d_binary_dilate(src_image,0,element,i,j,k);
  // Deal with low j side
  for (int i=ilo;i<=ihi;++i)
    for (int j=0;j<jlo;++j)
      for (unsigned int k=0;k<nk;++k)
        dest_image(i,j,k,0)=vil3d_binary_dilate(src_image,0,element,i,j,k);
  // Deal with high j side
  for (int i=ilo;i<=ihi;++i)
    for (unsigned int j=jhi+1;j<nj;++j)
      for (unsigned int k=0;k<nk;++k)
        dest_image(i,j,k,0)=vil3d_binary_dilate(src_image,0,element,i,j,k);
  // Deal with low k side
  for (int k=0;k<klo;++k)
    for (int i=ilo;i<=ihi;++i)
      for (int j=jlo;j<=jhi;++j)
        dest_image(i,j,k,0)=vil3d_binary_dilate(src_image,0,element,i,j,k);
  // Deal with high k side
  for (unsigned int k=khi+1;k<nk;++k)
    for (int i=ilo;i<=ihi;++i)
      for (int j=jlo;j<=jhi;++j)
        dest_image(i,j,k,0)=vil3d_binary_dilate(src_image,0,element,i,j,k);

  for (int k=klo;k<=khi;++k)
    for (int j=jlo;j<=jhi;++j)
    {
      const bool* src_p = &src_image(ilo,j,k);
      bool* dest_p = &dest_image(ilo,j,k);

      for (int i=ilo;i<=ihi;++i,src_p+=s_istep,dest_p+=d_istep)
        *dest_p=vil3d_binary_dilate(src_p,&offset[0],offset.size());
    }
}
