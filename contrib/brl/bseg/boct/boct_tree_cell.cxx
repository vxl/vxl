#include "boct_tree_cell.h"

#include <vcl_iostream.h>

boct_tree_cell::boct_tree_cell(const boct_loc_code& code, short level)
{
    level_=level;
    code_=code;
    parent_=NULL;
    children_=NULL;
}

bool boct_tree_cell::is_leaf()
{
  return false;
}
  
const boct_loc_code& 
boct_tree_cell::get_code()
{
  return code_;
}
  
boct_tree_cell* boct_tree_cell::traverse(boct_loc_code code)
{
  // remove this
  return this;
}
  
boct_tree_cell* boct_tree_cell::traverse_to_level(boct_loc_code code, short level)
{
  // remove this
  return this;
}
  
bool boct_tree_cell::split()
{
  // create new children
  children_ = (boct_tree_cell*) malloc(sizeof(boct_tree_cell)*8);
  short child_level = level_-1;
  for (unsigned i=0; i<8; i++) {
    children_[i].level_ = child_level;
    children_[i].parent_ = this;
    children_[i].children_ = 0;
    children_[i].code_ = code_.child_loc_code(i, child_level);
  }
  return false;
}

void boct_tree_cell::print()
{
  vcl_cout << "LEVEL=" << level_ << vcl_endl; 
  vcl_cout << " code=" << code_;
  if (!children_)
    vcl_cout << "LEAF " << vcl_endl << vcl_endl;
  else {
    for (unsigned i=0; i<8; i++) {
      children_[i].print();
    }
  }
}