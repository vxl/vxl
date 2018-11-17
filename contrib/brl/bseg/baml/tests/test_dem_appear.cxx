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
#include <vil/vil_save.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/algo/vpgl_backproject_dem.h>
#include <baml/baml_census.h>
#include <baml/baml_warp.h>
#include <bsta/bsta_joint_histogram.h>
#include <brip/brip_filter_bank.h>

#define inten 0
#define grad_x 0
#define condp 0
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
static void update_hist_dem(const vil_image_resource_sptr& img0, vpgl_rational_camera<double>* cam0,
                            const vil_image_resource_sptr& img1, vpgl_rational_camera<double>* cam1,
                            const vil_image_resource_sptr& dem_resc,
                            bsta_joint_histogram<float>& h, bool use_inten = true/* vs x_grad*/){
    if(!img0||!img1||!cam0||!cam1||!dem_resc)
      return;

    vil_image_view<float> view0_int = img0->get_view();
    vil_image_view<float> view1_int = img1->get_view();


    vil_image_view<float> view0 = view0_int, view1 = view1_int;
    if(!use_inten){
#if 0
      brip_filter_bank fb0( 1, 1.0, 3.0f, 1.0f, 5.0, 0.05, view0_int);
      vil_image_view<float> filt0 = fb0.response(0);
      brip_filter_bank fb1( 1, 1.0, 3.0f, 1.0f, 5.0, 0.05, view1_int);
      vil_image_view<float> filt1 = fb1.response(0);
#endif
#if 1
      vil_image_view<float> grad_x_view0, grad_y_view0, grad_x_view1, grad_y_view1;
      vil_sobel_3x3<float,float>( view0_int, grad_x_view0, grad_y_view0 );
      vil_sobel_3x3<float,float>( view1_int, grad_x_view1, grad_y_view1 );
      size_t ni0 = grad_x_view0.ni(), nj0 = grad_x_view0.nj();
      size_t ni1 = grad_x_view1.ni(), nj1 = grad_x_view1.nj();
      vil_image_view<float> grad_mag_view0(ni0, nj0), grad_mag_view1(ni1, nj1);
      grad_mag_view0.fill(0.0f); grad_mag_view1.fill(0.0f);
      for(size_t j = 0; j<nj0; ++j)
        for(size_t i = 0; i<ni0; ++i)
          grad_mag_view0(i, j)= sqrt(grad_x_view0(i,j)*grad_x_view0(i,j) + grad_y_view0(i,j)*grad_y_view0(i,j));
      std::string filt_path = "D:/tests/chiletest/9DECgrad_mag.tif";
      vil_save(grad_mag_view0, filt_path.c_str());
      for(size_t j = 0; j<nj1; ++j)
        for(size_t i = 0; i<ni1; ++i)
          grad_mag_view1(i, j)= sqrt(grad_x_view1(i,j)*grad_x_view1(i,j) + grad_y_view1(i,j)*grad_y_view1(i,j));

      view0 = grad_mag_view0; view1 = grad_mag_view1;
#endif
      //      view0 = filt0; view1 = filt1;
    }
    vpgl_backproject_dem bdem(dem_resc);
    double max_z = bdem.zmax(), min_z =  bdem.zmin();
    vgl_point_3d<double> gc = bdem.geo_center();
    // construct image bounds
    size_t ni1 = view1.ni(), nj1 = view1.nj();
    std::vector<vgl_point_3d<double> > corns = bdem.dem_corners();
    double imin = std::numeric_limits<double>::max(), jmin = imin;
    double imax = 0.0, jmax = 0.0;
    std::vector<vgl_point_2d<double> > verts;
    for(auto & corn : corns){
      double id, jd;
      cam1->project(corn.x(), corn.y(), corn.z(), id, jd);
      verts.emplace_back(id, jd);
      if(id<imin) imin = id;
      if(jd<jmin) jmin = jd;
      if(id>imax) imax = id;
      if(jd>jmax) jmax = jd;
    }
    if(imin<0.0) imin = 0.0;
    if(jmin<0.0) jmin = 0.0;
    if(imax>(ni1-1)) imax = ni1-1;
    if(jmax>(nj1-1)) jmin = nj1-1;
    size_t is = vnl_math::rnd(imin);
    size_t js = vnl_math::rnd(jmin);
    size_t ie = vnl_math::rnd(imax);
    size_t je = vnl_math::rnd(jmax);
    vgl_polygon<double> poly(verts);
    for(size_t j = js; j<=je; ++j)
      for(size_t i = is; i<ie; ++i){
        vgl_point_2d<double> image_pt(i, j);
        if(!poly.contains(image_pt))
          continue;
        vgl_point_3d<double> world_pt;
        if(!bdem.bproj_dem(cam1, image_pt, max_z, min_z, gc, world_pt))
          continue;
        double u0d, v0d;
        cam0->project(world_pt.x(), world_pt.y(), world_pt.z(), u0d, v0d);
        size_t u0 = vnl_math::rnd(u0d), v0 = vnl_math::rnd(v0d);
        float v1 = view1(i,j);
        float minv = min_diff(view0, v1, u0, v0);
        h.upcount(minv, 1.0f, v1, 1.0f);
    }
}
static void test_dem_appear()
{
  std::string mesh_dir = "D:/tests/chiletest/app_exp/app_exp_objects/";
  std::string img_dir = "D:/tests/chiletest/crop_dir/";
  std::string dem_path = "D:/tests/chiletest/CL_roi.tif";
  std::string vrml_path = "D:/tests/chiletest/9DEC_10NOV_grad_mag.wrl";
  std::string i0p = "09DEC06145803-P1BS-052869858050_01_P002_bin_2.tif";
  std::string c0p = "09DEC06145803-P1BS-052869858050_01_P002_bin_2.rpb";
  //std::string i1p = "10JAN27150215-P1BS-052869798020_01_P004_bin_2.tif";
  //std::string c1p = "10JAN27150215-P1BS-052869798020_01_P004_bin_2.rpb";
  std::string i1p = "10NOV23150815-P1BS-052869854020_01_P001_bin_2.tif";
  std::string c1p = "10NOV23150815-P1BS-052869854020_01_P001_bin_2.rpb";
  //std::string i1p = "10DEC14151413-P1BS-052869844040_01_P004_bin_2.tif";
  //std::string c1p = "10DEC14151413-P1BS-052869844040_01_P004_bin_2.rpb";
  vil_image_resource_sptr img0 = vil_load_image_resource((img_dir + i0p).c_str());
  vpgl_camera<double>* cam0 = read_local_rational_camera<double>(img_dir + c0p);
  vpgl_rational_camera<double> cam0g(*(vpgl_rational_camera<double>*)cam0);
  vil_image_resource_sptr img1 = vil_load_image_resource((img_dir + i1p).c_str());
  vpgl_camera<double>* cam1 = read_local_rational_camera<double>(img_dir +c1p);
  vpgl_rational_camera<double> cam1g(*(vpgl_rational_camera<double>*)cam1);
  vil_image_resource_sptr dem_resc = vil_load_image_resource(dem_path.c_str());
  if(!img0||!img1||!cam0||!cam1||!dem_resc)
    return;
  bsta_joint_histogram<float> h(0.25f, 50);
  //bsta_joint_histogram<float> h(-0.15f, 0.15f, 50, -0.15f, 0.15f, 50);
  update_hist_dem(img0, &cam0g, img1, &cam1g, dem_resc, h, false);
  h.parzen(1.0f);
  std::ofstream vstr(vrml_path.c_str());
  h.print_cond_prob_to_vrml(vstr, false);
  vstr.close();
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
      float pcond = h.p_b_given_a(minv, view1(i,j));
      std::cout << "("<< x << ' ' << y << ") " << minv << ' ' << view1(i,j) << ' ' << fabs(view1(i,j)-minv) << ' '<< pcond << std::endl;
#endif
    }
}

TESTMAIN(test_dem_appear);
