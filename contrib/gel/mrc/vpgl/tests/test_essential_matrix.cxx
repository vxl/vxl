#include <testlib/testlib_test.h>
#include <vcl_iostream.h>

#include <vpgl/vpgl_essential_matrix.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/algo/vgl_h_matrix_3d.h>
#include <vnl/vnl_fwd.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_double_3x3.h>
static vnl_double_3x3 
skew_symmetric(const double tx, const double ty, const double tz)
{
  vnl_double_3x3 m;
  m[0][0] = 0;   m[0][1] = -tz; m[0][2] = ty; 
  m[1][0] = tz;   m[1][1] = 0; m[1][2] = -tx; 
  m[2][0] = -ty;   m[2][1] = tx; m[2][2] = 0;
  return m;
}
//To form an essential matrix from a F matrix
static vpgl_calibration_matrix<double> actual_K()
{
  vnl_double_3x3 K; 
  K[0][0] = 880;   K[0][1] = 0; K[0][2] = 330; 
  K[1][0] = 0;   K[1][1] =880 ; K[1][2] = 244; 
  K[2][0] = 0;   K[2][1] = 0; K[2][2] = 1;
  return vpgl_calibration_matrix<double>(K);
}

//For testing numerical issues
static vpgl_essential_matrix<double> actual_e_matrix()
{
  vnl_double_3x3 m;
  m[0][0] = 2.95608e-007;   m[0][1] = -2.39486e-005; m[0][2] = 0.00215856; 
  m[1][0] = 2.41431e-005;   m[1][1] = 6.97368e-007; m[1][2] = -0.0022267; 
  m[2][0] = -0.00208351;   m[2][1] = 0.0020394; m[2][2] = 0.000597002
;
  vnl_double_3x3 K = actual_K().get_matrix(); 
  vnl_double_3x3 mE = K.transpose()*m*K;
  return vpgl_essential_matrix<double>(mE);
}
static void test_essential_matrix()
{
   double cx = 10, cy = 0, cz = 0;
   vnl_double_3x3 T = skew_symmetric(-cx, -cy, -cz);
 // create an essential matrix for translation only
  vpgl_essential_matrix<double> E(T);
  // Test the camera construction
  //Image Point in the left camera (left side of E)
  vgl_point_2d<double> pl(-1,0);
  //Image Point in the right camera (right side of E)
  vgl_point_2d<double> pr(0,0);
  //This point in 3-d is (0,0,10);
  vpgl_perspective_camera<double> pc;
  bool success = extract_left_camera<double>(E, pl, pr, pc);
  vgl_point_3d<double> cen = pc.get_camera_center();
  TEST_NEAR("test translation only between cameras", cen.x(), 1.0, 1e-03);
  //
  //Test with a rotation of 30 degrees around the y axis
  vgl_h_matrix_3d<double> Rh;
  Rh.set_identity();
  vnl_vector_fixed<double,3> ax(0,1,0), cv(10, 0, 0), t;
  vgl_point_3d<double> c(10,0,0);  //Camera center
  Rh.set_rotation_about_axis(ax, 0.5236); //Rotation matrix
  vnl_matrix_fixed<double, 3,3> R = Rh.get_upper_3x3_matrix();
  t = -R*cv;
  vpgl_essential_matrix<double> Ei(skew_symmetric(t[0],t[1],t[2])*R);
  vcl_cout << "\nIdeal Essential Matrix\n" 
           << Ei << '\n';	
  vpgl_perspective_camera<double> pcl, pcr;
  pcl.set_rotation_matrix(Rh);
  pcl.set_camera_center(c);
  vcl_cout << "Ideal Left Perspective Camera  " << pcl << '\n';
  vgl_point_3d<double> X(0,0,10);
  vgl_point_2d<double> xl = pcl.project(X), xr(0,0);
  vpgl_essential_matrix<double> Er(pcr, pcl);
  vcl_cout << "Essential Matrix with rotation\n" << Er << '\n';
  vnl_matrix<double> error = Ei.get_matrix()-Er.get_matrix();
  TEST_NEAR("Construct essential matrix from cameras", error.frobenius_norm(), 0, 1);
  vpgl_perspective_camera<double> pclr;//reconstructed camera
  success = extract_left_camera<double>(Er, xl,xr, pclr);
  vcl_cout << "Left Camera with Rotation " << pclr << '\n';
  vgl_point_3d<double> rc = pclr.get_camera_center();
  TEST_NEAR("Extract Left Camera", (rc.x()-1)*(rc.x()-1), 0, 1e-3);
  //test using actual essential matrix
  vpgl_essential_matrix<double> Ea = actual_e_matrix();
  vcl_cout << "Actual E Matrix \n" << Ea << '\n';
  vgl_point_2d<double> xal(0.207847,0.2126),  xar(-0.1289,-0.0683432);
  vpgl_perspective_camera<double> palr;//reconstructed actual camera
  success = extract_left_camera<double>(Ea, xal,xar, palr);
  vcl_cout << "Actual Left Camera " << palr << '\n';
  rc = palr.get_camera_center();
  vgl_point_3d<double> ac(-0.259913,-0.165156,0.951404);
  TEST_NEAR("Test on actual E matrix", vgl_distance<double>(rc,ac), 0, 1e-3);
}

TESTMAIN(test_essential_matrix);
