// This is core/vil/vil_copy.cxx

//:
// \file
// \author Ian Scott, ISBE, Manchester
// \date   21 Aug 2003

#include "vil_copy.h"
#include <vcl_algorithm.h>
#include <vil/vil_image_resource.h>

//: Copy images in blocks of roughly this size
static const unsigned long large_image_limit = 1024ul * 1024ul * 8ul; //8M Pixels

//: Copy src to dest.
// This is useful if you want to copy on image into a window on another image.
// src and dest must have identical sizes, and pixel-types. Returns false if the copy
// failed.
//  O(size).
// \relates vil_image_resource
bool vil_copy_deep(const vil_image_resource_sptr &src, vil_image_resource_sptr &dest)
{
  if (dest->ni() != src->ni() || dest->nj() != src->nj() ||
      dest->nplanes() != src->nplanes() || dest->pixel_format() != src->pixel_format() )
    return false;

  if (src->ni() == 0 || src->nj() == 0 || src->nplanes() == 0) return true;

  if (src->ni() * src->nj() * src->nplanes() < large_image_limit)
  {
    vil_image_view_base_sptr view_ref = src->get_view();
    if (!view_ref) return false;
    return dest->put_view(*view_ref);
  }
  else
  {
    unsigned got_to_line =0;
    unsigned block_size = vcl_max(static_cast<unsigned>(large_image_limit / src->ni()),1u);

    while (got_to_line < src->nj())
    {
      vil_image_view_base_sptr view_ref = src->get_view(0, src->ni(), got_to_line,
                                                        vcl_min(got_to_line+block_size, src->nj()));
      if (!view_ref) return false;
      if (!dest->put_view(*view_ref,0,got_to_line)) return false;
      got_to_line += block_size;
    }
    return true;
  }
}

