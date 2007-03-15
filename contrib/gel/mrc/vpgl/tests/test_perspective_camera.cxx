#include <testlib/testlib_test.h>
#include <vcl_iostream.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vnl/vnl_fwd.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_double_3.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_line_3d_2_points.h>


static void test_perspective_camera()
{
    //Construct the camera
  vnl_double_3x3 m;
  m[0][0]=2000;m[0][1]=0;m[0][2]=512;
  m[1][0]=0;m[1][1]=2000;m[1][2]=384;
  m[2][0]=0;m[2][1]=0;m[2][2]=1;

  vpgl_calibration_matrix<double> K(m);
  vgl_rotation_3d<double> R; // the identity
  vgl_homg_point_3d<double>center(0,0,-10.0);
  vpgl_perspective_camera<double> P(K, center, R);
  vcl_cout << "Camera " << P;

  vgl_homg_point_3d<double>hp_3d(0,0,0);
  vgl_homg_point_2d<double>hp_2d, hpa_2d;
  hp_2d = P(hp_3d);
  vcl_cout << "p3d " << hp_3d << " p2d " << hp_2d << '\n';
  TEST_NEAR("test projection to principal point", hp_2d.x()/hp_2d.w(), 512, 1e-06);

  vgl_homg_point_3d<double>hpa_3d(10,20,10);
  hpa_2d = P(hpa_3d);
  vcl_cout << "p3da " << hpa_3d << " p2da " << hpa_2d << '\n';
  TEST_NEAR("test x projection of arbitrary point", hpa_2d.x()/hpa_2d.w(), 1512, 1e-06);

  TEST_NEAR("test y projection of arbitrary point", hpa_2d.y()/hpa_2d.w(), 2384, 1e-06);

 //rotation angle in radians
  double theta = vnl_math::pi/4;//45 degrees
  //y axis is the rotation axis
  vnl_double_3 axis(0.0, 1.0, 0.0);
  vgl_h_matrix_3d<double> tr;
  tr.set_identity();
  tr.set_rotation_about_axis(axis, theta);
  vcl_cout <<"Rotation Matrix \n" << tr << '\n';
  vpgl_perspective_camera<double> P_rot = 
    vpgl_perspective_camera<double>::postmultiply(P, tr);  
  vcl_cout << "P_rot\n" << P_rot << '\n';

  // test look_at
  {
    // rotate the camera to look at a target
    // the second point is "above" the first given the "up" direction (defaults to Z)
    vgl_homg_point_3d<double> target(4.0,2.5,-6.2), above(4.0,2.5,-3.2);
    vgl_homg_point_3d<double> center(8.12, 2.81, 10.0);
    vpgl_perspective_camera<double> P(K, center, R);
    P.look_at(target);
    bool infront = !P.is_behind_camera(target);
    vgl_point_2d<double> tgt(P(target));
    vgl_point_2d<double> abv(P(above));
    vcl_cout << "target projects to "<< tgt << vcl_endl;
    vcl_cout << "this point should be above "<< abv << vcl_endl;
    TEST("look_at - in front of camera", infront, true);
    TEST_NEAR("look_at - target projects to prin. pt.", (tgt - K.principal_point()).length(), 0.0, 1e-8);
    // a point above the target should project above in the image (-Y is "up" in the image)
    TEST("look_at - correct direction is up", (tgt.x()-abv.x())<1e-8 && tgt.y()>abv.y(), true );
  }
  
  vgl_homg_point_3d<double> X(10.,0,0);
  vgl_point_2d<double> hur = P_rot.project(X);
  vcl_cout << X << '\n' << hur << '\n';
  TEST_NEAR("test postmultipy with pure rotation", hur.x(), 5340.43, 0.01);
  //shift down 1 unit in z
  tr.set_translation(0.0, 0.0, -1.0);
  vpgl_perspective_camera<double> P_rott = 
    vpgl_perspective_camera<double>::postmultiply(P, tr);  
  vgl_point_2d<double> hurt = P_rott.project(X);
  vcl_cout << X << '\n' << hurt << '\n';
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
    for( int i = 0; i < 4; i++ ){
      vgl_point_3d<double> p;
      if( i == 0 ) p.set(0,0,0);
      if( i == 1 ) p.set(-1,0,-1);
      if( i == 2 ) p.set(-1,-1,0);
      if( i == 3 ) p.set(0,-1,-1);
      vgl_homg_point_2d<double> ip = P0.project( vgl_homg_point_3d<double>(p) );
      vgl_line_3d_2_points<double> l =
        P0.backproject( vgl_point_2d<double>( ip.x()/ip.w(), ip.y()/ip.w() ) );
      all_succeeded = all_succeeded && parallel( l.point1()-p, l.point2()-p, .001 ) &&
        !P0.is_behind_camera( vgl_homg_point_3d<double>( l.point2() ) );
    }
    TEST( "testing finite backprojection:", all_succeeded, true );
  }
}

TESTMAIN(test_perspective_camera);

