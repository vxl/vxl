//This is brl/bseg/boct/boct_bit_tree.h
#ifndef boct_bit_tree_h_
#define boct_bit_tree_h_
//:
// \file
// \brief  Test octree implementation using an implicit bit-array to denote
//         tree structure.  Data structure is currently float 16. 
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

class boct_bit_tree
{
 public:
 
  //data type is a float 16... 
  typedef vnl_vector_fixed<float,16> float16;
  typedef vnl_vector_fixed<unsigned char,16> uchar16;

  //: Default constructor
  boct_bit_tree();
  boct_bit_tree(char* bits);
  boct_bit_tree(boct_tree<short,float > * tree);

  //: Destructor
  ~boct_bit_tree() {}
  
  //: Returns index of data for given bit
  int get_data_index(int bit_index);

  //: traverse tree to get leaf index that contains point
  int traverse(const vgl_point_3d<double> p); 

  //: Return cell with a particular locational code
  int get_cell( boct_loc_code<short>& code);

  //: Return the maximum number of levels, which is root_level+1
  short number_levels() const { return num_levels_; }
  
  //: Return number of cells in this tree 
  int size();
  
  //: returns depth (0,1,2,3) at given index
  //: note that cumulative nodes = (1/7) * (8^(n+) -1)
  int depth_at(int index) {  
    return (int) vcl_floor( vcl_log(7.0*(index)+1.0)/vcl_log(8.0) );
  }
  
  //: encodes a boct_tree<short, float> (for testing purposes)
  void encode(boct_tree_cell<short, float>* node, int i);
  
  //: maps location code to the finest level generation index
  //: i.e. [0,0,0]->0, [1,1,1]->7
  unsigned short loc_code_to_gen_offset(boct_loc_code<short> loc_code, int depth);
  
  
  //-- BIT manipulation wrapper functions ------------------------------
  //: returns value (0 or 1) of bit at given index (0,73);
  unsigned char bit_at(int index);
  //: returns byte at given index (0,73), mask to get bit, byte index to set bit
  unsigned char byte_at(int index, unsigned char &mask, int &i);
  //: sets bit at given index with bool value
  void set_bit_at(int index, bool val);
  
  //get bits and data
  unsigned char* get_bits() {return bits_; }
  float16* get_data() {return data_; }

 private:
 
  //: Tree structure stored as "bits" = really a char array
  unsigned char bits_[16];
  
  //: data is in flat array - this should also maintain byte order
  float16 data_[585];
 
  //: Maximum number of levels in the octree
  short num_levels_;

};

vcl_ostream& operator <<(vcl_ostream &s, boct_bit_tree t);



#endif
