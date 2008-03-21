#ifndef brip_quadtree_h
#define brip_quadtree_h
//-----------------------------------------------------------------
//:
// \file
// \author  J.L. Mundy March 8, 2008
// \brief brip_quadtree - A quadtree structure for 2-d arrays (not necessarily an image)

// \verbatim
// Modifications:
//   None
// \endverbatim
//------------------------------------------------------------------
//  a quadtree node has the structure:
//      ( node )
//     /  |  |  \
//  q00 q01 q10 q11  (indexed by i, j as qij)
//
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vbl/vbl_ref_count.h>
#include <brip/brip_quadtree_node_base_sptr.h>
// The quadtree can hold any datatype, not necessarily 
// image data. 

//: the base class representing the quadtree datastructure - holds no data
class brip_quadtree_node_base : public vbl_ref_count
{
 public:
  brip_quadtree_node_base() : iul_(0), jul_(0), ilr_(0), jlr_(0),
    valid_(false),
    children_(vcl_vector<brip_quadtree_node_base_sptr>(4)){}

  brip_quadtree_node_base(unsigned iul, unsigned jul,
                          unsigned ilr, unsigned jlr, bool valid = false) :
    iul_(iul), jul_(jul), ilr_(ilr), jlr_(jlr), valid_(valid),
    children_(vcl_vector<brip_quadtree_node_base_sptr>(4)){}

  ~brip_quadtree_node_base(){}

  bool data_valid() {return valid_;}

  //: return the child, qij
  brip_quadtree_node_base_sptr child(unsigned i, unsigned j)
    { return children_[2*j+i];}
  
  //: set the child, qij
  void set_child(unsigned i, unsigned j, 
                 brip_quadtree_node_base_sptr const& child)
    {children_[2*j+i] = child;}

  //: the number of non-null child nodes
  unsigned n_children() {unsigned n = 0; for(unsigned i = 0; i<4; ++i) 
    if(children_[i]) n++; return n;}

  //: the parent node to *this node
  brip_quadtree_node_base_sptr parent(){return parent_;}

  //: set the parent
  void set_parent(brip_quadtree_node_base_sptr const& parent)
    {parent_ = parent;}

  //: the region in the array represented by *this node
  void region(unsigned& iul, unsigned& jul, unsigned& ilr, unsigned& jlr)
    {iul = iul_; jul = jul_; ilr = ilr_; jlr = jlr_;}

  //: set the region in the array represented by *this node
  void set_region(unsigned iul, unsigned jul, unsigned ilr, unsigned jlr)
    { iul_ = iul; jul_ = jul; ilr_ = ilr; jlr_ = jlr;}

 protected:
  unsigned iul_, jul_;  
  unsigned ilr_, jlr_;  
  bool valid_; // data valid
  brip_quadtree_node_base_sptr parent_;
  vcl_vector<brip_quadtree_node_base_sptr> children_;
};
//: the templated subclass that holds data values, T,  in a quadtree structure
template <class T> 
class brip_quadtree_node : public brip_quadtree_node_base
{
  // PUBLIC INTERFACE-d---------------------------------------------

 public:

  // Constructors/Initializers/Destructors--------------------------
  brip_quadtree_node(): data_(T(0)){}

  brip_quadtree_node(unsigned iul, unsigned jul, unsigned ilr, unsigned jlr) :
  brip_quadtree_node_base(iul, jul, ilr, jlr, false){}

  brip_quadtree_node(unsigned iul, unsigned jul, unsigned ilr,
                     unsigned jlr, T data) :
    brip_quadtree_node_base(iul, jul, ilr, jlr, true),data_(data){}

  ~brip_quadtree_node(){}

  // Data Access----------------------------------------------------

  T data() {return data_;}

  // Utility Methods

 protected:
  T data_;// pixel value
};

VCL_DEFINE_SPECIALIZATION
class brip_quadtree_node<float> : public brip_quadtree_node_base
{
public:
  // Constructors/Initializers/Destructors--------------------------
  brip_quadtree_node():  data_(float(0)){}

  brip_quadtree_node(unsigned iul, unsigned jul, unsigned ilr, unsigned jlr) : 
  brip_quadtree_node_base(iul, jul, ilr, jlr, false){}


  brip_quadtree_node(unsigned iul, unsigned jul, unsigned ilr,
                     unsigned jlr, float data) :
  brip_quadtree_node_base(iul, jul, ilr, jlr, true),
    data_(data){}

  ~brip_quadtree_node(){}

  // Data Access----------------------------------------------------

  float data() {return data_;}  
  // Utility Methods

 protected:
  float data_;// pixel value
};

#endif
