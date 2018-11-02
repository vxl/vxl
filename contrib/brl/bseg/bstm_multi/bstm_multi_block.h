#ifndef bstm_multi_block_h_
#define bstm_multi_block_h_

//:
// \file bstm_multi_block.h
// \brief A block in a space time scene that represents a region in
// space-time through several heirarchical levels of BSTM space/time
// trees. Space/time divisions can be arranged in any order, and are specified
// by metadata_.subdivisions_. There is only one tree at the root level, so the
// individual leaf voxel size is specified by the bounding box and subdivision
// order, stored in the block's metadata.
//
// Leaf nodes in trees at each level point to trees at the next level,
// etc. The buffers for each level of tree are stored in buffers_ and
// are owned by this class. Leaf nodes at the bottom level point into
// a data buffer that is maintained separately from this class.
//
// At the top level, trees are arranged in row-major order in a 4D
// grid. (Currently, only one tree is allowed at the top level, but
// this will be easy to change.) At lower levels, trees with the same
// parent are stored contiguously.
//
// To iterate over the children of a space tree: use get_leaf_bits(),
// but only consider indices greater than 72. This corresponds to
// voxels at the lowest level of the bit tree. Leaf nodes at higher
// levels indicate regions of empty space.
//
// To iterate over the children of a time tree: use get_leaf_bits().
//
// \author: Raphael Kargon
// \date: Aug 01, 2017

#include <iostream>
#include <cstddef>
#include <vector>
#include <utility>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.hxx>
#include <vnl/vnl_vector_fixed.h>

#include <bstm/basic/bstm_block_id.h>
#include <bstm_multi/bstm_multi_block_metadata.h>

class bstm_multi_block : public vbl_ref_count {
public:
  typedef bstm_block_id id_t;
  typedef bstm_multi_block_metadata metadata_t;

public:
  //: constructs block from given data.
  // TODO this copies the buffers, might not be what we want
  bstm_multi_block(const metadata_t &data,
                   std::vector<std::vector<unsigned char> > buffers)
      : metadata_(data), buffers_(std::move(buffers)), read_only_(false) {}

  //: creates empty block from metadata
  bstm_multi_block(const metadata_t &data);

  // accessors
  id_t &block_id() { return metadata_.id_; }
  const metadata_t &metadata() const { return metadata_; }
  std::vector<std::vector<unsigned char> > &buffers() { return buffers_; }
  const std::vector<std::vector<unsigned char> > &buffers() const {
    return buffers_;
  }
  std::vector<unsigned char> &get_buffer(int level) { return buffers_[level]; }
  const std::vector<unsigned char> &get_buffer(int level) const {
    return buffers_[level];
  }
  // Returns underlying buffer data
  unsigned char *get_data(int level) { return &buffers_[level].front(); }
  const unsigned char *get_data(int level) const {
    return &buffers_[level].front();
  }

  // Typed data accessors - return data buffer with appropriate type, or throw
  // an error
  space_tree_b *get_space_data(int level);
  time_tree_b *get_time_data(int level);

  // nothrow versions of typed accessors -- return null if wrong type
  space_tree_b *get_space_data(int level, std::nothrow_t);
  time_tree_b *get_time_data(int level, std::nothrow_t);

  //: returns number of bytes taken up by this block
  std::size_t byte_count() const;

  space_time_enum level_type(int level) const {
    return metadata_.subdivisions_[level];
  }

  // TODO num_cells -- calculate the total number of leaf cells in this
  // block

  //: mutators
  void set_block_id(id_t id) { metadata_.id_ = id; }

  // Sets the buffer for the given level to the contents of new_buffer, and sets
  // new_buffer to own the old buffer contents.
  void set_buffer(std::vector<unsigned char> &new_buffer, int level) {
    buffers_[level].swap(new_buffer);
  }

  // creates new zero-initialized buffer
  void new_buffer(std::size_t new_size, int level) {
    std::vector<unsigned char> new_vec(new_size, 0);
    buffers_[level].swap(new_vec);
  }

  //: regardless of the way the instance is constructed, enable write
  void enable_write() { read_only_ = false; }
  bool read_only() const { return read_only_; }

private:
  //: Block metadata
  metadata_t metadata_;

  //: Buffers for trees of each level. Note that buffers_[0]
  // correspond to the *top* (root) level, while
  // buffers_[buffers_.size()] is the bottom, "leaf" level.
  std::vector<std::vector<unsigned char> > buffers_;

  //: Whether this block is read-only
  bool read_only_;
};

typedef vbl_smart_ptr<bstm_multi_block> bstm_multi_block_sptr;

#endif // bstm_multi_block_h_
