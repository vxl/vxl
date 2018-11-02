#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_lvcs.h>
#include <vpgl/algo/vpgl_ray.h>

static void test_ray()
{
  //Make the rational cameras
  //Rational polynomial coefficients
  //from digital globe image  02JUL05075233-P1BS-005630613010_01_P001
  double neu_u[20] =
    { -0.0000168813 , 0.0000772759 , -.00000105831, -.000212391,
      -0.000025837  , 0.00000225817, -.00186827   , -.00000791212,
       0.000624679  , 1.01066      , -.000167775  , 0.0000130219,
      -0.00228311   , -.00000058781, -.000342256  , 0.0143708,
      .0000000762963, 0.0000110481 ,  .00381736   , -.00121939};
  double den_u[20] =
    { -.000000130931, 0.000000169418 , .000000020272, -.00000145873,
       .00000211783 , -.000000215762 , -.0000635984 , -.0000000107058,
       .00000349203 , 0.00147117     , -.00000286589, 0.000000268989,
       .0000119205  , -.0000000465223, -.00000204604, 0.00193974,
       .000000015139, -.00000836577  , -.000621847  , 1};
  double neu_v[20] =
    { .000000160189,  .0000175875 , -.000000203537, -.000717374,
      .0000278957  , -.00000105314, -.000112593   ,  .000000507345,
      .00000874784 , -.0228769    ,  .0000773345  , -.00000814762,
      0.00318176   ,  .0000220927 ,  .0000782713  , -1.00092,
     -.000000530236, -.00000517096,  .022347      , -0.00259056};
  double den_v[20] =
    { .0000000479097, -.000000148662 , -.0000000133299, -.0000166728,
     -.0000212508   ,  .000000082811 ,  .0000198373   ,  .0000000263071,
     -.000000566165 , -.000188585    , -.000302309    ,  .00000246881,
      .0000509931   ,  .000000274573 , -.00000463648  ,  .000667711,
      .00000012639  , -.0000219341   ,  .000132914    , 1};
  //Scale and offsets

  double sx = 0.1039, ox = 44.3542;
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
  std::cout << "Ray Direction " << v << '\n';
  double x_y = v.x()/v.y();

  TEST_NEAR("test ray_direction", x_y,-0.16767109029721888, 0.002);

  // test ray for local rational camera
  vpgl_lvcs lvcs(33.1855, 44.3542, 32, vpgl_lvcs::wgs84, vpgl_lvcs::DEG, vpgl_lvcs::METERS);

  vpgl_local_rational_camera<double> lrcam(lvcs, rcam);

  vgl_point_3d<double> orig;
  vgl_vector_3d<double> dir;

  bool good = vpgl_ray::ray(lrcam, ou, ov, orig, dir);
  TEST("ray()", good, true);
  std::cout << "Orig: " << orig << "\n Dir: " << dir << '\n';
  vgl_vector_3d<double> tdir(0.0689725,-0.487625,-0.870325);
  double err = (tdir-dir).length();
  TEST_NEAR("test local rational ray_direction", err, 0.0, 1e-6);

  vgl_point_2d<double> impt1(ou,ov);
  vgl_point_2d<double> impt2(ou+1,ov);

  vgl_plane_3d<double> outp;

  vpgl_ray::plane_ray(lrcam,impt1,impt2,outp);
  err=dot_product<double>(outp.normal(),dir);
  TEST_NEAR("test local rational plane", err, 0.0, 1e-8);
}

TESTMAIN(test_ray);
