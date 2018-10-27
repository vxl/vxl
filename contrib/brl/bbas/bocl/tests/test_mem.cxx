#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <bocl/bocl_mem.h>
#include <bocl/bocl_manager.h>
#include <vul/vul_timer.h>

bool test_fill_kernel()
{
  //initialize a buffer
  int length = 1280*720;
  int* a = new int[length];
  int* b = new int[length];
  for(int i=0; i<length; ++i) {
   a[i] = (int) i;
   b[i] = (int) i;
  }

  //make bocl manager (handles a lot of OpenCL stuff)
  bocl_manager_child &mgr = bocl_manager_child::instance();
  if(mgr.gpus_.size() < 1) {
    std::cout<<"NO GPUS!!!!"<<std::endl;
    return false;
  }

  //create command queue
  cl_command_queue queue = clCreateCommandQueue(mgr.context(), mgr.devices()[0], CL_QUEUE_PROFILING_ENABLE, nullptr);

  //cr  //make sure a is set to zero
  bocl_mem a_mem(mgr.context(), a, length * sizeof(int), "test int buffer");
  a_mem.create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  a_mem.zero_gpu_buffer(queue);
  a_mem.read_to_buffer(queue);
  for(int i=0; i<length; ++i) {
    if(a[i] != 0) {
      TEST("bocl_mem zero gpu buffer returned non zero value", true, false);
      return false;
    }
  }
  TEST("bocl_mem zero gpu buffer returned all zero values", true, true);

  //test float val
  auto* c = new float[length];
  for(int i=0; i<length; ++i)
    c[i] = (float) i / 2.0f;
  bocl_mem c_mem(mgr.context(), c, length * sizeof(float), "test float buffer");
  c_mem.create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  c_mem.fill(queue, 1.337f, "float");
  c_mem.read_to_buffer(queue);
  for(int i=0; i<length; ++i) {
    if(c[i] != 1.337f) {
      TEST("bocl_mem fill gpu buffer float returned bad value", true, false);
      return false;
    }
  }
  TEST("bocl_mem float fill gpu buffer returned all correct values", true, true);

  //make sure b is set
  bocl_mem b_mem(mgr.context(), b, length * sizeof(int), "test int buffer");
  b_mem.create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  b_mem.fill(queue, (unsigned int) 1337, "uint");
  b_mem.read_to_buffer(queue);
  for(int i=0; i<length; ++i) {
    if(b[i] != 1337) {
      TEST("bocl_mem fill gpu buffer returned bad value", true, false);
      return false;
    }
  }
  TEST("bocl_mem fill gpu buffer returned all correct values", true, true);

  //-------------------------------------------------
  // TIMING tests
  //-------------------------------------------------
  //Do timing tests on uints
  vul_timer t; t.mark();
  for(int i=0; i<1000; ++i)
    b_mem.fill(queue, (unsigned int) 1, "uint");
  float fill_time = t.all() / 1000.0f;

  //time instantiation
  t.mark();
  for(int i=0; i<1000; ++i) {
    int* ones = new int[length];
    for(int ii=0; ii<length; ++ii) ones[ii] = 1;
    b_mem.write_to_gpu_mem(queue, ones, sizeof(int)*length);
    delete[] ones;
  }
  float trans_time = t.all() / 1000.0f;

  if( fill_time < trans_time )
    std::cout<<"Kernel fill time faster on average!: ";
  else
    std::cout<<"Buffer transfer fill time faster on average: ";
  std::cout<<fill_time<<" ms vs. "<<trans_time<<" ms"<<std::endl;

  //clean up buffer
  delete[] a;
  delete[] b;
  delete[] c;
  return true;
}

static void test_mem()
{
  test_fill_kernel();
}

TESTMAIN(test_mem);
