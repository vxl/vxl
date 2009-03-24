#include "boct_tree.h"
#include <vgl/vgl_box_3d.h>
#include <vcl_iostream.h>
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
  //: convert point to location code.
  boct_loc_code* loccode_=new boct_loc_code(p, max_level_);

  //: check to see if point is contained in the octree
  if(!root_->code_.isequal(loccode_,curr_level))
    return NULL;
  
  //: temporary pointer to traverse 
  boct_tree_cell* curr_cell=root_;

  while(curr_cell->children())
  {
      short index_child=loccode_->child_index(curr_level);
      curr_cell=curr_cell->children()+index_child;  
      --curr_level;
  }
  //: delete the location code constructed
  delete loccode_;
  return curr_cell;
}

boct_tree_cell* boct_tree::locate_point_at_level(const vgl_point_3d<double>& p, short level)
{ 
  short curr_level=max_level_-1;
  //: convert point to location code.
  boct_loc_code* loccode_=new boct_loc_code(p, max_level_);

  //: check to see if point is contained in the octree
  if(!root_->code_.isequal(loccode_,curr_level))
    return NULL;
  
  //: temporary pointer to traverse 
  boct_tree_cell* curr_cell=root_;

  while(curr_cell->children()&& curr_level>level)
  {
      short child_index=loccode_->child_index(curr_level);
      curr_cell=curr_cell->children()+child_index;  
        --curr_level;
  }
  //: delete the location code constructed
  delete loccode_;
  return curr_cell;
}

boct_tree_cell* boct_tree::locate_region(const vgl_box_3d<double>& r)
{ 
  boct_loc_code* mincode=new boct_loc_code(r.min_point(), max_level_);
  boct_loc_code* maxcode=new boct_loc_code(r.max_point(), max_level_);

  boct_loc_code* xorcode=mincode->XOR(maxcode);

  short level_x=max_level_-1;
  short level_y=max_level_-1;
  short level_z=max_level_-1;
  while(!(xorcode->x_loc_&(1<<level_x))&& level_x) level_x--;
  while(!(xorcode->y_loc_&(1<<level_y))&& level_y>level_x) level_y--;
  while(!(xorcode->z_loc_&(1<<level_z))&& level_z>level_y) level_z--;

   
  
  return locate_point_at_level(r.min_point(),level_z);
}

boct_tree_cell* boct_tree::get_cell(const boct_loc_code& code)
{ 
  // remove this
  return root_;
}

bool boct_tree::split()
{ 
  return root_->split();
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

void boct_tree::print()
{
  vcl_cout << "Max Level=" << max_level_ << vcl_endl;
  root_->print();
}