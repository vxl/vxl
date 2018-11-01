// This is gel/gst/gst_vertex_2d.h
#ifndef gst_vertex_2d_h_
#define gst_vertex_2d_h_
//:
// \file
// \author crossge@crd.ge.com

#include <iostream>
#include <iosfwd>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_ref_count.h>

class gst_vertex_2d : public vbl_ref_count
{
 public:

  // default constructor
  gst_vertex_2d() = default;

  // useful constructor
  gst_vertex_2d(double x, double y) : vbl_ref_count(), x_(x), y_(y) {}

  // copy constructor
  gst_vertex_2d( const gst_vertex_2d &v) : vbl_ref_count(), x_(v.get_x()), y_(v.get_y()) {}

  // destructor
  ~gst_vertex_2d() override = default;

  // getters
  double get_x() const { return x_; }
  double get_y() const { return y_; }

  // setters
  void set_x(double x) { x_= x; }
  void set_y(double y) { y_= y; }
  void set(double x, double y) { x_=x; y_=y; }

  // operators
  friend inline bool operator==(const gst_vertex_2d& a, const gst_vertex_2d& b);

 protected:

  // members
  double x_, y_;
};

// operators
bool operator==(const gst_vertex_2d& a, const gst_vertex_2d& b)
{ return a.x_== b.x_ && a.y_== b.y_; }

std::ostream &operator <<( std::ostream &os, const gst_vertex_2d &v);

#endif // gst_vertex_2d_h_
