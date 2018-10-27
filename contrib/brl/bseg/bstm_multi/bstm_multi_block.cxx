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

space_tree_b *bstm_multi_block::get_space_data(int level) {
  switch (this->metadata().subdivisions_[level]) {
  case STE_SPACE:
    return reinterpret_cast<space_tree_b *>(this->get_data(level));
  case STE_TIME:
    throw tree_type_exception(STE_SPACE);
  }
}

time_tree_b *bstm_multi_block::get_time_data(int level) {
  switch (this->metadata().subdivisions_[level]) {
  case STE_SPACE:
    throw tree_type_exception(STE_TIME);
  case STE_TIME:
    return reinterpret_cast<time_tree_b *>(this->get_data(level));
  }
}

// nothrow versions of typed accessors -- return null if wrong type
space_tree_b *bstm_multi_block::get_space_data(int level, std::nothrow_t) {
  switch (this->metadata().subdivisions_[level]) {
  case STE_SPACE:
    return reinterpret_cast<space_tree_b *>(this->get_data(level));
  case STE_TIME:
    return nullptr;
  }
}

time_tree_b *bstm_multi_block::get_time_data(int level, std::nothrow_t) {
  switch (this->metadata().subdivisions_[level]) {
  case STE_SPACE:
    return nullptr;
  case STE_TIME:
    return reinterpret_cast<time_tree_b *>(this->get_data(level));
  }
}

std::size_t bstm_multi_block::byte_count() const {
  std::size_t total_bytes = 0;
  for (const auto & buffer : buffers_) {
    total_bytes += buffer.size();
  }
  return total_bytes;
}
