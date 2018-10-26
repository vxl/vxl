// This is brl/bbas/bgrl/bgrl_vertex.h
#ifndef bgrl_vertex_h_
#define bgrl_vertex_h_
//:
// \file
// \brief A vertex in a graph
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date March 17, 2004
//
// The vertex contains sets of incoming and outgoing
// edges to other vertices in the graph
//
// \verbatim
//  Modifications
// \endverbatim

#include <set>
#include <string>
#include <iostream>
#include <iosfwd>
#include <bgrl/bgrl_edge_sptr.h>
#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>
#include <vcl_compiler.h>

#include "bgrl_vertex_sptr.h"

// forward declare the edge
class bgrl_edge;

//: A vertex in a graph
class bgrl_vertex : public vbl_ref_count
{
 public:

  typedef std::set<bgrl_edge_sptr>::iterator edge_iterator;
  friend class bgrl_graph;

  //: Constructor
  bgrl_vertex();

  //: Copy Constructor
  bgrl_vertex(const bgrl_vertex& vertex);

  //: Destructor
  ~bgrl_vertex() override{}

  //: Returns an iterator to the beginning of the set of outgoing edges
  edge_iterator begin();

  //: Returns an iterator to the end of the list of outgoing edges
  edge_iterator end();

  //: Returns the total number of edges at this vertex
  int degree() const { return this->in_degree() + this->out_degree(); }

  //: Returns the number of incoming edges to this vertex
  unsigned int in_degree() const { return in_edges_.size(); }

  //: Returns the number of outgoing edges to this vertex
  unsigned int out_degree() const { return out_edges_.size(); }

  //: Return a platform independent string identifying the class
  virtual std::string is_a() const;

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bgrl_vertex* clone() const;

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  void print_summary(std::ostream &os) const;

 protected:
  //: Create an outgoing edge to \p vertex
  // \return a smart pointer to the edge if the vertex was added successfully
  // or a NULL smart pointer if the edge is not valid or already exists
  bgrl_edge_sptr add_edge_to( const bgrl_vertex_sptr& vertex,
                              const bgrl_edge_sptr& model_edge );

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
  std::set<bgrl_edge_sptr> out_edges_;

  //: The pointers to incoming edges
  std::set<bgrl_edge_sptr> in_edges_;
};


//: Allows derived class to be loaded by base-class pointer
//  A loader object exists which is invoked by calls
//  of the form "vsl_b_read(os,base_ptr)".  This loads derived class
//  objects from the disk, places them on the heap and
//  returns a base class pointer.
//  In order to work the loader object requires
//  an instance of each derived class that might be
//  found.  This function gives the model class to
//  the appropriate loader.
void vsl_add_to_binary_loader(const bgrl_vertex& v);

//: Print an ASCII summary to the stream
void vsl_print_summary(std::ostream &os, const bgrl_vertex* v);

#endif // bgrl_vertex_h_
