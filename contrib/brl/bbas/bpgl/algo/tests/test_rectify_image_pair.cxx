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
#include <bpgl/algo/bpgl_rectify_image_pair.h>
#include "vnl/vnl_vector_fixed.h"
#include "vpl/vpl.h"
#include "vil/vil_save.h"
#include "vil/vil_new.h"

static void test_rectify_image_pair() {

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
  bpgl_rectify_image_pair<vpgl_affine_camera<double> > temp;
  bool good = temp.load_camera(acam_path, loaded_acam);
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

  // this block throws std::runtime_error on failure
  bpgl_rectify_image_pair<vpgl_affine_camera<double> > rip;
  rip.set_images(res0, res1);
  rip.set_cameras(acam0, acam1);
  std::cout << "acam0: " << rip.cam0() << std::endl
            << "acam1: " << rip.cam1() << std::endl;
  rip.process(scene_box);
  TEST("rectify affine image pair", true, true);

  std::cout << "max affine image dimensions (w,h) (" << ni0 << ' ' << nj0 << ")" << std::endl;
  std::pair<size_t, size_t> dims = rip.rectified_dims();
  size_t width = dims.first, height = dims.second;
  std::cout << "affine rectified dimensions (w,h) (" << width << ' ' << height << ")" << std::endl;
  //=========== perspective case ==============
    //camera 0
  double kd[9] = { 8829.15, 0, 1024,
    0, 8829.15, 766,
    0, 0, 1 };
  vnl_matrix_fixed<double, 3, 3> Km(kd);
  vpgl_calibration_matrix<double> K(Km);
  double rd[9] = { 0.640151, -0.758607, 0.121334,
    -0.453832, -0.500846, -0.737014,
    0.619873, 0.416735, -0.664898 };
  vnl_matrix_fixed<double, 3, 3> R0m(rd);
  vgl_rotation_3d<double> R0(R0m);
  vgl_vector_3d<double> t0(8.71101, 327.467, 1698.42);
  vpgl_perspective_camera<double> P0(K, R0, t0);
  //camera 1
  double rd1[9] = { 0.930967, 0.365021, 0.00776682,
    0.24488, -0.608493, -0.754831,
    -0.270803, 0.704625, -0.655873 };
  vnl_matrix_fixed<double, 3, 3> R1m(rd1);
  vgl_rotation_3d<double> R1(R1m);
  vgl_vector_3d<double> t1(-463.118, 106.98, 2043.14);
  vpgl_perspective_camera<double> P1(K, R1, t1);
  std::cout << "\n========perspective camera 0==========\n" << P0 << std::endl;
  std::cout << "==========perspective camera 1==========\n" << P0 << std::endl;
  std::string pcam_path = "./pcam.txt";

  std::ofstream postr(pcam_path.c_str());
  if (postr) {
    postr << P0;
    postr.close();
  }
  else {
    TEST("load perspective camera", false, true);
    return;
  }
  vpgl_perspective_camera<double> loaded_pcam;
  bpgl_rectify_image_pair<vpgl_perspective_camera<double> > ptemp;
   good = ptemp.load_camera(pcam_path, loaded_pcam);
   good = good && loaded_pcam.get_calibration().x_scale() == 8829.15;
   TEST("load perspective camera", good, true);
  vpl_unlink(pcam_path.c_str());

  vgl_point_3d<double> ppmin(172.192, 123.176, -14.2), ppmax(568.8, 619.40, 49.83);
  vgl_box_3d<double> pscene_box;
  pscene_box.add(ppmin);   pscene_box.add(ppmax);

  ni0 = 2048; nj0 = 1532;
  ni1 = 2048; nj1 = 1532;
  vil_image_view<vxl_byte> pimg0(ni0, nj0);
  vil_image_view<vxl_byte> pimg1(ni1, nj1);
  pimg0.fill(vxl_byte(127));
  pimg1.fill(vxl_byte(127));

  vil_image_resource_sptr pres0 = vil_new_image_resource_of_view(pimg0);
  vil_image_resource_sptr pres1 = vil_new_image_resource_of_view(pimg1);

  // this block throws std::runtime_error on failure
  bpgl_rectify_image_pair<vpgl_perspective_camera<double> > prect;
  prect.set_images(pres0, pres1);
  prect.set_cameras(P0, P1);
  prect.process(pscene_box);
  TEST("perspective rectification", true, true);

  std::cout << "orig perspective image dimensions (w,h) (" << ni0 << ' ' << nj0 << ")" << std::endl;
  dims = prect.rectified_dims();
  width = dims.first, height = dims.second;
  std::cout << "perspective rectified dimensions (w,h) (" << width << ' ' << height << ")" << std::endl;

}

TESTMAIN(test_rectify_image_pair);
