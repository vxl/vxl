// This is gel/gst/gst_vertex_2d.h
#ifndef gst_vertex_2d_h_
#define gst_vertex_2d_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author crossge@crd.ge.com

#include <vcl_iostream.h>
#include <vbl/vbl_ref_count.h>

class gst_vertex_2d : public vbl_ref_count
{
 public:

  // constructors and destructors
  gst_vertex_2d();
  gst_vertex_2d( const double x, const double y);
  gst_vertex_2d( const gst_vertex_2d &v);
  ~gst_vertex_2d();

  // getters and setters
  double get_x() const;
  double get_y() const;
  void set_x( const double x);
  void set_y( const double y);
  void set( const double x, const double y);

  // operators
  friend bool operator==( const gst_vertex_2d& a, const gst_vertex_2d& b);

  // output
  friend vcl_ostream &operator <<( vcl_ostream &os, const gst_vertex_2d &v);

 protected:

  // members
  double x_, y_;
};

bool operator==( const gst_vertex_2d& a, const gst_vertex_2d& b);
vcl_ostream &operator <<( vcl_ostream &os, const gst_vertex_2d &v);

#endif // gst_vertex_2d_h_
