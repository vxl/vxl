// This is core/vil/file_formats/vil_nitf_blocking_info.h
#ifndef vil_nitf_blocking_info_h_
#define vil_nitf_blocking_info_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Blocking information for an NITF file.
//  Concept of blocking taken from TargetJr base class ImageTemplate and extracted into this class.
// \author    laymon@crd.ge.com
// \date 2003/12/26 00:19:34
class vil_nitf_blocking_info
{
  unsigned num_blocks_x_; //!< number of blocks in X direction
  unsigned num_blocks_y_; //!< number of blocks in Y direction

  unsigned block_size_x_; //!< block size in X direction
  unsigned block_size_y_; //!< block size in Y direction

 public:
  vil_nitf_blocking_info()
    : num_blocks_x_(0), num_blocks_y_(0), block_size_x_(0) , block_size_y_(0) {}

  unsigned int get_block_size_x() const { return block_size_x_; }
  void set_block_size_x(unsigned int new_val) { block_size_x_ = new_val; }

  unsigned int get_block_size_y() const { return block_size_y_; }
  void set_block_size_y(unsigned int new_val) { block_size_y_ = new_val; }

  unsigned int get_num_blocks_x() const { return num_blocks_x_; }
  void set_num_blocks_x(unsigned int new_val) { num_blocks_x_ = new_val; }

  unsigned int get_num_blocks_y() const { return num_blocks_y_; }
  void set_num_blocks_y(unsigned int new_val) { num_blocks_y_ = new_val; }
};

#endif
