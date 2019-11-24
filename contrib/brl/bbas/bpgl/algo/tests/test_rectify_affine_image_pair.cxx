#include <iostream>
#include <fstream>
#include "testlib/testlib_test.h"
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif
#include "vpgl/vpgl_rational_camera.h"
#include "vpgl/vpgl_local_rational_camera.h"
#include "vpgl/vpgl_affine_camera.h"
#include "vpgl/vpgl_lvcs.h"
#include <bpgl/algo/bpgl_project.h>
#include <bpgl/algo/bpgl_rectify_affine_image_pair.h>
#include "vnl/vnl_vector_fixed.h"
#include "vpl/vpl.h"
#include "vil/vil_save.h"
#include "vil/vil_new.h"

static void test_rectify_affine_image_pair() {

  vnl_vector_fixed<double, 4> row1, row2;
  row1[0] = 1.34832; row1[1] = 0.00382; row1[2] = 0.2765; row1[3] = 8.859995;
  row2[0] = 0.2181; row2[1] = -0.92631; row2[2] = -1.00105; row2[3] = 538.9338;
  vpgl_affine_camera<double> acam(row1, row2);
  std::string acam_path = "./acam.txt";
  std::ofstream ostr(acam_path.c_str());
  if (ostr) {
    ostr << acam;
    ostr.close();
  }
  else {
    TEST("load affine camera", false, true);
    return;
  }
  vpgl_affine_camera<double> loaded_acam;
  bool good = bpgl_rectify_affine_image_pair::load_affine_camera(acam_path, loaded_acam);
  good = good && (loaded_acam.get_matrix())[0][0] == 1.34832;
  TEST("load affine camera", good, true);
  vpl_unlink(acam_path.c_str());

  row1[0] = 3.24576; row1[1] = -0.000118349; row1[2] = -0.00137547; row1[3] = 317.948;
  row2[0] = -0.000249604; row2[1] = -3.24236; row2[2] = -0.0741213; row2[3] = 877.16;
  vpgl_affine_camera<double> acam0(row1, row2);

  row1[0] = 1.89339; row1[1] = -0.00154537; row1[2] = 0.68649;  row1[3] = 257.216;
  row2[0] = 0.111276; row2[1] = -1.99657; row2[2] = -0.309028; row2[3] = 650.835;
  vpgl_affine_camera<double> acam1(row1, row2);

  size_t ni0 = 1095, nj0 = 1209;
  size_t ni1 = 963, nj1 = 906;
  vil_image_view<vxl_byte> img0(ni0, nj0);
  vil_image_view<vxl_byte> img1(ni1, nj1);
  img0.fill(vxl_byte(127));
  img1.fill(vxl_byte(127));

  vil_image_resource_sptr res0 = vil_new_image_resource_of_view(img0);
  vil_image_resource_sptr res1 = vil_new_image_resource_of_view(img1);

  vgl_point_3d<double> pmin(0, 0, 60.0), pmax(135.0, 160.0, 95.0);
  vgl_box_3d<double> scene_box;
  scene_box.add(pmin);   scene_box.add(pmax);

  bpgl_rectify_affine_image_pair rip;
  bool success = rip.set_images_and_cams(res0, acam0, res1, acam1);
  if (!success) {
    std::cout << "bpgl_rectify_affine_image_pair could not set images & cameras" << std::endl;
  } else {
    std::cout << "acam0: " << rip.acam0() << std::endl
              << "acam1: " << rip.acam1() << std::endl;
  }

  success = success && rip.process(scene_box);
  TEST("rectify affine image pair", success, true);
}

TESTMAIN(test_rectify_affine_image_pair);
