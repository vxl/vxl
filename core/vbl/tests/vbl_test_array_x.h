#ifndef vbl_test_array_x_h_
#define vbl_test_array_x_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief A simple object to test a container.

#include <vcl_iostream.h>

class vbl_test_array_x
{
  double x_;
 public:
  double x() const { return x_; }
  void set_x(double d)  { x_=d; }
  vbl_test_array_x() : x_(0.0) { } // NB default constructor needed for vbl_array_2d<vbl_test_array_x>
  vbl_test_array_x(int a, int b) : x_(a + b) { }
  vbl_test_array_x(float a, float b) : x_(a + b) { }
  vbl_test_array_x(double a, double b) : x_(a + b) { }
  void method() const { vcl_cout << '[' << x_ << ']' << '\n' << vcl_flush; }
  bool operator==(vbl_test_array_x const& y) const { return x_ == y.x(); }
};

inline vcl_ostream& operator<<(vcl_ostream& is, vbl_test_array_x const& a) { return is << a.x(); }
inline vcl_istream& operator>>(vcl_istream& is, vbl_test_array_x & a) { double d; is >> d; a.set_x(d); return is;}
#endif
