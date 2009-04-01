#ifndef boct_tree_cell_h_
#define boct_tree_cell_h_
//:
// \file
#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>
#include "boct_loc_code.h"

template <class T_loc,class T>
class boct_tree_cell // public vbl_ref_count
{
 public:
  typedef enum {NONE, X_LOW, X_HIGH, Y_LOW, Y_HIGH, Z_LOW, Z_HIGH, ALL} FACE_IDX;

  //constructors
  boct_tree_cell<T_loc,T>()
  : children_(0), parent_(0) {}

  boct_tree_cell<T_loc,T>(const boct_loc_code<T_loc>& code, boct_tree_cell<T_loc,T>* p, short l)
  : code_(code), children_(0), parent_(p), level_(l) {}

  //constructor given code and level
  boct_tree_cell<T_loc,T>(const boct_loc_code<T_loc>& code, short level);

  ~boct_tree_cell<T_loc,T>();
  void set_parent(boct_tree_cell<T_loc,T>* p) {parent_ = p; }
  bool is_leaf();

  //: adds a pointer for each leaf children to v
  void leaf_children(vcl_vector<boct_tree_cell<T_loc,T>*>& v);

  const boct_loc_code<T_loc>& get_code();

  //boct_tree_cell* traverse(boct_loc_code code);
  boct_tree_cell<T_loc,T>* traverse_to_level(boct_loc_code<T_loc> *code, short level);
  bool split();
  void print();
  short level(){return level_;}
  void set_level(short level) {level_=level; }
  boct_tree_cell<T_loc,T>* children(){return children_;}
  boct_tree_cell<T_loc,T>* parent() { return parent_; }
  void  find_neighbors(FACE_IDX face,vcl_vector<boct_tree_cell<T_loc,T>*> & neighbors,short max_level);
  boct_tree_cell<T_loc,T>* get_common_ancestor(short binarydiff);
  void set_data(T const& data) {data_=data; }
  T data() {return data_; }
  boct_loc_code<T_loc> code_;
  static short version_no() { return 1; }
 protected:
  short level_;
  boct_tree_cell<T_loc,T>* parent_;
  boct_tree_cell<T_loc,T>* children_;
  T data_;
};

template <class T_loc,class T>
void vsl_b_write(vsl_b_ostream & os, boct_tree_cell<T_loc,T>& cell);

template <class T_loc,class T>
void vsl_b_read(vsl_b_istream & is, boct_tree_cell<T_loc,T>& cell, boct_tree_cell<T_loc,T>* parent);

//typedef vbl_smart_ptr<boct_tree_cell<vgl_point_3d<double> > boct_tree_cell_pt_sptr;

#endif
