// This is core/vcsl/vcsl_graph.h
#ifndef vcsl_graph_h
#define vcsl_graph_h
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/08/01 François BERTEL Creation.
//   2001/04/10 Ian Scott (Manchester) Converted perceps header to doxygen
//   2002/01/28 Peter Vanroose - vcl_vector member vertices_ changed to non-ptr
//   2004/09/10 Peter Vanroose - Added explicit copy constructor (ref_count !)
// \endverbatim

#include <vcsl/vcsl_graph_sptr.h>

#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>
#include <vcsl/vcsl_spatial_sptr.h>
//: Spatial coordinate system graph
// Graph where nodes are spatial coordinate systems and arrows are
// transformations. Only the nodes are in the graph class. The transformations
// are in the spatial coordinates systems
class vcsl_graph
  :public vbl_ref_count
{
 public:
  //***************************************************************************
  // Constructors/Destructor
  //***************************************************************************

  //: Default constructor
  explicit vcsl_graph(void) {}

  // Copy constructor
  vcsl_graph(vcsl_graph const& x) : vbl_ref_count(), vertices_(x.vertices_) {}

  //: Destructor
  virtual ~vcsl_graph() {}

  //***************************************************************************
  // Measurement
  //***************************************************************************

  //: Number of coordinate systems
  virtual int count(void) const;

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Has `this' `cs' as node ?
  virtual bool has(const vcsl_spatial_sptr &cs) const;

  //: Is `index' valid in the list of the spatial coordinate systems ?
  virtual bool valid_index(int index) const;

  //***************************************************************************
  // Access
  //***************************************************************************

  //: Spatial coordinate system number `index'
  //  REQUIRE: valid_index(index)
  virtual vcsl_spatial_sptr item(int index) const;

  //: Add `cs' in `this'
  //  REQUIRE: !has(cs)
  virtual void put(const vcsl_spatial_sptr &cs);

  //: Remove `cs' from `this'
  //  REQUIRE: has(cs)
  virtual void remove(const vcsl_spatial_sptr &cs);

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //: Set the flag `reached' to false for each spatial coordinate system.
  //  Used by the search path algorithm
  virtual void init_vertices(void) const;

 protected:

  //: Vertices of the graph: all the spatial coordinate systems
  vcl_vector<vcsl_spatial_sptr> vertices_;
};

#endif // vcsl_graph_h
