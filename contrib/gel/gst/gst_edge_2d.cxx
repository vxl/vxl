// This is gel/gst/gst_edge_2d.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author crossge@crd.ge.com

#include "gst_edge_2d.h"

// constructor
gst_edge_2d::gst_edge_2d( const gst_vertex_2d_sptr start, const gst_vertex_2d_sptr end)
  : start_( start), end_( end)
{
}

// getters (no setters)
gst_vertex_2d_sptr gst_edge_2d::get_start() const
{
  return start_;
}

gst_vertex_2d_sptr gst_edge_2d::get_end() const
{
  return end_;
}

// operations
void gst_edge_2d::flip()
{
  gst_vertex_2d_sptr temp= start_;
  start_= end_;
  end_= temp;
}

// output
vcl_ostream &operator<<( vcl_ostream &os, gst_edge_2d &e)
{
  return os << "<" << e.start_.ptr() << "," << e.end_.ptr() << ">";
}
