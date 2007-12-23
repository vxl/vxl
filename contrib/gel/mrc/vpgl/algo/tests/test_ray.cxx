#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/algo/vpgl_ray.h>

static void test_ray()
{
  //Make the rational cameras
  //Rational polynomial coefficients
  //from digital globe image  02JUL05075233-P1BS-005630613010_01_P001
  double neu_u[20] =
    { -1.68813e-005 , 7.72759e-005 , -1.05831e-006 , -0.000212391,
      -2.5837e-005 , 2.25817e-006 , -0.00186827 , -7.91212e-006,
      0.000624679 , 1.01066 , -0.000167775 , 1.30219e-005,
      -0.00228311 , -5.8781e-007 , -0.000342256 , 0.0143708,
      7.62963e-008 , 1.10481e-005 , 0.00381736 , -0.00121939};
  double den_u[20] =
    { -1.30931e-007 , 1.69418e-007 , 2.0272e-008 , -1.45873e-006,
      2.11783e-006 , -2.15762e-007 , -6.35984e-005 , -1.07058e-008,
      3.49203e-006 , 0.00147117 , -2.86589e-006 , 2.68989e-007,
      1.19205e-005 , -4.65223e-008 , -2.04604e-006 , 0.00193974,
      1.5139e-008 , -8.36577e-006 , -0.000621847 , 1};
  double neu_v[20] =
    { 1.60189e-007 , 1.75875e-005 , -2.03537e-007 , -0.000717374,
      2.78957e-005 , -1.05314e-006 , -0.000112593 , 5.07345e-007,
      8.74784e-006 , -0.0228769 , 7.73345e-005 , -8.14762e-006,
      0.00318176 , 2.20927e-005 , 7.82713e-005 , -1.00092,
      -5.30236e-007 , -5.17096e-006 , 0.022347 , -0.00259056};
  double den_v[20] =
    { 4.79097e-008 , -1.48662e-007 , -1.33299e-008 , -1.66728e-005,
      -2.12508e-005 , 8.2811e-008 , 1.98373e-005 , 2.63071e-008,
      -5.66165e-007 , -0.000188585 , -0.000302309 , 2.46881e-006,
      5.09931e-005 , 2.74573e-007 , -4.63648e-006 , 0.000667711,
      1.2639e-007 , -2.19341e-005 , 0.000132914 , 1};
  //Scale and offsets

  double sx = 0.1039, ox = 44.3542 ;
  double sy = 0.1129, oy = 33.1855;
  double sz = 501, oz = 32;
  double su = 13834, ou = 13792;
  double sv = 15787, ov = 15430;

  vpgl_rational_camera<double> rcam(neu_u, den_u, neu_v, den_v,
                                     sx, ox, sy, oy, sz, oz,
                                     su, ou, sv, ov);
  vgl_point_3d<double> p3d(ox, oy, oz);
  vgl_vector_3d<double> v;
  vpgl_ray::ray(rcam, p3d, v); 
  vcl_cout << "Ray Direction " << v << '\n';
  double x_y = v.x()/v.y();

 TEST_NEAR("test ray_direction", x_y,-0.16767109029721888, 0.001);
}

TESTMAIN(test_ray);
