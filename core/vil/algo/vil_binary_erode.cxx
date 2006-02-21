//:
// \file
// \brief Perform binary erosion on images
// \author Tim Cootes

#include "vil_binary_erode.h"
#include <vcl_cassert.h>
#include <vil/vil_fill.h>

//: Erodes src_image to produce dest_image (assumed single plane)
void vil_binary_erode(const vil_image_view<bool>& src_image,
                      vil_image_view<bool>& dest_image,
                      const vil_structuring_element& element)
{
  assert(src_image.nplanes()==1);
  unsigned ni = src_image.ni();
  unsigned nj = src_image.nj();
  dest_image.set_size(ni,nj,1);

  vcl_ptrdiff_t s_istep = src_image.istep(),  s_jstep = src_image.jstep();
  vcl_ptrdiff_t d_istep = dest_image.istep(), d_jstep = dest_image.jstep();

  const bool* src_row0 = src_image.top_left_ptr();
  bool* dest_row0 = dest_image.top_left_ptr();

  vcl_vector<vcl_ptrdiff_t> offset;
  vil_compute_offsets(offset,element,s_istep,s_jstep);

  // Define box in which all elements will be valid
  int ilo = -element.min_i();
  int ihi = ni-1-element.max_i();
  int jlo = -element.min_j();
  int jhi = nj-1-element.max_j();

  // Deal with left edge
  for (int i=0;i<ilo;++i)
    for (unsigned int j=0;j<nj;++j)
      dest_image(i,j,0)=vil_binary_erode(src_image,0,element,i,j);
  // Deal with right edge
  for (unsigned int i=ihi+1;i<ni;++i)
    for (unsigned int j=0;j<nj;++j)
      dest_image(i,j,0)=vil_binary_erode(src_image,0,element,i,j);
  // Deal with bottom edge
  for (int i=ilo;i<=ihi;++i)
    for (int j=0;j<jlo;++j)
      dest_image(i,j,0)=vil_binary_erode(src_image,0,element,i,j);
  // Deal with top edge
  for (int i=ilo;i<=ihi;++i)
    for (unsigned int j=jhi+1;j<nj;++j)
      dest_image(i,j,0)=vil_binary_erode(src_image,0,element,i,j);

  for (int j=jlo;j<=jhi;++j)
  {
    const bool* src_p = src_row0 + j*s_jstep + ilo*s_istep;
    bool* dest_p = dest_row0 + j*d_jstep + ilo * d_istep;

    for (int i=ilo;i<=ihi;++i,src_p+=s_istep,dest_p+=d_istep)
    {
      *dest_p=vil_binary_erode(src_p,&offset[0],offset.size());
    }
  }
}
