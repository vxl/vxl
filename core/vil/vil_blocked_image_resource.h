// This is core/vil/vil_blocked_image_resource.h
#ifndef vil_blocked_image_resource_h_
#define vil_blocked_image_resource_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A blocked representation of the image_resource
// \author J. L. Mundy
#include <vcl_vector.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_blocked_image_resource_sptr.h>

//:cast to blocked resource if possible
vil_blocked_image_resource_sptr 
blocked_image_resource(const vil_image_resource_sptr& ir);

class vil_blocked_image_resource : public vil_image_resource
{
  
 public:

  vil_blocked_image_resource();
  virtual ~vil_blocked_image_resource();

  virtual unsigned nplanes() const =0;
  virtual unsigned ni() const = 0;
  virtual unsigned nj() const = 0;
  
  //: Block size in columns
  virtual unsigned size_block_i() const = 0;

  //: Block size in rows
  virtual unsigned size_block_j() const = 0;

  //: Number of blocks in image width
  virtual unsigned n_block_i() const;

  //: Number of blocks in image height
  virtual unsigned n_block_j() const;
  

  virtual enum vil_pixel_format pixel_format() const = 0;

  virtual vil_image_view_base_sptr 
    get_copy_view(unsigned i0, unsigned n_i, unsigned j0, unsigned n_j) const;

  virtual bool put_view(const vil_image_view_base& im, unsigned i0, unsigned j0) = 0;

  //: Block access
  virtual vil_image_view_base_sptr get_block( unsigned  block_index_i,
                                              unsigned  block_index_j ) const = 0;


  //: the multiple blocks are in col row order, i.e. blocks[i][j]
  virtual bool get_blocks( unsigned start_block_i, unsigned end_block_i,
                           unsigned  start_block_j, unsigned end_block_j,
                           vcl_vector< vcl_vector< vil_image_view_base_sptr > >& blocks ) const;
  
  //: put the block into the resource at the indicated location  
  virtual bool put_block(unsigned  block_index_i,
                         unsigned  block_index_j,
                         const vil_image_view_base& view) = 0;


  //: put multiple blocks in raster order, i.e.,  blocks[i][j]
  virtual bool put_blocks( unsigned start_block_i, unsigned end_block_i,
                           unsigned  start_block_j, unsigned end_block_j,
                           vcl_vector< vcl_vector< vil_image_view_base_sptr > > const& blocks );

  //: Extra property information
  virtual bool get_property(char const* tag, void* property_value = 0) const = 0;

 protected:
  //Internal functions
  vil_image_view_base_sptr fill_block(unsigned i0, unsigned icrop,
                                      unsigned j0, unsigned jcrop,
                                      vil_image_view_base_sptr& view) const;
    
  //: Get the offset from the start of the block row for pixel position i
  bool block_i_offset(unsigned block_i, unsigned i,
                      unsigned& i_offset) const;

  //: Get the offset from the start of the block column for pixel position j
  bool block_j_offset(unsigned block_j, unsigned j,
                      unsigned& j_offset) const;
  
  bool trim_border_blocks(unsigned i0, unsigned ni,
                          unsigned j0, unsigned nj,
                          unsigned start_block_i,
                          unsigned start_block_j,
                          vcl_vector< vcl_vector< vil_image_view_base_sptr > >& blocks) const;

  vil_image_view_base_sptr  
    glue_blocks_together(const vcl_vector< vcl_vector< vil_image_view_base_sptr > >& blocks) const;

 protected:
  friend class vil_smart_ptr<vil_blocked_image_resource>;
};

#endif // vil_blocked_image_resource_h_
