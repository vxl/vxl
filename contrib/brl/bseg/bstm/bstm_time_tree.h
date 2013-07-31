#ifndef bstm_time_tree_h_
#define bstm_time_tree_h_
//:
// \file
// \brief  Binary tree implementation using an implicit bit-array to denote tree structure.
//
//
// \author Ali Osman Ulusoy
// \date   August 05, 2012
// \verbatim
//  Modifications
//    15 Jan 2012 Modified to store data for leaf cells only.
// \endverbatim

#include <vcl_iosfwd.h>
#include <vcl_vector.h>
#include <vcl_cmath.h> // for std::pow()

#define TT_NUM_BYTES  8
#define TT_NUM_LVLS  6

class bstm_time_tree
{
 public:

  //: Default constructor
  bstm_time_tree();
  bstm_time_tree(const unsigned char* bits, int num_levels=6);
  bstm_time_tree(const bstm_time_tree& other);

  //: Destructor
  ~bstm_time_tree() {
    if (bits_) {
      delete[] bits_; bits_ = NULL;
    }
  }

  //: Get bits
  unsigned char* get_bits() const { return bits_; }

  //: sets bit at given index with bool value
  void set_bit_at(int index, bool val);

  //: Return the maximum number of levels, which is root_level+1
  unsigned short number_levels() const { return num_levels_; }

  //: accessors
  int max_num_cells() const;
  int max_num_inner_cells() const;

  //: returns time tree cell length (assuming [0,1])
  float cell_len(int bit_index) const;

  //: tells you if a valid cell is at bit_index (if and only if its parent is 1)
  bool valid_cell(int bit_index) const;

  //: returns value (0 or 1) of bit at given index (0,73);
  unsigned char bit_at(int index) const;

  //: returns depth (0,1,2,3,4,5) at given index
  int depth_at(int index) const;

  //: tells you if the bit_index is a leaf
  bool is_leaf(int bit_index) const;

  //: Return number of cells in this tree
  int num_cells() const;

  //: Return number of leaves in this tree
  int num_leaves() const;

  //: returns parent index (invalid for bit_index = 0)
  int parent_index(int bit_index) const { return (bit_index-1)>>1; }

  //: returns bit index of first child
  int child_index(int bit_index) const { return (bit_index<<1) + 1; }

  //: Returns index of data for given bit
  int get_data_index(int bit_index) const;

  //: returns bit index assuming root data is located at 0
  int get_relative_index(int bit_index) const;

  //: gets pointers stored in bits 4,5,6,7
  int get_data_ptr();

  //: sets pointers stored in bits 4,5,6,7
  void set_data_ptr(int ptr);

  //: traverse tree to get leaf index that contains time t, assuming t \in [0,1]
  int traverse(const double t, int deepest=6) const;

  //: returns bit indices of leaf nodes under rootBit, using pre-order traversal
  vcl_vector<int> get_leaf_bits(int rootBit=0) const;

  //: gets the cell center (tree is assumed to be [0,1)
  float cell_center(int bit_index) const;

  //: gets the cell range (tree is assumed to be [0,1)
  void cell_range(int bit_index, float& min, float& max) const;

  //: the extent of the tree, aka max number of leaves.
  static float tree_range() {return 32;}

  //: erase tree to only the root, retain the data ptr.
  void erase_cells();

  //: return max depth of its leaves
  int max_depth(int rootBit) const;

  //: returns the leaves residing at the maximum depth
  vcl_vector<int> max_depth_leaves() const;

 private:
  //: Tree structure stored as "bits" = really a char array
  unsigned char* bits_;
  //: Maximum number of levels in the time tree
  unsigned short num_levels_;

  static unsigned char bit_lookup[256];
  static float cell_centers[63];
};

vcl_ostream& operator <<(vcl_ostream &s, bstm_time_tree &t);

#endif // bstm_time_tree_h_
