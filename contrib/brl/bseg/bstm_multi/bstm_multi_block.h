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
// \author: Raphael Kargon
// \date: Aug 01, 2017

#include <vbl/vbl_ref_count.h>
#include <vnl/vnl_vector_fixed.h>

#include <bstm/basic/bstm_block_id.h>
#include <bstm_multi/bstm_multi_block_metadata.h>

class bstm_multi_block : public vbl_ref_count {
public:
  typedef bstm_block_id id_t;
  typedef bstm_multi_block_metadata metadata_t;

public:
  //: constructs block from given data.
  bstm_multi_block(const metadata_t &data,
                   const vcl_vector<unsigned char *> &buffers)
      : metadata_(data), buffers_(buffers), read_only_(false) {}

  //: creates empty block from metadata
  bstm_multi_block(const metadata_t &data);

  //: default destructor
  virtual ~bstm_multi_block() {
    for (vcl_vector<unsigned char *>::iterator iter = buffers_.begin();
         iter != buffers_.end();
         ++iter) {
      delete[](*iter);
    }
  }

  //: accessors
  id_t &block_id() { return metadata_.id_; }
  const metadata_t &metadata() const { return metadata_; }
  vcl_vector<unsigned char *> buffers() { return buffers_; }
  // Sets this block's buffer for a given level.
  unsigned char *get_buffer(std::size_t level) { return buffers_[level]; }
  void set_buffer(unsigned char *new_buffer, std::size_t level) {
    unsigned char *old_buffer = buffers_[level];
    buffers_[level] = new_buffer;
    if (old_buffer != new_buffer) {
      delete[] old_buffer;
    }
  }
  // TODO byte count
  // long byte_count() const { return byte_count_; }
  // TODO num_cells -- calculate the total number of leaf cells in this block

  //: mutators
  void set_block_id(id_t id) { metadata_.id_ = id; }
  void set_byte_count(long bc) { byte_count_ = bc; }

  //: regardless of the way the instance is constructed, enable write
  void enable_write() { read_only_ = false; }
  bool read_only() const { return read_only_; }

private:
  //: Block metadata
  metadata_t metadata_;

  //: Buffers for trees of each level. Note that buffers_[0] correspond to the
  // *top* (root) level, while buffers_[buffers_.size()] is the bottom, "leaf"
  // level.
  vcl_vector<unsigned char *> buffers_;

  //: number of bytes this block takes up (on disk and ram)
  long byte_count_;

  //: Whether this block is read-only
  bool read_only_;
};

#endif // bstm_multi_block_h_
