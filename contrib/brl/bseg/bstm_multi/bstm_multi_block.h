#ifndef bstm_multi_block_h_
#define bstm_multi_block_h_

//:
// \file bstm_multi_block.h
// \brief A block in a space time scene that represents a region in
// space-time through several heirarchical levels of BSTM space-time
// trees.

#include <vbl/vbl_ref_count.h>
#include <vnl/vnl_vector_fixed.h>

#include <bstm/basic/bstm_block_id.h>
#include <bstm_multi/bstm_multi_block_metadata.h>

class bstm_multi_block : public vbl_ref_count {
public:
  typedef bstm_multi_block_metadata metadata;
  typedef vnl_vector_fixed<unsigned char, 16> space_tree_b;
  typedef vnl_vector_fixed<unsigned char, 8> time_tree_b;

public:
  bstm_multi_block(const metadata &data,
                   const vcl_vector<unsigned char *> &buffers)
      : metadata_(data), buffers_(buffers), read_only_(false) {}

  //: creates empty block from metadata
  bstm_multi_block(const metadata &data);

  //: default destructor
  virtual ~bstm_multi_block() {
    for (vcl_vector<unsigned char *>::iterator iter = buffers_.begin();
         iter != buffers_.end();
         ++iter) {
      delete[](*iter);
    }
  }

  //: accessors
  bstm_block_id &block_id() { return metadata_.id_; }
  vcl_vector<unsigned char *> buffers() { return buffers_; }
  long byte_count() const { return byte_count_; }
  //: mutators
  void set_block_id(bstm_block_id id) { metadata_.id_ = id; }
  void set_byte_count(long bc) { byte_count_ = bc; }

  //: regardless of the way the instance is constructed, enable write
  void enable_write() { read_only_ = false; }
  bool read_only() const { return read_only_; }

private:
  metadata metadata_;

  //: byte buffer
  vcl_vector<unsigned char *> buffers_;

  //: number of bytes this block takes up (on disk and ram)
  long byte_count_;

  bool read_only_;
};

#endif // bstm_multi_block_h_
