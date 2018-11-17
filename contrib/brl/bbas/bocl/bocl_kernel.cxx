#include "bocl_kernel.h"
#include <cstdio>
#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>
//:
// \file
#include "bocl_device_info.h"
#include "bocl_mem.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

bool bocl_kernel::create_kernel(cl_context* context,
                                cl_device_id* device,
                                std::vector<std::string> src_paths,
                                std::string const& kernel_name,
                                std::string options,
                                std::string id)
{
  context_  = context;
  device_   = device;
  id_       = std::move(id);

  //make sure paths exist
  if (src_paths.empty()) {
    std::cerr<<"bocl_kernel::create_kernel passed empty sources\n";
    return false;
  }

  //load source into single string
  if (!this->load_kernel_source(src_paths[0])) {
    std::cerr<<"bocl_kernel::couldn't load source from "<<src_paths[0]<<'\n';
    return false;
  }
  for (unsigned int i=1; i<src_paths.size(); ++i) {
    if (!this->append_process_kernels(src_paths[i])) {
      std::cerr<<"bocl_kernel::couldn't append source from "<<src_paths[i]<<'\n';
      return false;
    }
  }

  bocl_device_info info(device);
  if (info.is_nvidia_device_) {
    std::cout<<"Compiling kernel ["<<id_<<"] on an NVIDIA device"<<std::endl;
    options += " -cl-nv-verbose";
  }

  //build cl_program object
  if ( !this->build_kernel_program(program_, options) ) {
    std::cerr<<"bocl_kernel::couldn't build program "<<id_<<'\n';
    return false;
  }

  //create cl_kernel object
  cl_int status = SDK_FAILURE;
  kernel_ = clCreateKernel(program_, kernel_name.c_str(), &status);
  if ( !check_val(status,CL_SUCCESS,error_to_string(status)) ) {
    std::cerr<<"bocl_kernel:: couldn't build program "<<id_<<'\n';
    return false;
  }
  //else
  std::cout<<this->build_log()<<std::endl;
  return true;
}

bool bocl_kernel::create_kernel(const cl_context& context,
                                cl_device_id* device,
                                std::string const& src,
                                std::string const& kernel_name,
                                std::string options,
                                const std::string& id )
{
  id_ = id;
  device_ = device;

  //dev info
  bocl_device_info info(device);
  if (info.is_nvidia_device_) {
    std::cout<<"Compiling kernel ["<<id_<<"] on an NVIDIA device"<<std::endl;
    options += " -cl-nv-verbose";
  }

  //build program from string source
  int status;
  if (program_) {
    status = clReleaseProgram(program_);
    program_ = nullptr;
    if (!check_val(status, CL_SUCCESS, "clReleaseProgram failed."))
      return false;
  }

  //build program with source
  const char* source = src.c_str();
  std::size_t size = src.size();
  program_ = clCreateProgramWithSource(context, 1, &source, &size, &status);
  if (!check_val(status,CL_SUCCESS,"clCreateProgramWithSource failed.")) {
    std::cout<<"clCreate Program With Source Failed..."<<id<<std::endl;
    return SDK_FAILURE;
  }

  // create a cl program executable for all the devices specified
  status = clBuildProgram(program_, 1, device, options.c_str(), nullptr, nullptr);
  if (!check_val(status, CL_SUCCESS, error_to_string(status))){
    std::cout<<"BUILD ERROR: "<<this->build_log()<<std::endl;
    return false;
  }

  //create cl_kernel object
  kernel_ = clCreateKernel(program_, kernel_name.c_str(), &status);
  if ( !check_val(status,CL_SUCCESS,error_to_string(status)) ) {
    std::cerr<<"bocl_kernel:: couldn't build program "<<id_<<'\n';
    return false;
  }
  //else
  std::cout<<this->build_log()<<std::endl;
  return true;
}


bocl_kernel::~bocl_kernel()
{
  cl_int status = SDK_SUCCESS;
  if (kernel_)  {
    status = clReleaseKernel(kernel_);
  }
  kernel_ = nullptr;
  if ( !check_val(status,CL_SUCCESS,"clReleaseKernel failed: " + this->id_) )
    std::cout<<" release failed in bocl_kernel destructor"<<std::endl;

  if (program_) {
    status = clReleaseProgram(program_);
  }
  program_ = nullptr;
  if ( !check_val(status,CL_SUCCESS,"clReleaseProgram failed: " + this->id_) )
    std::cout<<" release failed in bocl_kernel destructor"<<std::endl;
}

bool bocl_kernel::execute(const cl_command_queue& cmd_queue,
                          cl_uint dim,
                          std::size_t* local_threads,
                          std::size_t* global_threads,
                          std::size_t* global_offsets)
{
  //set kernel args
  cl_int status = CL_SUCCESS;
  for (unsigned int i=0; i<args_.size(); ++i) {
    cl_mem& buff = args_[i]->buffer();
    status = clSetKernelArg(kernel_, i, sizeof(cl_mem), (void *)&buff);
    if ( !check_val(status,CL_SUCCESS,error_to_string(status) + "::clSetKernelArg failed: " + args_[i]->id())) {
      std::cout<<"ARG number "<<i<<std::endl;
      return false;
    }
  }

  //set local args
  for (unsigned int i=0; i<local_args_.size(); ++i) {
    auto status = (cl_int)clSetKernelArg(kernel_,args_.size() + i, local_args_[i], nullptr);
    if ( !check_val(status,CL_SUCCESS,"clSetLocal Arg Failed") ) {
      std::cout<<"Local argument "<<i<<" failed"<<std::endl;
      return false;
    }
  }

  //enqueue the kernel on the command queue
  ceEvent_ = nullptr;

  status = clEnqueueNDRangeKernel(cmd_queue, kernel_, dim, global_offsets, global_threads, local_threads, 0, nullptr, &ceEvent_);
  if ( !check_val(status,CL_SUCCESS,"clEnqueueNDRangeKernel failed (" + id_ + ") " +error_to_string(status)) )
    return false;
  else
    return true;


}

bool bocl_kernel::set_local_arg(std::size_t size)
{
  local_args_.push_back(size);
  return true;
}

bool bocl_kernel::set_arg(bocl_mem* buffer)
{
  //push arg to the back
  args_.push_back(buffer);
  return true;
}


//: THIS REQUIRES the queue to be finished
float bocl_kernel::exec_time()
{
  cl_ulong tend=0, tstart=0;
  int status = clGetEventProfilingInfo(ceEvent_,CL_PROFILING_COMMAND_END,sizeof(cl_ulong),&tend,nullptr);
  status &= clGetEventProfilingInfo(ceEvent_,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),&tstart,nullptr);
  if ( !check_val(status,CL_SUCCESS,"clFinish/ProfilingInfo failed (" + id_ + ") " +error_to_string(status)) )
    return false;

  //store execution time
  auto diff = (unsigned long)(tend - tstart);
  return 1.0e-6f*float(diff);
}

bool bocl_kernel::release_current_event()
{
  if(ceEvent_)
  {
    int status = clReleaseEvent(ceEvent_);
    if ( !check_val(status,CL_SUCCESS,"clReleaseEvent failed (" + id_ + ") " +error_to_string(status)) )
      return false;
    return true;
  }
  else
    return true;
}

unsigned long bocl_kernel::local_mem_size()
{
  unsigned long size;
  int status = clGetKernelWorkGroupInfo(kernel_, (*device_), CL_KERNEL_LOCAL_MEM_SIZE, sizeof(size), (void*) &size, nullptr);
  if ( !check_val(status,CL_SUCCESS,"bocl_kernel::local_mem_size() failed (" + id_ + ") " +error_to_string(status)) )
    return 0;
  return size;
}

std::size_t bocl_kernel::workgroup_size()
{
  std::size_t size;
  int status = clGetKernelWorkGroupInfo(kernel_, (*device_), CL_KERNEL_WORK_GROUP_SIZE, sizeof(size), (void*) &size, nullptr);
  if ( !check_val(status,CL_SUCCESS,"bocl_kernel::private_mem_size()  failed (" + id_ + ") " +error_to_string(status)) )
    return 0;
  else
    return size;
}

std::string bocl_kernel::build_log()
{
  char log[40*1024];
  int status = clGetProgramBuildInfo( program_, (*device_), CL_PROGRAM_BUILD_LOG, sizeof(log), (void*) log, nullptr);
  if ( !check_val(status,CL_SUCCESS,"bocl_kernel::build_log()  failed (" + id_ + ") " +error_to_string(status)) )
    return "";
  else
    return std::string(log);
}

std::string bocl_kernel::program_binaries()
{
  //get num devices
  cl_uint numDevices;
  cl_int status = clGetProgramInfo( program_, CL_PROGRAM_NUM_DEVICES, sizeof(numDevices), (void*) &numDevices, nullptr);
  if ( !check_val(status,CL_SUCCESS,"bocl_kernel::program_binaries() numDevices failed (" + id_ + ") " +error_to_string(status)) )
    return "";

  //get binary sizes for each device
  auto* sizes = new std::size_t[numDevices];
  status = clGetProgramInfo( program_, CL_PROGRAM_BINARY_SIZES, numDevices*sizeof(std::size_t), (void*) sizes, nullptr);
  if ( !check_val(status,CL_SUCCESS,"bocl_kernel::program_binaries() binary_sizes failed (" + id_ + ") " +error_to_string(status)) )
    return "";

  //get binary for each device
  std::size_t numBytes = 0;
  auto** binaries = new unsigned char*[numDevices];
  for (unsigned int i=0; i<numDevices; ++i) {
    numBytes += sizes[i];
    binaries[i] = new unsigned char[sizes[i]];
  }
  status = clGetProgramInfo( program_, CL_PROGRAM_BINARIES, numBytes, (void*) binaries, nullptr);
  if ( !check_val(status,CL_SUCCESS,"bocl_kernel::program_binaries()  failed (" + id_ + ") " +error_to_string(status)) )
    return "";

  //Stitch together binary string
  std::string toReturn = "";
  for (unsigned int i=0; i<numDevices; ++i)
    toReturn += std::string((char*)binaries[i]);

  //clean up aux data
  delete[] sizes;
  for (unsigned int i=0; i<numDevices; ++i)
    delete[] binaries[i];
  delete[] binaries;

  //return stitched string
  return toReturn;
}

//------------------------------------------------------------------------------
// Private helper functions
//------------------------------------------------------------------------------

//: load kernel into string
bool bocl_kernel::load_kernel_source(std::string const& path)
{
  prog_ = "";
  std::ifstream is(path.c_str());
  if (!is.is_open())
    return false;
  char temp[256];
  std::ostringstream ostr;
  while (!is.eof()) {
    is.getline(temp, 256);
    std::string s(temp);
    ostr << s << '\n';
  }
  prog_ =  ostr.str();
  return prog_.size() > 0;
}

bool bocl_kernel::append_process_kernels(std::string const& path)
{
  std::ifstream is(path.c_str());
  if (!is.is_open())
    return false;
  char temp[256];
  std::ostringstream ostr;
  while (!is.eof()) {
    is.getline(temp, 256);
    std::string s(temp);
    ostr << s << '\n';
  }
  prog_ += ostr.str();
  return true;
}

//: build opencl program
bool bocl_kernel::build_kernel_program(cl_program &program, const std::string& options)
{
  cl_int status = CL_SUCCESS;
  std::size_t sourceSize[] = { this->prog_.size() };
  if (!sourceSize[0])
    return false;
  if (program) {
    status = clReleaseProgram(program);
    program = nullptr;
    if (!check_val(status, CL_SUCCESS, "clReleaseProgram failed."))
      return false;
  }
  const char * source = this->prog_.c_str();

  program = clCreateProgramWithSource((*context_),
                                      1,
                                      &source,
                                      sourceSize,
                                      &status);
  if (!check_val(status,CL_SUCCESS,"clCreateProgramWithSource failed."))
    return SDK_FAILURE;

  // create a cl program executable for all the devices specified
  status = clBuildProgram(program,
                          1,
                          this->device_,
                          options.c_str(),
                          nullptr,
                          nullptr);
  if (!check_val(status, CL_SUCCESS, error_to_string(status)))
  {
    std::cout<<"BUILD ERROR: "<<this->build_log()<<std::endl;
    return false;
  }
  else
    return true;
}
