// This is gel/gtrl/gtrl_triangle.h
#ifndef gtrl_triangle_h_
#define gtrl_triangle_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author crossge@crd.ge.com

#include <vbl/vbl_smart_ptr.h>
#include <vbl/vbl_ref_count.h>

#include <gtrl/gtrl_vertex.h>

class gtrl_triangle : public vbl_ref_count
{
 public:
  gtrl_triangle( gtrl_vertex_sptr p1, gtrl_vertex_sptr p2, gtrl_vertex_sptr p3);
  // copy constructor - compiler-provided one sets ref_count to nonzero which is wrong -PVr
  gtrl_triangle(gtrl_triangle const& t)
    : vbl_ref_count(),p1_(t.p1_),p2_(t.p2_),p3_(t.p3_),midpoint_(t.midpoint_) {}

  gtrl_vertex_sptr p1() const { return p1_; }
  gtrl_vertex_sptr p2() const { return p2_; }
  gtrl_vertex_sptr p3() const { return p3_; }
  gtrl_vertex_sptr mid_point() const { return midpoint_; }

 protected:
  gtrl_vertex_sptr p1_;
  gtrl_vertex_sptr p2_;
  gtrl_vertex_sptr p3_;

  gtrl_vertex_sptr midpoint_;
};

typedef vbl_smart_ptr<gtrl_triangle> gtrl_triangle_sptr;

#endif // gtrl_triangle_h_
