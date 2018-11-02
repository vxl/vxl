#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "bstm_multi/bstm_multi_tree_util.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <boct/boct_bit_tree.h>
#include <bstm/bstm_time_tree.h>

std::string ste_as_string(space_time_enum ste) {
  switch (ste) {
  case STE_SPACE:
    return "space";
  case STE_TIME:
    return "time";
  }
}

bool ste_from_string(const std::string &s, space_time_enum &ste) {
  if (s == "space") {
    ste = STE_SPACE;
  } else if (s == "time") {
    ste = STE_TIME;
  } else {
    return false;
  }
  return true;
}

std::vector<space_time_enum> parse_subdivisions(const std::string &s) {
  std::vector<space_time_enum> result;
  std::stringstream ss(s);
  while (ss.good() > 0) {
    std::string substr;
    space_time_enum ste;
    std::getline(ss, substr, ',');
    // skip any invalid subdivisions that are not 'space' or 'time'
    if (ste_from_string(substr, ste)) {
      result.push_back(ste);
    }
  }
  return result;
}

std::string print_subdivisions(const std::vector<space_time_enum> &subdivisions) {
  std::string subdivs_str;
  for (auto iter = subdivisions.begin();
       iter != subdivisions.end();
       ++iter) {
    if (iter != subdivisions.begin()) {
      subdivs_str += ",";
    }
    subdivs_str += ste_as_string(*iter);
  }
  return subdivs_str;
}

std::size_t tree_size(space_time_enum ste) {
  switch (ste) {
  case STE_SPACE:
    return sizeof(space_tree_b);
  case STE_TIME:
    return sizeof(time_tree_b);
  }
}

#define GENERIC_TREE_CALL(function_call)                                       \
  (type_ == STE_SPACE ? space_tree_.function_call : time_tree_.function_call)

generic_tree::generic_tree(unsigned char *bits, space_time_enum type)
    : type_(type)
    , space_tree_(reinterpret_cast<boct_bit_tree &>(data_[0]))
    , time_tree_(reinterpret_cast<bstm_time_tree &>(data_[0])) {
  switch (type_) {
  case STE_SPACE:
    new (&space_tree_) boct_bit_tree(bits);
    return;
  case STE_TIME:
    new (&time_tree_) bstm_time_tree(bits);
    return;
  }
}

//: \brief creates non-owning space tree that points to bits of the given tree
generic_tree::generic_tree(boct_bit_tree &tree)
    : type_(STE_SPACE)
    , space_tree_(reinterpret_cast<boct_bit_tree &>(data_[0]))
    , time_tree_(reinterpret_cast<bstm_time_tree &>(data_[0])) {
  new (&space_tree_) boct_bit_tree(tree.get_bits());
}
//: \brief creates non-owning time tree that points to bits of the given tree
generic_tree::generic_tree(bstm_time_tree &tree)
    : type_(STE_TIME)
    , space_tree_(reinterpret_cast<boct_bit_tree &>(data_[0]))
    , time_tree_(reinterpret_cast<bstm_time_tree &>(data_[0])) {
  new (&time_tree_) bstm_time_tree(tree.get_bits());
}

// calls appropriate destructor
generic_tree::~generic_tree() {
  switch (type_) {
  case STE_SPACE:
    space_tree_.~boct_bit_tree();
  case STE_TIME:
    time_tree_.~bstm_time_tree();
  }
}

space_time_enum generic_tree::type() const { return type_; }

const boct_bit_tree &generic_tree::space_tree() const {
  if (type_ == STE_SPACE) {
    return space_tree_;
  } else {
    throw tree_type_exception(STE_SPACE);
  }
}
boct_bit_tree &generic_tree::space_tree() {
  if (type_ == STE_SPACE) {
    return space_tree_;
  } else {
    throw tree_type_exception(STE_SPACE);
  }
}

bstm_time_tree &generic_tree::time_tree() {
  if (type_ == STE_TIME) {
    return time_tree_;
  } else {
    throw tree_type_exception(STE_TIME);
  }
}
const bstm_time_tree &generic_tree::time_tree() const {
  if (type_ == STE_TIME) {
    return time_tree_;
  } else {
    throw tree_type_exception(STE_TIME);
  }
}

boct_bit_tree &generic_tree::space_tree(std::nothrow_t) { return space_tree_; }
const boct_bit_tree &generic_tree::space_tree(std::nothrow_t) const {
  return space_tree_;
}

bstm_time_tree &generic_tree::time_tree(std::nothrow_t) { return time_tree_; }
const bstm_time_tree &generic_tree::time_tree(std::nothrow_t) const {
  return time_tree_;
}

//: \brief returns buffer size
std::size_t generic_tree::tree_size() const { return ::tree_size(type_); }

unsigned char *generic_tree::bits() { return GENERIC_TREE_CALL(get_bits()); }
const unsigned char *generic_tree::bits() const {
  return GENERIC_TREE_CALL(get_bits());
}

bool generic_tree::root_bit() const { return GENERIC_TREE_CALL(bit_at(0)); }
bool generic_tree::bit_at(int index) const {
  return GENERIC_TREE_CALL(bit_at(index));
}
void generic_tree::set_bit_at(int index, bool val) {
  GENERIC_TREE_CALL(set_bit_at(index, val));
}

int generic_tree::get_data_ptr() const {
  return GENERIC_TREE_CALL(get_data_ptr());
}
void generic_tree::set_data_ptr(int ptr) {
  GENERIC_TREE_CALL(set_data_ptr(ptr));
}

std::vector<int> generic_tree::get_leaf_bits() const {
  return GENERIC_TREE_CALL(get_leaf_bits());
}

// number of leaf cells (not number of inner nodes). In multi-BSTM,
// this always corresponds to the number of data elements in the
// underlying data buffer, because we don't store data in inner nodes.
int generic_tree::num_leaves() const { return GENERIC_TREE_CALL(num_leaves()); }

index_4d generic_tree::dimensions() const {
  switch (type_) {
  case STE_SPACE:
    return index_4d(8, 8, 8, 1);
  case STE_TIME:
    return index_4d(1, 1, 1, 32);
  }
}

// Given an index to a node of a tree, returns the location of that
// node in the underlying grid, with row-major-ordered 4D
// coordinates. That is, a space tree is an 8x8x8x1 grid and a time
// tree is a 1x1x1x32 grid.
//
// If the node is not a voxel node (i.e. lowest-level node), then the location
// of the first
// voxel node it contains is returned.
index_4d generic_tree::local_voxel_coords(int index) const {
  switch (type_) {
  case STE_SPACE: {
    while (index < 73) {
      index = space_tree_.child_index(index);
    }
    vgl_box_3d<double> bbox = space_tree_.cell_box(index);
    bbox.scale_about_origin(8);
    vgl_point_3d<double> pt = bbox.min_point();
    return index_4d(pt.x(), pt.y(), pt.z(), 0);
  }
  case STE_TIME:
    while (index < 31) {
      index = time_tree_.child_index(index);
    }
    return index_4d(0, 0, 0, index - 31);
  }
}

// Unforunately has to call const_cast to properly pass in pointer,
// but const correctness of member methods guarantees that the
// underlying data is not modified.
const generic_tree generic_tree::wrap_const_ptr(const unsigned char *bits,
                                                space_time_enum type) {
  return generic_tree(const_cast<unsigned char *>(bits), type);
}

const generic_tree generic_tree::wrap_tree(const boct_bit_tree &t) {
  return generic_tree(const_cast<unsigned char *>(t.get_bits()), STE_SPACE);
}
const generic_tree generic_tree::wrap_tree(const bstm_time_tree &t) {
  return generic_tree(const_cast<unsigned char *>(t.get_bits()), STE_TIME);
}

#undef GENERIC_TREE_CALL
