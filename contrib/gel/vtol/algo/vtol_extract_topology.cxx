// This is gel/vtol/algo/vtol_extract_topology.cxx
#include "vtol_extract_topology.h"
//:
// \file
// \author Amitha Perera
// \date   July 2003

#include <vcl_iosfwd.h>
#include <vcl_cassert.h>

#include <vil/algo/vil_region_finder.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/algo/vgl_line_2d_regression.h>

#include <vtol/vtol_vertex_2d.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_one_chain.h>
#include <vtol/vtol_intensity_face.h>

#include <vsol/vsol_curve_2d_sptr.h>

#include <vdgl/vdgl_edgel.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_digital_region.h>

#ifndef NDEBUG
#  include <vcl_iostream.h>
#  define DEBUG( x ) x;
#else
#  define DEBUG( x ) /*debugging removed*/ do {} while (0)
#endif


// =============================================================================
//                                                                   VERTEX NODE
// =============================================================================


// ---------------------------------------------------------------------------
//                                                                 constructor

// If we include vbl_smart_ptr.txx in this file, Borland 55 will try
// to instantiate vbl_smart_ptr<vsol_point_2d> and fail because
// vsol_point_2d is incomplete. It seems to make the attempt because
// of the vtol_vertex_2d constructor call. Any call to a function in
// vtol_vertex_2d seems to cause the instantiation. I'm not sure if we
// are running into a compiler bug or missing some subtle detail in
// templates with incomplete types. -- Amitha Perera
//
// Uncommenting the following line will trigger the bug(?) with Borland.
//#include <vbl/vbl_smart_ptr.txx>

vtol_extract_topology_vertex_node::
vtol_extract_topology_vertex_node( unsigned in_i, unsigned in_j )
  : i( in_i ),
    j( in_j ),
    vertex( new vtol_vertex_2d( i-0.5, j-0.5 ) )
{
  // The edge[4] will already be default constructed to null. We need
  // to explicitly initialize the link[4] array since it is a built-in
  // type and thus will not be zero-initialized.
  for ( unsigned i = 0; i < 4; ++i ) {
    link[i] = null_index;
    back_dir[i] = 100;
  }
}


// =============================================================================
//                                                                   REGION TYPE
// =============================================================================


// ---------------------------------------------------------------------------
//                                                                   push back

void
vtol_extract_topology_region_type::
push_back( edgel_chain_sptr chain )
{
  list_.push_back( chain );
}


// ---------------------------------------------------------------------------
//                                                                        size

unsigned
vtol_extract_topology_region_type::
size() const
{
  return list_.size();
}


// ---------------------------------------------------------------------------
//                                                                 operator []

vdgl_edgel_chain_sptr const&
vtol_extract_topology_region_type::
operator[]( unsigned i ) const
{
  return list_[i]->chain;
}


// ---------------------------------------------------------------------------
//                                                              make one chain

vtol_one_chain_sptr
vtol_extract_topology_region_type::
make_one_chain( ) const
{
  vcl_vector< vtol_edge_sptr > edges;

  for ( unsigned i = 0; i < list_.size(); ++i ) {
    edges.push_back( &*list_[i]->edge );
  }

  return new vtol_one_chain( edges, /*is_cycle=*/ true );
}


// =============================================================================
//                                                          NON-MEMBER FUNCTIONS
// =============================================================================

