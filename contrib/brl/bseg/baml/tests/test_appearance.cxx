#include <vector>
#include <iostream>
#include <fstream>
#include <imesh/imesh_fileio.h>
#include <imesh/imesh_mesh.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_4point.h>
#include <sstream>
#include <testlib/testlib_test.h>
#include <vcl_compiler.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vpgl/vpgl_local_rational_camera.h>
//#include <brip/brip_vil_float_ops.h>
#include <baml/baml_census.h>

static void test_appearance()
{
  std::string mesh_dir = "D:/tests/chiletest/app_exp/app_exp_objects/";
  std::string img_dir = "D:/tests/chiletest/crop_dir/";
  std::string i0p = "09DEC06145803-P1BS-052869858050_01_P002_bin_2.tif";
  std::string c0p = "09DEC06145803-P1BS-052869858050_01_P002_bin_2.rpb";
  std::string i1p = "10JAN27150215-P1BS-052869798020_01_P004_bin_2.tif";
  std::string c1p = "10JAN27150215-P1BS-052869798020_01_P004_bin_2.rpb";
  vil_image_resource_sptr img0 = vil_load_image_resource((img_dir + i0p).c_str());
  vpgl_rational_camera<double>* cam0 = read_local_rational_camera<double>(img_dir + c0p);
  vil_image_resource_sptr img1 = vil_load_image_resource((img_dir + i1p).c_str());
  vpgl_rational_camera<double>* cam1 = read_local_rational_camera<double>(img_dir + c1p);
  imesh_mesh mesh;
  bool good = imesh_read_ply(mesh_dir + "mesh_0.ply", mesh);
  imesh_vertex_array_base& verts = mesh.vertices();
  size_t nverts = verts.size();
  std::vector<vgl_point_2d<double> > verts_2d_0, verts_2d_1;
  std::vector<vgl_homg_point_2d<double> > hverts_2d_0, hverts_2d_1;
  vgl_box_2d<double> bb0, bb1;
  for (size_t i = 0; i < nverts; ++i) {
    vgl_point_3d<double> p(verts(i, 0), verts(i, 1), verts(i, 2));
	vgl_point_2d<double> p2d0 = cam0->project(p);
	verts_2d_0.push_back(p2d0); bb0.add(p2d0);
        hverts_2d_0.push_back(vgl_homg_point_2d<double>(p2d0));
	vgl_point_2d<double> p2d1 = cam1->project(p);
	verts_2d_1.push_back(p2d1); bb1.add(p2d1);
        hverts_2d_1.push_back(vgl_homg_point_2d<double>(p2d1));
  }
  vgl_h_matrix_2d_compute_4point hc;
  vgl_h_matrix_2d<double> H;
  hc.compute(hverts_2d_0, hverts_2d_1, H);
  size_t ni0 = bb0.width(), nj0 = bb0.height();
  size_t i0 = bb0.min_x(), j0 = bb0.min_y();
  size_t ni1 = bb1.width(), nj1 = bb1.height();
  size_t i1 = bb1.min_x(), j1 = bb1.min_y();
  vil_image_view<float> view0 = img0->get_view(i0, ni0, j0, nj0);
  //brip_vil_float_ops::homography(view0, H, mapped_view0);
}

TESTMAIN(test_appearance);
