// This is gel/gtrl/gtrl_vertex.h
#ifndef gtrl_vertex_h_
#define gtrl_vertex_h_
//:
// \file
// \author crossge@crd.ge.com
//
// \verbatim
//  Modifications
//   10 Sep. 2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

#include <vbl/vbl_smart_ptr.h>
#include <vbl/vbl_ref_count.h>

class gtrl_vertex : public vbl_ref_count
{
 public:
  gtrl_vertex( double x, double y) : x_(x), y_(y) {}

  double x() const { return x_; }
  double y() const { return y_; }

  void set_x( const double x) { x_= x; }
  void set_y( const double y) { y_= y; }

 protected:
  double x_;
  double y_;
};

typedef vbl_smart_ptr<gtrl_vertex> gtrl_vertex_sptr;

#endif // gtrl_vertex_h_
