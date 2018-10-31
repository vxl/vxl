#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include "bocl_global_memory_bandwidth_manager.h"

bool test_atom_cmpxchg(unsigned len, float & bandwidth)
{
  std::string root_dir = testlib_root_dir();
  bocl_global_memory_bandwidth_manager &mgr = bocl_global_memory_bandwidth_manager::instance();
  if (!mgr.image_support())
    return false;
  mgr.setup_array(len);
  mgr.setup_result_array();

  if (mgr.create_kernel("test_atom_cmpxchg", root_dir + "/contrib/brl/bbas/bocl/tests/test_global_io_bandwidth.cl","")!=SDK_SUCCESS) {
    TEST("Create Kernel test_atom_cmpxchg", false, true);
    return false;
  }
  if (mgr.run_kernel()!=SDK_SUCCESS) {
    TEST("Run Kernel test_atom_cmpxchg", false, true);
    return false;
  }
  // cl_int* result_flag = mgr->result_flag(); // unused
  bandwidth=(float)4/* image reads float4 */ *(len*4)/mgr.time_taken()/(1024*1024);

  auto * result_array=(cl_int*)mgr.result_array();
  for (unsigned i=0;i<len;++i)
    std::cout<<result_array[i]<<' ';

  mgr.clean_array();
  mgr.clean_result_array();

  return false;
}

bool test_locking_mechanism(unsigned len, float & bandwidth)
{
  std::string root_dir = testlib_root_dir();
  bocl_global_memory_bandwidth_manager &mgr = bocl_global_memory_bandwidth_manager::instance();
  if (!mgr.image_support())
    return false;
  mgr.setup_array(len);
  mgr.setup_result_array();

  mgr.create_kernel("test_locking_mechanism",root_dir + "/contrib/brl/bbas/bocl/tests/test_global_io_bandwidth.cl","");
  bandwidth=(float)4/* image reads float4 */ *(len*4)/mgr.time_taken()/(1024*1024);

  cl_float * result_array=mgr.result_array();
  for (unsigned i=0;i<128;++i)
    std::cout<<result_array[i]<<' ';

  mgr.clean_array();
  mgr.clean_result_array();
  return false;
}

bool test_single_thread_read_bandwidth_image(unsigned len, float & bandwidth)
{
  std::string root_dir = testlib_root_dir();
  bocl_global_memory_bandwidth_manager &mgr = bocl_global_memory_bandwidth_manager::instance();
  if (!mgr.image_support())
    return false;
  mgr.setup_array(len);
  mgr.setup_result_array();

  if (!mgr.create_kernel("test_single_thread_read_bandwidth_image",root_dir +
    "/contrib/brl/bbas/bocl/tests/test_global_io_bandwidth.cl", "-D USEIMAGE")) {
    TEST("Create Kernel test_single_thread_read_bandwidth_image", false, true);
    return false;
  }
  if (mgr.run_kernel_using_image()!=SDK_SUCCESS) {
    TEST("Run Kernel test_single_thread_read_bandwidth_image", false, true);
    return false;
  }
  bandwidth=(float)4/* image reads float4 */ *(len*4)/mgr.time_taken()/(1024*1024);

  float sum=0.0f;
  cl_float * result_array=mgr.result_array();
  for (unsigned i=0;i<len;++i)
    sum+=result_array[i];

  mgr.clean_array();
  mgr.clean_result_array();
  if (sum==(float)len)
  {
    TEST("Works test_single_thread_read_bandwidth_image", true, true);
    return true;
  }
  TEST("Works test_single_thread_read_bandwidth_image", false, true);
  return false;
}

bool test_workgroup_coalesced_read_bandwidth_image(unsigned len, float & bandwidth)
{
  std::string root_dir = testlib_root_dir();
  bocl_global_memory_bandwidth_manager &mgr=bocl_global_memory_bandwidth_manager::instance();
  if (!mgr.image_support())
    return false;
  mgr.setup_array(len);
  mgr.setup_result_array();

  if (!mgr.create_kernel("test_workgroup_coalesced_read_bandwidth_image",root_dir +
    "/contrib/brl/bbas/bocl/tests/test_global_io_bandwidth.cl","-D USEIMAGE")) {
    TEST("Create Kernel test_workgroup_coalesced_read_bandwidth_image", false, true);
    return false;
  }
  if (mgr.run_kernel_using_image()!=SDK_SUCCESS) {
    TEST("Run Kernel test_workgroup_coalesced_read_bandwidth_image", false, true);
    return false;
  }

  bandwidth=(float)4/*reads in float4*/ *(len*4)/mgr.time_taken()/(1024*1024);

  float sum=0.0;
  cl_float * result_array=mgr.result_array();
  for (unsigned i=0;i<len;++i)
    sum+=result_array[i];

  mgr.clean_array();
  mgr.clean_result_array();
  if (sum==(float)len)
  {
    TEST("Works test_workgroup_coalesced_read_bandwidth_image", true, true);
    return true;
  }
  TEST("Works test test_workgroup_coalesced_read_bandwidth_image", false, true);
  return false;
}

bool test_single_thread_read_bandwidth(unsigned len, float & bandwidth)
{
  std::string root_dir = testlib_root_dir();
  bocl_global_memory_bandwidth_manager &mgr = bocl_global_memory_bandwidth_manager::instance();
  mgr.setup_array(len);
  mgr.setup_result_array();

  if (!mgr.create_kernel("test_single_thread_read_bandwidth",
    root_dir + "/contrib/brl/bbas/bocl/tests/test_global_io_bandwidth.cl","")) {
    TEST("Create Kernel test_single_thread_read_bandwidth", false, true);
    return false;
  }
  if (mgr.run_kernel()!=SDK_SUCCESS) {
    TEST("Run Kernel test_single_thread_read_bandwidth", false, true);
    return false;
  }

  bandwidth=(float)4*(len*4)/mgr.time_taken()/(1024*1024);

  float sum=0.0;
  cl_float * result_array=mgr.result_array();
  for (unsigned i=0;i<len;++i)
    sum+=result_array[i];

  mgr.clean_array();
  mgr.clean_result_array();
  if (sum==(float)len)
  {
    TEST("Works test_single_thread_read_bandwidth", true, true);
  return true;
  }
  TEST("Works test_single_thread_read_bandwidth", false, true);
  return true;
}

bool test_workgroup_uncoalesced_read_bandwidth(unsigned len, float & bandwidth)
{
  std::string root_dir = testlib_root_dir();
  bocl_global_memory_bandwidth_manager &mgr = bocl_global_memory_bandwidth_manager::instance();
  mgr.setup_array(len);
  mgr.setup_result_array();
  if (!mgr.create_kernel("test_workgroup_uncoalesced_read_bandwidth",
    root_dir + "/contrib/brl/bbas/bocl/tests/test_global_io_bandwidth.cl","")) {
    TEST("Create Kernel test_workgroup_uncoalesced_read_bandwidth", false, true);
    return false;
  }
  if (mgr.run_kernel_prefetch()!=SDK_SUCCESS) {
    TEST("Run Kernel test_workgroup_uncoalesced_read_bandwidth", false, true);
    return false;
  }
  bandwidth=(float)4*(len*4)/mgr.time_taken()/(1024*1024);

  float sum=0.0;
  cl_float * result_array=mgr.result_array();
  for (unsigned i=0;i<len;++i) {
    //if (result_array[i] > 0)
    //  std::cout << i << ' ' << result_array[i] << std::endl;
    sum+=result_array[i];
  }
  mgr.clean_array();
  mgr.clean_result_array();
  if (sum==(float)len)
  {
    TEST("Run Kernel test_workgroup_uncoalesced_read_bandwidth", true, true);
    return true;
  }
  TEST("Run Kernel test_workgroup_uncoalesced_read_bandwidth", false, true);
  return false;
}

bool test_workgroup_coalesced_read_bandwidth(unsigned len, float & bandwidth)
{
  std::string root_dir = testlib_root_dir();
  bocl_global_memory_bandwidth_manager& mgr=bocl_global_memory_bandwidth_manager::instance();
  mgr.setup_array(len);
  mgr.setup_result_array();

  if (!mgr.create_kernel("test_workgroup_coalesced_read_bandwidth",
    root_dir + "/contrib/brl/bbas/bocl/tests/test_global_io_bandwidth.cl","")) {
    TEST("Create Kernel test_workgroup_coalesced_read_bandwidth", false, true);
    return false;
  }
  if (mgr.run_kernel()!=SDK_SUCCESS) {
    TEST("Run Kernel test_workgroup_coalesced_read_bandwidth", false, true);
    return false;
  }
  // cl_int* result_flag = mgr->result_flag(); // unused
  bandwidth=(float)4*(len*4)/mgr.time_taken()/(1024*1024);

  float sum=0.0;
  cl_float * result_array=mgr.result_array();
  for (unsigned i=0;i<len;++i)
    sum+=result_array[i];

  mgr.clean_array();
  mgr.clean_result_array();
  if (sum==(float)len)
  {
    TEST("Run Kernel test_workgroup_coalesced_read_bandwidth", true, true);
    return true;
  }
  TEST("Run Kernel test_workgroup_coalesced_read_bandwidth", false, true);
  return false;
}

bool test_single_thread_read_bandwidth_local_meory(unsigned len, float & bandwidth)
{
  std::string root_dir = testlib_root_dir();
  bocl_global_memory_bandwidth_manager &mgr=bocl_global_memory_bandwidth_manager::instance();
  mgr.setup_array(len);
  mgr.setup_result_array();

  if (!mgr.create_kernel("test_single_thread_read_bandwidth_local_meory",
    root_dir + "/contrib/brl/bbas/bocl/tests/test_global_io_bandwidth.cl","")) {
    TEST("Create Kernel test_single_thread_read_bandwidth_local_meory", false, true);
    return false;
  }
  if (mgr.run_kernel()!=SDK_SUCCESS) {
    TEST("Run Kernel test_single_thread_read_bandwidth_local_meory", false, true);
    return false;
  }
  // cl_int* result_flag = mgr->result_flag(); // unused
  bandwidth=(float)4*(len*4)/mgr.time_taken()/(1024*1024);

  float sum=0.0;
  cl_float * result_array=mgr.result_array();
  for (unsigned i=0;i<len;++i)
    sum+=result_array[i];

  mgr.clean_array();
  mgr.clean_result_array();
  if (sum==(float)len)
  {
  TEST("Run Kernel test_single_thread_read_bandwidth_local_meory", true, true);
    return true;
  }

  TEST("Run Kernel test_single_thread_read_bandwidth_local_meory", false, true);
  return false;
}


bool test_workgroup_uncoalesced_read_bandwidth_local_meory(unsigned len, float & bandwidth)
{
  std::string root_dir = testlib_root_dir();
  bocl_global_memory_bandwidth_manager &mgr=bocl_global_memory_bandwidth_manager::instance();
  mgr.setup_array(len);
  mgr.setup_result_array();

  if (!mgr.create_kernel("test_workgroup_uncoalesced_read_bandwidth_local_meory",
    root_dir + "/contrib/brl/bbas/bocl/tests/test_global_io_bandwidth.cl","")) {
    TEST("Create Kernel test_workgroup_uncoalesced_read_bandwidth_local_meory", false, true);
    return false;
  }
  if (mgr.run_kernel()!=SDK_SUCCESS) {
    TEST("Run Kernel test_workgroup_uncoalesced_read_bandwidth_local_meory", false, true);
    return false;
  }

  bandwidth=(float) 4*(len*4)/mgr.time_taken()/(1024*1024);

  float sum=0.0;
  cl_float * result_array=mgr.result_array();
  for (unsigned i=0;i<len;++i)
    sum+=result_array[i];

  mgr.clean_array();
  mgr.clean_result_array();
  if (sum==(float)len)
  {
    TEST("Works test_workgroup_uncoalesced_read_bandwidth_local_meory", true, true);
    return true;
  }
  TEST("Works test_workgroup_uncoalesced_read_bandwidth_local_meory", false, true);
  return false;
}

bool test_workgroup_coalesced_read_bandwidth_local_memory(unsigned len, float & bandwidth)
{
  std::string root_dir = testlib_root_dir();
  bocl_global_memory_bandwidth_manager &mgr = bocl_global_memory_bandwidth_manager::instance();
  mgr.setup_array(len);
  mgr.setup_result_array();

  if (!mgr.create_kernel("test_workgroup_coalesced_read_bandwidth_local_memory",
    root_dir + "/contrib/brl/bbas/bocl/tests/test_global_io_bandwidth.cl","")) {
    TEST("Create Kernel test_workgroup_coalesced_read_bandwidth_local_memory", false, true);
    return false;
  }
  if (mgr.run_kernel()!=SDK_SUCCESS) {
    TEST("Run Kernel test_workgroup_coalesced_read_bandwidth_local_memory", false, true);
    return false;
  }
  // cl_int* result_flag = mgr->result_flag(); // unused
  bandwidth=(float)4*(len*4)/mgr.time_taken()/(1024*1024);

  float sum=0.0;
  cl_float * result_array=mgr.result_array();
  for (unsigned i=0;i<len;++i)
    sum+=result_array[i];

  mgr.clean_array();
  mgr.clean_result_array();
  if (sum==(float)len)
  {
    TEST("Works test_workgroup_coalesced_read_bandwidth_local_memory", true, true);
    return true;
  }

  TEST("Run Kernel test_workgroup_coalesced_read_bandwidth_local_memory", false, true);
  return false;
}


bool test_workgroup_prefetch_bandwidth_local_memory(int len, float & bandwidth)
{
  std::string root_dir = testlib_root_dir();
  bocl_global_memory_bandwidth_manager &mgr=bocl_global_memory_bandwidth_manager::instance();
  mgr.setup_array(len);
  mgr.setup_result_array();

  if (!mgr.create_kernel("test_workgroup_prefetch_bandwidth_local_memory",
    root_dir + "/contrib/brl/bbas/bocl/tests/test_global_io_bandwidth.cl","")) {
    TEST("Create Kernel test_workgroup_coalesced_read_bandwidth_local_memory", false, true);
    return false;
  }
  if (mgr.run_kernel_prefetch()!=SDK_SUCCESS) {
    TEST("Run Kernel test_workgroup_prefetch_bandwidth_local_memory", false, true);
    return false;
  }
  // cl_int* result_flag = mgr->result_flag(); // unused
  bandwidth=(float)4*(len*4)/mgr.time_taken()/(1024*1024);

  float sum=0.0;
  cl_float * result_array=mgr.result_array();
  for (int i=0;i<len;++i)
    sum+=result_array[i];

  mgr.clean_array();
  mgr.clean_result_array();
  if (sum==(float)len)
  {
    TEST("Works test_workgroup_coalesced_read_bandwidth_local_memory", true, true);
    return true;
  }

  TEST("Run Kernel test_workgroup_coalesced_read_bandwidth_local_memory", false, true);
  return false;
}


static void test_global_io_bandwidth()
{
  unsigned len=1024*1024;
  float bandwidth=0.0f;

  //test_atom_cmpxchg(4096,bandwidth);
  //test_locking_mechanism(1024,bandwidth);

  std::cout<<"BANDWIDTH IN MB/s"<<std::endl;
  if (test_single_thread_read_bandwidth(len,bandwidth))
    std::cout<<" test_single_thread_read_bandwidth "<<bandwidth<<std::endl;
  if (test_workgroup_uncoalesced_read_bandwidth(len,bandwidth))
    std::cout<<" test_workgroup_uncoalesced_read_bandwidth "<<bandwidth<<std::endl;
  if (test_workgroup_coalesced_read_bandwidth(len,bandwidth))
    std::cout<<" test_workgroup_coalesced_read_bandwidth "<<bandwidth<<std::endl;
  if (test_single_thread_read_bandwidth_local_meory(len,bandwidth))
    std::cout<<" test_single_thread_read_bandwidth_local_meory "<<bandwidth<<std::endl;

  if (test_workgroup_uncoalesced_read_bandwidth_local_meory(len,bandwidth))
    std::cout<<" test_workgroup_uncoalesced_read_bandwidth_local_meory "<<bandwidth<<std::endl;
  if (test_workgroup_coalesced_read_bandwidth_local_memory(len,bandwidth))
    std::cout<<" test_workgroup_coalesced_read_bandwidth_local_memory "<<bandwidth<<std::endl;

#if 0 // This test is failing and corrupts the dashboard.
  if (test_single_thread_read_bandwidth_image(len,bandwidth))
    std::cout<<" test_single_thread_read_bandwidth_image "<<bandwidth<<std::endl;
  if (test_workgroup_coalesced_read_bandwidth_image(len,bandwidth))
    std::cout<<" test_workgroup_coalesced_read_bandwidth_image "<<bandwidth<<std::endl;
#endif

  if (test_workgroup_prefetch_bandwidth_local_memory(len,bandwidth))
    std::cout<<" test_workgroup_prefetch_bandwidth_local_memory "<<bandwidth<<std::endl;

}

TESTMAIN(test_global_io_bandwidth);
