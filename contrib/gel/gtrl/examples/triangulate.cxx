//:
// \file
// Simple example program to demonstrate the use of the gtrl_triangulation class
// \author Peter Vanroose
// \date   8 October 2002

#include <gtrl/gtrl_triangulation.h>
#include <vcl_iostream.h>

inline vcl_ostream& operator<<(vcl_ostream& os, gtrl_vertex const& v)
{
  return os << '(' << v.x() << ',' << v.y() << ')';
}

inline vcl_ostream& operator<<(vcl_ostream& os, gtrl_triangle const& t)
{
  return os << '[' << *(t.p1()) << ',' << *(t.p2()) << ',' << *(t.p3()) << ']';
}

int main()
{
  vcl_vector<gtrl_vertex_sptr> v(4);
  v[0] = new gtrl_vertex( 1.0, 1.0);
  v[1] = new gtrl_vertex(50.0, 1.0);
  v[2] = new gtrl_vertex(50.0,50.0);
  v[3] = new gtrl_vertex( 1.0,50.0);
  gtrl_polygon poly(v);
  gtrl_triangulation tri(poly); tri.run();

  vcl_vector<gtrl_vertex_sptr> p = tri.get_points();
  vcl_vector<gtrl_triangle_sptr> t = tri.get_triangles();

  vcl_cout << p.size() << " triangle points:\n";
  for (int i=0; i<p.size(); ++i)
    vcl_cout << ' ' << *(p[i]) << '\n';

  vcl_cout << t.size() << " triangles:\n";
  for (int i=0; i<t.size(); ++i)
    vcl_cout << ' ' << *(t[i]) << '\n';

  return 0;
}
