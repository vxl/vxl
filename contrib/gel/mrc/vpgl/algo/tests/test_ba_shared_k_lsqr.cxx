
#include <testlib/testlib_test.h>
#include <vpgl/algo/vpgl_ba_shared_k_lsqr.h>

namespace {

// create a test scene with world points, cameras, and ideal projections
// of the points into the images
void setup_scene(const vpgl_calibration_matrix<double>& K,
                 vcl_vector<vgl_point_3d<double> >& world,
                 vcl_vector<vpgl_perspective_camera<double> >& cameras,
                 vcl_vector<vgl_point_2d<double> >& image_points)
{
  world.clear();
  // The world points are the 8 corners of a unit cube
  world.push_back(vgl_point_3d<double>(0.0, 0.0, 0.0));
  world.push_back(vgl_point_3d<double>(0.0, 0.0, 1.0));
  world.push_back(vgl_point_3d<double>(0.0, 1.0, 0.0));
  world.push_back(vgl_point_3d<double>(0.0, 1.0, 1.0));
  world.push_back(vgl_point_3d<double>(1.0, 0.0, 0.0));
  world.push_back(vgl_point_3d<double>(1.0, 0.0, 1.0));
  world.push_back(vgl_point_3d<double>(1.0, 1.0, 0.0));
  world.push_back(vgl_point_3d<double>(1.0, 1.0, 1.0));


  vgl_rotation_3d<double> I; // no rotation initially

  cameras.clear();
  cameras.push_back(vpgl_perspective_camera<double>(K,vgl_homg_point_3d<double>(8.0, 0.0, 8.0),I));
  cameras.push_back(vpgl_perspective_camera<double>(K,vgl_homg_point_3d<double>(10.0, 10.0, 0.0),I));
  cameras.push_back(vpgl_perspective_camera<double>(K,vgl_homg_point_3d<double>(7.0, 7.0, 7.0),I));
  cameras.push_back(vpgl_perspective_camera<double>(K,vgl_homg_point_3d<double>(0.0, -15.0, -2.0),I));
  cameras.push_back(vpgl_perspective_camera<double>(K,vgl_homg_point_3d<double>(5.0, 0.0, 0.0),I));

  // point all cameras to look at the origin
  for (unsigned int i=0; i<cameras.size(); ++i)
    cameras[i].look_at(vgl_homg_point_3d<double>(0.0, 0.0, 0.0));

  // project all points in all images
  image_points.clear();
  for (unsigned int i=0; i<cameras.size(); ++i){
    for (unsigned int j=0; j<world.size(); ++j){
      image_points.push_back(cameras[i](vgl_homg_point_3d<double>(world[j])));
    }
  }
}


static void test_ba_shared_k_lsqr()
{
  vcl_vector<vgl_point_3d<double> > world;
  vcl_vector<vpgl_perspective_camera<double> > cameras;
  vcl_vector<vgl_point_2d<double> > image_points;
  // our known internal calibration
  vpgl_calibration_matrix<double> K(2000.0,vgl_homg_point_2d<double>(512,384),1,0.7,2);
  setup_scene(K, world, cameras, image_points);
  vcl_vector<vcl_vector<bool> > mask(cameras.size(), vcl_vector<bool>(world.size(),true) );

  vpgl_ba_shared_k_lsqr func(K,image_points,mask,false);


  // Extract the camera and point parameters
  vnl_vector<double> a,b,c;
  vpgl_ba_shared_k_lsqr::create_param_vector(cameras,a,c);
  b = vpgl_ba_shared_k_lsqr::create_param_vector(world);

  // test conversion back to cameras
  bool valid = true;
  for (unsigned int i=0; i<cameras.size(); ++i)
  {
    vnl_double_3x4 P = func.vpgl_bundle_adjust_lsqr::param_to_cam(i,a,c).get_matrix();
    valid &= (cameras[i].get_matrix() - P).absolute_value_max() < 1e-8;
  }
  TEST("camera conversion", valid, true);

  // test conversion back to points
  valid = true;
  for (unsigned int j=0; j<world.size(); ++j)
    valid &= world[j] == func.vpgl_bundle_adjust_lsqr::param_to_point(j,b,c);
  TEST("point conversion", valid, true);


  vgl_rotation_3d<double> I; // no rotation
  K.set_focal_length(1500);
  vpgl_perspective_camera<double> init_cam(K,vgl_homg_point_3d<double>(1.0, 0.0, -4.0),I);
  init_cam.look_at(vgl_homg_point_3d<double>(0.0, 0.0, 0.0),vgl_vector_3d<double>(0,-1,0));
  vcl_vector<vpgl_perspective_camera<double> > unknown_cameras(cameras.size(),init_cam);
  vnl_vector<double> a2,c2;
  vpgl_ba_shared_k_lsqr::create_param_vector(unknown_cameras,a2,c2);

  vcl_cout << c2<<vcl_endl;
  vnl_vector<double> ai(a2.data_block(),6), bj(b.data_block()+21,3);
  vnl_matrix<double> Aij(2,6), Bij(2,3), Cij(2,1);
  func.jac_Aij(0,0,init_cam.get_matrix(),ai,bj,c2,Aij);
  func.jac_Bij(0,0,init_cam.get_matrix(),ai,bj,c2,Bij);
  func.jac_Cij(0,0,init_cam.get_matrix(),ai,bj,c2,Cij);

  double eps = 1e-6;
  vnl_matrix<double> fd_Aij(2,6), fd_Bij(2,3), fd_Cij(2,1);
  func.fd_jac_Aij(0,0,ai,bj,c2,fd_Aij,eps);
  func.fd_jac_Bij(0,0,ai,bj,c2,fd_Bij,eps);
  func.fd_jac_Cij(0,0,ai,bj,c2,fd_Cij,eps);

  TEST_NEAR("Jacobian A (same as finite diff)",
            (Aij-fd_Aij).absolute_value_max(),0.0,eps);
  TEST_NEAR("Jacobian B (same as finite diff)",
            (Bij-fd_Bij).absolute_value_max(),0.0,eps);
  TEST_NEAR("Jacobian C (same as finite diff)",
            (Cij-fd_Cij).absolute_value_max(),0.0,eps);
}

}

TESTMAIN(test_ba_shared_k_lsqr);
