#include <iostream>
#include <vector>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bbas/volm/volm_camera_space.h>

static void test_camera_space()
{
  double head_mid=0.0, head_radius=180.0, head_inc=45.0;
  double tilt_mid=90.0, tilt_radius=20.0, tilt_inc=10.0;
  double roll_mid=0.0,  roll_radius=3.0,  roll_inc=3.0;
  double top_fov_vals[] = {3.0,  4.0, 5.0, 12.0, 17.0, 18.0,19.0, 20.0, 24.0};
  std::vector<double> fovs(top_fov_vals, top_fov_vals + 9);
  double altitude = 1.6;
  unsigned ni = 1280, nj = 760;
  volm_camera_space csp(fovs, altitude, ni, nj,
                        head_mid, head_radius, head_inc,
                        tilt_mid, tilt_radius, tilt_inc,
                        roll_mid, roll_radius, roll_inc);
  //test iterator functions
  camera_space_iterator cit = csp.begin(0.0);
  cit += 110;
  ++cit;
  cam_angles ca = cit->camera_angles();
  ca.print();
  //test accessors and index transforms
  unsigned indx = cit->cam_index();
  unsigned roll_index,  fov_index,  head_index, tilt_index;
  cit->cam_indices(roll_index,  fov_index,  head_index, tilt_index);
  unsigned de_roll_index,  de_fov_index,  de_head_index, de_tilt_index;
  cit->cam_indices(indx,de_roll_index,  de_fov_index,
                   de_head_index, de_tilt_index);
  unsigned de_indx = cit->cam_index(roll_index,  fov_index,
                                    head_index, tilt_index);
  std::cout << "(index " << indx << ' ' << de_indx << ")\n"
           << '(' << roll_index << ' ' << fov_index << ' '
           << head_index << ' ' << tilt_index << ")\n"
           << '(' << de_roll_index << ' ' << de_fov_index << ' '
           << de_head_index << ' ' << de_tilt_index << ")\n";
  bool good = indx==471;
  good = good && (cit != csp.end());
  good = good && (de_indx == indx);
  good = good && (roll_index == 1)&&(fov_index == 2)&&(head_index ==6);
  good = good && (tilt_index = 1);
  good = good && (roll_index == de_roll_index)&&(fov_index == de_fov_index)&&
    (head_index ==de_head_index)&&(tilt_index == de_tilt_index);
  TEST("camera iterator and index transforms", good, true);
  // test camera angles
  double er = std::fabs(ca.roll_-0.0) + std::fabs(ca.top_fov_ - 5.0);
  er += std::fabs(ca.heading_-90.0) + std::fabs(ca.tilt_-80.0);
  TEST_NEAR("camera angles", er, 0.0, 0.01);
  //test generation of full camera space
  csp.generate_full_camera_index_space();
  good = csp.remove_camera_index(indx);
  TEST("full camera space, remove camera", good, true);

  // test binary I/O
  vsl_b_ofstream os("./temp.bin");
  vsl_b_write(os, &csp);
  os.close();
  vsl_b_ifstream is("./temp.bin");
  volm_camera_space* csp_in;
  vsl_b_read(is, csp_in);
  cam_angles in_angs = csp_in->camera_angles(indx);
  double er_in = std::fabs(in_angs.roll_-0.0) + std::fabs(in_angs.top_fov_ - 5.0);
  er_in += std::fabs(in_angs.heading_-90.0) + std::fabs(in_angs.tilt_-80.0);
  TEST_NEAR("camera angles from binary", er_in, 0.0, 0.01);
  TEST("number of valid indices", (csp_in->valid_indices().size()), 1079);
}


TESTMAIN(test_camera_space);
