#ifndef gst_edge_2d_h_
#define gst_edge_2d_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  crossge@crd.ge.com
*/

#include <vcl_iostream.h>
#include <vbl/vbl_ref_count.h>

#include "gst_vertex_2d_sptr.h"

class gst_edge_2d : public vbl_ref_count
{
public:

  gst_edge_2d( const gst_vertex_2d_sptr start, const gst_vertex_2d_sptr end);

  gst_vertex_2d_sptr get_start() const;
  gst_vertex_2d_sptr get_end() const;

  // operations
  void flip();

  // display
  friend vcl_ostream &operator<<( vcl_ostream &os, gst_edge_2d &e);

protected:

  gst_vertex_2d_sptr start_;
  gst_vertex_2d_sptr end_;

};

vcl_ostream &operator<<( vcl_ostream &os, gst_edge_2d &e);

#endif
