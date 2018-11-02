#ifndef bstm_multi_tree_util_h_
#define bstm_multi_tree_util_h_

#include <iostream>
#include <new>
#include <stdexcept>
#include <string>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector_fixed.h>

#include <boct/boct_bit_tree.h>
#include <bstm/bstm_time_tree.h>
#include <bstm_multi/basic/array_4d.h>

typedef vnl_vector_fixed<unsigned char, 16> space_tree_b;
typedef vnl_vector_fixed<unsigned char, 8> time_tree_b;
static const std::size_t space_tree_size = sizeof(space_tree_b);
static const std::size_t time_tree_size = sizeof(time_tree_b);

//: Used to distinguish between time and space subdivisions in a block.
enum space_time_enum { STE_SPACE, STE_TIME };

//: Returns "space" or "time"
std::string ste_as_string(space_time_enum ste);

//: Converts a string that is either "space" or "time" to a corresponding enum
// value.
// \param s      the string to convert
// \param ste    stores resulting value
// \returns      whether or not string is valid
bool ste_from_string(const std::string &s, space_time_enum &ste);

//: Accepts a list of subdivisions separated by commas, e.g.
// 'space,time,space,time,space' and returns a vector of corresponding enum
// values. Note that this skips invalid values.
std::vector<space_time_enum> parse_subdivisions(const std::string &s);

//: Prints subdivisions as a comma-separated list of space_time_enum
// values (i.e. either 'space' or 'time'.)
std::string print_subdivisions(const std::vector<space_time_enum> &subdivisions);

std::size_t tree_size(space_time_enum ste);

// Thrown by generic_tree when one tries to access a tree of the wrong type.
class tree_type_exception : std::runtime_error {
public:
  explicit tree_type_exception(space_time_enum ste)
      : std::runtime_error("Tree does not have type: " + ste_as_string(ste)) {}
};

//: \brief Class that wraps either a BSTM time tree or a BOCT bit
// tree. This is essentially a tagged union.
class generic_tree {
public:
  //: \brief creates non-owning tree of appropriate type
  generic_tree(unsigned char *bits, space_time_enum type);

  //: \brief creates non-owning space tree that points to bits of the given tree
  generic_tree(boct_bit_tree &tree);
  //: \brief creates non-owning time tree that points to bits of the given tree
  generic_tree(bstm_time_tree &tree);

  //: \brief calls appropriate destructor
  ~generic_tree();

  //: \brief returns tree type
  space_time_enum type() const;

  // getters - these throw an exception if tree is of the wrong type

  const boct_bit_tree &space_tree() const;
  boct_bit_tree &space_tree();

  bstm_time_tree &time_tree();
  const bstm_time_tree &time_tree() const;

  // These getters do not throw and do not check if tree is of proper
  // type. Use only when you know type of tree.

  boct_bit_tree &space_tree(std::nothrow_t);
  const boct_bit_tree &space_tree(std::nothrow_t) const;

  bstm_time_tree &time_tree(std::nothrow_t);
  const bstm_time_tree &time_tree(std::nothrow_t) const;

  //: returns buffer size
  std::size_t tree_size() const;

  unsigned char *bits();
  const unsigned char *bits() const;

  bool root_bit() const;
  bool bit_at(int index) const;
  void set_bit_at(int index, bool val);

  int get_data_ptr() const;
  void set_data_ptr(int ptr);

  std::vector<int> get_leaf_bits() const;

  //: \brief number of leaf cells (not number of inner nodes).
  //
  // In multi-BSTM, this always corresponds to the number of data
  // elements in the underlying data buffer, because we don't store
  // data in inner nodes.
  int num_leaves() const;

  index_4d dimensions() const;

  //: \brief get local coordinates of node in tree.
  //
  // Given an index to a node of a tree, returns the location of that
  // node in the underlying grid, with row-major-ordered 4D
  // coordinates. That is, a space tree is an 8x8x8x1 grid and a time
  // tree is a 1x1x1x32 grid.
  //
  // If the node is not a voxel node (i.e. lowest-level node), then
  // the location of the first voxel node it contains is returned.
  index_4d local_voxel_coords(int index) const;

  //: \brief Wraps a const pointer in a const generic_tree.
  static const generic_tree wrap_const_ptr(const unsigned char *bits,
                                           space_time_enum type);
  static const generic_tree wrap_tree(const boct_bit_tree &t);
  static const generic_tree wrap_tree(const bstm_time_tree &t);

private:
  const space_time_enum type_;
  unsigned char data_[sizeof(boct_bit_tree) > sizeof(bstm_time_tree)
                          ? sizeof(boct_bit_tree)
                          : sizeof(bstm_time_tree)];
  boct_bit_tree &space_tree_;
  bstm_time_tree &time_tree_;
};

#endif // bstm_multi_tree_util_h_
