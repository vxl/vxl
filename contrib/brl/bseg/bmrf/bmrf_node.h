// This is brl/bseg/bmrf/bmrf_node.h
#ifndef bmrf_node_h_
#define bmrf_node_h_
//:
// \file
// \brief A node in a Markov Random Field (MRF) network
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 1/13/04
//
// The MRF node contains a link to an epi-segment as well as
// links to other neighboring nodes in the network
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_list.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_iosfwd.h>
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>
#include <bmrf/bmrf_node_sptr.h>
#include <bmrf/bmrf_epi_seg_sptr.h>
#include <bmrf/bmrf_epi_transform_sptr.h>

// forward declare the arc
class bmrf_arc;

//: A Markov Random Field (MRF) node
class bmrf_node : public vbl_ref_count
{
 public:
  //: Directed arc from one node to another
  class bmrf_arc : public vbl_ref_count
  {
   public:
    friend class bmrf_node;
     
    //: Constructor
    bmrf_arc() : from_(NULL), to_(NULL) {}
    //: Constructor
    bmrf_arc( bmrf_node* f, bmrf_node* t) : from_(f), to_(t) {}
    //: Destructor
    ~bmrf_arc() {}

    //: Binary save self to stream.
    void b_write(vsl_b_ostream &os) const;

    //: Binary load self from stream.
    void b_read(vsl_b_istream &is);

    //: Smart pointer to the node where this arc originates
    bmrf_node_sptr from() { return bmrf_node_sptr(from_); }

    //: Smart pointer to the node where this arc ends
    bmrf_node_sptr to() { return bmrf_node_sptr(to_); }
   
  private:
    bmrf_node* from_;
    bmrf_node* to_;
  };


  friend class bmrf_network;
  
  //: Smart pointer to an arc
  typedef vbl_smart_ptr<bmrf_arc> bmrf_arc_sptr;

  //: iterator over neighboring nodes
  typedef vcl_list<bmrf_arc_sptr>::iterator arc_iterator;

  //: The values of this enum categorize the neighbors
  // \note ALL is a special type and should be kept last
  enum neighbor_type { SPACE, TIME, ALPHA, /*<--add new types here*/  ALL };

  //: Constructor
  bmrf_node( const bmrf_epi_seg_sptr& epi_seg = NULL, int frame_num = 0, double probability = 0.0 );

  //: Destructor
  ~bmrf_node(){}

  //: Calculate the conditional probability that this node is correct given its neighbors
  double probability();

  //: Calculate the probability given the trasformation and neighbors
  double probability(const bmrf_epi_transform_sptr& xform);

  //: Returns an iterator to the beginning of the type \p type neighbors
  // \note if \p type is ALL then iteration is over all types
  arc_iterator begin(neighbor_type type = ALL);

  //: Returns an iterator to the end of the type \p type neighbors
  // \note if \p type is ALL then iteration is over all types
  arc_iterator end(neighbor_type type = ALL);

  //: Return the frame number at which this node is found
  bmrf_epi_seg_sptr epi_seg() const { return segment_; }

  //: Returns the number of outgoing neighbors to this node of type \p type
  // \note if \p type is ALL then this returns the total number of neighbors
  int num_neighbors( neighbor_type type = ALL );

  //: Return the frame number at which this node is found
  int frame_num() const { return frame_num_; }

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  void print_summary(vcl_ostream &os) const;

 protected:
  //: Add \p node as a neighbor of type \p type
  // \return true if the node was added successfully
  // \return false if the neighbor is not valid or already exists
  bool add_neighbor( bmrf_node *node, neighbor_type type );

  //: Remove \p node from the neighborhood
  // \return true if the node is removed successfully
  // \return false if the node was not a neighbor
  bool remove_neighbor( bmrf_node *node, neighbor_type type = ALL);

  //: Strip all of the arcs from this node
  // This also removes arcs to and from this node in neighboring nodes
  void strip();

  //: Remove any arcs to or from NULL nodes
  // \retval true if any arcs were removed
  // \retval false of all arcs are valid
  bool purge();

 protected:
  //: Compute the weights of each node for use in probability computation
  // Nodes are weighted by alpha overlap and intesity similarity
  void compute_weights();

  //: Map nodes to weights
  vcl_map<bmrf_node*, double> weight_;

 private:  
  //: A smart pointer to the underlying epi-segment data
  bmrf_epi_seg_sptr segment_;

  //: The frame number associated with this node
  int frame_num_;

  //: The cached probability value
  double probability_;

  //: The pointers to outgoing arcs
  vcl_list<bmrf_arc_sptr> out_arcs_;

  //: The pointers to incoming arcs
  vcl_list<bmrf_arc_sptr> in_arcs_;

  //: The the iterators into neighbors_ that represent the boundary between types
  vcl_vector<arc_iterator> boundaries_;

  //: The number of neighbors for each type
  vcl_vector<int> sizes_;
};


//: Binary save bmrf_node* to stream.
void vsl_b_write(vsl_b_ostream &os, const bmrf_node* n);

//: Binary load bmrf_node* from stream.
void vsl_b_read(vsl_b_istream &is, bmrf_node* &n);

//: Print an ASCII summary to the stream
void vsl_print_summary(vcl_ostream &os, const bmrf_node* n);


//: Binary save bmrf_node::bmrf_arc* to stream.
void vsl_b_write(vsl_b_ostream &os, const bmrf_node::bmrf_arc* a);

//: Binary load bmrf_node::bmrf_arc* from stream.
void vsl_b_read(vsl_b_istream &is, bmrf_node::bmrf_arc* &a);

//: Print an ASCII summary to the stream
void vsl_print_summary(vcl_ostream &os, const bmrf_node::bmrf_arc* a);

#endif // bmrf_node_h_
