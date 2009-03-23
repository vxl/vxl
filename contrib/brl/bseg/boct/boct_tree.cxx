#include "boct_tree.h"

//; constructor initializes an empty tree
boct_tree::boct_tree(short max_level): max_level_(max_level) 
{
    //: root is allocated max_level_-1
    boct_loc_code code;
    if(max_level_>0)
        root_=new boct_tree_cell( code, max_level_-1);
}

boct_tree_cell* boct_tree::locate_point(const vgl_point_3d<double>& p)
{
  short curr_level=max_level_-1;
  boct_loc_code* loccode_=new boct_loc_code(p, curr_level);

  
  if(!root_->code_.isequal(loccode_,curr_level))
    return NULL;
  
  boct_tree_cell* curr_cell=root_;

  while(curr_cell->children())
  {
      short index_child=loccode_->child_index(curr_level);
      curr_cell=curr_cell->children()+index_child;  
      --curr_level;
  }
  delete loccode_;
  return curr_cell;
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