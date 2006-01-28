// This is core/vil/vil_cached_image_resource.h
#ifndef vil_cached_image_resource_h_
#define vil_cached_image_resource_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file  
// \brief A cached and blocked representation of the image_resource
// \author J. L. Mundy

#include <vcl_vector.h>
#include <vil/vil_blocked_image_resource.h>
#include <vil/vil_block_cache.h>
class vil_cached_image_resource : public vil_blocked_image_resource
{ 
 public:

  vil_cached_image_resource(vil_blocked_image_resource_sptr bir, 
                            const unsigned cache_size): 
    bir_(bir), cache_(vil_block_cache(cache_size)){}

  virtual ~vil_cached_image_resource(){}

 inline virtual unsigned nplanes() const
    {return bir_->nplanes();}
 inline virtual unsigned ni() const 
    {return bir_->ni();}
 inline virtual unsigned nj() const
  {return bir_->nj();}
  //: Block size in columns
 inline virtual unsigned size_block_i() const
    {return bir_->size_block_i();}
  //: Block size in rows
 inline virtual unsigned size_block_j() const
    {return bir_->size_block_j();}
  //: Number of blocks in image width
 inline virtual unsigned n_block_i() const
    {return bir_->n_block_i();}
  //: Number of blocks in image height
 inline virtual unsigned n_block_j() const
    {return bir_->n_block_i();}

 inline virtual enum vil_pixel_format pixel_format() const
    {return bir_->pixel_format();}

 inline virtual bool put_view(const vil_image_view_base& im, unsigned i0, unsigned j0) {return bir_->put_view(im, i0, j0);}

  //: Block access
  virtual vil_image_view_base_sptr get_block( unsigned  block_index_i,
                                              unsigned  block_index_j ) const;

  //: put the block into the resource at the indicated location  
  virtual bool put_block(unsigned  block_index_i,
                         unsigned  block_index_j,
                         const vil_image_view_base& view)
    {return bir_->put_block(block_index_i, block_index_j, view);}


  //: Extra property information
 inline virtual bool get_property(char const* tag, void* property_value = 0) const
    {return bir_->get_property(tag, property_value);}

 protected:
  vil_blocked_image_resource_sptr bir_;
  vil_block_cache cache_;
};

#endif // vil_cached_image_resource_h_
