// This is core/vil/vil_blocked_image_facade.h
#ifndef vil_blocked_image_facade_h_
#define vil_blocked_image_facade_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A blocked image facade for any image resource
// \author J. L. Mundy
// 
// this class "wraps" any image resource and provides blocking methods
//
#include <vcl_vector.h>
#include <vil/vil_blocked_image_resource.h>

class vil_blocked_image_facade : public vil_blocked_image_resource
{
  
 public:

  vil_blocked_image_facade(const vil_image_resource_sptr &src,
                           const unsigned sbi=0, const unsigned sbj=0);
  virtual ~vil_blocked_image_facade(){}

 inline virtual unsigned nplanes() const
    {return src_->nplanes();}
 inline virtual unsigned ni() const
    {return src_->ni();}
 inline virtual unsigned nj() const
  {return src_->nj();}
  //: Block size in columns
 inline virtual unsigned size_block_i() const
    {return sbi_;}
  //: Block size in rows
 inline virtual unsigned size_block_j() const
    {return sbj_;}
  //: Number of blocks in image width
 inline virtual unsigned n_block_i() const
    {if(sbi_>0)return (src_->ni()+sbi_-1)/sbi_; return 0;}
  //: Number of blocks in image height
 inline virtual unsigned n_block_j() const
    {if(sbj_>0)return (src_->nj()+sbj_-1)/sbj_; return 0;}

 inline virtual enum vil_pixel_format pixel_format() const
    {return src_->pixel_format();}

 inline virtual vil_image_view_base_sptr 
    get_copy_view(unsigned i0, unsigned n_i, unsigned j0, unsigned n_j) const
      {return src_->get_copy_view(i0, n_i, j0, n_j);}

 inline virtual bool put_view(const vil_image_view_base& im,
                        unsigned i0, unsigned j0)
    { return src_->put_view(im, i0, j0);}

  //: Block access
  virtual vil_image_view_base_sptr get_block( unsigned  block_index_i,
                                              unsigned  block_index_j ) const;

  //: put the block into the resource at the indicated location  
  virtual bool put_block(unsigned  block_index_i,
                         unsigned  block_index_j,
                         const vil_image_view_base& view);


  //: Extra property information
  virtual bool get_property(char const* tag, void* property_value = 0) const;

 protected:
  //Internal functions
  vil_image_view_base_sptr fill_block(unsigned i0, unsigned icrop,
                                      unsigned j0, unsigned jcrop,
                                      vil_image_view_base_sptr& view) const;
    
  //:the image resource masquerading as a blocked resource
  vil_image_resource_sptr src_;
  //:block size in i
  unsigned sbi_;
  //:block size in j
  unsigned sbj_;
  vil_blocked_image_facade();//not meaningful
};

#endif // vil_blocked_image_facade_h_
