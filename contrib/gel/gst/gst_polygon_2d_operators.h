// This is gel/gst/gst_polygon_2d_operators.h
#ifndef gst_polygon_2d_operators_h_
#define gst_polygon_2d_operators_h_
//:
// \file
// \author crossge@crd.ge.com

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include "gst_edge_2d_sptr.h"
#include "gst_polygon_2d_sptr.h"

//: Turn a list of edges into a list of polygons.
// This is not necessarily efficient, and not unique if the topology
// is complicated.  If there are shared edges, beware... it might be ok, though.
//
//  The edges must be correctly oriented (which makes possible shared edges
//  difficult).
std::vector<gst_polygon_2d_sptr> gst_make_polygons_2d( const std::vector<gst_edge_2d_sptr>& edges);

//: Turn a list of edges into a list of polygons.
// Each edge can only be used  once, BUT orientation is not important.
std::vector<gst_polygon_2d_sptr> gst_make_polygons_2d_unoriented( const std::vector<gst_edge_2d_sptr>& edges);

#endif // gst_polygon_2d_operators_h_
