//:
// \file
// \brief   testing the levenberg marquardt method of finding the right projection plane
// \author  Gamze Tunali Cetintemel
// \date    2005-03-03
//
#include <iostream>
#include <fstream>
#include <vcl_compiler.h>
#include <testlib/testlib_test.h>

#include <bwm/bwm_plane_fitting_lsf.h>
#include <bwm/bwm_observer_proj_cam.h>

#include <vgl/vgl_homg_plane_3d.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vsol/vsol_point_2d.h>

#if 0
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/vnl_vector.h>
#include <vgl/vgl_point_2d.h>
#endif

static vpgl_proj_camera<double> read_projective_camera(std::string cam_path)
{
  std::ifstream cam_stream(cam_path.data());
  vpgl_proj_camera<double> cam;
  cam_stream >> cam;
  return cam;
}

void test_move_proj_plane(int argc, char* argv[])
{
  vgl_homg_plane_3d<double> plane1(0,0,1,10);
  vgl_homg_plane_3d<double> plane2(0,0,1,50);
  std::string camera_path = argc < 2 ? "." : argv[1];
  vpgl_proj_camera<double> master_cam = read_projective_camera(camera_path + "/jcp1_042799_0930.txt");
  vsol_point_2d_sptr master_img_pt = new vsol_point_2d(100, 100);
  bwm_observer_proj_cam master_obs(VXL_NULLPTR);
  master_obs.set_camera(&master_cam, "");
#if 0
  vpgl_proj_camera<double> second_cam = read_projective_camera(camera_path + "/jcp4_042799_1030.txt");
  bwm_observer_cam_proj sec_obs;
  sec_obs.set_camera(&second_cam, "");

  // create two points on the second image, one with d=0, and one with d=100
  master_obs.set_proj_plane(plane1);
  sec_obs.set_proj_plane(plane1);
  vsol_point_3d_sptr pt1, pt2;
  vgl_point_2d<double> sec_pt1, sec_pt2;
  master_obs.backproj_point(master_img_pt, pt1);
  sec_obs.proj_point(pt1->get_p(), sec_pt1);

  master_obs.set_proj_plane(plane2);
  sec_obs.set_proj_plane(plane2);
  master_obs.backproj_point(master_img_pt, pt2);
  sec_obs.proj_point(pt2->get_p(), sec_pt2);

  master_obs.set_proj_plane(plane1);
  sec_obs.set_proj_plane(plane1);
  double d = 10;
  double a = 0;
  double b = 0;
  double c = 1;

  bwm_plane_fitting_lsf lsf(a, b, c, d, master_img_pt, (new vsol_point_2d(sec_pt2)),
                            &master_obs, &sec_obs);
  vnl_levenberg_marquardt lm(lsf);
  vnl_vector<double> x(1);
  x[0] = d;
  lm.set_x_tolerance(0.01); // lm.set_x_tolerance(lm.get_x_tolerance()*1e4);
  lm.set_g_tolerance(0.01); // lm.set_g_tolerance(lm.get_g_tolerance()*1e4);
  lm.set_epsilon_function(0.01);
  lm.set_trace(true);
  lm.minimize_without_gradient(x);
  lm.diagnose_outcome(std::cout);

  std::cout << " minimization ended\n"
           << "X value after--> " << x[0]
           << "\nError=" << lm.get_end_error()
           << "\nnum_iterations=" << lm.get_num_iterations()<< std::endl;

  TEST_NEAR("D", x[0], 50, 2.0);
#endif // 0
}

TESTMAIN_ARGS(test_move_proj_plane);
