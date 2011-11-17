#include <testlib/testlib_test.h>
#include <vcl_iostream.h>



#include <vpgl/vpgl_affine_camera.h>
#include <vpgl/io/vpgl_io_affine_camera.h>
#include <vnl/vnl_double_3x4.h>
#include <vpl/vpl.h>
#include <vsl/vsl_binary_io.h>

static void test_affine_camera_io()
{
  vcl_cout << "Testing Projective camera" << vcl_endl;

  // Some matrices for testing.
  double random_list[12] = { 10.6, 1.009, .676, .5,
                             -13, -10, 8, 5, 
                             0.0, 0.0, 0.0, 1.0 };
  vnl_double_3x4 random_matrix( random_list );

  vcl_cout << "Matrix:\n" << random_matrix << vcl_endl;

  vpgl_affine_camera<double> aff_cam( random_matrix );
  aff_cam.set_viewing_distance(1000.0);

  vsl_b_ofstream bp_out("test_affine_camera_io.tmp");
  TEST("Created test_affine_camera_io.tmp for writing",(!bp_out), false);
  vsl_b_write(bp_out, aff_cam);
  bp_out.close();

  // test input binary stream

  vsl_b_ifstream bp_in("test_affine_camera_io.tmp");
  TEST("Opened test_affine_camera_io.tmp for reading",(!bp_in), false);

  vpgl_affine_camera<double> aff_cam_r;
  vsl_b_read(bp_in, aff_cam_r);
  bp_in.close();

  vcl_cout << "Recovered Matrix:\n" << aff_cam_r.get_matrix() << vcl_endl;
  TEST("recovery from binary read", aff_cam.get_matrix(), random_matrix );
  double viewing_dist_r = aff_cam_r.viewing_distance();
  TEST("viewing distance", viewing_dist_r, 1000.0);
  // remove file:
  vpl_unlink("test_affine_camera_io.tmp");
}

TESTMAIN(test_affine_camera_io);
