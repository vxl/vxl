#ifndef gtrl_vertex_h_
#define gtrl_vertex_h_
#ifdef __GNUC__
#pragma interface "gtrl_vertex"
#endif
/*
  crossge@crd.ge.com
*/

#include <vbl/vbl_smart_ptr.h>
#include <vbl/vbl_ref_count.h>

class gtrl_vertex : public vbl_ref_count
{
protected:
  gtrl_vertex( const double x, const double y);

  double x() const { return x_;}
  double y() const { return y_;}

  void set_x( const double x) { x_= x; }
  void set_y( const double y) { y_= y; }

public:
  double x_;
  double y_;
};

typedef vbl_smart_ptr<gtrl_vertex> gtrl_vertex_ref;

#endif
