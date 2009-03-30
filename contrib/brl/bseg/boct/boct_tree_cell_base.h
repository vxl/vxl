#ifndef boct_tree_cell_base_h_
#define boct_tree_cell_base_h_

#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>
#include "boct_loc_code.h"
#include "boct_tree_cell_base_sptr.h"

class boct_tree_cell_base : public vbl_ref_count
{
public:
  typedef enum {NONE, X_LOW, X_HIGH, Y_LOW, Y_HIGH, Z_LOW, Z_HIGH, ALL} FACE_IDX;

  //constructors
  boct_tree_cell_base();
  boct_tree_cell_base(const boct_loc_code& code, boct_tree_cell_base* p, short l) {code_=code; children_=0; parent_=p; level_=l; }
  //constructor given code and level
  boct_tree_cell_base(const boct_loc_code& code, short level);
  boct_tree_cell_base(const boct_tree_cell_base& rhs);
  ~boct_tree_cell_base();
  void set_parent(boct_tree_cell_base* p) {parent_ = p; }
  bool is_leaf();

  //: adds a pointer for each leaf children to v
  void leaf_children(vcl_vector<boct_tree_cell_base_sptr>& v);

  const boct_loc_code& get_code();

  //boct_tree_cell* traverse(boct_loc_code code);
  boct_tree_cell_base_sptr traverse_to_level(boct_loc_code *code, short level);
  bool split();
  void print();
  short level(){return level_;}
  boct_tree_cell_base* children(){return children_;}
  boct_tree_cell_base* parent() { return parent_; }
  void  find_neighbors(FACE_IDX face,vcl_vector<boct_tree_cell_base_sptr> & neighbors,short max_level);
  boct_tree_cell_base_sptr get_common_ancestor(short binarydiff);

  boct_loc_code code_;
private:
  short level_;
  boct_tree_cell_base* parent_;
  boct_tree_cell_base* children_;
};

void vsl_b_write(vsl_b_ostream & os, boct_tree_cell_base& c);
void vsl_b_read(vsl_b_istream & is, boct_tree_cell_base& c);

#endif