// This is core/vil/vil_copy.cxx
#include <algorithm>
#include "vil_copy.h"
//:
// \file
// \author Ian Scott, ISBE, Manchester
// \date   21 Aug 2003

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_property.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_blocked_image_resource.h>
#include <vil/vil_new.h>

//: Copy images in blocks of roughly this size
static const unsigned long large_image_limit_ = 1024ul * 1024ul * 8ul; //8M Pixels

//If image resource is blocked then it makes sense to use the
//blocks to copy image data. src and det are known to be blocked with
//equal blocking parameters
static bool copy_resource_by_blocks(const vil_image_resource_sptr& src,
                                    vil_image_resource_sptr& det)
{
  //cast to blocked image resource
  vil_blocked_image_resource_sptr bsrc = blocked_image_resource(src);
  vil_blocked_image_resource_sptr bdet = blocked_image_resource(det);
  for (unsigned bi = 0; bi<bsrc->n_block_i(); ++bi)
    for (unsigned bj = 0; bj<bsrc->n_block_j(); ++bj)
    {
      vil_image_view_base_sptr blk = bsrc->get_block(bi, bj);
      if (!blk) return false;
      if (!bdet->put_block(bi, bj, *blk)) return false;
    }
  return true;
}

//: Copy src to dest.
// This is useful if you want to copy on image into a window on another image.
// src and dest must have identical sizes, and pixel-types. Returns false if the copy
// failed.
//  O(size).
// \relatesalso vil_image_resource
bool vil_copy_deep(const vil_image_resource_sptr &src, vil_image_resource_sptr &dest)
{
  if (dest->ni() != src->ni() || dest->nj() != src->nj() ||
      dest->nplanes() != src->nplanes() || dest->pixel_format() != src->pixel_format() )
    return false;

  if (src->ni() == 0 || src->nj() == 0 || src->nplanes() == 0) return true;

  //Check for a blocked resource.  Copying will be more
  //efficient in blocks,  unless a block is too large
  unsigned src_sbi=0, src_sbj=0, dest_sbi=0, dest_sbj=0;

  src->get_property(vil_property_size_block_i, &src_sbi);
  src->get_property(vil_property_size_block_j, &src_sbj);
  dest->get_property(vil_property_size_block_i, &dest_sbi);
  dest->get_property(vil_property_size_block_j, &dest_sbj);
  //If the source or destination is blocked then use that structure
  //to copy images
  if (src_sbi>0&&src_sbj>0&&src_sbi==dest_sbi&&src_sbj==dest_sbj)
    return copy_resource_by_blocks(src, dest);

  if (src->ni() * src->nj() * src->nplanes() < large_image_limit_)
  {
    vil_image_view_base_sptr view_ref = src->get_view();
    if (!view_ref) return false;
    return dest->put_view(*view_ref);
  }
  else
  {
    unsigned got_to_line =0;
    unsigned block_size = std::max(static_cast<unsigned>(large_image_limit_ / src->ni()),1u);

    while (got_to_line < src->nj())
    {
      vil_image_view_base_sptr view_ref = src->get_view(0, src->ni(), got_to_line,
                                                        std::min(block_size, src->nj()-got_to_line));
      if (!view_ref) return false;
      if (!dest->put_view(*view_ref,0,got_to_line)) return false;
      got_to_line += block_size;
    }
    return true;
  }
}

vil_image_resource_sptr vil_copy_deep( const vil_image_resource_sptr &src )
{
  if(src == nullptr) return nullptr;
  vil_image_resource_sptr result = vil_new_image_resource(src->ni(), src->nj(), src);
  bool copy_r = vil_copy_deep(src, result);
  if(!copy_r)
  {
    return nullptr;
  }
  return result;
}
