#ifndef gtrl_triangle_h_
#define gtrl_triangle_h_
#ifdef __GNUC__
#pragma interface "gtrl_triangle"
#endif
/*
  crossge@crd.ge.com
*/

#include <vbl/vbl_smart_ptr.h>
#include <vbl/vbl_ref_count.h>

#include <gtrl/gtrl_vertex.h>

class gtrl_triangle : public vbl_ref_count
{
public:
  gtrl_triangle( gtrl_vertex_ref p1, gtrl_vertex_ref p2, gtrl_vertex_ref p3);

  gtrl_vertex_ref mid_point() const { return midpoint_; }

protected:
  gtrl_vertex_ref p1_;
  gtrl_vertex_ref p2_;
  gtrl_vertex_ref p3_;

  gtrl_vertex_ref midpoint_;
};

typedef vbl_smart_ptr<gtrl_triangle> gtrl_triangle_ref;

#endif
