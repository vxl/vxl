#include "boct_tree.h"

boct_tree_cell* boct_tree::locate_point(const vgl_point_3d<double>& p)
{
  // remove this
  return root_;
}

boct_tree_cell* boct_tree::locate_point_at_level(const vgl_point_3d<double>& p, short level)
{ 
  // remove this
  return root_;
}

boct_tree_cell* boct_tree::locate_region(const vgl_box_3d<double>& r)
{ 
  // remove this
  return root_;
}

boct_tree_cell* boct_tree::get_cell(const boct_loc_code& code)
{ 
  // remove this
  return root_;
}

bool boct_tree::split()
{ 
  return false;
}

bool boct_tree::split_all()
{
  return false;
}

vcl_vector<boct_tree_cell*> 
boct_tree::leaf_cells()
{ 
  vcl_vector<boct_tree_cell*> v;
  return v;
}