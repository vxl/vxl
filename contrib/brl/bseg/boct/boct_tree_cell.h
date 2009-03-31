#ifndef boct_tree_cell_h_
#define boct_tree_cell_h_

#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>
#include "boct_loc_code.h"

template <class T>
class boct_tree_cell //: public vbl_ref_count
{
public:
  typedef enum {NONE, X_LOW, X_HIGH, Y_LOW, Y_HIGH, Z_LOW, Z_HIGH, ALL} FACE_IDX;

  //constructors
  boct_tree_cell<T>()
  : children_(0), parent_(0) {}

  boct_tree_cell<T>(const boct_loc_code& code, boct_tree_cell<T>* p, short l) 
  : code_(code), children_(0), parent_(p), level_(l) {}

  //constructor given code and level
  boct_tree_cell<T>(const boct_loc_code& code, short level);

  ~boct_tree_cell<T>();
  void set_parent(boct_tree_cell<T>* p) {parent_ = p; }
  bool is_leaf();

  //: adds a pointer for each leaf children to v
  void leaf_children(vcl_vector<boct_tree_cell<T>*>& v);

  const boct_loc_code& get_code();

  //boct_tree_cell* traverse(boct_loc_code code);
  boct_tree_cell<T>* traverse_to_level(boct_loc_code *code, short level);
  bool split();
  void print();
  short level(){return level_;}
  boct_tree_cell<T>* children(){return children_;}
  boct_tree_cell<T>* parent() { return parent_; }
  void  find_neighbors(FACE_IDX face,vcl_vector<boct_tree_cell<T>*> & neighbors,short max_level);
  boct_tree_cell<T>* get_common_ancestor(short binarydiff);
  void set_data(T& data) {data_=data; }
  T data() {return data_; }
  boct_loc_code code_;

protected:
  short level_;
  boct_tree_cell<T>* parent_;
  boct_tree_cell<T>* children_;
  T data_;
};

template <class T>
void vsl_b_write(vsl_b_ostream & os, boct_tree_cell<T>& cell);

template <class T>
void vsl_b_read(vsl_b_istream & is, boct_tree_cell<T>& cell, boct_tree_cell<T>* parent);

//typedef vbl_smart_ptr<boct_tree_cell<vgl_point_3d<double> > boct_tree_cell_pt_sptr;

#endif