#include <testlib/testlib_test.h>
#include <vpgl/algo/vpgl_ba_shared_k_lsqr.h>
#include <vnl/vnl_random.h>

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

  vpgl_ba_shared_k_lsqr func(K,image_points,mask);

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

  double eps = 1e-8;
  func.set_residual_scale(1.0);
  double weight = 0.0;
  vnl_vector<double> dummy(2,0.0), fij(2,0.0);
  func.compute_weight_ij(0,0,dummy,dummy,dummy,fij,weight);
  TEST_NEAR("weight(0) == 1.0", weight, 1.0, 1e-8);
  double val = 1e-10;
  double last_weight = 1.0;
  bool weight_decreasing = true;
  for (unsigned int i=0; i<20; ++i)
  {
    fij[0] = val;
    func.compute_weight_ij(0,0,dummy,dummy,dummy,fij,weight);
    if (weight> last_weight || weight < 0.0)
      weight_decreasing = false;
    last_weight = weight;
    val *= 10;
  }
  TEST("weight decreasing", weight_decreasing, true);

  vnl_random rnd;
  vnl_vector<double> a2(a),b2(b),c2(c);
  for ( unsigned i=0; i<b2.size(); ++i)
  {
    b2[i] += rnd.normal()/1000;
  }
  for ( unsigned i=0; i<a2.size(); ++i)
  {
    a2[i] += rnd.normal()/1000;
  }

  func.set_residual_scale(10.0);

  vcl_vector<vnl_matrix<double> > A(func.number_of_e(), vnl_matrix<double>(2,6));
  vcl_vector<vnl_matrix<double> > B(func.number_of_e(), vnl_matrix<double>(2,3));
  vcl_vector<vnl_matrix<double> > C(func.number_of_e(), vnl_matrix<double>(2,1));
  vcl_vector<vnl_matrix<double> > fdA(A), fdB(B), fdC(C);
  func.jac_blocks(a2, b2, c2, A, B, C);

  eps = 1e-8;
  func.fd_jac_blocks(a2, b2, c2, fdA, fdB, fdC, eps);

  TEST_NEAR("Jacobian A (same as finite diff)",
            (A[0]-fdA[0]).absolute_value_max(),0.0,1e-4);
  TEST_NEAR("Jacobian B (same as finite diff)",
            (B[0]-fdB[0]).absolute_value_max(),0.0,1e-4);
  TEST_NEAR("Jacobian C (same as finite diff)",
            (C[0]-fdC[0]).absolute_value_max(),0.0,1e-4);
}

} // namespace

TESTMAIN(test_ba_shared_k_lsqr);
