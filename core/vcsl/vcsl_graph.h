// This is core/vcsl/vcsl_graph.h
#ifndef vcsl_graph_h_
#define vcsl_graph_h_
//:
// \file
// \brief Spatial coordinate system transformation graph
// \author François BERTEL
//
// \verbatim
//  Modifications
//   2000/08/01 François BERTEL Creation.
//   2001/04/10 Ian Scott (Manchester) Converted perceps header to doxygen
//   2002/01/28 Peter Vanroose - vcl_vector member vertices_ changed to non-ptr
//   2004/09/10 Peter Vanroose - Added explicit copy constructor (ref_count !)
//   2004/09/17 Peter Vanroose - made count() non-virtual - it just returns a member and should not be overloaded
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vcsl/vcsl_graph_sptr.h>
#include <vcsl/vcsl_spatial_sptr.h>
#include <vcl_vector.h>

//: Spatial coordinate system transformation graph
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

  // Default constructor
  vcsl_graph() {}

  // Copy constructor
  vcsl_graph(vcsl_graph const& x) : vbl_ref_count(), vertices_(x.vertices_) {}

  // Destructor
  ~vcsl_graph() {}

  //***************************************************************************
  // Measurement
  //***************************************************************************

  //: Number of coordinate systems
  unsigned int count() const { return vertices_.size(); }

  //***************************************************************************
  // Status report
  //***************************************************************************

  //: Has `this' `cs' as node ?
  bool has(const vcsl_spatial_sptr &cs) const;

  //: Is `index' valid in the list of the spatial coordinate systems ?
  bool valid_index(unsigned int index) const { return index < count(); }

  //***************************************************************************
  // Access
  //***************************************************************************

  //: Spatial coordinate system number `index'
  //  REQUIRE: valid_index(index)
  vcsl_spatial_sptr item(unsigned int index) const;

  //: Add `cs' in `this'
  //  REQUIRE: !has(cs)
  void put(const vcsl_spatial_sptr &cs);

  //: Remove `cs' from `this'
  //  REQUIRE: has(cs)
  void remove(const vcsl_spatial_sptr &cs);

  //***************************************************************************
  // Basic operations
  //***************************************************************************

  //: Set the flag `reached' to false for each spatial coordinate system.
  //  Used by the search path algorithm
  void init_vertices() const;

 protected:

  //: Vertices of the graph: all the spatial coordinate systems
  vcl_vector<vcsl_spatial_sptr> vertices_;
};

#endif // vcsl_graph_h_
