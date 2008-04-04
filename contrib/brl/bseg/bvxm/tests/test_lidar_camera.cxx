#include <testlib/testlib_test.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <bgeo/bgeo_lvcs_sptr.h>

#include "../bvxm_world_params.h"
#include "../bvxm_lidar_camera.h"

static void test_lidar_camera()
{
  START("test_lidar test");

  // test the def. constructor
  bvxm_lidar_camera cam;
  double u,v;
  double x=0,y=0, z=0;
  cam.project(0,0,0,u,v);
  cam.backproject(u,v,x,y,z);
  bool equal = (x==0) && (y==0) && (z==0);
  TEST("identity camera, equality", equal, true);

  bgeo_lvcs_sptr lvcs = new bgeo_lvcs(33.3236, 44.3694, 0);
  
  vnl_matrix<double> trans_matrix;
  trans_matrix.set_size(4,4);
  trans_matrix.fill(0);
  trans_matrix.fill_diagonal(1);
  trans_matrix[1][1] = -1;
  trans_matrix[0][3] =  438000;
  trans_matrix[1][3] =  3.68978e+006;
  //vcl_cout << trans_matrix << vcl_endl;

  vcl_vector<vcl_vector<double> > tiepoints;
  tiepoints.resize(1);
  tiepoints[0].resize(6);
  tiepoints[0][0]=0;
  tiepoints[0][1]=0;
  tiepoints[0][2]=0;
  tiepoints[0][3]=438000;
  tiepoints[0][4]=3689775;
  tiepoints[0][5]=0;
  bvxm_lidar_camera cam2(trans_matrix, lvcs, tiepoints);
  cam2.set_utm(38,0);
  // test the camera project and backproject
  u=1000.0;
  v=1500.0;
  double u1, v1;
  cam2.backproject(u, v, x, y, z);
  cam2.project(x, y, z, u1, v1);
  TEST_NEAR("Project u" , u1, u, 5.5);
  TEST_NEAR("Project v" , v1, v, 5.5);
}

TESTMAIN( test_lidar_camera );