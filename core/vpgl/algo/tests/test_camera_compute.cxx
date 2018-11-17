#include <iostream>
#include <testlib/testlib_test.h>
#if 0
#include <testlib/testlib_root_dir.h>
#endif
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_fwd.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_distance.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>
#include <vpgl/algo/vpgl_camera_compute.h>
#include <vpgl/algo/vpgl_calibration_matrix_compute.h>
#include <vpgl/algo/vpgl_ray.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_affine_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_calibration_matrix.h>

static void test_camera_compute_setup()
{
  // PART 1: Test the affine camera computation

  vnl_vector_fixed<double,4> r1( 1, 2, 3, 4 );
  vnl_vector_fixed<double,4> r2( -1, 4, -2, 0 );
  vpgl_affine_camera<double> C1( r1, r2 );
  std::vector< vgl_point_3d<double> > world_pts;
  world_pts.emplace_back( 1, 0, -1 );
  world_pts.emplace_back( 6, 1, 2 );
  world_pts.emplace_back( -1, -3, -2 );
  world_pts.emplace_back( 0, 0, 2 );
  world_pts.emplace_back( 2, -1, -5 );
  world_pts.emplace_back( 8, 1, -2 );
  world_pts.emplace_back( -4, -4, 5 );
  world_pts.emplace_back( -1, 3, 4 );
  world_pts.emplace_back( 1, 2, -7 );
  std::vector< vgl_point_2d<double> > image_pts;
  image_pts.reserve(world_pts.size());
for (const auto & world_pt : world_pts)
    image_pts.emplace_back(C1.project( vgl_homg_point_3d<double>(world_pt) ) );

  vpgl_affine_camera<double> C1e;
  vpgl_affine_camera_compute::compute( image_pts, world_pts, C1e );

  std::cerr << "\nTrue camera matrix:\n" << C1.get_matrix() << '\n'
           << "\nEstimated camera matrix:\n" << C1e.get_matrix() << '\n';
  TEST_NEAR( "vpgl_affine_camera_compute:",
             ( C1.get_matrix()-C1e.get_matrix() ).frobenius_norm(), 0, 1 );
}

void test_perspective_compute()
{
  std::cout << "Test Perspective Compute\n";
  vnl_vector_fixed<double, 3> rv, trans;
  for (unsigned i = 0; i<3; ++i)
    rv[i]=0.9068996774314604; // axis along diagonal, rotation of 90 degrees
  vgl_rotation_3d<double> rr(rv);

  trans[0]=10.0;   trans[1]=20.0; trans[2] = 30;

  vnl_matrix<double> Y(3, 5);
  Y[0][0] = 1.1; Y[1][0] = -0.05; Y[2][0] = 0.01;
  Y[0][1] = 0.02; Y[1][1] = 0.995; Y[2][1] = -0.1;
  Y[0][2] = -0.01; Y[1][2] = 0.04; Y[2][2] = 1.04;
  Y[0][3] = 1.15; Y[1][3] = 0.97; Y[2][3] = -0.1;
  Y[0][4] = 1.01; Y[1][4] = 1.03; Y[2][4] = 0.96;

  vnl_matrix<double> J(4,6);
  for (unsigned c = 0; c<5; ++c)
  {
    for (unsigned r = 0; r<3; ++r)
      J[r][c]=Y[r][c];
      J[3][c] = 1.0;
  }
  J[0][5] = 0.5;   J[1][5] = 1.0;  J[2][5] = -0.5;  J[3][5] = 1.0;

  vnl_matrix_fixed<double, 3, 3> pr = rr.as_matrix();
  vnl_matrix_fixed<double, 3, 4> P;
  for (unsigned r = 0; r<3; ++r)
  {
    for (unsigned c = 0; c<3; ++c)
      P[r][c] = pr[r][c];
    P[r][3] = trans[r];
  }
  // Project the 3-d points
  vnl_matrix<double> Z(2, 6);
  for (unsigned c = 0; c<6; ++c)
  {
    vnl_vector_fixed<double, 4> vpr;
    for (unsigned r = 0; r<4; ++r)
      vpr[r]=J[r][c];
    vnl_vector_fixed<double, 3> pvpr = P*vpr;
    for (unsigned r = 0; r<2; ++r)
      Z[r][c] = pvpr[r]/pvpr[2];
  }
  std::cout << "Projected points\n " << Z << '\n';
  std::vector<vgl_point_2d<double> > image_pts;
  std::vector<vgl_point_3d<double> > world_pts;
  for (unsigned i = 0; i<6; ++i)
  {
    vgl_point_2d<double> ip(Z[0][i], Z[1][i]);
    vgl_point_3d<double> wp(J[0][i], J[1][i], J[2][i]);
    image_pts.push_back(ip);
    world_pts.push_back(wp);
  }
  vpgl_calibration_matrix<double> K;
  vpgl_perspective_camera<double> pc;

  vpgl_perspective_camera_compute::compute(image_pts, world_pts, K, pc);
  std::cout << pc << '\n';
  vgl_point_3d<double> cc = pc.get_camera_center();
  TEST_NEAR("perspective camera from 6 points exact", cc.z(), -14.2265, 0.001);
}
//Tests the compute(world_pts, image_pts, camera) method in
// the vpgl_camera_compute class
static void test_perspective_compute_direct_linear_transform()
{
  //Create the world points
  std::vector< vgl_point_3d<double> > world_pts;
  world_pts.emplace_back( 1, 0, -1 );
  world_pts.emplace_back( 6, 1, 2 );
  world_pts.emplace_back( -1, -3, -2 );
  world_pts.emplace_back( 0, 0, 2 );
  world_pts.emplace_back( 2, -1, -5 );
  world_pts.emplace_back( 8, 1, -2 );

  //Come up with the projection matrix.
  vnl_matrix_fixed<double, 3, 4> proj;
  proj.set(0,0,0); proj.set(0,1,1); proj.set(0,2,0);      proj.set(0, 3, 3);
  proj.set(1,0,-1); proj.set(1,1,0); proj.set(1,2,0);     proj.set(1, 3, 2);
  proj.set(2,0,0); proj.set(2,1,0); proj.set(2,2,1);      proj.set(2, 3, 0);

  //Do the projection for each of the points
  std::vector< vgl_point_2d<double> > image_pts;
  for (auto & i : world_pts) {
    vnl_vector_fixed<double, 4> world_pt;
    world_pt[0] = i.x();
    world_pt[1] = i.y();
    world_pt[2] = i.z();
    world_pt[3] = 1.0;

    vnl_vector_fixed<double, 3> projed_pt = proj * world_pt;

    image_pts.emplace_back(
        projed_pt[0] / projed_pt[2],
        projed_pt[1] / projed_pt[2]);
  }

  //Calculate the projected points
  vpgl_perspective_camera<double> camera;
  double err;
  vpgl_perspective_camera_compute::compute_dlt(
    image_pts, world_pts, camera, err);

  TEST_NEAR("Small error.", err, 0, .1);

  //Check that it is close.
  for (unsigned int i = 0; i < world_pts.size(); i++) {
    double x,y;
    camera.project(world_pts[i].x(), world_pts[i].y(), world_pts[i].z(),
                   x, y);

    TEST_NEAR("Testing that x coord is close", x, image_pts[i].x(), .001);
    TEST_NEAR("Testing that y coord is close", y, image_pts[i].y(), .001);
  }
}

static void test_perspective_compute_ground()
{
  vpgl_calibration_matrix<double> trueK( 1680, vgl_point_2d<double>( 959.5, 539.5 ) );
  vgl_rotation_3d<double> trueR( vnl_vector_fixed<double,3>(1.87379,0.0215981,-0.0331475));
  vgl_point_3d<double> trueC(14.5467,-6.71791,4.79478);
  vpgl_perspective_camera<double> trueP(trueK,trueC,trueR);

  // generate some points on the ground
  std::vector<vgl_point_2d<double> > ground_pts;
  ground_pts.emplace_back( 14.3256, 5.7912 );
  ground_pts.emplace_back( 14.3256, 9.4488 );
  ground_pts.emplace_back( 14.3256, 15.24 );
  ground_pts.emplace_back( 5.7404, 9.398 );
  ground_pts.emplace_back( 22.8092, 9.398 );

  // project them to the image
  std::vector< vgl_point_2d<double> > image_pts;
  for (auto & ground_pt : ground_pts)
  {
    vgl_homg_point_3d<double> world_pt(ground_pt.x(), ground_pt.y(), 0, 1);

    vgl_point_2d<double> img_pt = trueP.project(world_pt);
    assert( img_pt.x() >= 0 );
    assert( img_pt.x() <= 1920 );
    assert( img_pt.y() >= 0 );
    assert( img_pt.y() <= 1080 );

    image_pts.push_back( img_pt );
  }

  vpgl_perspective_camera<double> P;
  P.set_calibration(trueK);

  bool did_compute = vpgl_perspective_camera_compute::compute(image_pts, ground_pts, P);

  TEST("Calibrate from ground<->image correspondences", did_compute, true);
  TEST_NEAR("   C", vgl_distance( trueC, P.get_camera_center() ), 0, 1e-6);
  TEST_NEAR("   R", ( trueR.as_matrix() - P.get_rotation().as_matrix() ).frobenius_norm(), 0, 1e-6);
}

static void test_calibration_compute_natural()
{
  vpgl_calibration_matrix<double> trueK( 1680, vgl_point_2d<double>( 959.5, 539.5 ) );
  vgl_rotation_3d<double> trueR( vnl_vector_fixed<double,3>(1.87379,0.0215981,-0.0331475));
  vgl_point_3d<double> trueC(14.5467,-6.71791,4.79478);
  vpgl_perspective_camera<double> trueP(trueK,trueC,trueR);

  // generate some points on the ground
  std::vector<vgl_point_2d<double> > ground_pts;
  ground_pts.emplace_back( 14.3256, 5.7912 );
  ground_pts.emplace_back( 14.3256, 9.4488 );
  ground_pts.emplace_back( 14.3256, 15.24 );
  ground_pts.emplace_back( 5.7404, 9.398 );
  ground_pts.emplace_back( 22.8092, 9.398 );

  // project them to the image
  std::vector< vgl_point_2d<double> > image_pts;
  for (auto & ground_pt : ground_pts)
  {
    vgl_homg_point_3d<double> world_pt( ground_pt.x(), ground_pt.y(), 0, 1 );

    vgl_point_2d<double> img_pt = trueP.project( world_pt );
    assert( img_pt.x() >= 0 );
    assert( img_pt.x() <= 1920 );
    assert( img_pt.y() >= 0 );
    assert( img_pt.y() <= 1080 );

    image_pts.push_back( img_pt );
  }

  vpgl_calibration_matrix<double> K;
  bool did_compute = vpgl_calibration_matrix_compute::natural( image_pts, ground_pts, trueK.principal_point(), K );

  TEST( "Calibrate natural intrinsics from correspondences", did_compute, true );
  TEST_NEAR( "   K Discrepancy", ( trueK.get_matrix() - K.get_matrix() ).frobenius_norm(), 0, 1e-6 );
}

static void test_camera_compute()
{
  test_camera_compute_setup();
  test_perspective_compute();
  test_perspective_compute_direct_linear_transform();
  test_perspective_compute_ground();
  test_calibration_compute_natural();
}

TESTMAIN(test_camera_compute);
