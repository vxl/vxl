// This is brl/bbas/bgrl/bgrl_edge.h
#ifndef bgrl_edge_h_
#define bgrl_edge_h_
//:
// \file
// \brief A directed edge in a graph
// \author Matt Leotta, (mleotta@lems.brown.edu)
// \date 3/17/04
//
// The edge has a pointer to its start and end vertices
//
// \verbatim
//  Modifications
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <bgrl/bgrl_vertex_sptr.h>
#include <bgrl/bgrl_edge_sptr.h>
#include <vsl/vsl_binary_io.h>


//: Directed edge from one vertex to another
class bgrl_edge : public vbl_ref_count
{
 public:
  friend class bgrl_vertex;
  friend class bgrl_graph;

  //: Constructor
  bgrl_edge() : from_(NULL), to_(NULL) {}
  //: Constructor
  bgrl_edge( bgrl_vertex_sptr f, bgrl_vertex_sptr t) : from_(f), to_(t) {}
  //: Destructor
  ~bgrl_edge() {}

  //: Smart pointer to the vertex where this edge originates
  bgrl_vertex_sptr from() const { return from_; }

  //: Smart pointer to the vertex where this edge ends
  bgrl_vertex_sptr to() const { return to_; }

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const;

  //: Create a copy of the object on the heap.
  // The caller is responsible for deletion
  virtual bgrl_edge* clone() const;

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Print an ascii summary to the stream
  void print_summary(vcl_ostream &os) const;

 private:
  //: The starting vertex
  bgrl_vertex_sptr from_;
  //: The ending vertex
  bgrl_vertex_sptr to_;
};


//: Binary save bgrl_edge to stream.
//  \relates bgrl_edge
void vsl_b_write(vsl_b_ostream &os, const bgrl_edge* e);

//: Binary load bgrl_edge from stream.
//  \relates bgrl_edge
void vsl_b_read(vsl_b_istream &is, bgrl_edge* &e);

//: Print an ASCII summary to the stream
//  \relates bgrl_edge
void vsl_print_summary(vcl_ostream &os, bgrl_edge* e);


#endif // bgrl_edge_h_
