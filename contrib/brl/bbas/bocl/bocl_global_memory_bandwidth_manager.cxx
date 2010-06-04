#include <bocl/bocl_global_memory_bandwidth_manager.h>
#include <vcl_cstdio.h>

bocl_global_memory_bandwidth_manager::~bocl_global_memory_bandwidth_manager()
{
}

bool bocl_global_memory_bandwidth_manager::setup_array(unsigned len)
{
  len_=len;
#if defined (_WIN32)
  array_=(cl_float*)_aligned_malloc(len * sizeof(cl_float), 16);
  cl_len_=(cl_uint*)_aligned_malloc(sizeof(cl_uint),16);
#elif defined(__APPLE__)
  array_ = (cl_float*)malloc(len * sizeof(cl_float));
  cl_len_=(cl_uint*)malloc(sizeof(cl_uint));
#else
  array_ = (cl_float*)memalign(16, len * sizeof(cl_float));
  cl_len_=(cl_uint*)memalign(16,sizeof(cl_uint));

#endif
  unsigned i=0;
  unsigned grpsize=this->group_size();
  while (i<len_)
  {
    array_[i]=(float)(i%grpsize);
    i++;
  }

  if (array_)
    return true;
  return false;
}

void bocl_global_memory_bandwidth_manager::clean_array()
{
  if (array_)
  {
#ifdef _WIN32
    _aligned_free(array_);
#elif defined(__APPLE__)
    free(array_);
#else
    array_ = NULL;
#endif
  }
  len_=0;
}

bool bocl_global_memory_bandwidth_manager::setup_result_array()
{
#if defined (_WIN32)
  result_array_=(cl_float*)_aligned_malloc(len_ * sizeof(cl_float), 16);
#elif defined(__APPLE__)
  result_array_ = (cl_float*)malloc(len_ * sizeof(cl_float));
#else
  result_array_ = (cl_float*)memalign(16, len_ * sizeof(cl_float));
#endif
#if defined (_WIN32)
  result_flag_=(cl_int*)_aligned_malloc( sizeof(cl_int), 16);
#elif defined(__APPLE__)
  result_flag_ = (cl_int*)malloc(sizeof(cl_int));
#else
  result_flag_ = (cl_int*)memalign(16, sizeof(cl_int));
#endif
  unsigned i=0;
  while (i<len_)
  {
    result_array_[i]=0.0;
    i++;
  }

  result_flag_[0]=0;
  if (result_array_)
    return true;
  return false;
}

void bocl_global_memory_bandwidth_manager::clean_result_array()
{
  if (result_array_)
  {
#ifdef _WIN32
    _aligned_free(result_array_);
#elif defined(__APPLE__)
    free(result_array_);
#else
    result_array_ = NULL;
#endif
  }
}


bool bocl_global_memory_bandwidth_manager::run_kernel()
{
  cl_int status = CL_SUCCESS;
  // Create and initialize memory objects
  array_buf_ = clCreateBuffer(this->context_,
                              CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                              len_ * sizeof(cl_float),array_,
                              &status);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clCreateBuffer (input array) failed."))
    return SDK_FAILURE;

  result_array_buf_ = clCreateBuffer(this->context_,
                                     CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                     len_ * sizeof(cl_float),result_array_,
                                     &status);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clCreateBuffer (result array) failed."))
    return SDK_FAILURE;
  cl_len_buf_ = clCreateBuffer(this->context_,
                               CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                               sizeof(cl_uint),cl_len_,&status);
  if (!this->check_val(status,CL_SUCCESS,
    "clCreateBuffer (len) failed."))
    return SDK_FAILURE;
  result_flag_buf_ = clCreateBuffer(this->context_,
                                    CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                    sizeof(cl_int),result_flag_,&status);
  if (!this->check_val(status,CL_SUCCESS,
    "clCreateBuffer (result_flag) failed."))
    return SDK_FAILURE;

  // -- Set appropriate arguments to the kernel --
  status = clSetKernelArg(kernel_,0,sizeof(cl_mem),(void *)&cl_len_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (length of array)"))
    return SDK_FAILURE;

 status = clSetKernelArg(kernel_,1,sizeof(cl_mem),(void *)&array_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (input array)"))
    return SDK_FAILURE;

  status = clSetKernelArg(kernel_,2,sizeof(cl_mem),(void *)&result_array_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (result array)"))
    return SDK_FAILURE;
  status = clSetKernelArg(kernel_,3,sizeof(cl_mem),(void *)&result_flag_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (result flag)"))
    return SDK_FAILURE;
  status = clSetKernelArg(kernel_,4,sizeof(cl_float)*this->group_size(),0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (result flag)"))
    return SDK_FAILURE;

  cl_ulong used_local_memory;
  status = clGetKernelWorkGroupInfo(this->kernel(),this->devices()[0],CL_KERNEL_LOCAL_MEM_SIZE,
                                    sizeof(cl_ulong),&used_local_memory,NULL);
  if (!this->check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_LOCAL_MEM_SIZE failed."))
    return SDK_FAILURE;

  // determine the work group size
  cl_ulong kernel_work_group_size;
  status = clGetKernelWorkGroupInfo(this->kernel(),this->devices()[0],CL_KERNEL_WORK_GROUP_SIZE,
                                    sizeof(cl_ulong),&kernel_work_group_size,NULL);
  if (!this->check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_WORK_GROUP_SIZE, failed."))
    return SDK_FAILURE;

  vcl_size_t globalThreads[]= {RoundUp(len_,this->group_size())};
  vcl_size_t localThreads[] = {this->group_size()};

  if (used_local_memory > this->total_local_memory())
  {
    vcl_cout << "Unsupported: Insufficient local memory on device.\n";
    return SDK_FAILURE;
  }

  // set up a command queue
  command_queue_ = clCreateCommandQueue(this->context(),this->devices()[0],CL_QUEUE_PROFILING_ENABLE,&status);
  if (!this->check_val(status,CL_SUCCESS,"Falied in command queue creation" + error_to_string(status)))
    return false;

  cl_event ceEvent;
  status = clEnqueueNDRangeKernel(command_queue_,this->kernel_, 1,NULL,globalThreads,localThreads,0,NULL,&ceEvent);

  if (!this->check_val(status,CL_SUCCESS,"clEnqueueNDRangeKernel failed. "+error_to_string(status)))
    return SDK_FAILURE;

  status = clFinish(command_queue_);
  if (!this->check_val(status,CL_SUCCESS,"clFinish failed."+error_to_string(status)))
    return SDK_FAILURE;
  cl_ulong tstart,tend;
  status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),&tstart,0);
  status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_END,sizeof(cl_ulong),&tend,0);
  time_in_secs_=(float)(tend-tstart)/1000000000.0f;
  cl_event events[1];

  status = clEnqueueReadBuffer(command_queue_,result_array_buf_,CL_TRUE,
                               0,len_*sizeof(cl_float),
                               result_array_,
                               0,NULL,&events[0]);
  status = clWaitForEvents(1, &events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
    return SDK_FAILURE;

  status = clEnqueueReadBuffer(command_queue_,result_flag_buf_,CL_TRUE,
                               0,sizeof(cl_int),
                               result_flag_,
                               0,NULL,&events[0]);

  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (results flag)failed."))
    return SDK_FAILURE;

  // Wait for the read buffer to finish execution
  status = clWaitForEvents(1, &events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
    return SDK_FAILURE;

  status = clReleaseEvent(events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseEvent failed."))
    return SDK_FAILURE;

  // release the command Queue
  status = clReleaseCommandQueue(command_queue_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseCommandQueue failed."))
    return SDK_FAILURE;

  status = clReleaseMemObject(array_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject (array_buf_) failed."))
    return SDK_FAILURE;
  status = clReleaseMemObject(result_array_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject (result_array_buf_) failed."))
    return SDK_FAILURE;
  status = clReleaseMemObject(result_flag_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject (result_flag_buf_) failed."))
    return SDK_FAILURE;
  status = clReleaseMemObject(cl_len_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject (cl_len_buf_) failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}


int bocl_global_memory_bandwidth_manager::build_kernel_program()
{
  cl_int status = CL_SUCCESS;
  vcl_size_t sourceSize[] = { prog_.size() };
  if (!sourceSize[0]) return SDK_FAILURE;
  if (program_) {
    status = clReleaseProgram(program_);
    program_ = 0;
    if (!this->check_val(status,
      CL_SUCCESS,
      "clReleaseProgram failed."))
      return SDK_FAILURE;
  }
  const char * source = prog_.c_str();

  program_ = clCreateProgramWithSource(this->context_,
                                       1,
                                       &source,
                                       sourceSize,
                                       &status);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clCreateProgramWithSource failed."))
    return SDK_FAILURE;

  // create a cl program executable for all the devices specified
  status = clBuildProgram(program_,
                          1,
                          this->devices_,
                          NULL,
                          NULL,
                          NULL);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       error_to_string(status)))
  {
    vcl_size_t len;
    char buffer[2048];
    clGetProgramBuildInfo(program_, this->devices_[0],
                          CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
    vcl_printf("%s\n", buffer);
    return SDK_FAILURE;
  }
  else
    return SDK_SUCCESS;
}


int bocl_global_memory_bandwidth_manager::create_kernel(vcl_string const& kernel_name)
{
  cl_int status = CL_SUCCESS;
  // get a kernel object handle for a kernel with the given name
  kernel_ = clCreateKernel(program_,kernel_name.c_str(),&status);
  if (!this->check_val(status,CL_SUCCESS,error_to_string(status)))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}


int bocl_global_memory_bandwidth_manager::release_kernel()
{
  cl_int status = SDK_SUCCESS;
  if (kernel_)  {
    status = clReleaseKernel(kernel_);
  }
  kernel_ = NULL;
  if (!this->check_val(status,CL_SUCCESS,"clReleaseKernel failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}
