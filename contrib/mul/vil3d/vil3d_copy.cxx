// This is mul/vil3d/vil3d_copy.cxx

//:
// \file
// \author Ian Scott, ISBE, Manchester
// \date   5 Oct 2004

#include "vil3d_copy.h"
#include <vcl_algorithm.h>
#include <vil3d/vil3d_image_resource.h>
#include <vil3d/vil3d_property.h>

#if 0
//: Copy images in blocks of roughly this size
static const unsigned long large_image_limit = 1024ul * 1024ul * 16ul; //16M Pixels
#endif


//: Copy src to dest.
// This is useful if you want to copy on image into a window on another image.
// src and dest must have identical sizes, and pixel-types. Returns false if the copy
// failed.
//  O(size).
// \relates vil3d_image_resource
bool vil3d_copy_deep(const vil3d_image_resource_sptr &src, vil3d_image_resource_sptr &dest)
{
  if (dest->ni() != src->ni() || dest->nj() != src->nj() || dest->nk() != src->nk() ||
      dest->nplanes() != src->nplanes() || dest->pixel_format() != src->pixel_format() )
    return false;

  if (src->ni() == 0 || src->nj() == 0 || src->nk() == 0 || src->nplanes() == 0) return true;

  float sizes[3];
  if (dest->get_property(vil3d_property_voxel_size, sizes))
  {
    dest->set_voxel_size(sizes[0], sizes[1], sizes[2]);
  }

  vil3d_image_view_base_sptr view_ref = src->get_view();
  if (!view_ref) return false;
  return dest->put_view(*view_ref);

#if 0
  const unsigned long large_image_limit = 1024ul * 1024ul * 16ul; //16M Pixels
  if (src->ni() * src->nj() * src->nk() * src->nplanes() >= large_image_limit)
  {
    unsigned got_to_slice =0;
    unsigned block_size = vcl_max(static_cast<unsigned>(large_image_limit / src->ni()),1u);

    while (got_to_line < src->nj())
    {
      view_ref = src->get_view(0, src->ni(), got_to_slice, vcl_min(got_to_line+block_size, src->nj()));
      if (!view_ref) return false;
      if (!dest->put_view(*view_ref,0,got_to_line)) return false;
      got_to_line += block_size;
    }
    return true;
  }
#endif
}

