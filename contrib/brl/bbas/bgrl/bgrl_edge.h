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
  //: Destructor
  virtual ~bgrl_edge() {}

  //: Smart pointer to the vertex where this edge originates
  bgrl_vertex_sptr from() const { return bgrl_vertex_sptr(from_); }

  //: Smart pointer to the vertex where this edge ends
  bgrl_vertex_sptr to() const { return bgrl_vertex_sptr(to_); }

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

 protected:
  //: initialize the edge
  virtual void init() {}

  // The following pointers are only used to point back to the vertices
  // which own them.  The vertices are responsible for keeping these
  // pointers valid.
  
  //: The starting vertex
  // \note This must not be a smart pointer to prevent memory leaks
  bgrl_vertex* from_;
  //: The ending vertex
  // \note This must not be a smart pointer to prevent memory leaks
  bgrl_vertex* to_;
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
void vsl_add_to_binary_loader(const bgrl_edge& e);

//: Print an ASCII summary to the stream
//  \relates bgrl_edge
void vsl_print_summary(vcl_ostream &os, const bgrl_edge* e);

#endif // bgrl_edge_h_
