/*
  crossge@crd.ge.com
*/
#ifdef __GNUC__
#pragma implementation "gst_edge_2d"
#endif
#include "gst_edge_2d.h"


// constructor
gst_edge_2d::gst_edge_2d( const gst_vertex_2d_ref start, const gst_vertex_2d_ref end)
  : start_( start), end_( end)
{
}

// getters (no setters)
gst_vertex_2d_ref gst_edge_2d::get_start() const
{
  return start_;
}

gst_vertex_2d_ref gst_edge_2d::get_end() const
{
  return end_;
}
