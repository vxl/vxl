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
  gst_edge_2d(gst_vertex_2d_sptr start, gst_vertex_2d_sptr end)
  : start_(start), end_(end) {}

  // copy constructor - compiler-provided one sets ref_count to nonzero which is wrong -PVr
  gst_edge_2d(gst_edge_2d const& e)
  : vbl_ref_count(), start_(e.start_), end_(e.end_) {}

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
