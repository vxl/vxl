#ifndef gst_polygon_2d_operators_h_
#define gst_polygon_2d_operators_h_
#ifdef __GNUC__
#pragma interface "gst_polygon_2d_operators"
#endif
/*
  crossge@crd.ge.com
*/

#include <vcl/vcl_vector.h>

#include <gst/gst_edge_2d_ref.h>
#include <gst/gst_polygon_2d_ref.h>

// Turn a list of edges into a list of polygons.  This is not necessarily
//  efficient, and not unique if the topology is complicated.  If there are
//  shared edges, beware... it might be ok, though.  
//
//  The edges must be corrected oriented (which makes possible shared edges
//  difficult).
vcl_vector<gst_polygon_2d_ref> gst_make_polygons_2d( const vcl_vector<gst_edge_2d_ref> edges);

// Turn a list of edges into a list of polygons.  Each edge can only be used
//   once, BUT orientation is not important.
vcl_vector<gst_polygon_2d_ref> gst_make_polygons_2d_unoriented( const vcl_vector<gst_edge_2d_ref> edges);


#endif
