//This is brl/bseg/boct/boct_bit_tree2.h
#ifndef boct_bit_tree2_h_
#define boct_bit_tree2_h_
//:
// \file
// \brief  Test octree implementation using an implicit bit-array to denote tree structure.
//         Data structure is currently float 16.
//
// \author Andrew Miller
// \date   August 11, 2010
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vgl/vgl_point_3d.h>
#include <vcl_bitset.h>
#include <vcl_iosfwd.h>
#include <vcl_vector.h>
#include <vcl_cmath.h> // for std::pow()

class boct_bit_tree2
{
 public:

  //: Default constructor
  boct_bit_tree2();
  boct_bit_tree2(unsigned char* bits, int num_levels=4);

  //: Destructor
  ~boct_bit_tree2() {
    if (bits_) {
      delete[] bits_; bits_ = NULL;
    }
  }

  //: Returns index of data for given bit
  int  get_data_index(int bit_index, bool is_random=false) const;

  //: returns bit index assuming root data is located at 0
  int  get_relative_index(int bit_index) const;

  //: traverse tree to get leaf index that contains point
  int traverse(const vgl_point_3d<double> p, int deepest=4);

  //: gets the cell center (octree is assumed to be [0,1]x[0,1]x[0,1]
  vgl_point_3d<double> cell_center(int bit_index);

  //: tells you if a valid cell is at bit_index (if and only if its parent is 1)
  bool valid_cell(int bit_index);

  //: tells you if the bit_index is a leaf
  bool is_leaf(int bit_index);

  //: Return the maximum number of levels, which is root_level+1
  unsigned short number_levels() const { return num_levels_; }

  //: Return number of cells in this tree
  int num_cells() const;

  //: return maximum number of cells in this tree
  int max_num_cells();
  int max_num_inner_cells();

  //: returns depth (0,1,2,3) at given index
  //  Note that cumulative nodes = (1/7) * (8^(n+1) -1)
  int depth_at(int index) const;

  //: returns value (0 or 1) of bit at given index (0,73);
  unsigned char bit_at(int index) const;

  //: sets bit at given index with bool value
  void set_bit_at(int index, bool val);

  //get bits and data
  unsigned char* get_bits() { return bits_; }

  //: gets pointers stored in bits 10, 11, 12, 13
  int get_data_ptr(bool is_random=false);
  //: sets pointers stored in bits 10, 11, 12, 13
  int set_data_ptr(int ptr, bool is_random=false);
#if 0
  int get_buffer_ptr();
  int set_buffer_ptr(int ptr);
#endif

  //: returns bit indices of leaf nodes under rootBit
  vcl_vector<int> get_leaf_bits(int rootBit=0);

  //cached arrays are public - make em const too
  static unsigned char bit_lookup[256];
  static float         centerX[585];
  static float         centerY[585];
  static float         centerZ[585];

 private:

  //: Tree structure stored as "bits" = really a char array
  unsigned char* bits_;
  //: Maximum number of levels in the octree
  unsigned short num_levels_;
};

vcl_ostream& operator <<(vcl_ostream &s, boct_bit_tree2 &t);

#endif // boct_bit_tree2_h_
