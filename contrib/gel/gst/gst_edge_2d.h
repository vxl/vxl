#ifndef gst_edge_2d_h_
#define gst_edge_2d_h_
#ifdef __GNUC__
#pragma interface "gst_edge_2d"
#endif
/*
  crossge@crd.ge.com
*/

#include <vbl/vbl_ref_count.h>

#include <gst/gst_vertex_2d_ref.h>

class gst_edge_2d : public vbl_ref_count
{
public:

  gst_edge_2d( const gst_vertex_2d_ref start, const gst_vertex_2d_ref end);

  gst_vertex_2d_ref get_start() const;
  gst_vertex_2d_ref get_end() const;

protected:

  gst_vertex_2d_ref start_;
  gst_vertex_2d_ref end_;

};



#endif
