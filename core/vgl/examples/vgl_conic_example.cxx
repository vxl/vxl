// This is an example of how to use vgl_conic,
// written by Peter Vanroose, ESAT, K.U.Leuven, Belgium, 5 October 2001.
#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vgl/vgl_conic.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>

int main()
{
  // Select a point to be used as the centre for the conic (homogeneous notation)
  vgl_homg_point_2d<double> const centre(1,2,1);

  // circle, centre (1,2), radius 1, orientation irrelevant.
  vgl_conic<double> c(centre, 1,1, 0);
  vcl_cout << c << vcl_endl;
  vcl_string name = c.real_type();
  assert(name == "real circle");

  // same circle, now given by its equation
  vgl_conic<double> cc (1,0,1,-2,-4,4);
  assert(c == cc);

  // Return the centre point of a central conic, or its point at infinity if a parabola:
  assert(c.centre() == centre);

  // Verify that a point lies on a conic:
  vgl_homg_point_2d<double> npt(0,2,1);
  assert(c.contains(npt));

  // Tangent lines to the circle from npt: (only one, since npt lies on the circle)
  vcl_list<vgl_homg_line_2d<double> > lines = vgl_homg_operators_2d<double>::tangent_from(c, npt);
  assert(lines.front() == vgl_homg_line_2d<double>(1,0,0)); // The Y axis

  // Bounding box of this circle:
  assert(vgl_homg_operators_2d<double>::compute_bounding_box(c) == vgl_box_2d<double>(0,2,1,3));

  // Closest point on the circle to the point (1,20) is (1,3):
  npt = vgl_homg_operators_2d<double>::closest_point(c, vgl_homg_point_2d<double>(1.0,20.0,1.0));
  assert(npt == vgl_homg_point_2d<double>(1.0,3.0,1.0));

  // ellipse, centre (1,2), axes lengths 2,1, rotated by 1 radian counterclockwise:
  c = vgl_conic<double>(centre, 2,1, 1.0);
  vcl_cout << c << vcl_endl;

  // hyperbola, centre (1,2), axes lengths 2,1, not rotated:
  c = vgl_conic<double>(centre, 2,-1, 0.0); // assignment
  vcl_cout << c << vcl_endl;

  // parabola, "centre" (1,2,0) (at infinity), top (2,1), width parameter 3.
  c = vgl_conic<double>(vgl_homg_point_2d<double>(1,2,0), 2,1, 3);
  vcl_cout << c << vcl_endl;
  assert(c.centre() == vgl_homg_point_2d<double>(1,2,0));

  // imaginary circle, centre (1,2), radius 5i.
  c = vgl_conic<double>(1, 0, 1, -2, -4, 30);
  vcl_cout << c << '\n';
  assert(c.real_type() == "imaginary circle");

  // imaginary ellipse, centre (1,2), axes lengths 5i, 10i, orientation (1,0).
  c = vgl_conic<double>(4, 0, 1, -8, -4, 108);
  vcl_cout << c << '\n';
  assert(c.real_type() == "imaginary ellipse");

  // real parallel lines:
  c = vgl_conic<double>(1,4,4,8,16,15);
  vcl_cout << c << '\n';
  assert(c.real_type() == "real parallel lines");
  vgl_homg_line_2d<double> l1(1,2,3), l2(1,2,5);
  // The components of a degenerate conic are the two constituent lines:
  lines = c.components();
  assert(lines.front() == l1); assert(lines.back() == l2); // or vice versa

  // imaginary parallel lines:
  c = vgl_conic<double>(4,4,1,0,0,9); // lines 2x+y+/-3iw=0
  vcl_cout << c << '\n';
  assert(c.real_type() == "complex parallel lines");
  lines = c.components(); assert(lines.size() == 0); // no real components

  // finite coincident lines
  c = vgl_conic<double>(1,4,4,6,12,9);
  vcl_cout << c << '\n';
  assert(c.real_type() == "coincident lines");
  l1 = vgl_homg_line_2d<double>(1,2,3);
  lines = c.components();
  assert(lines.front() == l1); assert(lines.back() == l1); // 2x the same line

  // infinite coincident lines
  c = vgl_conic<double>(0,0,0,0,0,1);
  vcl_cout << c << '\n';
  assert(c.real_type() == "coincident lines");
  lines = c.components();
  l1 = vgl_homg_line_2d<double>(0,0,1); // the line at infinity
  assert(lines.front() == l1); assert(lines.back() == l1);

  // finite, real intersecting lines:
  c = vgl_conic<double>(2,3,-2,7,-1,3);
  vcl_cout << c << '\n';
  assert(c.real_type() == "real intersecting lines");
  l1 = vgl_homg_line_2d<double>(1,2,3);
  l2 = vgl_homg_line_2d<double>(2,-1,1);
  lines = c.components();
  assert(lines.front() == l2); assert(lines.back() == l1); // or vice versa

  // imaginary intersecting lines:
  c = vgl_conic<double>(4,0,1,12,0,9); // lines 2x+/-iy+3w=0
  vcl_cout << c << '\n';
  assert(c.real_type() == "complex intersecting lines");
  lines = c.components(); assert(lines.size() == 0); // no real components

  // one finite and one infinite intersecting line:
  c = vgl_conic<double>(0,0,0,1,2,3);
  vcl_cout << c << '\n';
  assert(c.real_type() == "real intersecting lines");
  l1 = vgl_homg_line_2d<double>(1,2,3);
  l2 = vgl_homg_line_2d<double>(0,0,1); // line at infinity
  lines = c.components();
  assert(lines.front() == l2); assert(lines.back() == l1); // or vice versa

  return 0;
}
