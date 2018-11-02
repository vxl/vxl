#include <iostream>
#include <fstream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpl/vpl.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vnl/vnl_fwd.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_double_3.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_line_3d_2_points.h>
#include <vgl/vgl_frustum_3d.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>

static bool cams_near_equal(vpgl_perspective_camera<double> const& c1,
                            vpgl_perspective_camera<double> const& c2,
                            double tolerance)
{
  vpgl_calibration_matrix<double> K1= c1.get_calibration(),
    K2 = c2.get_calibration();
  vgl_rotation_3d<double> R1 = c1.get_rotation(), R2 = c2.get_rotation();
  vgl_point_3d<double> cc1 = c1.get_camera_center(),
    cc2 = c2.get_camera_center();
  vnl_matrix_fixed<double, 3,3> Km1 = K1.get_matrix(), Km2 = K2.get_matrix();
  vnl_matrix_fixed<double, 3,3> Rm1 = R1.as_matrix(), Rm2 = R2.as_matrix();
  for (unsigned r = 0; r<3; ++r)
    for (unsigned c = 0; c<3; ++c) {
      if (std::fabs(Km1[r][c]-Km2[r][c])>tolerance) return false;
      if (std::fabs(Rm1[r][c]-Rm2[r][c])>tolerance) return false;
    }
  return std::fabs(cc1.x()-cc2.x())<=tolerance
      && std::fabs(cc1.y()-cc2.y())<=tolerance
      && std::fabs(cc1.z()-cc2.z())<=tolerance;
}

static void test_perspective_camera()
{
  //Construct the camera
  double data[] = { 2000,    0, 512,
                       0, 2000, 384,
                       0,    0,   1 };
  vpgl_calibration_matrix<double> K = vnl_double_3x3(data);
  vgl_rotation_3d<double> R; // the identity
  vgl_homg_point_3d<double>center(0,0,-10.0);
  vpgl_perspective_camera<double> P(K, center, R);
  std::cout << "Camera " << P;

  vgl_homg_point_3d<double>hp_3d(0,0,0);
  vgl_homg_point_2d<double>hp_2d, hpa_2d;
  hp_2d = P(hp_3d);
  std::cout << "p3d " << hp_3d << " p2d " << hp_2d << '\n';
  TEST_NEAR("test projection to principal point", hp_2d.x()/hp_2d.w(), 512, 1e-06);

  vgl_homg_point_3d<double>hpa_3d(10,20,10);
  hpa_2d = P(hpa_3d);
  std::cout << "p3da " << hpa_3d << " p2da " << hpa_2d << '\n';
  TEST_NEAR("test x projection of arbitrary point", hpa_2d.x()/hpa_2d.w(), 1512, 1e-06);

  TEST_NEAR("test y projection of arbitrary point", hpa_2d.y()/hpa_2d.w(), 2384, 1e-06);

  //rotation angle in radians
  double theta = vnl_math::pi_over_4;//45 degrees
  //y axis is the rotation axis
  vnl_double_3 axis(0.0, 1.0, 0.0);
  vgl_h_matrix_3d<double> tr;
  tr.set_identity();
  tr.set_rotation_about_axis(axis, theta);
  std::cout <<"Rotation Matrix\n" << tr << '\n';
  vpgl_perspective_camera<double> P_rot =
    vpgl_perspective_camera<double>::postmultiply(P, tr);
  std::cout << "P_rot\n" << P_rot << '\n';

  // test look_at
  {
    // rotate the camera to look at a target
    // the second point is "above" the first given the "up" direction (defaults to Z)
    vgl_homg_point_3d<double> target(4.0,2.5,-6.2), above(4.0,2.5,-3.2);
    vgl_homg_point_3d<double> center_pt2(8.12, 2.81, 10.0);
    vpgl_perspective_camera<double> P(K, center_pt2, R);
    P.look_at(target);
    bool infront = !P.is_behind_camera(target);
    vgl_point_2d<double> tgt(P(target));
    vgl_point_2d<double> abv(P(above));
    std::cout << "target projects to "<< tgt << std::endl
             << "this point should be above "<< abv << std::endl;
    TEST("look_at - in front of camera", infront, true);
    TEST_NEAR("look_at - target projects to prin. pt.", (tgt - K.principal_point()).length(), 0.0, 1e-8);
    // a point above the target should project above in the image (-Y is "up" in the image)
    TEST("look_at - correct direction is up", (tgt.x()-abv.x())<1e-8 && tgt.y()>abv.y(), true );
  }

  vgl_homg_point_3d<double> X(10.,0,0);
  vgl_point_2d<double> hur = P_rot.project(X);
  std::cout << X << '\n' << hur << '\n';
  TEST_NEAR("test postmultipy with pure rotation", hur.x(), 5340.43, 0.01);
  //shift down 1 unit in z
  tr.set_translation(0.0, 0.0, -1.0);
  vpgl_perspective_camera<double> P_rott =
    vpgl_perspective_camera<double>::postmultiply(P, tr);
  vgl_point_2d<double> hurt = P_rott.project(X);
  std::cout << X << '\n' << hurt << '\n';
  TEST_NEAR("test postmultipy with general Euclidean", hurt.x(),  7843.59, 0.01);

  // test align up/down
  {
    vgl_rotation_3d<double> P0_R(.1,.4,-1); // euler angles
    vpgl_perspective_camera<double> P0;
    P0.set_rotation( P0_R );
    P0.set_camera_center( vgl_point_3d<double>( 0, 2, -1 ) );

    vgl_rotation_3d<double> P1_R(.2,-.3,.7); // euler angles
    vpgl_perspective_camera<double> P1;
    P1.set_rotation( P1_R );
    P1.set_camera_center( vgl_point_3d<double>( 6, -1, 2 ) );

    vpgl_perspective_camera<double> P2 = P1;
    P1 = vpgl_align_up( P0, P1 );
    P1 = vpgl_align_down( P0, P1 );
    TEST_NEAR( "testing align up/down:",
               (P2.get_matrix()-P1.get_matrix()).frobenius_norm(), 0.0, .01 );
  }

  // Test finite backprojection.
  {
    vgl_rotation_3d<double> P0_R(.1,.4,-1); // euler angles
    vpgl_perspective_camera<double> P0;
    P0.set_rotation( P0_R );
    P0.set_camera_center( vgl_point_3d<double>( 1, 1, 1 ) );
    P0.look_at( vgl_homg_point_3d<double>(0,0,0) );

    bool all_succeeded = true;
    for ( int i = 0; i < 4; ++i )
    {
      vgl_point_3d<double> p;
           if ( i == 0 ) p.set(0,0,0);
      else if ( i == 1 ) p.set(-1,0,-1);
      else if ( i == 2 ) p.set(-1,-1,0);
      else /* i == 3 */  p.set(0,-1,-1);
      vgl_homg_point_2d<double> ip = P0.project( vgl_homg_point_3d<double>(p) );
      vgl_line_3d_2_points<double> l =
        P0.backproject( vgl_point_2d<double>( ip.x()/ip.w(), ip.y()/ip.w() ) );
      all_succeeded = all_succeeded &&
                      parallel( l.point1()-p, l.point2()-p, .001 ) &&
                      !P0.is_behind_camera( vgl_homg_point_3d<double>( l.point2() ) );
    }
    TEST( "testing finite backprojection:", all_succeeded, true );
  }
  // test stream operators
  vnl_matrix_fixed<double, 3, 3> k, Rm;
  vgl_point_3d<double> c(-25.3302, -31.0114, 1030.63);
  k[0][0] = 2200;   k[0][1] = 0;      k[0][2] = 640;
  k[1][0] = 0;      k[1][1] = 2200;   k[1][2] = 360;
  k[2][0] = 0;      k[2][1] = 0;      k[2][2] = 1;

  Rm[0][0] =  0.739671;   Rm[0][1] = -0.654561;      Rm[0][2] =  0.156323;
  Rm[1][0] = -0.201219;   Rm[1][1] = -0.436776;      Rm[1][2] = -0.876777;
  Rm[2][0] =  0.642182;   Rm[2][1] =  0.617071;      Rm[2][2] = -0.45478;

  vgl_rotation_3d<double> rot(Rm);
  vpgl_calibration_matrix<double> Ko(k);
  vpgl_perspective_camera<double> Po;
  Po.set_camera_center(c);
  Po.set_rotation(rot);
  Po.set_calibration(Ko);
  std::string cam_path = "cam_path";
  std::ofstream os(cam_path.c_str());
  os << Po;
  os.close();
  std::ifstream is(cam_path.c_str());
  vpgl_perspective_camera<double> Pin;
  is >> Pin;
  bool eql = cams_near_equal(Pin, Po, 0.01);
  TEST("Test stream operators", eql, true);
  vpl_unlink(cam_path.c_str());
  // test constructors and methods using translation
  vnl_vector_fixed<double, 3> cv(-25.3302, -31.0114, 1030.63), tv;
  tv = -(Rm*cv);
  vgl_vector_3d<double> t(tv[0], tv[1], tv[2]);
  vpgl_perspective_camera<double> Ptc(Ko, rot, t);
  eql = cams_near_equal(Po, Ptc, 0.01);
  TEST("translation constructor", eql, true);
  vpgl_perspective_camera<double> Pts(Po);
  Pts.set_translation(t);
  eql = cams_near_equal(Po, Pts, 0.01);
  TEST("set translation ", eql, true);
  vgl_vector_3d<double> tg = Po.get_translation();
  Pin.set_translation(tg);
  eql = cams_near_equal(Po, Pin, 0.01);
  TEST("get translation ", eql, true);
  // test the frustum
  vgl_frustum_3d<double> fr = frustum(P, 10.0, 20.0);
  std::cout << P << fr;
  vgl_point_3d<double> cnt = fr.centroid();
  std::cout << cnt;
  double dif = std::fabs(cnt.z()-5.0) +std::fabs( cnt.x()) + std::fabs(cnt.y());
  TEST_NEAR("Perspective frustum", dif, 0.0, 1.0e-04);
  // actual camera parameters  DSC_0070
  std::stringstream str;
  str << 664.975 << ' '<< 0 << ' ' << 752 << '\n';
  str << 0 << ' '<< 664.975 << ' ' << 500 << '\n';
  str << 0 << ' '<< 0 << ' ' << 1 << '\n';
  str << 0.895636  << ' ' << -0.443045  << ' ' << -0.0393379 << '\n';
  str << -0.0777288 << ' ' << -0.0688225 << ' ' << -0.994597 << '\n';
  str << 0.437943  << ' ' << 0.893854   << ' ' << -0.0960772 << '\n';
  str << -23.7517  << ' ' << 3.91779 << ' ' << -1.61185 << '\n';

  vpgl_perspective_camera<double> Pact;
  str >> Pact;
  vgl_frustum_3d<double> fr_act = frustum(Pact, 0.5, 60.0);
  vgl_point_3d<double> pt_in(20.7763, 0.42938, 0.8249);
  vgl_point_3d<double> pt_out(-20.0135,-1.3294,0.7445);
  std::cout << "in " << pt_in<<" out " <<  pt_out <<'\n'
          << Pact <<'\n' << fr_act << '\n';
  bool cont = fr_act.contains(pt_in);
  bool not_cont = !fr_act.contains(pt_out);
  TEST("point_in_actual_frustum", cont&&not_cont, true);
}

TESTMAIN(test_perspective_camera);
