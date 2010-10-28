#include <testlib/testlib_test.h>
#include <bapl/bapl_affine_transform.h>
#include <vnl/vnl_double_2.h>

#include <vcl_iostream.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vpgl/vpgl_perspective_camera.h>

MAIN( test_transform )
{
  START ("affine transform");

  bapl_affine_transform T;
  vnl_double_2 p(13.3, -101.23);
  
  TEST("Identity Transform",p,T*p);

  SUMMARY();
}
