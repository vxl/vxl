#ifndef boct_tree_h_
#define boct_tree_h_

#include "boct_tree_cell.h"
#include "boct_loc_code.h"

#include <vgl/vgl_point_3d.h>
#include <vsl/vsl_binary_io.h>

template <class T_loc, class T>
class boct_tree {
public:
  boct_tree(): root_(0){}
  boct_tree(short max_level, short init_levels=1);
  ~boct_tree() {}
  boct_tree_cell<T_loc,T>* locate_point(const vgl_point_3d<double>& p);
  boct_tree_cell<T_loc,T>* locate_point_at_level(const vgl_point_3d<double>& p, short level);
  boct_tree_cell<T_loc,T>* locate_region(const vgl_box_3d<double>& r);
  boct_tree_cell<T_loc,T>* get_cell( boct_loc_code<T_loc>& code) { return root_->traverse(code); }
  bool split();
  vcl_vector<boct_tree_cell<T_loc,T>*> leaf_cells();
  //: return the max level
  short num_levels() { return max_level_; }
  void print();
  void b_write(vsl_b_ostream & os);
  void b_read(vsl_b_istream & is);
  short version_no() { return 1; }
private:
  short max_level_;
  boct_tree_cell<T_loc,T>* root_;
};

/*template <class T>
void vsl_b_write(vsl_b_ostream & os, boct_tree<T>& tree);

template <class T>
void vsl_b_read(vsl_b_istream & is, boct_tree<T>& tree);*/

#endif