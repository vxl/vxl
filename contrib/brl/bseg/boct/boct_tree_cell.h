#ifndef boct_tree_cell_h_
#define boct_tree_cell_h_
//:
// \file
// \brief  Generic octree cell, traversal operations as described in
//         [Simple and Efficient Traversal Methods for Quadtrees and Octrees/ Frisken, Perry 2002]
//
// \author Gamze Tunali/Vishal Jain
// \date   July 31, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vsl/vsl_binary_io.h>
#include "boct_loc_code.h"
#include <vgl/vgl_box_3d.h>

enum boct_cell_face {NONE = 0x00,
                     Z_LOW = 0x01,
                     Z_HIGH = 0x02,
                     Y_LOW = 0x04,
                     Y_HIGH = 0x08,
                     X_LOW = 0x10,
                     X_HIGH = 0x20,
                     ALL = 0x3F};

typedef unsigned char boct_face_idx;

template <class T_loc,class T_data>
class boct_cell_vis_graph_node;

template <class T_loc,class T_data>
class boct_tree_cell
{
 public:
  //: Default Constructor
  boct_tree_cell<T_loc,T_data>()
  : children_(0), parent_(0), vis_node_(0) {}

  //: Constructor from locational code and octree cell
  boct_tree_cell<T_loc,T_data>(const boct_loc_code<T_loc>& code, boct_tree_cell<T_loc,T_data>* p)
  : code_(code), children_(0), parent_(p), vis_node_(0) {}

  //: Constructor given code and level
  boct_tree_cell<T_loc,T_data>(const boct_loc_code<T_loc>& code);

  ~boct_tree_cell<T_loc,T_data>();

  //: Creates a new cell with the same data
  boct_tree_cell<T_loc,T_data>* clone(boct_tree_cell<T_loc,T_data>* parent);

  //Clones a cell, shifting its location code according to shift_code. This is useful when creating subtrees.
  boct_tree_cell<T_loc,T_data>* clone(boct_tree_cell<T_loc,T_data>* parent, boct_loc_code<T_loc> *shift_code);

  //: Clones a cell if it intesects a region
  boct_tree_cell<T_loc,T_data>* clone_and_intersect(boct_tree_cell<T_loc,T_data>* parent,
                                                    vgl_box_3d<double> local_crop_box, short root_level);

  //: Clones and shifts a cell if it intesects a region
  boct_tree_cell<T_loc,T_data>* clone_and_intersect(boct_tree_cell<T_loc,T_data>* parent,
                                                    boct_loc_code<T_loc> *shift_code,
                                                    vgl_box_3d<double> local_crop_box, short root_level);

  template <class T_data_to>
  boct_tree_cell<T_loc,T_data_to>*  clone_to_type(boct_tree_cell<T_loc,T_data_to>* parent)
  {
    vis_node_=0;
    boct_tree_cell<T_loc,T_data_to>* cell = new boct_tree_cell<T_loc,T_data_to>(this->get_code());
    cell->set_parent(parent);
    if (!this->is_leaf()) {
      cell->split();
      for (unsigned i=0; i<8; i++) {
        boct_tree_cell<T_loc,T_data_to>* c=this->children_[i].clone_to_type<T_data_to>(cell);
        cell->set_children(i,c);
        c->set_children_null();
        delete c;
      }
    }
    return cell;
  }


  void set_parent(boct_tree_cell<T_loc,T_data>* p) {parent_ = p; }
  void set_children(unsigned i, boct_tree_cell<T_loc,T_data>* p) {if (children_) children_[i] = *p; else vcl_cout << "Children should be allocated first" << vcl_endl;}
  void set_children_null() { children_=0; }
  bool is_leaf();

  //: Returns the bounting box of this cell in local coordinates i.e [0,1)x[0,1)x[0,1)
  vgl_box_3d<double> local_bounding_box(short root_level);

  //: adds a pointer for each leaf children to v
  void leaf_children(vcl_vector<boct_tree_cell<T_loc,T_data>*>& v);

  //: adds a pointer to vector v, for each leaf children at a particular level
  void leaf_children_at_level(vcl_vector<boct_tree_cell<T_loc,T_data>*>& v, short level);

  //: adds a pointer to vector v, for each children at a particular level
  void children_at_level(vcl_vector<boct_tree_cell<T_loc,T_data>*>& v, short level);

  //: adds a pointer to vector v, for each children in a recursive fashion
  void all_children(vcl_vector<boct_tree_cell<T_loc,T_data>*>& v);

  //: Fills a cells with the average value of the 8 children in a dynamic programming manner
  void set_data_to_avg_children();

  const boct_loc_code<T_loc> get_code();

  //: currently this function just goes down the tree
  //  TODO: make it flexible and go from one node to another.
  boct_tree_cell<T_loc,T_data>* traverse(boct_loc_code<T_loc> &code);
  boct_tree_cell<T_loc,T_data>* traverse_to_level(boct_loc_code<T_loc> *code, short level);
  boct_tree_cell<T_loc,T_data>* traverse_force(boct_loc_code<T_loc> & code);
#if 0
  // TODO: not yet implemented -- currently just returns false
  bool traverse_and_split(boct_loc_code<T_loc> & /*code*/) { return false; }
#endif
  short level() const { return code_.level; }
  void set_level(short level) { code_.level=level; }
  boct_tree_cell<T_loc,T_data>* children() { return children_; }
  boct_tree_cell<T_loc,T_data>* parent() { return parent_; }

  void  find_neighbors(boct_face_idx face,vcl_vector<boct_tree_cell<T_loc,T_data>*> & neighbors,short root_level);
  //: at the same level or coarser level
  bool  find_neighbor(boct_face_idx face, boct_tree_cell<T_loc,T_data>* &neighbor,short root_level);

  boct_tree_cell<T_loc,T_data>* get_common_ancestor(short binarydiff);

  void set_data(T_data const& data) {data_=data; }
  T_data data() {return data_; }

  void set_vis_node(boct_cell_vis_graph_node<T_loc,T_data> * node) {vis_node_=node; }
  boct_cell_vis_graph_node<T_loc,T_data> * vis_node() {return vis_node_; }

  //: splits the cell and puts the parent's data into the newly created cells
  bool split();

  // splits and puts the data value new_data at the newly created cells
  bool split(T_data new_data);

  void print();

  void delete_children();

  static short version_no() { return 1; }

  boct_loc_code<T_loc> code_;

  boct_tree_cell<T_loc,T_data>* children_;

 protected:

  boct_tree_cell<T_loc,T_data>* parent_;

  T_data data_;

  boct_cell_vis_graph_node<T_loc,T_data>* vis_node_;
};

template<class T_loc,class T_data>
vcl_ostream& operator <<(vcl_ostream &s, boct_tree_cell<T_loc,T_data>& cell);

template <class T_loc,class T_data>
void vsl_b_write(vsl_b_ostream & os, boct_tree_cell<T_loc,T_data>& cell);

template <class T_loc,class T_data>
void vsl_b_read(vsl_b_istream & is, boct_tree_cell<T_loc,T_data>& cell, boct_tree_cell<T_loc,T_data>* parent);

template<class T_loc,class T_data>

class boct_cell_vis_graph_node
{
 public:
  boct_cell_vis_graph_node() : incoming_count(0), visible(false) {}
  ~boct_cell_vis_graph_node() {
    for (unsigned i=0;i<outgoing_links.size();++i)
      outgoing_links[i]=0;
  }
  unsigned int incoming_count;
  vcl_vector<boct_tree_cell<T_loc,T_data> * > outgoing_links;
  bool visible;
};

// Forward declaration of template specialization
template<>
void boct_tree_cell<short,float>::set_data_to_avg_children();

#endif // boct_tree_cell_h_
