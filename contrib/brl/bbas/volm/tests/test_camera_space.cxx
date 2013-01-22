#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <bbas/volm/volm_camera_space.h>

static void test_camera_space()
{

  double head_mid=0.0, head_radius=180.0, head_inc=45.0;
  double tilt_mid=90.0, tilt_radius=20.0, tilt_inc=10.0;
  double roll_mid=0.0,  roll_radius=3.0,  roll_inc=3.0;
  double top_fov_vals[] = {3.0,  4.0, 5.0, 12.0, 17.0, 18.0,19.0, 20.0, 24.0};
  vcl_vector<double> fovs(top_fov_vals, top_fov_vals + 9);
  double altitude = 1.6;
  unsigned ni = 1280, nj = 760;
  volm_camera_space csp(fovs, altitude, ni, nj,
			head_mid, head_radius, head_inc,
			tilt_mid, tilt_radius, tilt_inc,
			roll_mid, roll_radius, roll_inc);
  for(camera_space_iterator cit = csp.begin(0.0); cit != csp.end(); ++cit){
    cam_angles ca = cit->camera_angles();
    ca.print();
  }
}

TESTMAIN(test_camera_space);
