#include <testlib/testlib_test.h>
// not used? #include <vcl_iostream.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vnl/vnl_fwd.h>

static void test_calibration_matrix()
{
  double focal_length = 3;
  vgl_homg_point_2d<double> principal_point(20,30,2);
  double pixel_aspect_ratio = 1;
  double skew = 0;
  vpgl_calibration_matrix<double> K1( focal_length, principal_point,
    pixel_aspect_ratio, skew );

  // Test equality of constructors.
  vnl_matrix_fixed<double,3,3> M( K1.get_matrix() * -100.0 );
  vpgl_calibration_matrix<double> K1b(M);

  TEST_NEAR( "test equality of constructors 1", 
             K1.get_matrix() == K1b.get_matrix(), true, 1e-06);
  TEST_NEAR( "test equality of constructors 2",
             K1.focal_length() == K1b.focal_length() &&
             K1.pixel_aspect_ratio() == K1b.pixel_aspect_ratio() &&
             K1.skew() == K1b.skew(), true, 1e-06);

  // Test the focal length setter.
  focal_length = 5;
  vpgl_calibration_matrix<double> K2( focal_length, principal_point,
    pixel_aspect_ratio, skew );
  K1.set_focal_length( focal_length );
  TEST_NEAR( "test focal length setter",
             K1.get_matrix() == K2.get_matrix(), true, 1e-06);

  // Test the skew setter.
  skew = 2;
  vpgl_calibration_matrix<double> K3( focal_length, principal_point,
    pixel_aspect_ratio, skew );
  K1.set_skew( skew );
  TEST_NEAR( "test skew setter", 
             K1.get_matrix() == K3.get_matrix(), true, 1e-06);

  // Test the pixel aspect ratio setter.
  pixel_aspect_ratio = 0.8;
  vpgl_calibration_matrix<double> K4( focal_length, principal_point,
    pixel_aspect_ratio, skew );
  K1.set_pixel_aspect_ratio( pixel_aspect_ratio );
  TEST_NEAR( "test pixel_aspect_ratio setter",
             K1.get_matrix() == K4.get_matrix(), true, 1e-06);

  // Test the principal point setter.
  principal_point.set(17,100);
  vpgl_calibration_matrix<double> K5( focal_length, principal_point,
    pixel_aspect_ratio, skew );
  K1.set_principal_point( principal_point );
  TEST_NEAR( "test principal point setter", 
             K1.get_matrix() == K5.get_matrix(), true, 1e-06);
}

TESTMAIN(test_calibration_matrix);
