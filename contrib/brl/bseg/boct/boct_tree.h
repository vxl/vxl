#ifndef boct_tree_h_
#define boct_tree_h_

#include "boct_tree_cell.h"

#include <vgl/vgl_point_3d.h>

class boct_tree {
public:
  boct_tree(short max_level) ;
  boct_tree_cell* locate_point(const vgl_point_3d<double>& p);
  boct_tree_cell* locate_point_at_level(const vgl_point_3d<double>& p, short level);
  boct_tree_cell* locate_region(const vgl_box_3d<double>& r);
  boct_tree_cell* get_cell(const boct_loc_code& code);
  bool split();
  bool split_all();
  vcl_vector<boct_tree_cell*> leaf_cells();
  //: return the max level
  short numlevels(){return max_level_;}

private:
  short max_level_;
  boct_tree_cell* root_;
};

#endif