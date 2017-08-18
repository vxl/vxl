#include "bstm_multi_block.h"

#include <boct/boct_bit_tree.h>
#include <bstm/bstm_time_tree.h>
#include <bstm_multi/bstm_multi_tree_util.h>

// Initializes top-level buffer to an unrefined tree, sets other
// buffers to null. Top level's data pointer is also set to null.
bstm_multi_block::bstm_multi_block(const metadata_t &data)
    : metadata_(data), buffers_(data.subdivisions_.size()), read_only_(false) {
  // TODO allow multiple top-level trees?

  // Allocate enough space at top level for one tree, either space or
  // time depending on value of subdivisions_.
  space_time_enum root_type = metadata_.subdivisions_[0];
  buffers_[0].insert(buffers_[0].begin(), tree_size(root_type), 0);
}
