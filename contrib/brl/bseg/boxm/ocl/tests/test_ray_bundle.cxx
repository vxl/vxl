#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include "ray_bundle_test_driver.h"
#include "open_cl_test_data.h"
#include <boxm/ocl/boxm_ray_trace_manager.h>
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boct/boct_loc_code.h>
#include <vul/vul_timer.h>

template <class T>
static void test_load_data(ray_bundle_test_driver<T>& driver)
{
  if (driver.create_kernel("test_load_data")!=SDK_SUCCESS) {
    TEST("Create Kernel test_loc_code", false, true);
    return;
  }
  vul_timer t;
  if (driver.run_bundle_test_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_loc_code", false, true);
    return;
  }
  
  cl_int* results = driver.tree_results();
  int count = results[0];
  vcl_cout << "Transfer rate = " << 64.0f*(count/(static_cast<float>(t.real())))/1000.0f << " Mbytes/second\n";
  bool good = false;
  vcl_size_t size = 6*4;
  if (size>driver.tree_result_size_bytes())
    return;
#if 0
  if (results) {
    int test[]={0,0,0,0,
                2,0,0,0,
                0,2,0,0,
                3,2,3,0};
    for (vcl_size_t i= 0; i<size; i++)
      good = good && results[i]==test[i];
    TEST("test_loc_code_data", good, true);
#endif
    if (!good)
      for (vcl_size_t i= 0; i<size; i+=4)
        vcl_cout << "test_loc_code_result(" << results[i] << ' '
                 << results[i+1] << ' '
                 << results[i+2] << ' '
                 << results[i+3] << ")\n";
  
  driver.release_kernel();
}


template <class T>
void tree_tests(ray_bundle_test_driver<T>& test_driver)
{
  boxm_ray_trace_manager<T>* ray_mgr = boxm_ray_trace_manager<T>::instance();
  vcl_string root_dir = testlib_root_dir();
  test_driver.set_buffers();
  if (!ray_mgr->load_kernel_source(root_dir + "/contrib/brl/bseg/boxm/ocl/octree_library_functions.cl"))
    return;
  if (!ray_mgr->append_process_kernels(root_dir + "/contrib/brl/bseg/boxm/ocl/tests/ray_bundle_test_kernels.cl"))
    return;
  if (test_driver.build_program()!=SDK_SUCCESS)
    return;

  //START TESTS
  //================================================================
  test_load_data(test_driver);
  //==============================================================
  //END OCTREE TESTS
  test_driver.cleanup_bundle_test();
}

static void test_ray_bundle()
{
  vcl_string root_dir = testlib_root_dir();
  ray_bundle_test_driver<float > test_driver;
  test_driver.set_ni(8);
  test_driver.set_nj(8);
  boxm_ray_trace_manager<float >* ray_mgr = boxm_ray_trace_manager<float >::instance();
  ray_mgr->set_tree(open_cl_test_data::tree<float>());
  ray_mgr->setup_tree();
  ray_mgr->setup_tree_results();
  ray_mgr->setup_tree_input_buffers();
  if (test_driver.init())
  { tree_tests(test_driver); }
  else
  { TEST("ray_bundle_test_driver", true, false); }
}

TESTMAIN(test_ray_bundle);
