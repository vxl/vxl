// This is gel/gst/gst_edge_2d.h
#ifndef gst_edge_2d_h_
#define gst_edge_2d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author crossge@crd.ge.com

#include <vcl_iosfwd.h>
#include <vbl/vbl_ref_count.h>

#include "gst_vertex_2d_sptr.h"

class gst_edge_2d : public vbl_ref_count
{
 public:
  // constructor
  gst_edge_2d( const gst_vertex_2d_sptr start, const gst_vertex_2d_sptr end)
  : start_( start), end_( end) {}

  // getters (no setters)

  gst_vertex_2d_sptr get_start() const { return start_; }
  gst_vertex_2d_sptr get_end() const { return end_; }

  // operations
  void flip() { gst_vertex_2d_sptr temp= start_; start_= end_; end_= temp; }

  // display
  friend vcl_ostream &operator<<( vcl_ostream &os, gst_edge_2d &e);

 protected:
  gst_vertex_2d_sptr start_;
  gst_vertex_2d_sptr end_;
};

vcl_ostream &operator<<( vcl_ostream &os, gst_edge_2d &e);

#endif // gst_edge_2d_h_
