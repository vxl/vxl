// This is gel/gst/gst_vertex_2d.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author crossge@crd.ge.com

#include "gst_vertex_2d.h"

// default constructor
gst_vertex_2d::gst_vertex_2d()
{
}

// useful constructor
gst_vertex_2d::gst_vertex_2d( const double x, const double y)
  : vbl_ref_count(), x_(x), y_(y)
{
}

// copy constructor
gst_vertex_2d::gst_vertex_2d( const gst_vertex_2d &v)
  : vbl_ref_count(), x_(v.get_x()), y_(v.get_y())
{
}

// destructor
gst_vertex_2d::~gst_vertex_2d()
{
}

// getters
double gst_vertex_2d::get_x() const
{
  return x_;
}

double gst_vertex_2d::get_y() const
{
  return y_;
}

// setters
void gst_vertex_2d::set_x( const double x)
{
  x_= x;
}

void gst_vertex_2d::set_y( const double y)
{
  y_= y;
}

void gst_vertex_2d::set( const double x, const double y)
{
  x_=x; y_=y;
}

// operators
bool operator==( const gst_vertex_2d& a, const gst_vertex_2d& b)
{ return a.x_== b.x_ && a.y_== b.y_; }


// output
vcl_ostream &operator <<( vcl_ostream &os, const gst_vertex_2d &v)
{
  return os << v.x_ << ' ' << v.y_;
}
