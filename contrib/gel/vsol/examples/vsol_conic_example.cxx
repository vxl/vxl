// Example use of the vsol_conic_2d class
//
// Author: Peter Vanroose, March 2006

#include <vsol/vsol_conic_2d.h>
#include <vsol/vsol_conic_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vcl_cmath.h>

int main()
{
  // Construct a circle with centre (0,0) and radius 10, i.e., with equation $x^2+y^2=100$:
  vsol_conic_2d_sptr Ell = new vsol_conic_2d(1,0,1,0,0,-100);

  // Verify the type of this circle:
  if (Ell->real_type() == vsol_conic_2d::real_circle)
    vcl_cout << "This conic is really a circle\n";
  vcl_cout << "Its midpoint is (" << Ell->midpoint()->x() << ',' << Ell->midpoint()->y() << ")\n";

  // origin:
  vgl_homg_point_2d<double> origin(0,0);

  // Construct a circle with radius 1, centered at (10,0) :
  Ell = new vsol_conic_2d(vsol_point_2d(10,0), 1,1, 0);
  // Closest distance from origin to circle:
  double Dist = vgl_homg_operators_2d<double>::distance_squared(*Ell , origin);
  vcl_cout << "distance from origin to " << *Ell << "is " << vcl_sqrt(Dist) << '\n';

  // Construct an ellipse with axes lengths 1,2, orientation (1,2), centered at (10,0) :
  Ell = new vsol_conic_2d(vsol_point_2d(10,0), 1,2, vcl_atan2(2.0,1.0));
  // Closest distance from origin to ellipse:
  Dist = vgl_homg_operators_2d<double>::distance_squared(*Ell , origin);
  vcl_cout << "distance from origin to " << *Ell << "is " << vcl_sqrt(Dist) << '\n';

  if (Ell->real_type() == vsol_conic_2d::real_ellipse)
    vcl_cout << "The second conic with is really an ellipse\n";
  vcl_cout << "Its midpoint is (" << Ell->midpoint()->x() << ',' << Ell->midpoint()->y() << ")\n"
           << "Its equation is "  << Ell->a() << " X^2 + "
                                  << Ell->b() << " XY + "
                                  << Ell->c() << " Y^2 + "
                                  << Ell->d() << " X + "
                                  << Ell->e() << " Y + "
                                  << Ell->f() << '\n';

  // Calculate the curve length of the ellipse:
  vcl_cout << "The length of the ellipse is " << Ell->length() << '\n';

  // Construct a hyperbola with axes lengths 1,-2, orientation (1,2) centered at (10,0) :
  Ell = new vsol_conic_2d(vsol_point_2d(10,0), 1,-2, vcl_atan2(2.0,1.0));
  // Closest distance from origin to hyperbola:
  Dist = vgl_homg_operators_2d<double>::distance_squared(*Ell , origin);
  vcl_cout << "distance from origin to " << *Ell << "is " << vcl_sqrt(Dist) << '\n';

  // Construct a parabola with top (10,0), direction (1,2), excentricity 5 :
  Ell = new vsol_conic_2d(vgl_vector_2d<double>(1,2), vsol_point_2d(10,0), 5);
  // Closest distance from origin to parabola:
  Dist = vgl_homg_operators_2d<double>::distance_squared(*Ell , origin);
  vcl_cout << "distance from origin to " << *Ell << "is " << vcl_sqrt(Dist) << '\n';

  return 0;
}
