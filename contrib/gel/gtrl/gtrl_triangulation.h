#ifndef gtrl_triangulation_h_
#define gtrl_triangulation_h_
#ifdef __GNUC__
#pragma interface "gtrl_triangulation"
#endif
/*
  crossge@crd.ge.com
*/

// triangulates holeless polygons 

#include <gtrl/gtrl_polygon.h>
#include <gtrl/gtrl_triangle.h>

class gtrl_triangulation
{
public:
  // constructors / destructors
  gtrl_triangulation( gtrl_polygon poly);

  // implementation
  void run();
  vcl_vector<gtrl_triangle_ref> get_triangles() const { return tris_; }

protected:
  gtrl_polygon poly_;
  
  vcl_vector<gtrl_triangle_ref> tris_;
};

#endif
