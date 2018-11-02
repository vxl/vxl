//:
// \file
// \author Vishal jain
// \date 26-Oct-2010

#include <iostream>
#include <ios>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <testlib/testlib_test.h>
#include <testlib/testlib_root_dir.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_kernel.h>
#include <bocl/bocl_mem.h>
#include <bocl/bocl_device.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

void test_cubic_estimation()
{
  //load BOCL stuff
  bocl_manager_child* mgr = bocl_manager_child::instance();
  if (mgr->gpus_.size()==0)   return;
  bocl_device* device = mgr->gpus_[0];

  //compile pyramid test
  std::vector<std::string> src_paths;
  std::string source_dir = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
  src_paths.push_back(source_dir + "onl/onl_inverse_4x4.cl");
  src_paths.push_back(source_dir + "onl/cubic_fit.cl");
  src_paths.push_back(source_dir + "onl/test_onl_kernels.cl");

  bocl_kernel inverse_test;
  inverse_test.create_kernel(&device->context(),device->device_id(), src_paths, "test_matrix4x4_inverse", "", "test matrix 4x4 kernel");

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),
                                                *(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);

  //create all the arguments
  float data[16]={2,0,0,0,0,2,0,0,0,0,2,0,0,0,0,2};
  bocl_mem_sptr matbuff = new bocl_mem( device->context(), data, 16*sizeof(float), "input matrix");
  matbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  float odata[16];
  bocl_mem_sptr invmatbuff = new bocl_mem( device->context(), odata, 16*sizeof(float), "inverse input matrix");
  invmatbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  //set workspace
  std::size_t lThreads[] = {4, 4};
  std::size_t gThreads[] = {4, 4};
  //set first kernel args
  inverse_test.set_arg( matbuff.ptr() );
  inverse_test.set_arg( invmatbuff.ptr() );
  inverse_test.set_local_arg( lThreads[0]*lThreads[1]*sizeof(float) );//local tree,
  inverse_test.set_local_arg( lThreads[0]*lThreads[1]*sizeof(float) ); //ray bundle,
  inverse_test.set_local_arg( lThreads[0]*lThreads[1]*sizeof(float) );    //cell pointers,

  //execute kernel
  inverse_test.execute( queue, 2, lThreads, gThreads);
  clFinish( queue );
  matbuff->read_to_buffer(queue);
  invmatbuff->read_to_buffer(queue);

  for (unsigned i = 0 ; i < 16; i ++)
    std::cout<<odata[i]<<' ';

  int nobs = 16;

  float Iobs[16];
  float vis[16];
  float s[16];
  float coeffs[5]={1.2,0.4,0.5,0.6,0.0};

  float var;
  for (unsigned i =0; i<16; i++)
  {
    Iobs[i] = 0.5;
    vis[i] = 0.1;
    s[i] = (float)i/16;

    Iobs[i] = coeffs[0]+coeffs[1]*s[i]+coeffs[2]*s[i]*s[i]+coeffs[3]*s[i]*s[i]*s[i];
  }
  bocl_kernel cubic_fit_test;
  cubic_fit_test.create_kernel(&device->context(),device->device_id(), src_paths, "test_cubic_fit", "", "test cubic fitting kernel");

  bocl_mem_sptr Ibuff = new bocl_mem( device->context(), Iobs, 16*sizeof(float), "Intensities");
  Ibuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bocl_mem_sptr visbuff = new bocl_mem( device->context(), vis, 16*sizeof(float), "Visibilities");
  visbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bocl_mem_sptr sbuff = new bocl_mem( device->context(), s, 16*sizeof(float), "parameterization");
  sbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bocl_mem_sptr coeffsbuff = new bocl_mem( device->context(), coeffs, 4*sizeof(float), "coeffs");
  coeffsbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bocl_mem_sptr varbuff = new bocl_mem( device->context(), &var, 1*sizeof(float), "vars");
  varbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bocl_mem_sptr nobsbuff = new bocl_mem( device->context(), &nobs, sizeof(int), "nobs");
  nobsbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //set first kernel args
  cubic_fit_test.set_arg( Ibuff.ptr() );
  cubic_fit_test.set_arg( visbuff.ptr() );
  cubic_fit_test.set_arg( sbuff.ptr() );
  cubic_fit_test.set_arg( coeffsbuff.ptr() );
  cubic_fit_test.set_arg( varbuff.ptr() );
  cubic_fit_test.set_arg( nobsbuff.ptr() );
  cubic_fit_test.set_local_arg( lThreads[0]*lThreads[1]*sizeof(float) );//local tree,
  cubic_fit_test.set_local_arg( lThreads[0]*lThreads[1]*sizeof(float) ); //ray bundle,
  cubic_fit_test.set_local_arg( lThreads[0]*lThreads[1]*sizeof(float) );    //cell pointers,
  cubic_fit_test.set_local_arg( lThreads[0]*lThreads[1]*sizeof(float) );    //cell pointers,
  cubic_fit_test.set_local_arg( lThreads[0]*lThreads[1]*sizeof(float) );    //cell pointers,
  cubic_fit_test.set_local_arg( lThreads[0]*lThreads[1]*sizeof(float) );    //cell pointers,
  cubic_fit_test.set_local_arg( lThreads[0]*lThreads[1]*sizeof(float) );    //cell pointers,
  cubic_fit_test.set_local_arg( lThreads[0]*sizeof(float) );    //cell pointers,

  //execute kernel
  cubic_fit_test.execute( queue, 2, lThreads, gThreads);
  clFinish( queue );
  coeffsbuff->read_to_buffer(queue);
  varbuff->read_to_buffer(queue);
  for (unsigned i = 0 ; i < 4; i ++)
    std::cout<<coeffs[i]<<' ';

  std::cout<<var<<std::endl;
}


TESTMAIN( test_cubic_estimation );
