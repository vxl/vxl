#ifndef boct_tree_cell_reader_h_
#define boct_tree_cell_reader_h_
//:
// \file
// \brief  Sequential binary octree reader. next() methods returns the leaf cells one by one until end of file is found.
//
// \author Gamze Tunali
// \date   July 31, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vsl/vsl_binary_io.h>

#include "boct_tree_cell.h"

template <class T_loc,class T_data>
class boct_tree_cell_reader
{
 public:
  boct_tree_cell_reader(std::string path): is_(new vsl_b_ifstream(path)) { }
  ~boct_tree_cell_reader() = default;

#if 0
  boct_tree_cell_reader(vsl_b_istream* s) : is_(*s) {}
  boct_tree_cell_reader(boct_tree_cell_reader const &r) { is_= r.is_; }
#endif // 0

  //: reads the tree header info and gets ready to read the cells
  void begin();

  //: returns the next leaf cell read from the binary file
  bool next(boct_tree_cell<T_loc,T_data>& c);

  void close() {is_->close(); }

 private:
  //: input binary stream of octree
  vsl_b_ifstream* is_;
  unsigned num_cells_;
  unsigned version_;    // the binary version of the tree
};


#endif // boct_tree_cell_reader_h_
