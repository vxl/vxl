#ifndef vtol_extract_topology_internals_h_
#define vtol_extract_topology_internals_h_

//:
// \file
// \author Amitha Perera
// \date   Dec 2003

// See the comments in vtol_extract_topology.cxx:47 on why this file exists.

#include <vbl/vbl_ref_count.h>

#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_one_chain_sptr.h>

#include <vtol/algo/vtol_extract_topology.h>

//: Stores an edgel chain and a corresponding topological edge
//
// Although the edgel chain can be recovered from the edge, we will
// need the edgel chain often enough that it is worthwhile to cache
// the information.
//
struct vtol_extract_topology::edgel_chain
  : public vbl_ref_count
{
  vdgl_edgel_chain_sptr chain;
  vtol_edge_2d_sptr edge;
};


//: Stores the boundary of a region
//
// This stores the one chain corresponding to a complete region
// boundary. It also stores a point completely inside that
// region. This point is used to perform containment checks between
// regions.
//
// \sa contains
//
class vtol_extract_topology::region_type
  : public vbl_ref_count
{
 public:
  //: Add an edge to this region
  void
  push_back( edgel_chain_sptr chain );

  //: The number of edges in the boundary
  unsigned
  size() const;

  //: Extract segment \a i of the boundary one chain
  vdgl_edgel_chain_sptr const&
  operator[]( unsigned i ) const;

  //: Create a vtol_one_chain describing the boundary
  vtol_one_chain_sptr
  make_one_chain() const;

  //: Location of a pixel inside the region
  unsigned i, j;

 private:

  //: The list of bounday edges (which are edgel chains)
  vcl_vector< edgel_chain_sptr > list_;
};

#endif // vtol_extract_topology_internals_h_
