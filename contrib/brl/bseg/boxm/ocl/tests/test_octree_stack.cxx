#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include "octree_test_driver.h"
#include "open_cl_test_data.h"
#include <boxm/ocl/boxm_ray_trace_manager.h>
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boct/boct_loc_code.h>
#include <vul/vul_timer.h>

template <class T>
static void test_traverse_stack(octree_test_driver<T> & driver)
{
  if (driver.create_kernel("test_traverse_stack")!=SDK_SUCCESS) {
    TEST("Create Kernel test_traverse", false, true);
    return;
  }
  vul_timer t;

  if (driver.run_stack_tree_test_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_traverse", false, true);
    return;
  }
  cl_int* results = driver.tree_results();
  vcl_size_t size = 24*4;
  if (size>driver.tree_result_size_bytes())
    return;
  if (results)
  {
    int test[] = {0,0,0,0,
                  9,9,9,9,
                  2,2,2,2,
                  2,0,0,0,
                  17,17,17,17,
                  2,2,2,2,
                  0,2,0,0,
                  25,25,25,25,
                  2,2,2,2,
                  2,2,0,0,
                  33,33,33,33,
                  2,2,2,2,
                  0,0,2,0,
                  41,41,41,41,
                  2,2,2,2,
                  2,0,2,0,
                  49,49,49,49,
                  2,2,2,2,
                  0,2,2,0,
                  57,57,57,57,
                  2,2,2,2,
                  2,2,2,0,
                  65,65,65,65,
                  2,2,2,2};
    bool good = true;
    for (vcl_size_t i= 0; i<size; i++)
      good = good && results[i]==test[i];
    TEST("test_traverse_data", good, true);
    if (!good)
      for (vcl_size_t i= 0; i<size; i+=4)
        vcl_cout << "test_traverse_result(" << results[i] << ' '
                 << results[i+1] << ' '
                 << results[i+2] << ' '
                 << results[i+3] << ")\n";
	else
	{
		vcl_cout<<"Time spent "<<driver.gpu_time()/(10000.0*8)<<" ms"<<vcl_endl;
	}
  }
  driver.release_kernel();
}

template <class T>
static void test_traverse_to_level_stack(octree_test_driver<T>& driver)
{
  if (driver.create_kernel("test_traverse_to_level_stack")!=SDK_SUCCESS) {
    TEST("Create Kernel test_traverse_to_level", false, true);
    return;
  }
  if (driver.run_stack_tree_test_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_traverse_to_level", false, true);
    return;
  }
  cl_int* results = driver.tree_results();
  vcl_size_t size = 24*4;
  if (size>driver.tree_result_size_bytes())
    return;
  if (results)
  {
    int test[] ={0,0,0,1,
                 1,1,1,1,
                 1,1,1,1, // stack_ptr
                 2,0,0,1,
                 2,2,2,2,
                 1,1,1,1, // stack_ptr
                 0,2,0,1,
                 3,3,3,3,
                 1,1,1,1, // stack_ptr
                 2,2,0,1,
                 4,4,4,4,
                 1,1,1,1, // stack_ptr
                 0,0,2,1,
                 5,5,5,5,
                 1,1,1,1, // stack_ptr
                 2,0,2,1,
                 6,6,6,6,
                 1,1,1,1, // stack_ptr
                 0,2,2,1,
                 7,7,7,7,
                 1,1,1,1, // stack_ptr
                 2,2,2,1,
                 8,8,8,8,
                 1,1,1,1};
    bool good = true;
    for (vcl_size_t i= 0; i<size; i++)
      good = good && results[i]==test[i];
    TEST("test_traverse_to_level_data", good, true);
    if (!good)
      for (vcl_size_t i= 0; i<size; i+=4)
        vcl_cout << "traverse_to_level_result(" << results[i] << ' '
                 << results[i+1] << ' '
                 << results[i+2] << ' '
                 << results[i+3] << ")\n";
		else
	{
		vcl_cout<<"Time spent "<<driver.gpu_time()/(10000.0*8)<<" ms"<<vcl_endl;
	}

  }
  driver.release_kernel();
}

template <class T>
static void test_traverse_force_stack(octree_test_driver<T>& driver)
{
  if (driver.create_kernel("test_traverse_force_stack")!=SDK_SUCCESS) {
    TEST("Create Kernel test_traverse_force", false, true);
    return;
  }
  if (driver.run_stack_tree_test_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_traverse_force", false, true);
    return;
  }

  cl_int* results = driver.tree_results();
  vcl_size_t size = 12*4;
  if (size>driver.tree_result_size_bytes())
    return;
  if (results) {
    int test[]={1,0,0,0,
                10,10,10,10,
                1,1,1,1,
                0,1,0,0,
                11,11,11,11,
                1,1,1,1,
                0,0,1,0,
                13,13,13,13,
                1,1,1,1,
                1,1,1,0,
                16,16,16,16,
                1,1,1,1};
    bool good = true;
    for (vcl_size_t i= 0; i<size; i++)
      good = good && results[i]==test[i];
    TEST("test_traverse_force_data", good, true);
    if (!good)
      for (vcl_size_t i= 0; i<size; i+=4)
        vcl_cout << "traverse_force_result(" << results[i] << ' '
                 << results[i+1] << ' '
                 << results[i+2] << ' '
                 << results[i+3] << ")\n";
		else
	{
		vcl_cout<<"Time spent "<<driver.gpu_time()/(10000.0)<<" ms"<<vcl_endl;
	}

  }
  driver.release_kernel();
}


template <class T>
static void test_common_ancestor_stack(octree_test_driver<T>& driver)
{
  if (driver.create_kernel("test_common_ancestor_stack")!=SDK_SUCCESS) {
    TEST("Create Kernel test_common_ancestor", false, true);
    return;
  }
  if (driver.run_stack_tree_test_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_common_ancestor", false, true);
    return;
  }

  cl_int* results = driver.tree_results();
  vcl_size_t size = 16*4;
  if (size>driver.tree_result_size_bytes())
    return;
  if (results)
  {
    int test[] = {0,0,0,1,
                  1,1,1,1,
                  2,0,0,1,
                  2,2,2,2,
                  0,2,0,1,
                  3,3,3,3,
                  2,2,0,1,
                  4,4,4,4,
                  0,0,2,1,
                  5,5,5,5,
                  2,0,2,1,
                  6,6,6,6,
                  0,2,2,1,
                  7,7,7,7,
                  2,2,2,1,
                  8,8,8,8};
    bool good = true;
    for (vcl_size_t i= 0; i<size; i++)
      good = good && results[i]==test[i];
    TEST("test_common_ancestor_data", good, true);
    if (!good)
      for (vcl_size_t i= 0; i<size; i+=4)
        vcl_cout << "common_ancestor_result(" << results[i] << ' '
                 << results[i+1] << ' '
                 << results[i+2] << ' '
                 << results[i+3] << ")\n";
			else
	{
		vcl_cout<<"Time spent "<<driver.gpu_time()/(10000.0*8)<<" ms"<<vcl_endl;
	}


  }
  driver.release_kernel();
}


template <class T>
static void test_neighbor_stack(octree_test_driver<T>& driver)
{
  if (driver.create_kernel("test_neighbor_stack")!=SDK_SUCCESS) {
    TEST("Create Kernel test_neighbor", false, true);
    return;
  }
  if (driver.run_stack_tree_test_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_neighbor", false, true);
    return;
  }

  cl_int* results = driver.tree_results();
  vcl_size_t size = 24*4;
  if (size>driver.tree_result_size_bytes())
    return;
  if (results)
  {
    int test[] = {-1,-1,-1,-1,
                  0,0,0,0,
                  1,0,0,0,
                  10,10,10,10,
                  -1,-1,-1,-1,
                  0,0,0,0,
                  0,1,0,0,
                  11,11,11,11,
                  -1,-1,-1,-1,
                  0,0,0,0,
                  0,0,1,0,
                  13,13,13,13,
                  0,1,1,0,
                  15,15,15,15,
                  2,1,1,0,
                  23,23,23,23,
                  1,0,1,0,
                  14,14,14,14,
                  1,2,1,0,
                  30,30,30,30,
                  1,1,0,0,
                  12,12,12,12,
                  1,1,2,0,
                  44,44,44,44};
    bool good = true;
    for (vcl_size_t i= 0; i<size; i++)
      good = good && results[i]==test[i];
    TEST("test_neighbor_data", good, true);
    if (!good)
      for (vcl_size_t i= 0; i<size; i+=4)
        vcl_cout << "neighbor_result(" << results[i] << ' '
                 << results[i+1] << ' '
                 << results[i+2] << ' '
                 << results[i+3] << ")\n";
			else
	{
		vcl_cout<<"Time spent "<<driver.gpu_time()/(10000.0*6)<<" ms"<<vcl_endl;
	}


  }
  driver.release_kernel();
}


template <class T>
void tree_tests_stack(octree_test_driver<T>& test_driver)
{
  boxm_ray_trace_manager<T>* ray_mgr = boxm_ray_trace_manager<T>::instance();
  vcl_string root_dir = testlib_root_dir();
  test_driver.set_buffers();
  if (!ray_mgr->load_kernel_source(root_dir + "/contrib/brl/bseg/boxm/ocl/octree_library_functions.cl"))
    return;
  if (!ray_mgr->append_process_kernels(root_dir + "/contrib/brl/bseg/boxm/ocl/tests/octree_stack_test_kernels.cl"))
    return;
  if (test_driver.build_program()!=SDK_SUCCESS)
    return;

  //START TESTS
  //================================================================
  test_traverse_stack(test_driver);
  test_traverse_to_level_stack(test_driver);
  test_traverse_force_stack(test_driver);
  test_common_ancestor_stack(test_driver);
  test_neighbor_stack(test_driver);
  //==============================================================
  //END OCTREE TESTS
  test_driver.cleanup_tree_test();
}

static void test_octree_stack()
{
  vcl_string root_dir = testlib_root_dir();
  octree_test_driver<float > test_driver;

  boxm_ray_trace_manager<float >* ray_mgr = boxm_ray_trace_manager<float >::instance();
  ray_mgr->set_tree(open_cl_test_data::tree<float>());
  ray_mgr->setup_tree();
  ray_mgr->setup_tree_results();
  ray_mgr->setup_tree_input_buffers();
  //ray_mgr->setup_camera_input_buffer();
  //ray_mgr->setup_tree_processing();
  if (test_driver.init())
  { tree_tests_stack(test_driver); }
  else
  { TEST("octree_test_driver", true, false); }
}

TESTMAIN(test_octree_stack);
