#ifndef gst_vertex_2d_h_
#define gst_vertex_2d_h_
#ifdef __GNUC__
#pragma interface "gst_vertex_2d"
#endif
/*
  crossge@crd.ge.com
*/

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

protected:

  // members
  double x_, y_;
};

bool operator==( const gst_vertex_2d& a, const gst_vertex_2d& b);

#endif
