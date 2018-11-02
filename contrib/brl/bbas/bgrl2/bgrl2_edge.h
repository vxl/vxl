// This is brl/bbas/bgrl2/bgrl2_edge.h
#ifndef bgrl2_edge_h_
#define bgrl2_edge_h_

//:
// \file
// \brief A templated directed edge class for a generic graph class
// \author Amir Tamrakar
// \date February 30, 2005
//
// This edge has pointers to its source and target vertices
//
// \verbatim
//  Modifications
//   Amir Tamrakar   Feb 30, 2005  Initial version.
//   Ozge C. Ozcanli Nov 15, 2008  Moved up to vxl, minor fixes
// \endverbatim

#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_ref_count.h>
#include <vbl/vbl_smart_ptr.h>

//: A Directed edge in a graph
template<class V>
class bgrl2_edge : public vbl_ref_count
{
 protected:
  typedef vbl_smart_ptr<V> V_sptr;

 public:
  // Constructor
  bgrl2_edge() : vbl_ref_count(), source_(nullptr), target_(nullptr) {}

  // Constructor
  bgrl2_edge(V_sptr v1, V_sptr v2) : vbl_ref_count(), source_(v1), target_(v2) {}

  // Destructor
  ~bgrl2_edge() override= default;

  //: Smart pointer to the vertex where this edge originates
  V_sptr source() const { return source_; }

  //: Smart pointer to the vertex where this edge points to
  V_sptr target() const { return target_; }

  //: Returns target(edge) if v = source(edge) and source(edge) otherwise.
  V_sptr opposite(V_sptr v);

  //: set the source vertex
  void set_source(V_sptr v) { source_ = v; }

  //: set the target vertex
  void set_target(V_sptr v) { target_ = v; }

  //: Determine if ``this'' edge shares a vertex with other edge
  // Return the shared node if so, otherwise return 0
  V_sptr shared_vertex(const bgrl2_edge<V >& other) const;

  //: Print an ascii summary to the stream
  virtual void print_summary(std::ostream &os) const;

 protected:

  //: The source vertex
  V_sptr source_;

  //: The target vertex
  V_sptr target_;
};

#endif // bgrl2_edge_h_
