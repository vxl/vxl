#include <iostream>
#include <vcl_where_root_dir.h>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_cholesky.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_kernel.h>
#include <bocl/bocl_mem.h>
#include <bocl/bocl_device.h>
#include <boxm2/ocl/tests/boxm2_ocl_test_utils.h>

void ocl_levenberg_marquardt(vnl_vector<float> x,
                             vnl_vector<float> y,
                             int m, int n, int  /*max_iter*/)
{
  //load BOCL stuff
  bocl_manager_child &mgr = bocl_manager_child::instance();
  if (mgr.gpus_.size()==0)   return;
  bocl_device_sptr device = mgr.gpus_[0];

  //compile pyramid test
  std::vector<std::string> src_paths;
  std::string source_dir = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
  src_paths.push_back(source_dir + "onl/test_levenberg_marquardt.cl");
  src_paths.push_back(source_dir + "onl/cholesky_decomposition.cl");
  src_paths.push_back(source_dir + "onl/quadratic_example.cl");
  src_paths.push_back(source_dir + "onl/levenberg_marquardt.cl");


  bocl_kernel lm_test;
  lm_test.create_kernel(&device->context(),device->device_id(), src_paths, "test_levenberg_marquardt", "-D QUADRATIC", "test levenberg marquardt");

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),
                                                *(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);

  //create all the arguments
  bocl_mem_sptr xbuff = new bocl_mem( device->context(), x.data_block(), x.size()*sizeof(float), "x vector");
  xbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr ybuff = new bocl_mem( device->context(), y.data_block(), y.size()*sizeof(float), "y vector");
  ybuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr mbuff = new bocl_mem( device->context(), &m, sizeof(int), "dimension of x");
  mbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr nbuff = new bocl_mem( device->context(), &n, sizeof(int), "dimension of y");
  nbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  int maxiter = 100;
  bocl_mem_sptr maxiterbuff = new bocl_mem( device->context(), &maxiter, sizeof(int), "Maximum # of iterations");
  maxiterbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  float output[1000];
  bocl_mem_sptr outputbuff = new bocl_mem( device->context(), output, 1000* sizeof(float), "Vector for Output");
  outputbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //set workspace
  std::size_t lThreads[] = {16};
  std::size_t gThreads[] = {16};
  //set first kernel args
  lm_test.set_arg( maxiterbuff.ptr() );
  lm_test.set_arg( nbuff.ptr() );
  lm_test.set_arg( mbuff.ptr() );
  lm_test.set_arg( xbuff.ptr() );
  lm_test.set_arg( ybuff.ptr() );
  lm_test.set_arg( outputbuff.ptr() );
  lm_test.set_local_arg( x.size()*sizeof(float) );          //local tree,
  lm_test.set_local_arg( x.size()*sizeof(float) );          //local tree,
  lm_test.set_local_arg( y.size()*sizeof(float) );          //local tree,
  lm_test.set_local_arg( x.size()*sizeof(float) );          //local tree,
  lm_test.set_local_arg( x.size()*y.size()*sizeof(float) ); //local tree,
  lm_test.set_local_arg( x.size()*x.size()*sizeof(float) ); //local tree,
  lm_test.set_local_arg( y.size()*sizeof(float) );          //local tree,
  lm_test.set_local_arg( x.size()*sizeof(float) );          //local tree,
  //execute kernel
  lm_test.execute( queue, 1, lThreads, gThreads);
  clFinish( queue );
  outputbuff->read_to_buffer(queue);

  for ( unsigned i = 0 ; i < 10; i++)
    std::cout<<output[i]<<' ';
}

void test_ocl_levenberg_marquardt()
{
  vnl_vector<float>  x(3) ;
  x[0] = 1.0;
  x[1] = 2.0;
  x[2] = 7.0;
  int n = x.size();

  vnl_vector<float>  y(2) ;

  y[0] = 2.0;
  y[1] = 2.0;
  int m = y.size();

  int max_iter = 100;
  ocl_levenberg_marquardt(x,y,m,n,max_iter);
}


TESTMAIN(test_ocl_levenberg_marquardt);
