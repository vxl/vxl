#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <bocl/bocl_mem.h>
#include "bocl_command_queue_mgr.h"

bool test_multiply_kernel()
{
  //initialize a buffer
  int length = 128*128;
  int* a = new int[length];
  for(int i=0; i<length; ++i) a[i] = (int) i;
  int* b = new int[length];
  for(int i=0; i<length; ++i) b[i] = (int) i;

  //get manager
  bocl_command_queue_mgr &mgr = bocl_command_queue_mgr::instance();

  //create command queue
  cl_command_queue queue = clCreateCommandQueue(mgr.context(), mgr.devices()[0], CL_QUEUE_PROFILING_ENABLE, nullptr);

  //create kernel
  std::string src_path = testlib_root_dir() + "/contrib/brl/bbas/bocl/tests/test_kernel.cl";
  std::vector<std::string> paths; paths.push_back(src_path);
  bocl_kernel test_kernel;
  test_kernel.create_kernel( &mgr.context(), &mgr.devices()[0], paths, "test_multiply", "", "bocl test multiply kernel");

  //create in and out mems
  bocl_mem a_mem(mgr.context(), a, length * sizeof(int), "test int buffer");
  a_mem.create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bocl_mem b_mem(mgr.context(), b, length * sizeof(int), "test int buffer");
  b_mem.create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //set up thread arrays
  std::size_t lthread[] = { 1, 1, 1 };
  std::size_t gthread[] = { 128, 128, 1 };

  //execute kernel
  test_kernel.set_arg(&a_mem);
  test_kernel.set_arg(&b_mem);
  test_kernel.execute(queue, 3, lthread, gthread);
  clFinish(queue);
  std::cout<<"Execution time: "<<test_kernel.exec_time()<<" ms"<<std::endl;

  //read from output
  b_mem.read_to_buffer(queue);
  for(int i=0; i<length; ++i) {
    if(b[i] != i*i)
     return false;
  }

  //clean up buffer
  delete[] a;
  delete[] b;

  return true;
}

static void test_kernel()
{
  if( test_multiply_kernel() )
    TEST("Test kernel produced correct output", true, true);
  else
    TEST("Test Kernel failed to produce correct output", true, false);

}

TESTMAIN(test_kernel);
