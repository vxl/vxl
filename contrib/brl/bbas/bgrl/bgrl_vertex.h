// This is brl/bbas/bgrl/bgrl_vertex.h
#ifndef bgrl_vertex_h_
#define bgrl_vertex_h_
//:
// \file
// \brief A vertex in a graph
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 3/17/04
//
// The vertex contains sets of incoming and outgoing 
// edges to other vertices in the graph
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_set.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/vbl_ref_count.h>
#include <bgrl/bgrl_vertex_sptr.h>
#include <bgrl/bgrl_edge_sptr.h>

// forward declare the edge
class bgrl_edge;

//: A vertex in a graph
class bgrl_vertex : public vbl_ref_count
{
 public:

  typedef vcl_set<bgrl_edge_sptr>::iterator edge_iterator;
  friend class bgrl_graph;

  //: Constructor
  bgrl_vertex();

  //: Destructor
  ~bgrl_vertex(){}

  //: Returns an iterator to the beginning of the set of outgoing edges
  edge_iterator begin();

  //: Returns an iterator to the end of the list of outgoing edges
  edge_iterator end();

  //: Returns the total number of edges at this vertex 
  int degree() const { return this->in_degree() + this->out_degree(); }

  //: Returns the number of incoming edges to this vertex 
  int in_degree() const { return in_edges_.size(); }

  //: Returns the number of outgoing edges to this vertex 
  int out_degree() const { return out_edges_.size(); }

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  void print_summary(vcl_ostream &os) const;

 protected:
  //: Create an outgoing edge to \p vertex
  // \return a smart pointer to the edge if the vertex was added successfully
  // or a NULL smart pointer if the edge is not valid or already exists
  bgrl_edge_sptr add_edge_to(const bgrl_vertex_sptr& vertex);

  //: Remove the outgoing edge to \p vertex 
  // \retval true if the edge was removed successfully
  // \retval false if the edge was not found
  bool remove_edge_to(const bgrl_vertex_sptr& vertex);

  //: Strip all of the edges from this vertex
  // This also removes edges to and from this vertex in neighboring vertices
  void strip();

  //: Remove any edges to or from NULL vertices
  // \retval true if any edges were removed
  // \retval false if all edges are valid
  bool purge();


  //: The pointers to outgoing edges
  vcl_set<bgrl_edge_sptr> out_edges_;

  //: The pointers to incoming edges
  vcl_set<bgrl_edge_sptr> in_edges_;
};


//: Binary save bgrl_vertex* to stream.
void vsl_b_write(vsl_b_ostream &os, const bgrl_vertex* v);

//: Binary load bgrl_vertex* from stream.
void vsl_b_read(vsl_b_istream &is, bgrl_vertex* &v);

//: Print an ASCII summary to the stream
void vsl_print_summary(vcl_ostream &os, const bgrl_vertex* v);


#endif // bgrl_vertex_h_
