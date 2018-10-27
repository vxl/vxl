#include <testlib/testlib_test.h>
#include <vpgl/algo/vpgl_ba_fixed_k_lsqr.h>
#include <vnl/vnl_random.h>

namespace {

// create a test scene with world points, cameras, and ideal projections
// of the points into the images
void setup_scene(const vpgl_calibration_matrix<double>& K,
                 std::vector<vgl_point_3d<double> >& world,
                 std::vector<vpgl_perspective_camera<double> >& cameras,
                 std::vector<vgl_point_2d<double> >& image_points)
{
  world.clear();
  // The world points are the 8 corners of a unit cube
  world.emplace_back(0.0, 0.0, 0.0);
  world.emplace_back(0.0, 0.0, 1.0);
  world.emplace_back(0.0, 1.0, 0.0);
  world.emplace_back(0.0, 1.0, 1.0);
  world.emplace_back(1.0, 0.0, 0.0);
  world.emplace_back(1.0, 0.0, 1.0);
  world.emplace_back(1.0, 1.0, 0.0);
  world.emplace_back(1.0, 1.0, 1.0);

  vgl_rotation_3d<double> I; // no rotation initially

  cameras.clear();
  cameras.emplace_back(K,vgl_homg_point_3d<double>(8.0, 0.0, 8.0),I);
  cameras.emplace_back(K,vgl_homg_point_3d<double>(10.0, 10.0, 0.0),I);
  cameras.emplace_back(K,vgl_homg_point_3d<double>(7.0, 7.0, 7.0),I);
  cameras.emplace_back(K,vgl_homg_point_3d<double>(0.0, -15.0, -2.0),I);
  cameras.emplace_back(K,vgl_homg_point_3d<double>(5.0, 0.0, 0.0),I);

  // point all cameras to look at the origin
  for (auto & camera : cameras)
    camera.look_at(vgl_homg_point_3d<double>(0.0, 0.0, 0.0));

  // project all points in all images
  image_points.clear();
  for (auto & camera : cameras){
    for (const auto & j : world){
      image_points.emplace_back(camera(vgl_homg_point_3d<double>(j)));
    }
  }
}


static void test_ba_fixed_k_lsqr()
{
  std::vector<vgl_point_3d<double> > world;
  std::vector<vpgl_perspective_camera<double> > cameras;
  std::vector<vgl_point_2d<double> > image_points;
  // our known internal calibration
  vpgl_calibration_matrix<double> K(2000.0,vgl_homg_point_2d<double>(512,384),1,0.7,2);
  setup_scene(K, world, cameras, image_points);
  std::vector<std::vector<bool> > mask(cameras.size(), std::vector<bool>(world.size(),true) );
  std::vector<vpgl_calibration_matrix<double> > Ks(cameras.size(),K);

  vpgl_ba_fixed_k_lsqr func(Ks,image_points,mask);

  // Extract the camera and point parameters
  vnl_vector<double> a,b,c;
  a = vpgl_ba_fixed_k_lsqr::create_param_vector(cameras);
  b = vpgl_ba_fixed_k_lsqr::create_param_vector(world);

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
  vnl_vector<double> a2(a),b2(b);
  for (double & i : b2)
  {
    i += rnd.normal()/1000;
  }
  for (double & i : a2)
  {
    i += rnd.normal()/1000;
  }

  func.set_residual_scale(10.0);

  std::vector<vnl_matrix<double> > A(func.number_of_e(), vnl_matrix<double>(2,6));
  std::vector<vnl_matrix<double> > B(func.number_of_e(), vnl_matrix<double>(2,3));
  std::vector<vnl_matrix<double> > C(func.number_of_e(), vnl_matrix<double>(2,0));
  std::vector<vnl_matrix<double> > fdA(A), fdB(B);
  func.jac_blocks(a2, b2, c, A, B, C);

  eps = 1e-8;
  func.fd_jac_blocks(a2, b2, c, fdA, fdB, C, eps);

  TEST_NEAR("Jacobian A (same as finite diff)",
            (A[0]-fdA[0]).absolute_value_max(),0.0,1e-4);
  TEST_NEAR("Jacobian B (same as finite diff)",
            (B[0]-fdB[0]).absolute_value_max(),0.0,1e-4);
}

} // namespace

TESTMAIN(test_ba_fixed_k_lsqr);
