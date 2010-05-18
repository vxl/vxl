#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include "octree_test_driver.h"
#include "open_cl_test_data.h"
#include <boxm2/ocl/boxm_ray_trace_manager.h>
#include <boct/boct_tree.h>
#include <boct/boct_tree_cell.h>
#include <boct/boct_loc_code.h>

template <class T>
static void test_loc_code(octree_test_driver<T>& driver)
{
  if (driver.create_kernel("test_loc_code")!=SDK_SUCCESS) {
    TEST("Create Kernel test_loc_code", false, true);
    return;
  }
  if (driver.run_tree_test_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_loc_code", false, true);
    return;
  }
  cl_int* results = driver.tree_results();
  vcl_size_t size = 4*4;
  if (size>driver.tree_result_size_bytes())
    return;
  if (results) {
    int test[]={0,0,0,0,
                2,0,0,0,
                0,2,0,0,
                3,2,3,0};
    bool good = true;
    for (vcl_size_t i= 0; i<size; i++)
      good = good && results[i]==test[i];
    TEST("test_loc_code_data", good, true);
    if (!good)
      for (vcl_size_t i= 0; i<size; i+=4)
        vcl_cout << "test_loc_code_result(" << results[i] << ' '
                 << results[i+1] << ' '
                 << results[i+2] << ' '
                 << results[i+3] << ")\n";
  }
  driver.release_kernel();
}

template <class T>
static void test_traverse(octree_test_driver<T> & driver)
{
  if (driver.create_kernel("test_traverse")!=SDK_SUCCESS) {
    TEST("Create Kernel test_traverse", false, true);
    return;
  }
  if (driver.run_tree_test_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_traverse", false, true);
    return;
  }
  cl_int* results = driver.tree_results();
  vcl_size_t size = 16*4;
  if (size>driver.tree_result_size_bytes())
    return;
  if (results)
  {
    int test[] = {0,0,0,0,
                  9,9,9,9,
                  2,0,0,0,
                  17,17,17,17,
                  0,2,0,0,
                  25,25,25,25,
                  2,2,0,0,
                  33,33,33,33,
                  0,0,2,0,
                  41,41,41,41,
                  2,0,2,0,
                  49,49,49,49,
                  0,2,2,0,
                  57,57,57,57,
                  2,2,2,0,
                  65,65,65,65};
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
  }
  driver.release_kernel();
}

template <class T>
static void test_traverse_to_level(octree_test_driver<T>& driver)
{
  if (driver.create_kernel("test_traverse_to_level")!=SDK_SUCCESS) {
    TEST("Create Kernel test_traverse_to_level", false, true);
    return;
  }
  if (driver.run_tree_test_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_traverse_to_level", false, true);
    return;
  }
  cl_int* results = driver.tree_results();
  vcl_size_t size = 16*4;
  if (size>driver.tree_result_size_bytes())
    return;
  if (results)
  {
    int test[] ={0,0,0,1,
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
    TEST("test_traverse_to_level_data", good, true);
    if (!good)
      for (vcl_size_t i= 0; i<size; i+=4)
        vcl_cout << "traverse_to_level_result(" << results[i] << ' '
                 << results[i+1] << ' '
                 << results[i+2] << ' '
                 << results[i+3] << ")\n";
  }
  driver.release_kernel();
}

template <class T>
static void test_traverse_force(octree_test_driver<T>& driver)
{
  if (driver.create_kernel("test_traverse_force")!=SDK_SUCCESS) {
    TEST("Create Kernel test_traverse_force", false, true);
    return;
  }
  if (driver.run_tree_test_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_traverse_force", false, true);
    return;
  }

  cl_int* results = driver.tree_results();
  vcl_size_t size = 8*4;
  if (size>driver.tree_result_size_bytes())
    return;
  if (results) {
    int test[]={1,0,0,0,
                10,10,10,10,
                0,1,0,0,
                11,11,11,11,
                0,0,1,0,
                13,13,13,13,
                1,1,1,0,
                16,16,16,16};
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
  }
  driver.release_kernel();
}

template <class T>
static void test_cell_bounding_box(octree_test_driver<T>& driver)
{
  if (driver.create_kernel("test_cell_bounding_box")!=SDK_SUCCESS) {
    TEST("Create Kernel test_cell_bounding_box", false, true);
    return;
  }
  if (driver.run_tree_test_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_cell_bounding_box", false, true);
    return;
  }

  cl_int* results = driver.tree_results();
  vcl_size_t size = 16*4;
  if (size>driver.tree_result_size_bytes())
    return;
  if (results)
  {
    int test[] = {0,0,0,0,
                  250,250,250,0,
                  500,0,0,0,
                  750,250,250,0,
                  0,500,0,0,
                  250,750,250,0,
                  500,500,0,0,
                  750,750,250,0,
                  0,0,500,0,
                  250,250,750,0,
                  500,0,500,0,
                  750,250,750,0,
                  0,500,500,0,
                  250,750,750,0,
                  500,500,500,0,
                  750,750,750,0};
    bool good = true;
    for (vcl_size_t i= 0; i<size; i++)
      good = good && results[i]==test[i];
    TEST("test_cell_bounding_box_data", good, true);
    if (!good)
      for (vcl_size_t i= 0; i<size; i+=4)
        vcl_cout << "cell_bounding_box_result(" << results[i] << ' '
                 << results[i+1] << ' '
                 << results[i+2] << ' '
                 << results[i+3] << ")\n";
  }
  driver.release_kernel();
}

template <class T>
static void test_common_ancestor(octree_test_driver<T>& driver)
{
  if (driver.create_kernel("test_common_ancestor")!=SDK_SUCCESS) {
    TEST("Create Kernel test_common_ancestor", false, true);
    return;
  }
  if (driver.run_tree_test_kernels()!=SDK_SUCCESS) {
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
  }
  driver.release_kernel();
}

template <class T>
static void test_cell_exit_face(octree_test_driver<T>& driver)
{
  if (driver.create_kernel("test_cell_exit_face")!=SDK_SUCCESS) {
    TEST("Create Kernel test_cell_exit_face", false, true);
    return;
  }
  if (driver.run_tree_test_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_cell_exit_face", false, true);
    return;
  }

  cl_int* results = driver.tree_results();
  vcl_size_t size = 6*4;
  if (size>driver.tree_result_size_bytes())
    return;
  if (results) {
    int test[] = {1,0,0,0,
                  1,0,0,1,
                  0,1,0,0,
                  0,1,0,1,
                  0,0,1,0,
                  0,0,1,1};
    bool good = true;
    for (vcl_size_t i= 0; i<size; i++)
      good = good && results[i]==test[i];
    TEST("test_cell_exit_face_data", good, true);
    if (!good)
      for (vcl_size_t i= 0; i<size; i+=4)
        vcl_cout << "cell_exit_face_result(" << results[i] << ' '
                 << results[i+1] << ' '
                 << results[i+2] << ' '
                 << results[i+3] << ")\n";
  }
  driver.release_kernel();
}

template <class T>
static void test_neighbor(octree_test_driver<T>& driver)
{
  if (driver.create_kernel("test_neighbor")!=SDK_SUCCESS) {
    TEST("Create Kernel test_neighbor", false, true);
    return;
  }
  if (driver.run_tree_test_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_neighbor", false, true);
    return;
  }

  cl_int* results = driver.tree_results();
  vcl_size_t size = 12*4;
  if (size>driver.tree_result_size_bytes())
    return;
  if (results)
  {
    int test[] = {-1,-1,-1,-1,
                  -1,-1,-1,-1,
                  1,0,0,0,
                  10,10,10,10,
                  -1,-1,-1,-1,
                  -1,-1,-1,-1,
                  0,1,0,0,
                  11,11,11,11,
                  -1,-1,-1,-1,
                  -1,-1,-1,-1,
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
  }
  driver.release_kernel();
}

template <class T>
static void test_ray_trace(octree_test_driver<T>& driver)
{
  if (driver.create_kernel("test_ray_trace")!=SDK_SUCCESS) {
    TEST("Create Kernel test_ray_trace", false, true);
    return;
  }
  if (driver.run_ray_trace_test_kernels()!=SDK_SUCCESS) {
    TEST("Run Kernel test_ray_trace", false, true);
    return;
  }

  cl_int* results = driver.tree_results();
  vcl_size_t size = 48*4;
  if (size>driver.tree_result_size_bytes())
    return;
  if (results)
  {
    int test[] = {0,0,3,0, 250,250,250,250,
                  0,0,2,0, 250,250,250,250,
                  0,0,1,0, 250,250,250,250,
                  0,0,0,0, 250,250,250,250,
                  0,0,0,0, 250,250,250,250,
                  1,0,0,0, 250,250,250,250,
                  2,0,0,0, 250,250,250,250,
                  3,0,0,0, 250,250,250,250,
                  0,0,0,0, 250,250,250,250,
                  0,1,0,0, 250,250,250,250,
                  0,2,0,0, 250,250,250,250,
                  0,3,0,0, 250,250,250,250,
                  3,3,3,0, 433,433,433,433,
                  2,2,2,0, 433,433,433,433,
                  1,1,1,0, 433,433,433,433,
                  0,0,0,0, 433,433,433,433,
                  0,3,3,0, 433,433,433,433,
                  1,2,2,0, 433,433,433,433,
                  2,1,1,0, 433,433,433,433,
                  3,0,0,0, 433,433,433,433,
                  3,0,3,0, 433,433,433,433,
                  2,1,2,0, 433,433,433,433,
                  1,2,1,0, 433,433,433,433,
                  0,3,0,0, 433,433,433,433};
    bool good = true;
    for (vcl_size_t i= 0; i<size; i++)
      good = good && results[i]==test[i];
    TEST("test_ray_trace_data", good, true);
    if (!good)
      for (vcl_size_t i= 0; i<size; i+=4)
        vcl_cout << "ray_trace_result(" << results[i] << ' '
                 << results[i+1] << ' '
                 << results[i+2] << ' '
                 << results[i+3] << ")\n";
  }
  driver.release_kernel();
}

template <class T>
void tree_tests(octree_test_driver<T>& test_driver)
{
  boxm_ray_trace_manager<T>* ray_mgr = boxm_ray_trace_manager<T>::instance();
  vcl_string root_dir = testlib_root_dir();
  test_driver.set_buffers();
  if (!ray_mgr->load_kernel_source(root_dir + "/contrib/brl/bseg/boxm/opt/open_cl/octree_library_functions.cl"))
    return;
  if (!ray_mgr->append_process_kernels(root_dir + "/contrib/brl/bseg/boxm/opt/open_cl/tests/octree_test_kernels.cl"))
    return;
  if (test_driver.build_program()!=SDK_SUCCESS)
    return;

  //START TESTS
  //================================================================
  test_loc_code(test_driver);
  test_traverse(test_driver);
  test_traverse_to_level(test_driver);
  test_traverse_force(test_driver);
  test_cell_bounding_box(test_driver);
  test_common_ancestor(test_driver);
  test_cell_exit_face(test_driver);
  test_neighbor(test_driver);
  //test_ray_trace(test_driver);
  //==============================================================
  //END OCTREE TESTS
  test_driver.cleanup_tree_test();
}

static void test_octree()
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
  { tree_tests(test_driver); }
  else
  { TEST("octree_test_driver", true, false); }
}

TESTMAIN(test_octree);
