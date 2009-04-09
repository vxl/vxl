#ifndef boct_tree_cell_h_
#define boct_tree_cell_h_
//:
// \file
#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>
#include "boct_loc_code.h"

class boct_cell_face
{
public:
  static const unsigned char NONE = 0x00;
  static const unsigned char Z_LOW = 0x01;
  static const unsigned char Z_HIGH = 0x02;
  static const unsigned char Y_LOW = 0x04;
  static const unsigned char Y_HIGH = 0x08;
  static const unsigned char X_LOW = 0x10;
  static const unsigned char X_HIGH = 0x20;
  static const unsigned char ALL = 0x3F;
};

typedef unsigned char boct_face_idx;

template <class T_loc,class T>
class boct_tree_cell // public vbl_ref_count
{
 public:

  //constructors
  boct_tree_cell<T_loc,T>()
  : children_(0), parent_(0) {}

  boct_tree_cell<T_loc,T>(const boct_loc_code<T_loc>& code, boct_tree_cell<T_loc,T>* p)
  : code_(code), children_(0), parent_(p) {}

  //constructor given code and level
  boct_tree_cell<T_loc,T>(const boct_loc_code<T_loc>& code);

  ~boct_tree_cell<T_loc,T>();
  void set_parent(boct_tree_cell<T_loc,T>* p) {parent_ = p; }
  bool is_leaf();

  //: adds a pointer for each leaf children to v
  void leaf_children(vcl_vector<boct_tree_cell<T_loc,T>*>& v);

  const boct_loc_code<T_loc>& get_code();

  //: currently this function just goes down the tree 
  //: TODO to make it flexible and go from one node to another.
  boct_tree_cell<T_loc,T>* traverse(boct_loc_code<T_loc> &code);
  boct_tree_cell<T_loc,T>* traverse_to_level(boct_loc_code<T_loc> *code, short level);
 
  bool traverse_and_split(boct_loc_code<T_loc> &code);

  short level(){return code_.level;}
  void set_level(short level) {code_.level=level; }
  boct_tree_cell<T_loc,T>* children(){return children_;}
  boct_tree_cell<T_loc,T>* parent() { return parent_; }

  void  find_neighbors(boct_face_idx face,vcl_vector<boct_tree_cell<T_loc,T>*> & neighbors,short max_level);
  boct_tree_cell<T_loc,T>* get_common_ancestor(short binarydiff);
  
  void set_data(T const& data) {data_=data; }
  T data() {return data_; }
  boct_loc_code<T_loc> code_;
  
  bool split();
  void print();
  void delete_children();
  static short version_no() { return 1; }
 protected:
  boct_tree_cell<T_loc,T>* parent_;
  boct_tree_cell<T_loc,T>* children_;
  T data_;
};
template<class T_loc,class T>
vcl_ostream& operator <<(vcl_ostream &s, boct_tree_cell<T_loc,T>& cell);

template <class T_loc,class T>
void vsl_b_write(vsl_b_ostream & os, boct_tree_cell<T_loc,T>& cell);

template <class T_loc,class T>
void vsl_b_read(vsl_b_istream & is, boct_tree_cell<T_loc,T>& cell, boct_tree_cell<T_loc,T>* parent);

//typedef vbl_smart_ptr<boct_tree_cell<vgl_point_3d<double> > boct_tree_cell_pt_sptr;

#endif
