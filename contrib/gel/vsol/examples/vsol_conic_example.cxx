#include <vsol/vsol_conic_2d.h>
#include <vsol/vsol_conic_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vgl/algo/vgl_homg_operators_2d.h>
#include <vcl_cmath.h>

int main()
{
  // origin:
  vgl_homg_point_2d<double> origin(0,0);

  // circle, radius 1, centered at (10,0) :
  vsol_conic_2d_sptr Ell = new vsol_conic_2d(vsol_point_2d(10,0), 1,1, 0);
  // distance:
  double Dist = vgl_homg_operators_2d<double>::distance_squared(*(Ell) , origin);
  vcl_cout << "distance from origin to " << *Ell << "is " << vcl_sqrt(Dist) << '\n';

  // ellipse, axes 1,2, orientation (1,2), centered at (10,0) :
  Ell = new vsol_conic_2d(vsol_point_2d(10,0), 1,2, vcl_atan2(2.0,1.0));
  // distance:
  Dist = vgl_homg_operators_2d<double>::distance_squared(*(Ell) , origin);
  vcl_cout << "distance from origin to " << *Ell << "is " << vcl_sqrt(Dist) << '\n';

  // hyperbola, axes 1,-2, orientation (1,2) centered at (10,0) :
  Ell = new vsol_conic_2d(vsol_point_2d(10,0), 1,-2, vcl_atan2(2.0,1.0));
  // distance:
  Dist = vgl_homg_operators_2d<double>::distance_squared(*(Ell) , origin);
  vcl_cout << "distance from origin to " << *Ell << "is " << vcl_sqrt(Dist) << '\n';

  // parabola, top (10,0), direction (1,2), excentricity 5 :
  Ell = new vsol_conic_2d(vgl_vector_2d<double>(1,2), vsol_point_2d(10,0), 5);
  // distance:
  Dist = vgl_homg_operators_2d<double>::distance_squared(*(Ell) , origin);
  vcl_cout << "distance from origin to " << *Ell << "is " << vcl_sqrt(Dist) << '\n';

  return 0;
}
