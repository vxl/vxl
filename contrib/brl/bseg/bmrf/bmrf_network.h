// This is brl/bseg/bmrf/bmrf_network.h
#ifndef bmrf_network_h_
#define bmrf_network_h_
//:
// \file
// \brief A Markov Random Field (MRF) network
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 1/13/04
//
// The MRF network object maintains pointers to all of the nodes
// in the network.  It also handle the creation and destruction of
// arcs between nodes.  The network object also contains search
// iterators to iterate through the nodes along arcs using search
// algorithms such as depth-first or breadth-first.
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_deque.h>
#include <vcl_set.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/vbl_ref_count.h>
#include "bmrf_node_sptr.h"
#include "bmrf_node.h"
#include "bmrf_network_sptr.h"
#include "bmrf_epi_seg_sptr.h"

//: The MRF network
class bmrf_network : public vbl_ref_count
{
 public:
  typedef vcl_map<bmrf_epi_seg*, bmrf_node_sptr> seg_node_map;
  typedef vcl_map<int, seg_node_map > frame_node_map;

  typedef bmrf_node::neighbor_type neighbor_type;

  //: Constructor
  bmrf_network();

  //: Destructor
  ~bmrf_network(){}

  //: Adds a new to the network
  // \retval true if the node was added
  // \retval false if the node could not be added
  // \note every node in the network must have a unique epi_segment
  bool add_node(const bmrf_node_sptr& node);

  //: Deletes a node in the network
  // \retval true if the node was deleted
  // \retval false if the node was not found in the network
  bool remove_node(const bmrf_node_sptr& node);

  //: Add an arc between \p n1 and \p n2 of type \p type
  bool add_arc( const bmrf_node_sptr& n1, const bmrf_node_sptr& n2, neighbor_type type );

  //: Add an arc between \p n1 and \p n2 of type \p type
  bool remove_arc( const bmrf_node_sptr& n1, const bmrf_node_sptr& n2, neighbor_type type = bmrf_node::ALL );

  //: Remove all arcs to NULL nodes and node not found in this network
  // \retval true if any arcs have been purged
  // \retval false if all arcs were found to be valid
  bool purge();

  //: Look up the node corresponding to an epi-segment
  // \return a null smart pointer if no node exists
  // \note if the optional paramater \p frame is positive the search is restricted to that frame
  bmrf_node_sptr seg_to_node(const bmrf_epi_seg_sptr& seg, int frame = -1) const;

  //: Returns the number of nodes in the network
  // \ note if the optional parameter \p frame is positive then the size is of that frame
  int size( int frame = -1 );

  //: Returns the probability that the entire network is correct
  double probability();

  //: Returns the beginning const iterator to the map of nodes in frame \p frame
  // \note if \p frame is negative the iterator will cover all frames
  seg_node_map::const_iterator begin(int frame = -1) const;

  //: Returns the end const iterator to the map of nodes in frame \p frame
  // \note if \p frame is negative the iterator will cover all frames
  seg_node_map::const_iterator end(int frame = -1) const;

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  void print_summary(vcl_ostream &os) const;

 private:
  //: The map from epi_seg pointers to nodes in the network
  // \note indexed by epi_seg pointers for quick reverse lookup
  seg_node_map node_from_seg_;

  //: The map from frame number to list of nodes in that frame
  frame_node_map nodes_from_frame_;

 public:
  class iterator
  {
   public:
    //: Constructor
    iterator( bmrf_network_sptr network, bmrf_node_sptr node ) : network_(network), curr_node_(node) {}

    //: Destructor
    virtual ~iterator() {}

    //: Increment
    iterator& operator++ () { next_node(); return *this; }

    //: Dereference
    bmrf_node_sptr operator -> () const { return curr_node_; }
    //: Dereference
    bmrf_node_sptr operator * () const { return curr_node_; }

    //: Equality comparison
    bool operator == (const iterator& rhs) const { return rhs.curr_node_ == this->curr_node_; }

    //: Inequality comparison
    bool operator != (const iterator& rhs) const { return rhs.curr_node_ != this->curr_node_; }

   protected:
    //: Increment the current node
    virtual void next_node() = 0;

    bmrf_network_sptr network_;
    bmrf_node_sptr curr_node_;
  };

  // Depth first search iterator
  class depth_iterator : public iterator
  {
   public:
    //: Constructor
    depth_iterator( bmrf_network_sptr network, bmrf_node_sptr node ) : iterator(network, node){ visited_.insert(node); }

   protected:
    //: Increment the current node
    void next_node();

    vcl_deque<bmrf_node_sptr> eval_queue_;
    vcl_set<bmrf_node_sptr> visited_;
  };

  // Breadth first search iterator
  class breadth_iterator : public iterator
  {
   public:
    //: Constructor
    breadth_iterator( bmrf_network_sptr network, bmrf_node_sptr node ) : iterator(network, node){ visited_.insert(node); }

   protected:
    //: Increment the current node
    void next_node();

    vcl_deque<bmrf_node_sptr> eval_queue_;
    vcl_set<bmrf_node_sptr> visited_;
  };

  //: Depth first search begin iterator
  depth_iterator depth_begin(bmrf_node_sptr node) { return depth_iterator(this, node); }
  //: Depth first search end iterator
  depth_iterator depth_end()   { return depth_iterator(this, NULL); }

  //: Breadth first search begin iterator
  breadth_iterator breadth_begin(bmrf_node_sptr node) { return breadth_iterator(this, node); }
  //: Breadth first search end iterator
  breadth_iterator breadth_end()   { return breadth_iterator(this, NULL); }
};


//: Binary save bmrf_network* to stream.
void vsl_b_write(vsl_b_ostream &os, const bmrf_network* n);

//: Binary load bmrf_network* from stream.
void vsl_b_read(vsl_b_istream &is, bmrf_network* &n);

//: Print an ASCII summary to the stream
void vsl_print_summary(vcl_ostream &os, const bmrf_network* n);


#endif // bmrf_network_h_
