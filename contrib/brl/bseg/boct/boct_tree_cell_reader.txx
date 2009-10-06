#ifndef boct_tree_cell_reader_txx_
#define boct_tree_cell_reader_txx_

#include "boct_tree_cell_reader.h"

#include <vgl/vgl_box_3d.h>
#include <vgl/io/vgl_io_box_3d.h>


template <class T_loc,class T_data>
void boct_tree_cell_reader<T_loc,T_data>::begin()
{
  // read header info of the tree
  if (!is_) return;

  short v;
  short max_level;
  vgl_box_3d<double> global_bbox, tree_bb;
  vsl_b_read(*is_, v);
  
  
  switch (v)
  {
   case (1):
     vsl_b_read(*is_, global_bbox);
     
     // read the treee header
     vsl_b_read(*is_, v);
     vsl_b_read(*is_, max_level);
     vsl_b_read(*is_, tree_bb);
     vsl_b_read(*is_, num_cells_);
     break;
   default:
     vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, boct_tree<T_loc,T_data>&)\n"
              << "           Unknown version number "<< v << '\n';
//     *is_.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
     return;
  }
}

template <class T_loc,class T_data>
bool boct_tree_cell_reader<T_loc,T_data>::next(boct_tree_cell<T_loc,T_data>& c)
{
  if (!is_)
    return false;

  if (num_cells_-- > 0) {
    vsl_b_read(*is_, c.code_);
    T_data data;
    vsl_b_read(*is_, data);
    c.set_data(data);
    c.set_vis_node(NULL);
    return true;
  } else {
    is_->close();
  }
  return false;
}

#define BOCT_TREE_CELL_READER_INSTANTIATE(T_loc,T_data) \
template class boct_tree_cell_reader<T_loc,T_data >

#endif // boct_tree_cell_reader_txx_
