#include "bocl_global_memory_bandwidth_manager.h"
#include <cstdio>
#include <iostream>
#include <utility>
//
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#define local_workgroup_size 32

bocl_global_memory_bandwidth_manager::~bocl_global_memory_bandwidth_manager()
= default;

bool bocl_global_memory_bandwidth_manager::setup_array(unsigned len)
{
  len_=len;
  create_buffer((void**) &array_, "cl_float4",len * sizeof(cl_float4), 16);
  create_buffer((void**) &cl_len_, "cl_uint", sizeof(cl_uint), 16);
  unsigned i=0;
  //unsigned grpsize=this->group_size();
  while (i<len_*4)
  {
    array_[i]=(float)((i/4)%local_workgroup_size);
    array_[i+1]=0.0f;
    array_[i+2]=0.0f;
    array_[i+3]=0.0f;
    i+=4;
  }

  if (array_)
    return true;
  return false;
}

void bocl_global_memory_bandwidth_manager::clean_array()
{
  free_buffer(array_);
  free_buffer(cl_len_);
  len_=0;
}

bool bocl_global_memory_bandwidth_manager::setup_result_array()
{
  create_buffer((void**)&result_array_, "cl_float", len_ * sizeof(cl_float), 16);
  create_buffer((void**)&result_flag_, "cl_int", sizeof(cl_int), 16);
  unsigned i=0;
  while (i<len_)
    result_array_[i++]=0.0;
  result_flag_[0]=0;
  if (result_array_)
    return true;
  return false;
}

void bocl_global_memory_bandwidth_manager::clean_result_array()
{
  free_buffer(result_array_);
  free_buffer(result_flag_);
}

void bocl_global_memory_bandwidth_manager::create_buffers()
{
  array_buf_ = new bocl_mem(this->context(), array_, len_ * sizeof(cl_float4), "array_buf_");
  array_buf_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  result_array_buf_ = new bocl_mem(this->context(), result_array_, len_ * sizeof(cl_float), "result_array_buf_");
  result_array_buf_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  cl_len_buf_ = new bocl_mem(this->context(), cl_len_, sizeof(cl_uint), "cl_len_");
  cl_len_buf_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  result_flag_buf_ = new bocl_mem(this->context(), result_flag_, sizeof(cl_int), "result_flag_buf_");
  result_flag_buf_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
}

void bocl_global_memory_bandwidth_manager::release_buffers()
{
  array_buf_->release_memory();
  result_array_buf_->release_memory();
  cl_len_buf_->release_memory();
  result_flag_buf_->release_memory();
  kernel_.clear_args();
}

bool bocl_global_memory_bandwidth_manager::run_kernel()
{
  cl_int status = CL_SUCCESS;

  // Create and initialize memory object
  create_buffers();

  // -- Set appropriate arguments to the kernel --
  kernel_.set_arg( cl_len_buf_ );
  kernel_.set_arg( array_buf_ );
  kernel_.set_arg( result_array_buf_ );
  kernel_.set_arg( result_flag_buf_ );
  kernel_.set_local_arg(sizeof(cl_float)*this->group_size());

  cl_ulong used_local_memory;
  status = clGetKernelWorkGroupInfo(kernel_.kernel(),this->devices()[0],CL_KERNEL_LOCAL_MEM_SIZE,
                                    sizeof(cl_ulong),&used_local_memory,nullptr);
  if (!check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_LOCAL_MEM_SIZE failed."))
    return SDK_FAILURE;

  // determine the work group size
  cl_ulong kernel_work_group_size;
  status = clGetKernelWorkGroupInfo(kernel_.kernel(),this->devices()[0],CL_KERNEL_WORK_GROUP_SIZE,
                                    sizeof(cl_ulong),&kernel_work_group_size,nullptr);
  if (!check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_WORK_GROUP_SIZE, failed."))
    return SDK_FAILURE;

  std::size_t globalThreads[]= {RoundUp(len_,local_workgroup_size)};
  std::size_t localThreads[] = {local_workgroup_size};

  if (used_local_memory > this->total_local_memory())
  {
    std::cout << "Unsupported: Insufficient local memory on device.\n";
    return SDK_FAILURE;
  }

  // set up a command queue
  command_queue_ = clCreateCommandQueue(this->context(),this->devices()[0],CL_QUEUE_PROFILING_ENABLE,&status);
  if (!check_val(status,CL_SUCCESS,"Falied in command queue creation" + error_to_string(status)))
    return false;

  kernel_.execute(command_queue_, 1, localThreads, globalThreads);
  status = clFinish(command_queue_);
  time_in_secs_ = kernel_.exec_time();
  if (!check_val(status,CL_SUCCESS,"clFinish failed."+error_to_string(status)))
    return SDK_FAILURE;

  result_array_buf_->read_to_buffer(command_queue_);
  result_flag_buf_->read_to_buffer(command_queue_);

  // release the command Queue
  status = clReleaseCommandQueue(command_queue_);
  if (!check_val(status,CL_SUCCESS,"clReleaseCommandQueue failed."))
    return SDK_FAILURE;

  release_buffers();
  return CL_SUCCESS;
}

bool bocl_global_memory_bandwidth_manager::run_kernel_prefetch()
{
  cl_int status = CL_SUCCESS;

  // Create and initialize memory objects
  create_buffers();

  // -- Set appropriate arguments to the kernel --
  kernel_.set_arg( cl_len_buf_ );
  kernel_.set_arg( array_buf_ );
  kernel_.set_arg( result_array_buf_ );
  kernel_.set_arg( result_flag_buf_ );
  kernel_.set_local_arg(sizeof(cl_float4)*this->group_size());
  kernel_.set_local_arg(sizeof(cl_uint));

  cl_ulong used_local_memory;
  status = clGetKernelWorkGroupInfo(kernel_.kernel(),this->devices()[0],CL_KERNEL_LOCAL_MEM_SIZE,
                                    sizeof(cl_ulong),&used_local_memory,nullptr);
  if (!check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_LOCAL_MEM_SIZE failed."))
    return SDK_FAILURE;

  // determine the work group size
  cl_ulong kernel_work_group_size;
  status = clGetKernelWorkGroupInfo(kernel_.kernel(),this->devices()[0],CL_KERNEL_WORK_GROUP_SIZE,
                                    sizeof(cl_ulong),&kernel_work_group_size,nullptr);
  if (!check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_WORK_GROUP_SIZE, failed."))
    return SDK_FAILURE;

  std::size_t globalThreads[]= {RoundUp(len_,local_workgroup_size)};
  std::size_t localThreads[] = {local_workgroup_size};//this->group_size()

  if (used_local_memory > this->total_local_memory())
  {
    std::cout << "Unsupported: Insufficient local memory on device.\n";
    return SDK_FAILURE;
  }

  // set up a command queue
  command_queue_ = clCreateCommandQueue(this->context(),this->devices()[0],CL_QUEUE_PROFILING_ENABLE,&status);
  if (!check_val(status,CL_SUCCESS,"Falied in command queue creation" + error_to_string(status)))
    return false;

  kernel_.execute(command_queue_, 1, localThreads, globalThreads);
  status = clFinish(command_queue_);
  time_in_secs_ = kernel_.exec_time();
  if (!check_val(status,CL_SUCCESS,"clFinish failed."+error_to_string(status)))
    return SDK_FAILURE;

  result_array_buf_->read_to_buffer(command_queue_);
  result_flag_buf_->read_to_buffer(command_queue_);

  // release the command Queue
  status = clReleaseCommandQueue(command_queue_);
  if (!check_val(status,CL_SUCCESS,"clReleaseCommandQueue failed."))
    return SDK_FAILURE;

  release_buffers();
  return SDK_SUCCESS;
}


bool bocl_global_memory_bandwidth_manager::run_kernel_using_image()
{
  cl_int status = CL_SUCCESS;
  inputformat.image_channel_order = CL_RGBA;
  inputformat.image_channel_data_type = CL_FLOAT;

  // Create and initialize memory objects
  array_buf_ = new bocl_mem(this->context(), array_, curr_device_->info().image2d_max_width_ * sizeof(cl_float4), "array_buf_");
  array_buf_->create_image_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,&inputformat,
                                  this->image2d_max_width(),len_/this->image2d_max_width());

  result_array_buf_ = new bocl_mem(this->context(), result_array_, len_ * sizeof(cl_float), "result_array_buf_");
  result_array_buf_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  cl_len_buf_ = new bocl_mem(this->context(), cl_len_, sizeof(cl_uint), "cl_len_buf_");
  cl_len_buf_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  result_flag_buf_ = new bocl_mem(this->context(), result_flag_, sizeof(cl_int), "result_flag_buf_");
  result_flag_buf_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // -- Set appropriate arguments to the kernel --
  kernel_.set_arg( cl_len_buf_ );
  kernel_.set_arg( array_buf_ );
  kernel_.set_arg( result_array_buf_ );
  kernel_.set_arg( result_flag_buf_ );
  kernel_.set_local_arg(sizeof(cl_float)*this->group_size());

  cl_ulong used_local_memory;
  status = clGetKernelWorkGroupInfo(kernel_.kernel(),this->devices()[0],CL_KERNEL_LOCAL_MEM_SIZE,
                                    sizeof(cl_ulong),&used_local_memory,nullptr);
  if (!check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_LOCAL_MEM_SIZE failed."))
    return SDK_FAILURE;

  // determine the work group size
  cl_ulong kernel_work_group_size;
  status = clGetKernelWorkGroupInfo(kernel_.kernel(),this->devices()[0],CL_KERNEL_WORK_GROUP_SIZE,
                                    sizeof(cl_ulong),&kernel_work_group_size,nullptr);
  if (!check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_WORK_GROUP_SIZE, failed."))
    return SDK_FAILURE;

  std::size_t globalThreads[]= {RoundUp(len_,local_workgroup_size)};
  std::size_t localThreads[] = {local_workgroup_size};

  if (used_local_memory > this->total_local_memory())
  {
    std::cout << "Unsupported: Insufficient local memory on device.\n";
    return SDK_FAILURE;
  }

  // set up a command queue
  command_queue_ = clCreateCommandQueue(this->context(),this->devices()[0],CL_QUEUE_PROFILING_ENABLE,&status);
  if (!check_val(status,CL_SUCCESS,"Falied in command queue creation" + error_to_string(status)))
    return false;

  kernel_.execute(command_queue_, 1, localThreads, globalThreads);
  status = clFinish(command_queue_);
  time_in_secs_ = kernel_.exec_time();
  if (!check_val(status,CL_SUCCESS,"clFinish failed."+error_to_string(status)))
    return SDK_FAILURE;

  result_array_buf_->read_to_buffer(command_queue_);
  result_flag_buf_->read_to_buffer(command_queue_);

  // release the command Queue
  status = clReleaseCommandQueue(command_queue_);
  if (!check_val(status,CL_SUCCESS,"clReleaseCommandQueue failed."))
    return SDK_FAILURE;

  release_buffers();
  return SDK_SUCCESS;
}

int bocl_global_memory_bandwidth_manager::create_kernel(std::string const& kernel_name,
                                                        const std::string& src_path,
                                                        std::string options)
{
  std::vector<std::string> src_paths;
  src_paths.push_back(src_path);
  return kernel_.create_kernel(&this->context(),&this->devices()[0],
                               src_paths, kernel_name, std::move(options), "the kernel");
}
