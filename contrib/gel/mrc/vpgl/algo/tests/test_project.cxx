#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vpgl/algo/vpgl_project.h>
#include <vpgl/vpgl_calibration_matrix.h>
static void test_project()
{
  //Construct the camera
  vnl_double_3x3 m;
  m[0][0]=2000;m[0][1]=0;m[0][2]=512;
  m[1][0]=0;m[1][1]=2000;m[1][2]=384;
  m[2][0]=0;m[2][1]=0;m[2][2]=1;

  vpgl_calibration_matrix<double> K(m);
  vgl_h_matrix_3d<double> R;
  R.set_identity();
  vgl_homg_point_3d<double>center(0,0,-10.0);
  vpgl_perspective_camera<double> P(K, center, R);
  vcl_cout << "Camera " << P << '\n';
  //construct a 3-d box
  vgl_point_3d<double> pm(-1,-1,-1), pp(1,1,1);
  vgl_box_3d<double> box_3d(pm, pp);
  vgl_box_2d<double> box_2d = vpgl_project::project_bounding_box(P, box_3d);
  vcl_cout << "Input Box 3d " << box_3d << '\n';
  vcl_cout << "Ouput Box 2d " << box_2d << '\n';
  double minx = box_2d.min_x(), act_minx = (512.0 - 2000/9.0);
  TEST_NEAR("test bounding box projection", minx, act_minx , 1e-09);
}


TESTMAIN(test_project);
