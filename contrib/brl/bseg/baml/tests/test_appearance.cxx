#include <vector>
#include <iostream>
#include <fstream>
#include <limits>
#include <sstream>
#include <imesh/imesh_fileio.h>
#include <imesh/imesh_mesh.h>
#include <vnl/vnl_math.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_4point.h>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_load.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <baml/baml_census.h>
#include <baml/baml_warp.h>
#include <bsta/bsta_joint_histogram.h>

#define inten 0
#define grad_x 0
#define condp 1
static std::vector<float> prob_a(bsta_joint_histogram<float> const& h){
  unsigned nba = h.nbins_a(), nbb = h.nbins_b();
  std::vector<float> pa(nba, 0.0f);
  float vol = h.volume();
  for(unsigned ia = 0; ia<nba; ++ia){
    for(unsigned ib = 0; ib<nbb; ++ib)
      pa[ia] += h.get_count(ia, ib);
    pa[ia]/=vol;
  }
  return pa;
}
static float min_diff(vil_image_view<float> const & view0, float v1, size_t i1, size_t j1, int radius=1){
  int ni = view0.ni(), nj = view0.nj();
  float min_d = std::numeric_limits<float>::max(), min_v;
  for(int j = -radius; j<=radius; ++j){
    int js = j1 + j;
    if(js<0 || js>=nj)
      continue;
    for(int i = -radius; i<=radius; ++i){
      int is = i1+i;
      if(is<0 || is>=ni)
        continue;
      float v = view0(is, js);
      float dif = fabs(v1-v);
      if(dif<min_d){
        min_d = dif;
        min_v = v;
      }
    }
  }
  return min_v;
}
static void update_hist(vil_image_view<float> const& view01, vil_image_view<float> const& view1, vgl_polygon<double> const& poly, double bb_min_x, double bb_min_y, bsta_joint_histogram<float>& h){
  size_t ni=view1.ni() , nj = view1.nj();
  double x , y;
  for(size_t j = 0; j<nj; ++j){
    y = bb_min_y + j;
    for(size_t i = 0; i<ni; ++i){
      x = bb_min_x + i;
      if(!poly.contains(x, y))
        continue;
      float v = view1(i,j);
      float minv = min_diff(view01, v, i, j, 2);
      h.upcount(minv, 1.0f, v, 1.0f);
    }
  }
}
static void read_meshes( std::vector<std::string> const& mesh_paths,
                         std::vector<std::vector<vgl_point_3d<double> > >& region_verts){
  for(const auto & mesh_path : mesh_paths){
    imesh_mesh mesh;
    bool good = imesh_read_ply(mesh_path, mesh);
    imesh_vertex_array_base& verts = mesh.vertices();
    size_t nverts = verts.size();
    std::vector<vgl_point_3d<double> > temp;
    for (size_t i = 0; i < nverts; ++i) {
      vgl_point_3d<double> p(verts(i, 0), verts(i, 1), verts(i, 2));
      temp.push_back(p);
    }
    region_verts.push_back(temp);
  }
}
static void update_hist_multi_regions(const vil_image_resource_sptr& img0, vpgl_camera<double>* cam0,
                                      const vil_image_resource_sptr& img1, vpgl_camera<double>* cam1,
                                      std::vector<std::vector<vgl_point_3d<double> > > const& region_verts,
                                      bsta_joint_histogram<float>& h){
    if(!img0||!img1||!cam0||!cam1)
      return;
    for(const auto & verts : region_verts){
      std::vector<vgl_point_2d<double> > verts_2d_0, verts_2d_1;
      std::vector<vgl_homg_point_2d<double> > hverts_2d_0, hverts_2d_1;
      vgl_box_2d<double> bb0, bb1;
      size_t nverts = verts.size();
      for (size_t i = 0; i < nverts; ++i) {
        double u0, v0, u1, v1;
        cam0->project(verts[i].x(),verts[i].y(),verts[i].z(), u0, v0);
        vgl_point_2d<double> p2d0(u0, v0);
        verts_2d_0.push_back(p2d0); bb0.add(p2d0);
        hverts_2d_0.emplace_back(p2d0);
         cam1->project(verts[i].x(),verts[i].y(),verts[i].z(), u1, v1);
         vgl_point_2d<double> p2d1(u1, v1);
        verts_2d_1.push_back(p2d1); bb1.add(p2d1);
        hverts_2d_1.emplace_back(p2d1);
      }
      vgl_polygon<double> poly0(verts_2d_0), poly1(verts_2d_1);
      size_t ni0 = vnl_math::rnd(bb0.width()), nj0 = vnl_math::rnd(bb0.height());
      size_t i0 = vnl_math::rnd(bb0.min_x()), j0 = vnl_math::rnd(bb0.min_y());
      size_t ni1 = vnl_math::rnd(bb1.width()), nj1 = vnl_math::rnd(bb1.height());
      size_t i1 = vnl_math::rnd(bb1.min_x()), j1 = vnl_math::rnd(bb1.min_y());
      vgl_h_matrix_2d_compute_4point hc;
      vgl_h_matrix_2d<double> H;
      hc.compute(hverts_2d_0, hverts_2d_1, H);
      vgl_h_matrix_2d<double> img1_to_cropped_img1;
      img1_to_cropped_img1.set_identity();
      img1_to_cropped_img1.set_translation(-bb1.min_x(), -bb1.min_y());
      vgl_h_matrix_2d<double> img0_to_img1_cropped(img1_to_cropped_img1.get_matrix()*H.get_matrix());
      vil_image_view<float> view0 = img0->get_view(), view01;
      baml_warp_perspective(view0, img0_to_img1_cropped, ni1, nj1, view01);
      vil_image_view<float> view1 = img1->get_view(i1, ni1, j1, nj1);
      update_hist(view01, view1, poly1, bb1.min_x(),  bb1.min_y(), h);
    }
}
static void test_appearance()
{
  std::string mesh_dir = "D:/tests/chiletest/app_exp/app_exp_objects/";
  std::string img_dir = "D:/tests/chiletest/crop_dir/";
  std::string i0p = "09DEC06145803-P1BS-052869858050_01_P002_bin_2.tif";
  std::string c0p = "09DEC06145803-P1BS-052869858050_01_P002_bin_2.rpb";
  std::string i1p = "10JAN27150215-P1BS-052869798020_01_P004_bin_2.tif";
  std::string c1p = "10JAN27150215-P1BS-052869798020_01_P004_bin_2.rpb";
  //std::string i1p = "10NOV23150815-P1BS-052869854020_01_P001_bin_2.tif";
  //std::string c1p = "10NOV23150815-P1BS-052869854020_01_P001_bin_2.rpb";
  //std::string i1p = "10DEC14151413-P1BS-052869844040_01_P004_bin_2.tif";
  //std::string c1p = "10DEC14151413-P1BS-052869844040_01_P004_bin_2.rpb";
  vil_image_resource_sptr img0 = vil_load_image_resource((img_dir + i0p).c_str());
  vpgl_camera<double>* cam0 = read_local_rational_camera<double>(img_dir + c0p);
  vil_image_resource_sptr img1 = vil_load_image_resource((img_dir + i1p).c_str());
  vpgl_camera<double>* cam1 = read_local_rational_camera<double>(img_dir +c1p);
  if(!img0||!img1||!cam0||!cam1)
    return;
  std::vector<std::vector<vgl_point_3d<double> > > region_verts;
  std::vector<std::string> mpaths;
  mpaths.push_back(mesh_dir + "mesh_4.ply");
  mpaths.push_back(mesh_dir + "mesh_6.ply");
  mpaths.push_back(mesh_dir + "mesh_7.ply");
  mpaths.push_back(mesh_dir + "mesh_8.ply");
  mpaths.push_back(mesh_dir + "mesh_9.ply");
  mpaths.push_back(mesh_dir + "mesh_10.ply");
  mpaths.push_back(mesh_dir + "mesh_11.ply");
  read_meshes(mpaths, region_verts);
  bsta_joint_histogram<float> h(0.5, 32);
  update_hist_multi_regions(img0, cam0, img1, cam1, region_verts,h);
  h.parzen(3.2f);
  std::cout << "=== multimesh hist update===" << std::endl;
  h.print_to_text(std::cout);
  imesh_mesh mesh;
  bool good = imesh_read_ply(mesh_dir + "mesh_5.ply", mesh);
  imesh_vertex_array_base& verts = mesh.vertices();
  size_t nverts = verts.size();
  std::vector<vgl_point_2d<double> > verts_2d_0, verts_2d_1;
  std::vector<vgl_homg_point_2d<double> > hverts_2d_0, hverts_2d_1;
  vgl_box_2d<double> bb0, bb1;
  for (size_t i = 0; i < nverts; ++i) {
    double u0, v0, u1, v1;
    cam0->project(verts(i, 0), verts(i, 1), verts(i, 2), u0, v0);
    vgl_point_2d<double> p2d0(u0, v0);
    verts_2d_0.push_back(p2d0); bb0.add(p2d0);
    hverts_2d_0.emplace_back(p2d0);
    cam1->project(verts(i, 0), verts(i, 1), verts(i, 2), u1, v1);
    vgl_point_2d<double> p2d1(u1, v1);
    verts_2d_1.push_back(p2d1); bb1.add(p2d1);
    hverts_2d_1.emplace_back(p2d1);
  }
  vgl_polygon<double> poly0(verts_2d_0), poly1(verts_2d_1);
  size_t ni0 = vnl_math::rnd(bb0.width()), nj0 = vnl_math::rnd(bb0.height());
  size_t i0 = vnl_math::rnd(bb0.min_x()), j0 = vnl_math::rnd(bb0.min_y());
  size_t ni1 = vnl_math::rnd(bb1.width()), nj1 = vnl_math::rnd(bb1.height());
  size_t i1 = vnl_math::rnd(bb1.min_x()), j1 = vnl_math::rnd(bb1.min_y());
  vgl_h_matrix_2d_compute_4point hc;
  vgl_h_matrix_2d<double> H;
  hc.compute(hverts_2d_0, hverts_2d_1, H);
  vgl_h_matrix_2d<double> img1_to_cropped_img1;
  img1_to_cropped_img1.set_identity();
  img1_to_cropped_img1.set_translation(-bb1.min_x(), -bb1.min_y());
  vgl_h_matrix_2d<double> img0_to_img1_cropped(img1_to_cropped_img1.get_matrix()*H.get_matrix());
  vil_image_view<float> view0 = img0->get_view(), view01;
  baml_warp_perspective(view0, img0_to_img1_cropped, ni1, nj1, view01);
  vil_image_view<float> view1 = img1->get_view(i1, ni1, j1, nj1);
  vil_image_view<float> grad_x_view01, grad_y_view01, grad_x_view1, grad_y_view1;
  vil_sobel_3x3<float,float>( view01, grad_x_view01, grad_y_view01 );
  vil_sobel_3x3<float,float>( view1, grad_x_view1, grad_y_view1 );

  std::vector<float> pa = prob_a(h);
  for (size_t j = 0; j < nj1; ++j)
    for (size_t i = 0; i < ni1; ++i){
      double x = i+ bb1.min_x(), y = j+bb1.min_y();
      if(!poly1.contains(vgl_point_2d<double>(x, y)))
        continue;
#if inten
      float minv = min_diff(view01, view1(i,j), i, j, 2);
      std::cout << "("<< x << ' ' << y << ") " << minv << ' ' << view1(i,j) << ' ' << fabs(view1(i,j)-minv) << std::endl;
#elif  grad_x
      float minv = min_diff(grad_x_view01, grad_x_view1(i,j), i, j, 2);
      std::cout << "("<< x << ' ' << y << ") " << minv << ' ' << grad_x_view1(i,j) << ' ' << fabs(grad_x_view1(i,j)-minv) << std::endl;
#elif condp
      float minv = min_diff(view01, view1(i,j), i, j, 1);
      unsigned ia;
      if(!h.index_a(minv, ia))
        continue;
      if(pa[ia]==0.0f)
        continue;
      float pcond = h.p(minv, view1(i,j))/pa[ia];
      std::cout << "("<< x << ' ' << y << ") " << minv << ' ' << view1(i,j) << ' ' << fabs(view1(i,j)-minv) << ' '<< pcond << std::endl;
#endif
    }
}

TESTMAIN(test_appearance);
