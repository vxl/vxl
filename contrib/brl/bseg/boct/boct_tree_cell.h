#ifndef boct_tree_cell_h_
#define boct_tree_cell_h_

#include "boct_loc_code.h"

class boct_tree_cell
{
public:
  typedef enum {NONE, X_LOW, X_HIGH, Y_LOW, Y_HIGH, Z_LOW, Z_HIGH, ALL} FACE_IDX;

  //constructors
  boct_tree_cell(const boct_loc_code& code);
  //constructor given code and level
  boct_tree_cell(const boct_loc_code& code, short level);
  boct_tree_cell(const boct_tree_cell& rhs);
  bool is_leaf();
  const boct_loc_code& get_code();
  boct_tree_cell* traverse(boct_loc_code code);
  boct_tree_cell* traverse_to_level(boct_loc_code code, short level);
  bool split();

private:
  short level_;
  boct_tree_cell* parent_;
  boct_tree_cell* children_;
  boct_loc_code code_;
};

#endif