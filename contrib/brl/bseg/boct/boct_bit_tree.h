// This is brl/bseg/boct/boct_bit_tree.h
#ifndef boct_bit_tree_h_
#define boct_bit_tree_h_
//:
// \file
// \brief  Test octree implementation using an implicit bit-array to denote tree
// structure.
//         Data structure is currently float 16.
//
// \author Andrew Miller
// \date   August 11, 2010
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bitset>
#include <cmath>
#include <iosfwd>
#include <iostream>
#include <vector>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_vector_fixed.h>

class boct_bit_tree {
public:
  typedef vnl_vector_fixed<unsigned char, 16> uchar16;

  // These constructor are owning
  //: default constructor
  boct_bit_tree()
      : is_owning_(true), bits_(new unsigned char[16]()), num_levels_(4) {}
  boct_bit_tree(const unsigned char *bits, int num_levels = 4);
  // Copy constructor -- copies ownership type as well, and if other is owning,
  // then creates and owns a copy of the data.
  boct_bit_tree(const boct_bit_tree &other);
  // non-owning constructors
  boct_bit_tree(unsigned char *bits, int num_levels = 4)
      : is_owning_(false), bits_(bits), num_levels_(num_levels) {}
  boct_bit_tree(uchar16 &bits, int num_levels = 4)
      : is_owning_(false)
      , bits_(reinterpret_cast<unsigned char *>(&bits))
      , num_levels_(num_levels) {}
  //: constructor with all parameters
  // This creates a tree that wraps,  and optionally owns, the bits pointed to.
  boct_bit_tree(unsigned char *bits, bool is_owning, int num_levels)
      : is_owning_(is_owning), bits_(bits), num_levels_(num_levels) {}

  //: Destructor
  ~boct_bit_tree() {
    if (is_owning_ && bits_) {
      delete[] bits_;
      bits_ = nullptr;
    }
  }

  // Copy assignment operator
  boct_bit_tree &operator=(boct_bit_tree that);

  //: Returns index of data for given bit
  int get_data_index(int bit_index, bool is_random = false) const;

  //: returns bit index assuming root data is located at 0
  int get_relative_index(int bit_index) const;

  //: traverse tree to get leaf index that contains point. If full is
  // true, this ignored whether bits are refined or not and just goes
  // down to the lowest level.
  int traverse(const vgl_point_3d<double> p,
               int deepest = 4,
               bool full = false);

  //: traverse tree to get leaf index that contains point
  int traverse_to_level(const vgl_point_3d<double> p, int deepest = 4);

  //: gets the cell center (octree is assumed to be [0,1]x[0,1]x[0,1]
  vgl_point_3d<double> cell_center(int bit_index);

  //: gets the cell bounding box (octree is assumed to be [0,1]x[0,1]x[0,1]
  vgl_box_3d<double>
  cell_box(int bit_index,
           vgl_point_3d<double> orig = vgl_point_3d<double>(0, 0, 0),
           double len = 1.0);

  //: returns octree cell length on one side (assumed [0,1]^3)
  double cell_len(int bit_index) const;

  //: tells you if a valid cell is at bit_index (if and only if its parent is 1)
  bool valid_cell(int bit_index) const;

  //: tells you if the bit_index is a leaf
  bool is_leaf(int bit_index) const;

  //: Return the maximum number of levels, which is root_level+1
  unsigned short number_levels() const { return num_levels_; }

  //: Return number of cells in this tree
  int num_cells() const;

  // returns the number of leaf cells
  int num_leaves() const;

  //: return maximum number of cells in this tree
  int max_num_cells();
  int max_num_inner_cells();

  //: returns depth (0,1,2,3) at given index
  //  Note that cumulative nodes = (1/7) * (8^(n+1) -1)
  int depth_at(int index) const;

  //: returns depth of tree
  int depth();

  //: returns value (0 or 1) of bit at given index (0,73);
  unsigned char bit_at(int index) const;

  //: sets bit at given index with bool value
  void set_bit_at(int index, bool val);

  // Sets the bit at the given index to 'true', and also set every
  // parent bit to true. If index <0, does nothing.
  void set_bit_and_parents_to_true(int index);

  // get bits and data
  unsigned char *get_bits() { return bits_; }
  const unsigned char *get_bits() const { return bits_; }

  //: gets pointers stored in bits 10, 11, 12, 13
  int get_data_ptr(bool is_random = false);

  //: sets pointers stored in bits 10, 11, 12, 13
  void set_data_ptr(int ptr, bool is_random = false);

  //: returns bit indices of leaf nodes under rootBit
  std::vector<int> get_leaf_bits(int rootBit = 0) const;
  std::vector<int> get_leaf_bits(int rootBit, int depth) const;

  //: returns bit indices of every node under rootBit
  std::vector<int> get_cell_bits(int rootBit = 0) const;

  //: returns parent index (invalid for bit_index = 0)
  int parent_index(int bit_index) { return (bit_index - 1) >> 3; }

  //: returns bit index of first child
  int child_index(int bit_index) { return (bit_index << 3) + 1; }

  //: wrap a const reference in a const, non-owning bit tree.
  static const boct_bit_tree wrap_const(const uchar16 &data, int num_levels = 4);

  //: Compares structure bits of the two trees
  static bool same_structure(const boct_bit_tree &t1, const boct_bit_tree &t2) {
    return std::memcmp(t1.get_bits(), t2.get_bits(), 10) == 0;
  }

  // cached arrays are public - make em const too
  static unsigned char bit_lookup[256];
  static float centerX[585];
  static float centerY[585];
  static float centerZ[585];

private:
  // Whether this tree owns the underlying buffer and should delete it on
  // destruction.
  bool is_owning_;
  //: Tree structure stored as "bits" = really a char array
  unsigned char *bits_;
  //: Maximum number of levels in the octree
  unsigned short num_levels_;
};

std::ostream &operator<<(std::ostream &s, const boct_bit_tree &t);

#endif // boct_bit_tree_h_
