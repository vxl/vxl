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
#include <vbl/vbl_smart_ptr.h>
#include <bgrl/bgrl_vertex_sptr.h>
#include <vsl/vsl_binary_io.h>


//: Directed edge from one vertex to another
class bgrl_edge : public vbl_ref_count
{
public:
  friend class bgrl_vertex;
  
  //: Constructor
  bgrl_edge() : from_(NULL), to_(NULL) {}
  //: Constructor
  bgrl_edge( bgrl_vertex_sptr f, bgrl_vertex_sptr t) : from_(f.ptr()), to_(t.ptr()) {}
  //: Destructor
  ~bgrl_edge() {}
  
  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;
  
  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);
  
  //: Smart pointer to the vertex where this edge originates
  bgrl_vertex_sptr from() { return bgrl_vertex_sptr(from_); }
  
  //: Smart pointer to the vertex where this edge ends
  bgrl_vertex_sptr to() { return bgrl_vertex_sptr(to_); }
  
private:
  //: The starting vertex
  bgrl_vertex* from_;
  //: The ending vertex
  bgrl_vertex* to_;
};


//: Binary save bgrl_vertex::bgrl_edge* to stream.
void vsl_b_write(vsl_b_ostream &os, const bgrl_edge* e);

//: Binary load bgrl_vertex::bgrl_edge* from stream.
void vsl_b_read(vsl_b_istream &is, bgrl_edge* &e);

//: Print an ASCII summary to the stream
void vsl_print_summary(vcl_ostream &os, const bgrl_edge* e);


#endif // bgrl_edge_h_
