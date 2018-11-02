//:
// \file
// Simple example program to demonstrate the use of the gtrl_triangulation class
// \author Peter Vanroose
// \date   8 October 2002

#include <iostream>
#include <gtrl/gtrl_triangulation.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

inline std::ostream& operator<<(std::ostream& os, gtrl_vertex const& v)
{
  return os << '(' << v.x() << ',' << v.y() << ')';
}

inline std::ostream& operator<<(std::ostream& os, gtrl_triangle const& t)
{
  return os << '[' << *(t.p1()) << ',' << *(t.p2()) << ',' << *(t.p3()) << ']';
}

int main()
{
  std::vector<gtrl_vertex_sptr> v(4);
  v[0] = new gtrl_vertex( 1.0, 1.0);
  v[1] = new gtrl_vertex(50.0, 1.0);
  v[2] = new gtrl_vertex(50.0,50.0);
  v[3] = new gtrl_vertex( 1.0,50.0);
  gtrl_polygon poly(v);
  gtrl_triangulation tri(poly); tri.run();

  std::vector<gtrl_vertex_sptr> p = tri.get_points();
  std::vector<gtrl_triangle_sptr> t = tri.get_triangles();

  std::cout << p.size() << " triangle points:\n";
  for (auto & i : p)
    std::cout << ' ' << *i << '\n';

  std::cout << t.size() << " triangles:\n";
  for (auto & i : t)
    std::cout << ' ' << *i << '\n';

  return 0;
}
