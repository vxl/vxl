// This is gel/gtrl/gtrl_triangulation.h
#ifndef gtrl_triangulation_h_
#define gtrl_triangulation_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author crossge@crd.ge.com

#include <gtrl/gtrl_polygon.h>
#include <gtrl/gtrl_triangle.h>

//: triangulates holeless polygons

class gtrl_triangulation
{
 public:
  // constructors / destructors
  gtrl_triangulation( gtrl_polygon poly);

  // implementation
  void run();
  vcl_vector<gtrl_triangle_sptr> get_triangles() const { return tris_; }
  vcl_vector<gtrl_vertex_sptr> get_points() const { return pts_; }

 protected:
  gtrl_polygon poly_;

  vcl_vector<gtrl_triangle_sptr> tris_;
  vcl_vector<gtrl_vertex_sptr>   pts_;
};

#endif // gtrl_triangulation_h_
