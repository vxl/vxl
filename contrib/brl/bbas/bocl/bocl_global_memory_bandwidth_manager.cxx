#include "bocl_global_memory_bandwidth_manager.h"
//
#include <bocl/bocl_buffer_mgr.h>
#include <vcl_cstdio.h>

bocl_global_memory_bandwidth_manager::~bocl_global_memory_bandwidth_manager()
{
}

bool bocl_global_memory_bandwidth_manager::setup_array(unsigned len)
{
  len_=len;
  create_buffer((void**)&array_,"cl_float4",len,16);
  create_buffer((void**)&cl_len_,"cl_uint",1,16);

  unsigned i=0;
  unsigned grpsize=this->group_size();
  while (i<len_*4)
  {
    array_[i]=(float)((i/4)%grpsize);
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
  len_=0;
}

bool bocl_global_memory_bandwidth_manager::setup_result_array()
{
  create_buffer((void**)&result_array_,"cl_float",len_,16);
  create_buffer((void**)&result_flag_,"cl_int",1,16);

  unsigned i=0;
  while (i<len_)
  {
    result_array_[i]=0.0;
    i++;
  }
  result_array_[0]=0.0;
  result_flag_[0]=0;
  if (result_array_)
    return true;
  return false;
}

void bocl_global_memory_bandwidth_manager::clean_result_array()
{
  free_buffer(result_array_);
}


bool bocl_global_memory_bandwidth_manager::run_kernel()
{
  cl_int status = CL_SUCCESS;
  // Create and initialize memory objects
  if (!BOCL_BUFFER_MGR->create_read_buffer(this->context_,ARRAY_BUFFER_NAME, array_, len_*sizeof(cl_float4)))
    return SDK_FAILURE;

  if (!BOCL_BUFFER_MGR->create_write_buffer(this->context_,RESULT_ARRAY_BUFFER_NAME, result_array_ ,len_*sizeof(cl_float)))
    return SDK_FAILURE;

  if (!BOCL_BUFFER_MGR->create_read_buffer(this->context_,CL_LEN_BUFFER_NAME, cl_len_ ,sizeof(cl_uint)))
    return SDK_FAILURE;

  if (!BOCL_BUFFER_MGR->create_write_buffer(this->context_,RESULT_FLAG_BUFFER_NAME, result_flag_ ,sizeof(cl_int)))
    return SDK_FAILURE;

  // -- Set appropriate arguments to the kernel --
  if (!kernel_.set_arg(0,CL_LEN_BUFFER_NAME))
    return SDK_FAILURE;

  if (!kernel_.set_arg(1,ARRAY_BUFFER_NAME))
    return SDK_FAILURE;

  if (!kernel_.set_arg(2,RESULT_ARRAY_BUFFER_NAME))
    return SDK_FAILURE;

  if (!kernel_.set_arg(3,RESULT_FLAG_BUFFER_NAME))
    return SDK_FAILURE;

  if (!kernel_.set_local_arg(4,sizeof(cl_float)*this->group_size()))
    return SDK_FAILURE;

  cl_ulong used_local_memory;
  status = clGetKernelWorkGroupInfo(kernel_.kernel(),this->devices()[0],CL_KERNEL_LOCAL_MEM_SIZE,
                                    sizeof(cl_ulong),&used_local_memory,NULL);
  if (!this->check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_LOCAL_MEM_SIZE failed."))
    return SDK_FAILURE;

  // determine the work group size
  cl_ulong kernel_work_group_size;
  status = clGetKernelWorkGroupInfo(kernel_.kernel(),this->devices()[0],CL_KERNEL_WORK_GROUP_SIZE,
                                    sizeof(cl_ulong),&kernel_work_group_size,NULL);
  if (!this->check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_WORK_GROUP_SIZE, failed."))
    return SDK_FAILURE;

  vcl_size_t globalThreads[]= {RoundUp(len_,64)};
  vcl_size_t localThreads[] = {64};

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
  status = clEnqueueNDRangeKernel(command_queue_,kernel_.kernel(), 1,NULL,globalThreads,localThreads,0,NULL,&ceEvent);

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

  if (!BOCL_BUFFER_MGR->enqueue_read_buffer(command_queue_,RESULT_ARRAY_BUFFER_NAME,CL_TRUE,0,len_*sizeof(cl_float),
                                           result_array_,0,NULL,&events[0]))
    return SDK_FAILURE;

  status = clWaitForEvents(1, &events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
    return SDK_FAILURE;

  if (!BOCL_BUFFER_MGR->enqueue_read_buffer(command_queue_,RESULT_FLAG_BUFFER_NAME,CL_TRUE,0,sizeof(cl_int),
                                            result_flag_,0,NULL,&events[0]))
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

  if (!BOCL_BUFFER_MGR->release_buffer(ARRAY_BUFFER_NAME) ||
      !BOCL_BUFFER_MGR->release_buffer(RESULT_ARRAY_BUFFER_NAME) ||
      !BOCL_BUFFER_MGR->release_buffer(RESULT_FLAG_BUFFER_NAME) ||
      !BOCL_BUFFER_MGR->release_buffer(CL_LEN_BUFFER_NAME))
    return SDK_FAILURE;
  return SDK_SUCCESS;
}

bool bocl_global_memory_bandwidth_manager::run_kernel_prefetch()
{
  cl_int status = CL_SUCCESS;
  // Create and initialize memory objects
  if (!BOCL_BUFFER_MGR->create_read_buffer(this->context_,ARRAY_BUFFER_NAME, array_, len_*sizeof(cl_float4)))
    return SDK_FAILURE;

  if (!BOCL_BUFFER_MGR->create_write_buffer(this->context_,RESULT_ARRAY_BUFFER_NAME, result_array_ ,len_*sizeof(cl_float)))
    return SDK_FAILURE;

  if (!BOCL_BUFFER_MGR->create_read_buffer(this->context_,CL_LEN_BUFFER_NAME, cl_len_ ,sizeof(cl_uint)))
    return SDK_FAILURE;

  if (!BOCL_BUFFER_MGR->create_write_buffer(this->context_,RESULT_FLAG_BUFFER_NAME, result_flag_ ,sizeof(cl_int)))
    return SDK_FAILURE;

  // -- Set appropriate arguments to the kernel --
  if (!kernel_.set_arg(0,CL_LEN_BUFFER_NAME))
    return SDK_FAILURE;

  if (!kernel_.set_arg(1,ARRAY_BUFFER_NAME))
    return SDK_FAILURE;

  if (!kernel_.set_arg(2,RESULT_ARRAY_BUFFER_NAME))
    return SDK_FAILURE;

  if (!kernel_.set_arg(3,RESULT_FLAG_BUFFER_NAME))
    return SDK_FAILURE;

  if (!kernel_.set_local_arg(4,sizeof(cl_float4)*this->group_size()))
    return SDK_FAILURE;

  if (!kernel_.set_local_arg(5,sizeof(cl_uint)))
    return SDK_FAILURE;

  cl_ulong used_local_memory;
  status = clGetKernelWorkGroupInfo(kernel_.kernel(),this->devices()[0],CL_KERNEL_LOCAL_MEM_SIZE,
                                    sizeof(cl_ulong),&used_local_memory,NULL);
  if (!this->check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_LOCAL_MEM_SIZE failed."))
    return SDK_FAILURE;

  // determine the work group size
  cl_ulong kernel_work_group_size;
  status = clGetKernelWorkGroupInfo(kernel_.kernel(),this->devices()[0],CL_KERNEL_WORK_GROUP_SIZE,
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
  status = clEnqueueNDRangeKernel(command_queue_,kernel_.kernel(), 1,NULL,globalThreads,localThreads,0,NULL,&ceEvent);

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

  if (!BOCL_BUFFER_MGR->enqueue_read_buffer(command_queue_,RESULT_ARRAY_BUFFER_NAME,CL_TRUE,0,len_*sizeof(cl_float),
                                           result_array_,0,NULL,&events[0]))
    return SDK_FAILURE;

  status = clWaitForEvents(1, &events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
    return SDK_FAILURE;

  if (!BOCL_BUFFER_MGR->enqueue_read_buffer(command_queue_,RESULT_FLAG_BUFFER_NAME,CL_TRUE,0,sizeof(cl_int),
                                            result_flag_,0,NULL,&events[0]))
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

  if (!BOCL_BUFFER_MGR->release_buffer(ARRAY_BUFFER_NAME) ||
      !BOCL_BUFFER_MGR->release_buffer(RESULT_ARRAY_BUFFER_NAME) ||
      !BOCL_BUFFER_MGR->release_buffer(RESULT_FLAG_BUFFER_NAME) ||
      !BOCL_BUFFER_MGR->release_buffer(CL_LEN_BUFFER_NAME))
    return SDK_FAILURE;
  return SDK_SUCCESS;
}


bool bocl_global_memory_bandwidth_manager::run_kernel_using_image()
{
  cl_int status = CL_SUCCESS;
  inputformat.image_channel_order = CL_RGBA;
  inputformat.image_channel_data_type = CL_FLOAT;

#if 0
  // Create and initialize memory objects
  cl_mem array_buf = clCreateImage2D(this->context_,
                                     CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,&inputformat,image2d_max_width_,len_/image2d_max_width_,
                                     image2d_max_width_ * sizeof(cl_float4),array_,&status);

  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clCreateBuffer (input array) failed."))
    return SDK_FAILURE;
#endif

  if (!BOCL_BUFFER_MGR->create_image2D(this->context_,ARRAY_BUFFER_NAME,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,&inputformat,image2d_max_width_,len_/image2d_max_width_,
                               image2d_max_width_ * sizeof(cl_float4),array_))
    return SDK_FAILURE;

  if (!BOCL_BUFFER_MGR->create_write_buffer(this->context_,RESULT_ARRAY_BUFFER_NAME, result_array_ ,len_*sizeof(cl_float)))
    return SDK_FAILURE;

  if (!BOCL_BUFFER_MGR->create_read_buffer(this->context_,CL_LEN_BUFFER_NAME, cl_len_ ,sizeof(cl_uint)))
    return SDK_FAILURE;

  if (!BOCL_BUFFER_MGR->create_write_buffer(this->context_,RESULT_FLAG_BUFFER_NAME, result_flag_ ,sizeof(cl_int)))
    return SDK_FAILURE;

  // -- Set appropriate arguments to the kernel --
  if (!kernel_.set_arg(0,CL_LEN_BUFFER_NAME))
    return SDK_FAILURE;

  if (!kernel_.set_arg(1,ARRAY_BUFFER_NAME))
    return SDK_FAILURE;

  if (!kernel_.set_arg(2,RESULT_ARRAY_BUFFER_NAME))
    return SDK_FAILURE;

  if (!kernel_.set_arg(3,RESULT_FLAG_BUFFER_NAME))
    return SDK_FAILURE;

  if (!kernel_.set_local_arg(4,sizeof(cl_float)*this->group_size()))
    return SDK_FAILURE;

  cl_ulong used_local_memory;
  status = clGetKernelWorkGroupInfo(kernel_.kernel(),this->devices()[0],CL_KERNEL_LOCAL_MEM_SIZE,
                                    sizeof(cl_ulong),&used_local_memory,NULL);
  if (!this->check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_LOCAL_MEM_SIZE failed."))
    return SDK_FAILURE;

  // determine the work group size
  cl_ulong kernel_work_group_size;
  status = clGetKernelWorkGroupInfo(kernel_.kernel(),this->devices()[0],CL_KERNEL_WORK_GROUP_SIZE,
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
  cl_event events[1];

  //status=clEnqueueWriteBuffer(command_queue_,array_buf_,CL_FALSE,0,len_*sizeof(float),array_,0,NULL,NULL);
  //if (!this->check_val(status,CL_SUCCESS,"clEnqueueWriteBuffer failed. "+error_to_string(status)))
  //  return SDK_FAILURE;

  cl_event ceEvent;
  status = clEnqueueNDRangeKernel(command_queue_,kernel_.kernel(), 1,NULL,globalThreads,localThreads,0,NULL,&ceEvent);

  if (!this->check_val(status,CL_SUCCESS,"clEnqueueNDRangeKernel failed. "+error_to_string(status)))
    return SDK_FAILURE;

  status = clFinish(command_queue_);
  if (!this->check_val(status,CL_SUCCESS,"clFinish failed."+error_to_string(status)))
    return SDK_FAILURE;
  cl_ulong tstart,tend;
  status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),&tstart,0);
  status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_END,sizeof(cl_ulong),&tend,0);
  time_in_secs_=(float)(tend-tstart)/1000000000.0f;

  if (!BOCL_BUFFER_MGR->enqueue_read_buffer(command_queue_,RESULT_ARRAY_BUFFER_NAME,CL_TRUE,0,len_*sizeof(cl_float),
                                            result_array_,0,NULL,&events[0]))
    return SDK_FAILURE;
  status = clWaitForEvents(1, &events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
    return SDK_FAILURE;

  if (!BOCL_BUFFER_MGR->enqueue_read_buffer(command_queue_,RESULT_FLAG_BUFFER_NAME,CL_TRUE,0,sizeof(cl_int),
                                            result_flag_,0,NULL,&events[0]))
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

  if (!BOCL_BUFFER_MGR->release_buffer(ARRAY_BUFFER_NAME) ||
      !BOCL_BUFFER_MGR->release_buffer(RESULT_ARRAY_BUFFER_NAME) ||
      !BOCL_BUFFER_MGR->release_buffer(RESULT_FLAG_BUFFER_NAME) ||
      !BOCL_BUFFER_MGR->release_buffer(CL_LEN_BUFFER_NAME))
    return SDK_FAILURE;

  return SDK_SUCCESS;
}

int bocl_global_memory_bandwidth_manager::build_kernel_program(bool useimage)
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

  if (!useimage)
  {
    // create a cl program executable for all the devices specified
    status = clBuildProgram(program_,
                            1,
                            this->devices_,
                            NULL,
                            NULL,
                            NULL);
#if 0
    unsigned char buffer[21740];
    vcl_size_t length[10];
    vcl_size_t bufflength=0;
    clGetProgramInfo(program_,CL_PROGRAM_BINARY_SIZES,sizeof(vcl_size_t),&length,&bufflength);
    clGetProgramInfo(program_,CL_PROGRAM_BINARIES,21740,(void*)&buffer,&bufflength);
    vcl_cout<<length[0];
    int a; vcl_cin>>a;
#endif // 0
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
  }
  else
  {
    status = clBuildProgram(program_,
                            1,
                            this->devices_,
                            "-D USEIMAGE",
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
  }

  return SDK_SUCCESS;
}

int bocl_global_memory_bandwidth_manager::create_kernel(vcl_string const& kernel_name)
{
  cl_int status = CL_SUCCESS;
  // get a kernel object handle for a kernel with the given name
  kernel_.create_kernel(program_,kernel_name.c_str(),status);
  return status;
}


int bocl_global_memory_bandwidth_manager::release_kernel()
{
  cl_int status;
  kernel_.release_kernel(status);
  return status;
}

