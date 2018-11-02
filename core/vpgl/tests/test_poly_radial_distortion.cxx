#include <iostream>
#include <exception>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vpgl/vpgl_poly_radial_distortion.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_homg_point_2d.h>

static void test_poly_radial_distortion_constructors()
{
    bool did_construct = false;
    try
    {
        vpgl_poly_radial_distortion<double,3> lens (vgl_point_2d<double>(100.0,100.0), nullptr );
        did_construct = true;
    }
    catch( std::exception& exception )
    {
                std::cout << exception.what() << std::endl;
    }
    TEST( "Centre & Coefficients Constructor with NULL pointer", did_construct, true );

}

static void test_poly_radial_distortion()
{
    test_poly_radial_distortion_constructors();

  double k[3] = {0.5, 2.0, 3.0};
  vpgl_poly_radial_distortion<double,3> rd(vgl_point_2d<double>(100.0,100.0), k);
  TEST("distort r=0",rd.distort_radius(0), 1);
  double r1 = rd.distort_radius(1)*1;
  double r2 = rd.distort_radius(2)*2;
  TEST("distort r=1",r1, 6.5);
  TEST("distort r=2",r2, 68.0);

  TEST("undistort r=0", rd.undistort_radius(0), 1.0);
  TEST_NEAR("undistort r=1", rd.undistort_radius(r1)*r1, 1.0, 1e-8);
  TEST_NEAR("undistort r=2", rd.undistort_radius(r2)*r2, 2.0, 1e-8);


  vgl_homg_point_2d<double> pt(101.0, 101.0);
  std::cout << "distort point "<<rd.distort(pt) << std::endl;
  std::cout << "undistort point "<<rd.undistort(pt) << std::endl;
}


TESTMAIN(test_poly_radial_distortion);
