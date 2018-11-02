#include <iostream>
#include <fstream>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpl/vpl.h>

#include <bpgl/bpgl_segmented_rolling_shutter_camera.h>
#include <vpgl/vpgl_perspective_camera.h>

#include <vnl/vnl_fwd.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_double_3.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_line_3d_2_points.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vgl/algo/vgl_rotation_3d.h>


static void test_segmented_rolling_shutter_camera()
{
  //Construct the camera
  double data[] = { 2000,    0, 512,
                       0, 2000, 384,
                       0,    0,   1 };
  vpgl_calibration_matrix<double> K = vnl_double_3x3(data);
  vgl_rotation_3d<double> R1; // the identity
  vgl_homg_point_3d<double>center1(0,0,-10.0);
  vpgl_perspective_camera<double> P1(K, center1, R1);
  std::cout << "Camera 1 " << P1;

  vgl_rotation_3d<double> R2; // the identity
  vgl_homg_point_3d<double>center2(0,0,-10.0);
  vpgl_perspective_camera<double> P2(K, center2, R2);
  std::cout << "Camera 2 " << P2;


  std::map<unsigned int,vpgl_perspective_camera<double> >  cam_map;
  cam_map[0]=P1;
  cam_map[384]=P2;

  bpgl_segmented_rolling_shutter_camera<double> rc(cam_map);
  double u,v;rc.project(0.0,0.0,0.0,u,v);

  unsigned int index=0;
  rc.cam_index(v,index);

#if 0
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
  double theta = vnl_math::pi_over_4; // 45 degrees
  //y axis is the rotation axis
  vnl_double_3 axis(0.0, 1.0, 0.0);
  vgl_h_matrix_3d<double> tr;
  tr.set_identity();
  tr.set_rotation_about_axis(axis, theta);
  std::cout <<"Rotation Matrix\n" << tr << '\n';
  bpgl_segmented_rolling_shutter_camera<double> P_rot =
    bpgl_segmented_rolling_shutter_camera<double>::postmultiply(P, tr);
  std::cout << "P_rot\n" << P_rot << '\n';

  // test look_at
  {
    // rotate the camera to look at a target
    // the second point is "above" the first given the "up" direction (defaults to Z)
    vgl_homg_point_3d<double> target(4.0,2.5,-6.2), above(4.0,2.5,-3.2);
    vgl_homg_point_3d<double> center(8.12, 2.81, 10.0);
    bpgl_segmented_rolling_shutter_camera<double> P(K, center, R);
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
  bpgl_segmented_rolling_shutter_camera<double> P_rott =
    bpgl_segmented_rolling_shutter_camera<double>::postmultiply(P, tr);
  vgl_point_2d<double> hurt = P_rott.project(X);
  std::cout << X << '\n' << hurt << '\n';
  TEST_NEAR("test postmultipy with general Euclidean", hurt.x(),  7843.59, 0.01);

  // test align up/down
  {
    vgl_rotation_3d<double> P0_R(.1,.4,-1); // euler angles
    bpgl_segmented_rolling_shutter_camera<double> P0;
    P0.set_rotation( P0_R );
    P0.set_camera_center( vgl_point_3d<double>( 0, 2, -1 ) );

    vgl_rotation_3d<double> P1_R(.2,-.3,.7); // euler angles
    bpgl_segmented_rolling_shutter_camera<double> P1;
    P1.set_rotation( P1_R );
    P1.set_camera_center( vgl_point_3d<double>( 6, -1, 2 ) );

    bpgl_segmented_rolling_shutter_camera<double> P2 = P1;
    P1 = bpgl_align_up( P0, P1 );
    P1 = bpgl_align_down( P0, P1 );
    TEST_NEAR( "testing align up/down:",
               (P2.get_matrix()-P1.get_matrix()).frobenius_norm(), 0.0, .01 );
  }

  // Test finite backprojection.
  {
    vgl_rotation_3d<double> P0_R(.1,.4,-1); // euler angles
    bpgl_segmented_rolling_shutter_camera<double> P0;
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
  bpgl_segmented_rolling_shutter_camera<double> Po;
  Po.set_camera_center(c);
  Po.set_rotation(rot);
  Po.set_calibration(Ko);
  std::string cam_path = "cam_path";
  std::ofstream os(cam_path.c_str());
  os << Po;
  os.close();
  std::ifstream is(cam_path.c_str());
  bpgl_segmented_rolling_shutter_camera<double> Pin;
  is >> Pin;
  bool eql = cams_near_equal(Pin, Po, 0.01);
  TEST("Test stream operators", eql, true);
  vpl_unlink(cam_path.c_str());
  // test constructors and methods using translation
  vnl_vector_fixed<double, 3> cv(-25.3302, -31.0114, 1030.63), tv;
  tv = -(Rm*cv);
  vgl_vector_3d<double> t(tv[0], tv[1], tv[2]);
  bpgl_segmented_rolling_shutter_camera<double> Ptc(Ko, rot, t);
  eql = cams_near_equal(Po, Ptc, 0.01);
  TEST("translation constructor", eql, true);
  bpgl_segmented_rolling_shutter_camera<double> Pts(Po);
  Pts.set_translation(t);
  eql = cams_near_equal(Po, Pts, 0.01);
  TEST("set translation ", eql, true);
  vgl_vector_3d<double> tg = Po.get_translation();
  Pin.set_translation(tg);
  eql = cams_near_equal(Po, Pin, 0.01);
  TEST("get translation ", eql, true);
#endif // 0
}

TESTMAIN(test_segmented_rolling_shutter_camera);
