#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include "stat_test_driver.h"
#include "open_cl_test_data.h"
#include <boxm/ocl/boxm_stat_manager.h>
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boct/boct_loc_code.h>
#include <vul/vul_timer.h>
#include <vbl/vbl_array_2d.h>
static bool near_eq(float x, float y, float tol)
{ return vcl_fabs(x - y) <= tol;}

template <class T>
static void test_gaussian_pdf(stat_test_driver<T>& driver)
{
  boxm_stat_manager<T>::instance()->set_gauss_1d(0.1f, 0.5f, 1.0f);
  driver.setup_result_data(1);
  
  if (driver.create_kernel("test_gaussian_pdf")!=SDK_SUCCESS) {
    TEST("Create Kernel test_gaussian", false, true);
    return;
  }
  if (driver.set_stat_args()!=SDK_SUCCESS)
    return;
 
  if (driver.run_stat_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_gaussian", false, true);
    return;
  }
  bool good = true;
  cl_float* results = boxm_stat_manager<T>::instance()->stat_results();

  vcl_size_t size = boxm_stat_manager<T>::instance()->result_size();
    if (results) {
      float test[]={0.36827f};
      for (vcl_size_t i= 0; i<size; i++)
        good = good && near_eq(results[i],test[i], 0.0001f);
      TEST("test_gaussian", good, true);
      if (!good)
        for (vcl_size_t i= 0; i<size; i++)
          vcl_cout << "test_gaussian_result["<< i <<"](" << results[i] << ")\n";
    }
  driver.release_kernel();
}
template <class T>
static void test_gaussian_3_mixture_pdf(stat_test_driver<T>& driver)
{
  boxm_stat_manager<T>::instance()->
    set_gauss_3_mixture_1d(0.5f, 0.5f, 0.05f, 1.0f/3.0f, 
                           0.25f, 0.01f, 1.0f/3.0f, 
                           0.75f, 0.2f, 1.0f/3.0f);
  driver.setup_result_data(3);
  
  if (driver.create_kernel("test_gaussian_3_mixture_pdf")!=SDK_SUCCESS) {
    TEST("Create Kernel test_gaussian_3_mixture", false, true);
    return;
  }
  if (driver.set_stat_args()!=SDK_SUCCESS)
    return;
 
  if (driver.run_stat_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_gaussian_3_mixture", false, true);
    return;
  }
  bool good = true;
  cl_float* results = boxm_stat_manager<T>::instance()->stat_results();

  vcl_size_t size = boxm_stat_manager<T>::instance()->result_size();
    if (results) {
      float test[]={2.96403f, 3.98942f, 7.97885f};
      for (vcl_size_t i= 0; i<size; i++)
        good = good && near_eq(results[i],test[i], 0.0001f);
      TEST("test_gaussian_3_mixture", good, true);
      if (!good)
        for (vcl_size_t i= 0; i<size; i++)
          vcl_cout << "test_gaussian_3_mixture_result["<< i <<"](" << results[i] << ")\n";
    }
  driver.release_kernel();
}
template <class T>
void stat_tests(stat_test_driver<T>& test_driver)
{
  boxm_stat_manager<T>* stat_mgr = boxm_stat_manager<T>::instance();
  vcl_string root_dir = testlib_root_dir();
  if (!stat_mgr->load_kernel_source(root_dir + "/contrib/brl/bseg/boxm/ocl/statistics_library_functions.cl"))
    return;
  if (!stat_mgr->append_process_kernels(root_dir + "/contrib/brl/bseg/boxm/ocl/tests/stat_test_kernels.cl"))
    return;
  if (test_driver.build_program()!=SDK_SUCCESS)
    return;

  //START TESTS
  //================================================================
  test_gaussian_pdf(test_driver);
  test_gaussian_3_mixture_pdf(test_driver);

  //==============================================================
  //END STAT  TESTS
  test_driver.cleanup_stat_test();
}

static void test_statistics()
{
  bool good = true;
  vcl_string root_dir = testlib_root_dir();
  stat_test_driver<float > test_driver;
  boxm_stat_manager<float >* stat_mgr = boxm_stat_manager<float >::instance();
  good = test_driver.init();
  if(good)stat_tests(test_driver);
  else{ TEST("stat_test_driver", true, false); }
}

TESTMAIN(test_statistics);
