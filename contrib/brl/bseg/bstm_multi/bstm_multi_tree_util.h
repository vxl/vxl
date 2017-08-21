#ifndef bstm_multi_tree_util_h_
#define bstm_multi_tree_util_h_

#include <vcl_new.h>
#include <vcl_stdexcept.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vnl/vnl_vector_fixed.h>

#include <boct/boct_bit_tree.h>
#include <bstm/bstm_time_tree.h>
#include <bstm_multi/basic/array_4d.h>

typedef vnl_vector_fixed<unsigned char, 16> space_tree_b;
typedef vnl_vector_fixed<unsigned char, 8> time_tree_b;
static const vcl_size_t space_tree_size = sizeof(space_tree_b);
static const vcl_size_t time_tree_size = sizeof(time_tree_b);

//: Used to distinguish between time and space subdivisions in a block.
enum space_time_enum { STE_SPACE, STE_TIME };

//: Returns "space" or "time"
vcl_string ste_as_string(space_time_enum ste);

//: Converts a string that is either "space" or "time" to a corresponding enum
// value.
// \param s      the string to convert
// \param ste    stores resulting value
// \returns      whether or not string is valid
bool ste_from_string(const vcl_string &s, space_time_enum &ste);

//: Accepts a list of subdivisions separated by commas, e.g.
// 'space,time,space,time,space' and returns a vector of corresponding enum
// values. Note that this skips invalid values.
vcl_vector<space_time_enum> parse_subdivisions(const vcl_string &s);

//: Prints subdivisions as a comma-separated list of space_time_enum
// values (i.e. either 'space' or 'time'.)
vcl_string print_subdivisions(const vcl_vector<space_time_enum> &subdivisions);

vcl_size_t tree_size(space_time_enum ste);

// Thrown by generic_tree when one tries to access a tree of the wrong type.
class tree_type_exception : vcl_runtime_error {
public:
  explicit tree_type_exception(space_time_enum ste)
      : vcl_runtime_error("Tree does not have type: " + ste_as_string(ste)) {}
};

#define GENERIC_TREE_CALL(function_call)                                       \
  (type_ == STE_SPACE ? space_tree_.function_call : time_tree_.function_call)

//: Class that wraps either a BSTM time tree or a BOCT bit tree. This is
// essentially a tagged union.
class generic_tree {
public:
  generic_tree(unsigned char *bits, space_time_enum type)
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

  // calls appropriate destructor
  ~generic_tree() {
    switch (type_) {
    case STE_SPACE:
      space_tree_.~boct_bit_tree();
    case STE_TIME:
      time_tree_.~bstm_time_tree();
    }
  }

  //: returns tree type
  space_time_enum type() const { return type_; }

  // getters - these throw an exception if tree is of the wrong type

  const boct_bit_tree &space_tree() const {
    if (type_ == STE_SPACE) {
      return space_tree_;
    } else {
      throw tree_type_exception(STE_SPACE);
    }
  }
  boct_bit_tree &space_tree() {
    if (type_ == STE_SPACE) {
      return space_tree_;
    } else {
      throw tree_type_exception(STE_SPACE);
    }
  }

  bstm_time_tree &time_tree() {
    if (type_ == STE_TIME) {
      return time_tree_;
    } else {
      throw tree_type_exception(STE_TIME);
    }
  }
  const bstm_time_tree &time_tree() const {
    if (type_ == STE_TIME) {
      return time_tree_;
    } else {
      throw tree_type_exception(STE_TIME);
    }
  }

  // These getters do not throw and do not check if tree is of proper
  // type. Use only when you know type of tree.

  boct_bit_tree &space_tree(std::nothrow_t) { return space_tree_; }
  const boct_bit_tree &space_tree(std::nothrow_t) const { return space_tree_; }

  bstm_time_tree &time_tree(std::nothrow_t) { return time_tree_; }
  const bstm_time_tree &time_tree(std::nothrow_t) const { return time_tree_; }

  //: returns buffer size
  vcl_size_t tree_size() const { return ::tree_size(type_); }

  unsigned char *bits() { return data_; }
  const unsigned char *bits() const { return data_; }

  bool root_bit() const { return GENERIC_TREE_CALL(bit_at(0)); }
  bool bit_at(int index) const { return GENERIC_TREE_CALL(bit_at(index)); }
  void set_bit_at(int index, bool val) {
    GENERIC_TREE_CALL(set_bit_at(index, val));
  }

  int get_data_ptr() const { return GENERIC_TREE_CALL(get_data_ptr()); }
  void set_data_ptr(int ptr) { GENERIC_TREE_CALL(set_data_ptr(ptr)); }

  vcl_vector<int> get_leaf_bits() const {
    return GENERIC_TREE_CALL(get_leaf_bits());
  }

  // number of leaf cells (not number of inner nodes). In multi-BSTM,
  // this always corresponds to the number of data elements in the
  // underlying data buffer, because we don't store data in inner nodes.
  int num_leaves() const { return GENERIC_TREE_CALL(num_leaves()); }

  index_4d dimensions() const {
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
  // If the node is not a voxel node (i.e. lowest-level node), then the location of the first
  // voxel node it contains is returned.
  index_4d local_voxel_coords(int index) const {
    switch (type_) {
    case STE_SPACE:
      while (index > 73) {
        index = space_tree_.child_index(index);
      }
      return array_4d<int>(VXL_NULLPTR, this->dimensions())
          .coords_from_index(index - 73);
    case STE_TIME:
      while (index < 31) {
        index = time_tree_.child_index(index);
      }
      return array_4d<int>(VXL_NULLPTR, this->dimensions())
          .coords_from_index(index - 31);
    }
  }

  // Wraps a const pointer in a const generic_tree. Unforunately has
  // to call const_cast to properly pass in pointer, but const
  // correctness of member methods guarantees that the underlying data
  // is not modified.
  static const generic_tree wrap_const_ptr(const unsigned char *bits,
                                           space_time_enum type) {
    return generic_tree(const_cast<unsigned char *>(bits), type);
  }

private:
  const space_time_enum type_;
  unsigned char data_[sizeof(boct_bit_tree) > sizeof(bstm_time_tree)
                          ? sizeof(boct_bit_tree)
                          : sizeof(bstm_time_tree)];
  boct_bit_tree &space_tree_;
  bstm_time_tree &time_tree_;
};

#undef GENERIC_TREE_CALL

#endif // bstm_multi_tree_util_h_
