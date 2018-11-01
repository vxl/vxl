// This is core/vil/vil_blocked_image_facade.h
#ifndef vil_blocked_image_facade_h_
#define vil_blocked_image_facade_h_
//:
// \file
// \brief A blocked image facade for any image resource
// \author J. L. Mundy
//
// this class "wraps" any image resource and provides blocking methods
//
// not used? #include <vcl_compiler.h>
#include <vector>
#include <vil/vil_blocked_image_resource.h>

class vil_blocked_image_facade : public vil_blocked_image_resource
{
 public:
  vil_blocked_image_facade(const vil_image_resource_sptr &src,
                           const unsigned sbi=0, const unsigned sbj=0);
  ~vil_blocked_image_facade() override = default;

  inline unsigned nplanes() const override
  { return src_->nplanes();}
  inline unsigned ni() const override
  { return src_->ni();}
  inline unsigned nj() const override
  { return src_->nj();}
  //: Block size in columns
  inline unsigned size_block_i() const override
  { return sbi_;}
  //: Block size in rows
  inline unsigned size_block_j() const override
  { return sbj_;}
  //: Number of blocks in image width
  inline unsigned n_block_i() const override
  { if (sbi_>0)return (src_->ni()+sbi_-1)/sbi_; return 0; }
  //: Number of blocks in image height
  inline unsigned n_block_j() const override
  { if (sbj_>0)return (src_->nj()+sbj_-1)/sbj_; return 0; }

  inline enum vil_pixel_format pixel_format() const override
  { return src_->pixel_format();}

  inline vil_image_view_base_sptr
  get_copy_view(unsigned i0, unsigned n_i, unsigned j0, unsigned n_j) const override
  { return src_->get_copy_view(i0, n_i, j0, n_j);}

  inline bool put_view(const vil_image_view_base& im,
                               unsigned i0, unsigned j0) override
  { return src_->put_view(im, i0, j0);}

  //: Block access
  vil_image_view_base_sptr get_block( unsigned  block_index_i,
                                              unsigned  block_index_j ) const override;

  //: put the block into the resource at the indicated location
  bool put_block(unsigned  block_index_i,
                         unsigned  block_index_j,
                         const vil_image_view_base& view) override;


  //: Extra property information
  bool get_property(char const* tag, void* property_value = nullptr) const override;

 protected:
  //Internal functions
  vil_image_view_base_sptr fill_block(vil_image_view_base_sptr& view) const;

  //:the image resource masquerading as a blocked resource
  vil_image_resource_sptr src_;
  //:block size in i
  unsigned sbi_;
  //:block size in j
  unsigned sbj_;
  vil_blocked_image_facade();//not meaningful
};

#endif // vil_blocked_image_facade_h_
