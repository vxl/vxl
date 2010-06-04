#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <bocl/bocl_global_memory_bandwidth_manager.h>

bool test_single_thread_read_bandwidth(int len, float & bandwidth)
{
  vcl_string root_dir = testlib_root_dir();
  bocl_global_memory_bandwidth_manager * mgr=bocl_global_memory_bandwidth_manager::instance();
  mgr->setup_array(len);
  mgr->setup_result_array();
  if (!mgr->load_kernel_source(root_dir + "/contrib/brl/bbas/bocl/tests/test_global_io_bandwidth.cl"))
    return false;
  if (mgr->build_kernel_program()!=SDK_SUCCESS)
    return false;

  if (mgr->create_kernel("test_single_thread_read_bandwidth")!=SDK_SUCCESS) {
    TEST("Create Kernel test_single_thread_read_bandwidth", false, true);
    return false;
  }
  if (mgr->run_kernel()!=SDK_SUCCESS) {
    TEST("Run Kernel test_single_thread_read_bandwidth", false, true);
    return false;
  }
  cl_int* result_flag = mgr->result_flag();
  bandwidth=(float)(len*4)/mgr->time_taken()/(1024*1024);


  float sum=0.0;
  cl_float * result_array=mgr->result_array();
  for (unsigned i=0;i<len;i++)
    sum+=result_array[i];

  mgr->clean_array();
  mgr->clean_result_array();
  return sum==(float)len;
}

bool test_workgroup_uncoalesced_read_bandwidth(int len, float & bandwidth)
{
  vcl_string root_dir = testlib_root_dir();
  bocl_global_memory_bandwidth_manager * mgr=bocl_global_memory_bandwidth_manager::instance();
  mgr->setup_array(len);
  mgr->setup_result_array();
  if (!mgr->load_kernel_source(root_dir + "/contrib/brl/bbas/bocl/tests/test_global_io_bandwidth.cl"))
    return false;
  if (mgr->build_kernel_program()!=SDK_SUCCESS)
    return false;

  if (mgr->create_kernel("test_workgroup_uncoalesced_read_bandwidth")!=SDK_SUCCESS) {
    TEST("Create Kernel test_workgroup_uncoalesced_read_bandwidth", false, true);
    return false;
  }
  if (mgr->run_kernel()!=SDK_SUCCESS) {
    TEST("Run Kernel test_workgroup_uncoalesced_read_bandwidth", false, true);
    return false;
  }
  cl_int* result_flag = mgr->result_flag();
  bandwidth=(float)(len*4)/mgr->time_taken()/(1024*1024);


  float sum=0.0;
  cl_float * result_array=mgr->result_array();
  for (unsigned i=0;i<len;i++)
    sum+=result_array[i];

  mgr->clean_array();
  mgr->clean_result_array();
  return sum==(float)len;
}

bool test_workgroup_coalesced_read_bandwidth(int len, float & bandwidth)
{
  vcl_string root_dir = testlib_root_dir();
  bocl_global_memory_bandwidth_manager * mgr=bocl_global_memory_bandwidth_manager::instance();
  mgr->setup_array(len);
  mgr->setup_result_array();
  if (!mgr->load_kernel_source(root_dir + "/contrib/brl/bbas/bocl/tests/test_global_io_bandwidth.cl"))
    return false;
  if (mgr->build_kernel_program()!=SDK_SUCCESS)
    return false;

  if (mgr->create_kernel("test_workgroup_coalesced_read_bandwidth")!=SDK_SUCCESS) {
    TEST("Create Kernel test_workgroup_coalesced_read_bandwidth", false, true);
    return false;
  }
  if (mgr->run_kernel()!=SDK_SUCCESS) {
    TEST("Run Kernel test_workgroup_coalesced_read_bandwidth", false, true);
    return false;
  }
  cl_int* result_flag = mgr->result_flag();
  bandwidth=(float)(len*4)/mgr->time_taken()/(1024*1024);


  float sum=0.0;
  cl_float * result_array=mgr->result_array();
  for (unsigned i=0;i<len;i++)
    sum+=result_array[i];

  mgr->clean_array();
  mgr->clean_result_array();
  return sum==(float)len;
}

bool test_single_thread_read_bandwidth_local_meory(int len, float & bandwidth)
{
  vcl_string root_dir = testlib_root_dir();
  bocl_global_memory_bandwidth_manager * mgr=bocl_global_memory_bandwidth_manager::instance();
  mgr->setup_array(len);
  mgr->setup_result_array();
  if (!mgr->load_kernel_source(root_dir + "/contrib/brl/bbas/bocl/tests/test_global_io_bandwidth.cl"))
    return false;
  if (mgr->build_kernel_program()!=SDK_SUCCESS)
    return false;

  if (mgr->create_kernel("test_single_thread_read_bandwidth_local_meory")!=SDK_SUCCESS) {
    TEST("Create Kernel test_single_thread_read_bandwidth_local_meory", false, true);
    return false;
  }
  if (mgr->run_kernel()!=SDK_SUCCESS) {
    TEST("Run Kernel test_single_thread_read_bandwidth_local_meory", false, true);
    return false;
  }
  cl_int* result_flag = mgr->result_flag();
  bandwidth=(float)(len*4)/mgr->time_taken()/(1024*1024);


  float sum=0.0;
  cl_float * result_array=mgr->result_array();
  for (unsigned i=0;i<len;i++)
    sum+=result_array[i];

  mgr->clean_array();
  mgr->clean_result_array();
  return sum==(float)len;
}


bool test_workgroup_uncoalesced_read_bandwidth_local_meory(int len, float & bandwidth)
{
  vcl_string root_dir = testlib_root_dir();
  bocl_global_memory_bandwidth_manager * mgr=bocl_global_memory_bandwidth_manager::instance();
  mgr->setup_array(len);
  mgr->setup_result_array();
  if (!mgr->load_kernel_source(root_dir + "/contrib/brl/bbas/bocl/tests/test_global_io_bandwidth.cl"))
    return false;
  if (mgr->build_kernel_program()!=SDK_SUCCESS)
    return false;

  if (mgr->create_kernel("test_workgroup_uncoalesced_read_bandwidth_local_meory")!=SDK_SUCCESS) {
    TEST("Create Kernel test_workgroup_uncoalesced_read_bandwidth_local_meory", false, true);
    return false;
  }
  if (mgr->run_kernel()!=SDK_SUCCESS) {
    TEST("Run Kernel test_workgroup_uncoalesced_read_bandwidth_local_meory", false, true);
    return false;
  }
  cl_int* result_flag = mgr->result_flag();
  bandwidth=(float)(len*4)/mgr->time_taken()/(1024*1024);

  float sum=0.0;
  cl_float * result_array=mgr->result_array();
  for (unsigned i=0;i<len;i++)
    sum+=result_array[i];

  mgr->clean_array();
  mgr->clean_result_array();
  return sum==(float)len;
}

bool test_workgroup_coalesced_read_bandwidth_local_memory(int len, float & bandwidth)
{
  vcl_string root_dir = testlib_root_dir();
  bocl_global_memory_bandwidth_manager * mgr=bocl_global_memory_bandwidth_manager::instance();
  mgr->setup_array(len);
  mgr->setup_result_array();
  if (!mgr->load_kernel_source(root_dir + "/contrib/brl/bbas/bocl/tests/test_global_io_bandwidth.cl"))
    return false;
  if (mgr->build_kernel_program()!=SDK_SUCCESS)
    return false;

  if (mgr->create_kernel("test_workgroup_coalesced_read_bandwidth_local_memory")!=SDK_SUCCESS) {
    TEST("Create Kernel test_workgroup_coalesced_read_bandwidth_local_memory", false, true);
    return false;
  }
  if (mgr->run_kernel()!=SDK_SUCCESS) {
    TEST("Run Kernel test_workgroup_coalesced_read_bandwidth_local_memory", false, true);
    return false;
  }
  cl_int* result_flag = mgr->result_flag();
  bandwidth=(float)(len*4)/mgr->time_taken()/(1024*1024);

  float sum=0.0;
  cl_float * result_array=mgr->result_array();
  for (unsigned i=0;i<len;i++)
    sum+=result_array[i];

  mgr->clean_array();
  mgr->clean_result_array();
  return sum==(float)len;
}


static void test_global_io_bandwidth()
{
  int len=1024*1024*64;
  float bandwidth=0.0f;
  if (test_single_thread_read_bandwidth(len,bandwidth))
    vcl_cout<<" test_single_thread_read_bandwidth "<<bandwidth<<vcl_endl;
  if (test_workgroup_uncoalesced_read_bandwidth(len,bandwidth))
    vcl_cout<<" test_workgroup_uncoalesced_read_bandwidth "<<bandwidth<<vcl_endl;
  if (test_workgroup_coalesced_read_bandwidth(len,bandwidth))
    vcl_cout<<" test_workgroup_coalesced_read_bandwidth "<<bandwidth<<vcl_endl;
  if (test_single_thread_read_bandwidth_local_meory(len,bandwidth))
    vcl_cout<<" test_single_thread_read_bandwidth_local_meory "<<bandwidth<<vcl_endl;
  if (test_workgroup_uncoalesced_read_bandwidth_local_meory(len,bandwidth))
    vcl_cout<<" test_workgroup_uncoalesced_read_bandwidth_local_meory "<<bandwidth<<vcl_endl;
  if (test_workgroup_coalesced_read_bandwidth_local_memory(len,bandwidth))
    vcl_cout<<" test_workgroup_coalesced_read_bandwidth_local_memory "<<bandwidth<<vcl_endl;
}

TESTMAIN(test_global_io_bandwidth);
