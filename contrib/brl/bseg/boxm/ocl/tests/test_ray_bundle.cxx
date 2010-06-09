#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include "ray_bundle_test_driver.h"
#include "open_cl_test_data.h"
#include <boxm/ocl/boxm_ray_trace_manager.h>
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boct/boct_loc_code.h>
#include <vul/vul_timer.h>
#include <vbl/vbl_array_2d.h>
template <class T>
static void test_load_data(ray_bundle_test_driver<T>& driver)
{
  driver.set_bundle_ni(2);
  driver.set_bundle_nj(2);
  driver.set_work_space_ni(8);
  driver.set_work_space_nj(8);
  if (driver.create_kernel("test_load_data")!=SDK_SUCCESS) {
    TEST("Create Kernel test_load_data", false, true);
    return;
  }
  if (driver.set_basic_test_args()!=SDK_SUCCESS)
    return;
  vul_timer t;
  t.mark();
  if (driver.run_bundle_test_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_load_data", false, true);
    return;
  }
  bool good = true;
  cl_int* results = driver.tree_results();
#if 0
  int count = results[0];
  vcl_cout<<t.all()<<" ";
  vcl_cout << "Transfer rate = " << 64.0f*((float)count*count/(static_cast<float>(t.real())))/1000.0f << " Mbytes/second\n";
#endif
  vcl_size_t size = 14*4;
  if (size<driver.tree_result_size_bytes())
    if (results) {

      int test[]={1,1,1,1,
                  1,1,3,0,
                  3,1,3,0,
                  1,3,3,0,
                  3,3,3,0,
                  39,39,39,39,
                  47,47,47,47,
                  55,55,55,55,
                  63,63,63,63,
                  0,1,2,3,
                  1,1,1,1,
                  1,1,3,0,
                  39,39,39,39,
                  0,0,0,0};
      for (vcl_size_t i= 0; i<size; i++)
        good = good && results[i]==test[i];
      TEST("test_load_data_into_cache", good, true);
      if (!good)
        for (vcl_size_t i= 0; i<size; i+=4)
          vcl_cout << "test_load_data_result(" << results[i] << ' '
                   << results[i+1] << ' '
                   << results[i+2] << ' '
                   << results[i+3] << ")\n";
    }
  driver.release_kernel();
}
template <class T>
static void test_load_data_mutable(ray_bundle_test_driver<T>& driver)
{
  driver.set_bundle_ni(2);
  driver.set_bundle_nj(2);
  driver.set_work_space_ni(8);
  driver.set_work_space_nj(8);
  if (driver.create_kernel("test_load_data_mutable")!=SDK_SUCCESS) {
    TEST("Create Kernel test_load_data_mutable", false, true);
    return;
  }
  // the false argument below means use uchar4 instead of char 
  // for the ray_bundle_array
  if (driver.set_basic_test_args(false)!=SDK_SUCCESS)
    return;
  if (driver.run_bundle_test_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_load_data_mutable", false, true);
    return;
  }
  bool good = true;
  cl_int* results = driver.tree_results();

  vcl_size_t size = 20*4;
  if (size<driver.tree_result_size_bytes())
    if (results) {
      int test[]={1,1,1,1,
                  1,1,3,0,
                  3,1,3,0,
                  1,3,3,0,
                  3,3,3,0,
                  39,39,39,39,
                  47,47,47,47,
                  55,55,55,55,
                  63,63,63,63,
                  0,0,0,1,
                  1,0,1,1,
                  2,0,2,1,
                  3,0,3,1,
                  1,1,1,1,
                  1,1,3,0,
                  39,39,39,39,
                  0,1,3,3,
                  0,2,0,3,
                  0,3,0,3,
                  0,0,0,1};
      for (vcl_size_t i= 0; i<size; i++)
        good = good && results[i]==test[i];
      TEST("test_load_data_mutable", good, true);
      if (!good)
        for (vcl_size_t i= 0; i<size; i+=4)
          vcl_cout << "test_load_data_mutable_result(" << results[i] << ' '
                   << results[i+1] << ' '
                   << results[i+2] << ' '
                   << results[i+3] << ")\n";
    }
  driver.release_kernel();
}
template <class T>
static void test_map_work_space(ray_bundle_test_driver<T>& driver)
{
  driver.set_bundle_ni(2);
  driver.set_bundle_nj(2);
  driver.set_work_space_ni(8);
  driver.set_work_space_nj(8);
  if (driver.create_kernel("test_map_work_space")!=SDK_SUCCESS) {
    TEST("Create Kernel test_map_work_space", false, true);
    return;
  }
  if (driver.set_basic_test_args()!=SDK_SUCCESS)
    return;
  if (driver.run_bundle_test_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_map_work_space", false, true);
    return;
  }
  bool good = true;
  cl_int* results = driver.tree_results();

  vcl_size_t size = 65*4;
  if (size<driver.tree_result_size_bytes())

    if (results) {
      int test[]={64,0,0,0};
      for (vcl_size_t i= 0; i<4; i++)
        good = good && results[i]==test[i];
      vbl_array_2d<int> mi(4,4), mj(4,4);

      mi[0][0]=0; mi[1][0]=2; mi[2][0]=0; mi[3][0]=2;
      mi[0][1]=0; mi[1][1]=2; mi[2][1]=0; mi[3][1]=2;
      mi[0][2]=1; mi[1][2]=3; mi[2][2]=1; mi[3][2]=3;
      mi[0][3]=1; mi[1][3]=3; mi[2][3]=1; mi[3][3]=3;

      mj[0][0]=0; mj[1][0]=0; mj[2][0]=2; mj[3][0]=2;
      mj[0][1]=1; mj[1][1]=1; mj[2][1]=3; mj[3][1]=3;
      mj[0][2]=0; mj[1][2]=0; mj[2][2]=2; mj[3][2]=2;
      mj[0][3]=1; mj[1][3]=1; mj[2][3]=3; mj[3][3]=3;

      for (vcl_size_t i= 4; i<size; i+=4){
        int gidi = results[i], gidj = results[i+1];
        int mpi = results[i+2], mpj = results[i+3];
        int grpi = gidi/2, grpj = gidj/2;
        int mgrpi = mpi/2, mgrpj = mpj/2;
        good = good && mgrpi == mi[grpi][grpj];
        good = good && mgrpj == mj[grpi][grpj];
      }
      TEST("test_map_work_space", good, true);
      if (!good)
        for (vcl_size_t i= 0; i<size; i+=4)
          vcl_cout << "test_map_work_space_result(" << results[i] << ' '
                   << results[i+1] << ' '
                   << results[i+2] << ' '
                   << results[i+3] << ")\n";
    }
  driver.release_kernel();
}
template <class T>
static void test_ray_entry_point(ray_bundle_test_driver<T>& driver)
{
  // set up camera
  vnl_matrix_fixed<double, 3, 3> mk(0.0);
  mk[0][0]=990.0; mk[0][2]=4.0;
  mk[1][1]=990.0; mk[1][2]=4.0; mk[2][2]=8.0/7.0;
  vpgl_calibration_matrix<double> K(mk);
  vnl_matrix_fixed<double, 3, 3> mr(0.0);
  mr[0][0]=1.0; mr[1][1]=-1.0; mr[2][2]=-1.0;
  vgl_rotation_3d<double> R(mr);
  vgl_vector_3d<double> t(-0.5,0.5,100);
  vpgl_camera_double_sptr cam = new vpgl_perspective_camera<double>(K,R,t);
  boxm_ray_trace_manager<float >::instance()->set_camera(cam);
  driver.set_bundle_ni(2);
  driver.set_bundle_nj(2);
  driver.set_work_space_ni(8);
  driver.set_work_space_nj(8);
  boxm_ray_trace_manager<float >::instance()->setup_roi_dims(0,7,0,7);
  if (driver.create_kernel("test_ray_entry_point")!=SDK_SUCCESS) {
    TEST("Create Kernel test_map_work_space", false, true);
    return;
  }
  if(!driver.setup_image_cam_data())
    return;
  if (driver.set_image_cam_args()!=SDK_SUCCESS)
    return;
  if (driver.run_bundle_test_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_map_work_space", false, true);
    return;
  }
  bool good = true;
  cl_int* results = driver.tree_results();

  vcl_size_t size = 64*4;
  if (size<driver.tree_result_size_bytes())
    if (results) {
      int test[]={99,900,999,0,214,900,999,1000,
                  328,900,999,0,442,900,999,1000,
                  557,900,1000,0,671,900,999,1000,
                  785,900,1000,0,900,900,999,1000,
                  99,785,999,2000,214,785,1000,3000,
                  328,785,999,2000,442,785,1000,3000,
                  557,785,1000,2000,671,785,999,3000,
                  785,785,999,2000,900,785,1000,3000,
                  100,671,1000,0,214,671,1000,1000,
                  328,671,1000,0,442,671,1000,1000,
                  557,671,999,0,671,671,1000,1000,
                  785,671,999,0,900,671,999,1000,
                  99,557,1000,2000,214,557,999,3000,
                  328,557,1000,2000,442,557,999,3000,
                  557,557,1000,2000,671,557,999,3000,
                  785,557,1000,2000,900,557,1000,3000,
                  99,442,999,0,214,442,999,1000,
                  328,442,1000,0,442,442,999,1000,
                  557,442,999,0,671,442,1000,1000,
                  785,442,1000,0,900,442,999,1000,
                  99,328,1000,2000,214,328,999,3000,
                  328,328,1000,2000,442,328,1000,3000,
                  557,328,1000,2000,671,328,1000,3000,
                  785,328,999,2000,900,328,999,3000,
                  99,214,999,0,214,214,1000,1000,
                  328,214,999,0,442,214,999,1000,
                  557,214,999,0,671,214,1000,1000,
                  785,214,1000,0,899,214,999,1000,
                  99,99,999,2000,214,99,999,3000,
                  328,100,1000,2000,442,99,999,3000,
                  557,99,1000,2000,671,99,1000,3000,
                  785,99,999,2000,900,99,999,3000};
      for (vcl_size_t i= 0; i<size; i++)
        good = good && results[i]==test[i];
      TEST("test_entry_points", good, true);

      if (!good)
        for (vcl_size_t i= 0; i<size; i+=4)
          vcl_cout << "test_ray_entry_point_result(" 
                   << results[i]   << ' '
                   << results[i+1] << ' '
                   << results[i+2] << ' '
                   << results[i+3] << ")\n";
    }
  driver.clean_image_cam_data();
  driver.release_kernel();
}
template <class T>
void ray_bundle_tests(ray_bundle_test_driver<T>& test_driver)
{
  boxm_ray_trace_manager<T>* ray_mgr = boxm_ray_trace_manager<T>::instance();
  vcl_string root_dir = testlib_root_dir();
  if (!ray_mgr->load_kernel_source(root_dir + "/contrib/brl/bseg/boxm/ocl/octree_library_functions.cl"))
    return;
  if (!ray_mgr->append_process_kernels(root_dir + "/contrib/brl/bseg/boxm/ocl/backproject.cl"))
    return;
  if (!ray_mgr->append_process_kernels(root_dir + "/contrib/brl/bseg/boxm/ocl/ray_bundle_library_functions.cl"))
    return;
  if (!ray_mgr->append_process_kernels(root_dir + "/contrib/brl/bseg/boxm/ocl/tests/ray_bundle_test_kernels.cl"))
    return;
  if (test_driver.build_program()!=SDK_SUCCESS)
    return;

  //START TESTS
  //================================================================
  test_load_data(test_driver);
  test_load_data_mutable(test_driver);
  test_map_work_space(test_driver);
  test_ray_entry_point(test_driver);
  //==============================================================
  //END RAY BUNDLE TESTS
  test_driver.cleanup_bundle_test();
}

static void test_ray_bundle()
{
  bool good = true;
  vcl_string root_dir = testlib_root_dir();
  ray_bundle_test_driver<float > test_driver;
  boxm_ray_trace_manager<float >* ray_mgr = boxm_ray_trace_manager<float >::instance();
  ray_mgr->set_tree(open_cl_test_data::tree<float>());
  ray_mgr->setup_tree();
  ray_mgr->setup_tree_results();
  ray_mgr->setup_tree_input_buffers();
  good = test_driver.init();
  if(good)ray_bundle_tests(test_driver);
  else{ TEST("ray_bundle_test_driver", true, false); }
  ray_mgr->clean_tree_input_buffers();
}

TESTMAIN(test_ray_bundle);
