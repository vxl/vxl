//This is brl/bseg/boct/boct_bit_tree.h
#ifndef boct_bit_tree_h_
#define boct_bit_tree_h_
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

#include "boct_tree.h"
#include "boct_loc_code.h"

#include <vnl/vnl_vector_fixed.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vcl_bitset.h>
#include <vcl_iosfwd.h>
#include <vcl_vector.h>

class boct_bit_tree
{
  typedef vnl_vector_fixed<int,4> int4;
  typedef vnl_vector_fixed<float,16> float16;

 public:

  //: Default constructor
  boct_bit_tree();
  boct_bit_tree(char* bits);
  boct_bit_tree(boct_tree<short,float > * tree);
  boct_bit_tree(vcl_vector<int4> tree, vcl_vector<float16> data);

  //: Destructor
  ~boct_bit_tree() {
    if (bits_) {
      delete[] bits_; bits_ = NULL;
    }
    if (data_) {
      delete[] data_; data_ = NULL;
    }
  }

  //: Returns index of data for given bit
  int get_data_index(int bit_index);

  //: traverse tree to get leaf index that contains point
  int traverse(const vgl_point_3d<double> p);

  //: Return cell with a particular locational code
  int get_cell( boct_loc_code<short>& code);

  //: Return the maximum number of levels, which is root_level+1
  short number_levels() const { return num_levels_; }

  //: Return number of cells in this tree
  int size() const;

  //: returns depth (0,1,2,3) at given index
  //  Note that cumulative nodes = (1/7) * (8^(n+1) -1)
  int depth_at(int index) const;

  //: encodes a boct_tree<short, float> (for testing purposes)
  void encode(boct_tree_cell<short, float>* node, int i);
  void encode(int4 node, int i, vcl_vector<int4> tree);
  void encode_data(vcl_vector<int4> tree, vcl_vector<float16> data);
  bool verify_tree(int i, int node, vcl_vector<int4> tree, vcl_vector<float16> data);
  void print_input_tree(vcl_vector<int4> tree, vcl_vector<float16> data);

  //: maps location code to the finest level generation index
  //  I.e., [0,0,0]->0, [1,1,1]->7
  unsigned short loc_code_to_gen_offset(boct_loc_code<short> loc_code, int depth);
  static int loc_code_to_index(boct_loc_code<short> loc_code, int root_level);

  //-- BIT manipulation wrapper functions ------------------------------

  //: returns value (0 or 1) of bit at given index (0,73);
  unsigned char bit_at(int index);
  //: sets bit at given index with bool value
  void set_bit_at(int index, bool val);

  //get bits and data
  unsigned char* get_bits() {return bits_; }
  float* get_data() {return data_; }

 private:

  //: Tree structure stored as "bits" = really a char array
  unsigned char* bits_;

  //: data - kept as float 16...
  float* data_;

  //: Maximum number of levels in the octree
  short num_levels_;
};

vcl_ostream& operator <<(vcl_ostream &s, boct_bit_tree &t);


#endif // boct_bit_tree_h_
