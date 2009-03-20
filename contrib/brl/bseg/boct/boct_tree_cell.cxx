#include "boct_tree_cell.h"

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
  return false;
}