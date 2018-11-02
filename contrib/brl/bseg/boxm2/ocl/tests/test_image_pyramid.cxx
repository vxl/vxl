//:
// \file
// \author Andy Miller
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
#include <vgl/vgl_vector_3d.h>


//Helper print method for 2d column major buffers
void print_column_major( float* buffer, int nrows, int ncols, int elementSize=4)
{
  for (int i=0; i<nrows; ++i) {
    for (int j=0; j<ncols; ++j) {
      int idx = elementSize*(nrows*j + i);
      std::cout<<'[';
      for (int el=0; el<elementSize; ++el)
        std::cout<<buffer[idx+el]<<',';
      std::cout<<']';
    }
    std::cout<<std::endl;
  }
}

void test_image()
{
  //load BOCL stuff
  bocl_manager_child &mgr = bocl_manager_child::instance();
  if (mgr.gpus_.size()==0)
    return;
  bocl_device_sptr device = mgr.gpus_[0];

  //compile pyramid test
  std::vector<std::string> src_paths;
  const std::string source_dir = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
  src_paths.push_back(source_dir + "basic/image_pyramid.cl");
  bocl_kernel pyramid_test;
  std::cout << "Reading file: " << source_dir + "basic/image_pyramid.cl" << std::endl;
  pyramid_test.create_kernel(&device->context(),device->device_id(), src_paths, "test_image_pyramid", " -D IMG_TYPE=float ", "test image pyramid kernel");

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),
                                                *(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);

  //create all the arguments
  float in_img[8*8];
  int count = 0;
  for (int j=0; j<8; ++j) {
    for (int i=0; i<8; ++i) {
      in_img[count+0] = (float) i+j;
    }
  }
  cl_uint imgdims[4] = {0, 0, 8, 8};
  float out1[4*4];
  float out2[2*2];
  float out3[1*1];
  bocl_mem_sptr obuff = new bocl_mem( device->context(), in_img, sizeof(in_img), "input image");
  obuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bocl_mem_sptr dimsbuff = new bocl_mem( device->context(), imgdims, sizeof(imgdims), "dims buff");
  dimsbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bocl_mem_sptr o1buff = new bocl_mem( device->context(), out1, sizeof(out1), "out1");
  o1buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bocl_mem_sptr o2buff = new bocl_mem( device->context(), out2, sizeof(out2), "out2");
  o2buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bocl_mem_sptr o3buff = new bocl_mem( device->context(), out3, sizeof(out3), "out3");
  o3buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //set first kernel args
  pyramid_test.set_arg( obuff.ptr() );
  pyramid_test.set_arg( dimsbuff.ptr() );
  pyramid_test.set_arg( o1buff.ptr() );
  pyramid_test.set_arg( o2buff.ptr() );
  pyramid_test.set_arg( o3buff.ptr() );

  //set workspace
  std::size_t lThreads[] = {8, 8};
  std::size_t gThreads[] = {8, 8};

  //execute kernel
  pyramid_test.execute( queue, 2, lThreads, gThreads);
  clFinish( queue );
  o1buff->read_to_buffer(queue);
  o2buff->read_to_buffer(queue);
  o3buff->read_to_buffer(queue);

  //calculate ground truth values
  float out1GT[4*4*4];
  int oc=0;
  for (int j=0; j<8; j+=2) {
    for (int i=0; i<8; i+=2) {
      //upper left idx
      int idx = (8*j + i);
      int idx1 = (8*(j+1) + i);
      int idx2 = (8*j + i+1);
      int idx3 = (8*(j+1) + i+1);
      out1GT[oc++] = (in_img[idx] + in_img[idx1] + in_img[idx2] + in_img[idx3])/4.0f;
    }
  }

  float out2GT[4*2*2];
  oc=0;
  for (int j=0; j<4; j+=2) {
    for (int i=0; i<4; i+=2) {
      //upper left idx
      int idx = (4*j + i);
      int idx1 = (4*(j+1) + i);
      int idx2 = (4*j + i+1);
      int idx3 = (4*(j+1) + i+1);
      out2GT[oc++] = (out1GT[idx] + out1GT[idx1] + out1GT[idx2] + out1GT[idx3])/4.0f;
    }
  }
  float out3GT = (out2GT[0] + out2GT[1] + out2GT[2] + out2GT[3]) / 4.0f;

  //test 4x4 image
  for (int i=0; i<4*4; ++i) {
    if ( std::fabs(out1GT[i] - out1[i]) > 1e-6f )
      std::cout<<" img1 No match at "<<i<<std::endl;
  }

  //test 2x2 image
  for (int i=0; i<2*2; ++i) {
    if ( std::fabs(out2GT[i] - out2[i]) > 1e-6f )
      std::cout<<" img2 No match at "<<i<<':'<<out2GT[i]<<" != "<<out2[i]<<std::endl;
  }

  //test 1x1 image
  if ( std::fabs(out3GT - out3[0]) > 1e-6f )
      std::cout<<" img3 No match at :"<<out3GT<<" != "<<out3[0]<<std::endl;
}


void test_ray_pyramid()
{
  //load BOCL stuff
  bocl_manager_child &mgr = bocl_manager_child::instance();
  if (mgr.gpus_.size()==0)
    return;
  bocl_device_sptr device = mgr.gpus_[0];

  //compile pyramid test
  std::vector<std::string> src_paths;
  std::string source_dir = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
  src_paths.push_back(source_dir + "basic/ray_pyramid.cl");
  bocl_kernel pyramid_test;
  pyramid_test.create_kernel(&device->context(),device->device_id(), src_paths, "test_ray_pyramid", "", "test ray pyramid kernel");

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),
                                                *(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);

  //create all the arguments
  float ray_o[8*8*4];
  float ray_d[8*8*4];
  int count = 0;
  for (int j=0; j<8; ++j) {
    for (int i=0; i<8; ++i) {
      ray_d[count+0] = (float)i;
      ray_d[count+1] = (float)j;
      ray_d[count+2] = (float)0.0f;
      ray_d[count+3] = .5f;
      count+=4;
    }
  }
  cl_uint imgdims[4] = {0, 0, 8, 8};
  float out1[4*4*4];
  float out2[2*2*4];
  float out3[1*1*4];
  bocl_mem_sptr obuff = new bocl_mem( device->context(), ray_o, sizeof(ray_o), "input ray origins");
  obuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bocl_mem_sptr dbuff = new bocl_mem( device->context(), ray_d, sizeof(ray_d), "input ray dirs");
  dbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bocl_mem_sptr dimsbuff = new bocl_mem( device->context(), imgdims, sizeof(imgdims), "dims buff");
  dimsbuff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bocl_mem_sptr o1buff = new bocl_mem( device->context(), out1, sizeof(out1), "out1");
  o1buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bocl_mem_sptr o2buff = new bocl_mem( device->context(), out2, sizeof(out2), "out2");
  o2buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bocl_mem_sptr o3buff = new bocl_mem( device->context(), out3, sizeof(out3), "out3");
  o3buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //set first kernel args
  pyramid_test.set_arg( obuff.ptr() );
  pyramid_test.set_arg( dbuff.ptr() );
  pyramid_test.set_arg( dimsbuff.ptr() );
  pyramid_test.set_arg( o1buff.ptr() );
  pyramid_test.set_arg( o2buff.ptr() );
  pyramid_test.set_arg( o3buff.ptr() );

  //set workspace
  std::size_t lThreads[] = {8, 8};
  std::size_t gThreads[] = {8, 8};

  //execute kernel
  pyramid_test.execute( queue, 2, lThreads, gThreads);
  clFinish( queue );
  o1buff->read_to_buffer(queue);
  o2buff->read_to_buffer(queue);
  o3buff->read_to_buffer(queue);
  clReleaseCommandQueue(queue);

  //calculate ground truth values
  float out1GT[4*4*4];
  int oc=0;
  for (int j=0; j<8; j+=2) {
    for (int i=0; i<8; i+=2) {
      //upper left idx
      int idx = 4*(8*j + i);
      int idx1 = 4*(8*(j+1) + i);
      int idx2 = 4*(8*j + i+1);
      int idx3 = 4*(8*(j+1) + i+1);
      vgl_vector_3d<float> r(ray_d[idx] + ray_d[idx1] + ray_d[idx2] + ray_d[idx3],
                             ray_d[idx+1] + ray_d[idx1+1] + ray_d[idx2+1] + ray_d[idx3+1],
                             ray_d[idx+2] + ray_d[idx1+2] + ray_d[idx2+2] + ray_d[idx3+2] );
      normalize(r);
      out1GT[oc++] = r.x();
      out1GT[oc++] = r.y();
      out1GT[oc++] = r.z();
      out1GT[oc++] = 1.0f;
    }
  }

  float out2GT[4*2*2];
  oc=0;
  for (int j=0; j<4; j+=2) {
    for (int i=0; i<4; i+=2) {
      //upper left idx
      int idx = 4*(4*j + i);
      int idx1 = 4*(4*(j+1) + i);
      int idx2 = 4*(4*j + i+1);
      int idx3 = 4*(4*(j+1) + i+1);
      vgl_vector_3d<float> r(out1GT[idx] + out1GT[idx1] + out1GT[idx2] + out1GT[idx3],
                             out1GT[idx+1] + out1GT[idx1+1] + out1GT[idx2+1] + out1GT[idx3+1],
                             out1GT[idx+2] + out1GT[idx1+2] + out1GT[idx2+2] + out1GT[idx3+2] );
      normalize(r);
      out2GT[oc++] = r.x();
      out2GT[oc++] = r.y();
      out2GT[oc++] = r.z();
      out2GT[oc++] = 2.0f;
    }
  }

  float out3GT[4*1*1] = {0};
  for (int i=0; i<4*2*2; i+=4) {
    out3GT[0] += out2GT[i+0];
    out3GT[1] += out2GT[i+1];
    out3GT[2] += out2GT[i+2];
    out3GT[3] += out2GT[i+3];
  }
  vgl_vector_3d<float> r(out3GT[0],out3GT[1],out3GT[2]);
  normalize(r);
  out3GT[0] = r.x();
  out3GT[1] = r.y();
  out3GT[2] = r.z();
  out3GT[3] = 4.0f;

#if 0
  std::cout<<"Out1GT.."<<std::endl;
  print_column_major(out1GT, 4, 4, 4);

  std::cout<<"Out2GT.."<<std::endl;
  print_column_major(out2GT, 2, 2, 4);
  std::cout<<"Out2.."<<std::endl;
  print_column_major(out2, 2, 2, 4);

  std::cout<<"Out3GT.."<<std::endl;
  print_column_major(out3GT,1,1,4);
  std::cout<<"Out3..."<<std::endl;
  print_column_major(out3, 1,1,4);
#endif

  //test 4x4 image
  for (int i=0; i<4*4*4; ++i) {
    if ( std::fabs(out1GT[i] - out1[i]) > 1e-6f )
      std::cout<<" img1 No match at "<<i<<std::endl;
  }

  //test 2x2 image
  for (int i=0; i<4*2*2; ++i) {
    if ( std::fabs(out2GT[i] - out2[i]) > 1e-6f )
      std::cout<<" img2 No match at "<<i<<':'<<out2GT[i]<<" != "<<out2[i]<<std::endl;
  }

  //test 1x1 image
  for (int i=0; i<4*1*1; ++i) {
    if ( std::fabs(out3GT[i] - out3[i]) > 1e-6f )
      std::cout<<" img3 No match at "<<i<<':'<<out3GT[i]<<" != "<<out3[i]<<std::endl;
  }
}

void test_image_pyramid()
{
  test_ray_pyramid();
  test_image();
}

TESTMAIN( test_image_pyramid );
