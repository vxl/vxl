// This is core/vil/vil_cached_image_resource.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
#include <vil/vil_property.h>
#include <vcl_cstring.h>
#include <vil/vil_image_view.h>
#include <vil/vil_crop.h>
#include <vil/vil_copy.h>
#include <vil/vil_cached_image_resource.h>


//Get a view that is the size of a block.
//Uses the cache to retrieve frequently used blocks
vil_image_view_base_sptr 
vil_cached_image_resource::get_block( unsigned  block_index_i,
                                      unsigned  block_index_j ) const
{
  //check if the block is already in the buffer
   vil_image_view_base_sptr blk;
  if(cache_.get_block(block_index_i, block_index_j, blk))
    return blk;
  //no - so get the block from the resource
  blk = bir_->get_block(block_index_i, block_index_j);
  if(!blk)
    return blk;//get block failed
    //put the block in the cache (cast away const since we are just caching)
  vil_cached_image_resource* non_const = (vil_cached_image_resource*)this;
  non_const->cache_.add_block(block_index_i, block_index_j, blk);
  return blk;
}


