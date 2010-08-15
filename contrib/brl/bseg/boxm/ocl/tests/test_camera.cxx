#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include "ray_bundle_test_driver.h"
#include "open_cl_test_data.h"
#include <boxm/ocl/boxm_ocl_camera_manager.h>
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boct/boct_loc_code.h>
#include <vul/vul_timer.h>
#include <vbl/vbl_array_2d.h>

vpgl_perspective_camera<double>* init_camera()
{
  vnl_matrix_fixed<double, 3, 3> mk(0.0);
  mk[0][0]=990.0; mk[0][2]=4.0;
  mk[1][1]=990.0; mk[1][2]=4.0; mk[2][2]=8.0/7.0;
  vpgl_calibration_matrix<double> K(mk);
  vnl_matrix_fixed<double, 3, 3> mr(0.0);
  mr[0][0]=1.0; mr[1][1]=-1.0; mr[2][2]=-1.0;
  vgl_rotation_3d<double> R(mr);
  vgl_vector_3d<double> t(-0.5,0.5,100);
  vpgl_perspective_camera<double>* cam = new vpgl_perspective_camera<double>(K,R,t);
  return cam;
}

static void test_project()
{
  boxm_ocl_camera_manager* cam_mgr = boxm_ocl_camera_manager::instance();
  cam_mgr->create_kernel("test_project");
  vpgl_perspective_camera<double>* cam=init_camera();

  cam_mgr->set_input_cam(cam);
  cam_mgr->setup_point_data();
  cam_mgr->set_point3d(0.5,0.5,0.0);
  cam_mgr->setup_cam_buffer();
  cam_mgr->set_point_buffers();
  cam_mgr->set_project_args();
  cam_mgr->run();
  //cam_mgr->release_point_buffers();
  cam_mgr->release_cam_buffer();

  cam_mgr->release_kernel();
  cl_float* results = cam_mgr->point2d();

  double u,v;
  cam->project(0.5,0.5,0.0,u,v);

  vcl_cout<<"ground truth: "<<u<<','<<v<<'\n'
          << "test_cam_project: " << results[0] << ',' << results[1] <<vcl_endl;

  TEST("test_cam_project", u==results[0] && v==results[1], true);
}

void camera_tests()
{
  //START TESTS
  //================================================================
  test_project();
  //==============================================================
  //END RAY BUNDLE TESTS
}

void init_test()
{
  boxm_ocl_camera_manager* cam_mgr = boxm_ocl_camera_manager::instance();
  vcl_string root_dir = testlib_root_dir();
  cam_mgr->append_process_kernels(root_dir + "/contrib/brl/bseg/boxm/ocl/backproject.cl")
  &&
  cam_mgr->append_process_kernels(root_dir + "/contrib/brl/bseg/boxm/ocl/tests/test_cam_kernels.cl")
  &&
  cam_mgr->build_kernel_program();
}


static void test_camera()
{
  init_test();
  camera_tests();
}

TESTMAIN(test_camera);
