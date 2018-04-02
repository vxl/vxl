#include <iostream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_fwd.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_inverse.h>
#include <vnl/algo/vnl_svd.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_intersection.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/algo/vpgl_backproject.h>

#include <vpgl/algo/vpgl_camera_homographies.h>


static void test_camera_homographies()
{
  // Actual camera
  //========
  std::cout << " ====== actual camera X-Y plane ======" << std::endl;
  vnl_matrix_fixed<double, 3, 4> P(0.0);
  P[0][0]= 7.093; P[0][1]= -2.932; P[0][2]= 0.231; P[0][3]= 173.85;
  P[1][0]= 0.715; P[1][1]= -0.054; P[1][2]= -10.255; P[1][3]= 160.956;
  P[2][0]= 0.004; P[2][1]= 0.004; P[2][2]= -0.001; P[2][3]= 0.326;
  vpgl_proj_camera<double> c(P);
  vgl_point_2d<double> pact(275.5,413.0);
  std::cout << "img_pt_act_scene" << pact << std::endl;
  vgl_point_3d<double> p3d_act;
  vgl_plane_3d<double> gpl(vgl_vector_3d<double>(0, 0, 1), vgl_point_3d<double>(0, 0, 0));
  vpgl_camera<double>* c_ptr = dynamic_cast<vpgl_camera<double>*>(&c);
  bool good = vpgl_backproject::bproj_plane(c_ptr, pact, gpl, vgl_point_3d<double>(0,0,0), p3d_act);
  if(good){
    std::cout << "Actual world point " << p3d_act << std::endl;
  }

  vgl_point_2d<double> p2d_bp = c.project(p3d_act);
  std::cout << "project  act world pt"<< p2d_bp << "compared to" << pact << std::endl;
  double er = (pact-p2d_bp).length();
  TEST_NEAR(" Project from X-Y Plane", er , 0.0 , 0.001);

  vgl_homg_point_2d<double> h_pact(pact), homg_world_pt, homg_world_pt_vert;
  vgl_h_matrix_2d<double> H_from_act = vpgl_camera_homographies::homography_from_camera(c, gpl);
  homg_world_pt = H_from_act * h_pact;
  vgl_point_3d<double> act_world_pt_from_H(homg_world_pt.x()/ homg_world_pt.w(), homg_world_pt.y() / homg_world_pt.w(),0.0);
  std::cout << " act world pt" << p3d_act << "compared to" << act_world_pt_from_H << std::endl;
  er = (p3d_act - act_world_pt_from_H).length();
  TEST_NEAR("Image to World H X-Y Plane" , er , 0.0 , 0.001);

  std::cout << "\n ===== vertical plane actual camera ======" << std::endl;
  vgl_plane_3d<double> vert_pl(vgl_vector_3d<double>(-1/sqrt(2), -1/sqrt(2), 0.0), p3d_act);
  vgl_point_3d<double> vert_act_pt_3d;
  vgl_ray_3d<double> ray = c.backproject_ray(h_pact);
  vgl_point_3d<double> pint;
  good = vgl_intersection(ray, vert_pl, pint);
  std::cout << "Ray from camera " << ray << std::endl;

  vgl_h_matrix_2d<double> H_from_act_vert = vpgl_camera_homographies::homography_from_camera(c, vert_pl);
  homg_world_pt_vert = H_from_act_vert ( h_pact);
  vgl_point_3d<double> act_world_pt_from_H_vert(homg_world_pt_vert.x() / homg_world_pt_vert.w(), homg_world_pt_vert.y() / homg_world_pt_vert.w(), 0.0);
  vgl_point_3d<double> mapped_vert_origin_pt(-9.8,9.8, 0.0);

  er = (mapped_vert_origin_pt - act_world_pt_from_H_vert).length();
  TEST_NEAR("Image to World H vertical Plane" , er ,0.0 , 0.1);
}

TESTMAIN(test_camera_homographies);
