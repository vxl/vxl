#include "bocl_kernel.h"
//:
// \file
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_sstream.h>
#include <vcl_cstdio.h>

bool bocl_kernel::create_kernel(cl_context* context,
                                cl_device_id* device,
                                vcl_vector<vcl_string> src_paths,
                                vcl_string const& kernel_name,
                                vcl_string options,
                                vcl_string id)
{
  context_  = context;
  device_   = device;
  id_       = id;

  //make sure paths exist
  if (src_paths.empty()) {
    vcl_cerr<<"bocl_kernel::create_kernel passed empty sources\n";
    return false;
  }

  //load source into single string
  if (!this->load_kernel_source(src_paths[0])) {
    vcl_cerr<<"bocl_kernel::couldn't load source from "<<src_paths[0]<<'\n';
    return false;
  }
  for (unsigned int i=1; i<src_paths.size(); ++i) {
    if (!this->append_process_kernels(src_paths[i])) {
      vcl_cerr<<"bocl_kernel::couldn't load source from "<<src_paths[i]<<'\n';
      return false;
    }
  }

  //build cl_program object
  if ( !this->build_kernel_program(program_, options) ) {
    vcl_cerr<<"bocl_kernel::couldn't build program "<<id_<<'\n';
    return false;
  }

  //create cl_kernel object
  cl_int status = SDK_FAILURE;
  kernel_ = clCreateKernel(program_, kernel_name.c_str(), &status);
  if ( !check_val(status,CL_SUCCESS,error_to_string(status)) ) {
    vcl_cerr<<"bocl_kernel:: couldn't build program "<<id_<<'\n';
    return false;
  }

  return true;
}

bocl_kernel::~bocl_kernel()
{
  cl_int status = SDK_SUCCESS;
  if (kernel_)  {
    status = clReleaseKernel(kernel_);
  }
  kernel_ = 0;
  if ( !check_val(status,CL_SUCCESS,"clReleaseKernel failed: " + this->id_) )
    vcl_cout<<" release failed in bocl_kernel destructor "<<vcl_endl;
    
  if(program_) {
    status = clReleaseProgram(program_);
  }
  program_ = 0;
  if ( !check_val(status,CL_SUCCESS,"clReleaseProgram failed: " + this->id_) )
    vcl_cout<<" release failed in bocl_kernel destructor "<<vcl_endl;
}

bool bocl_kernel::execute(cl_command_queue& cmdQueue, vcl_size_t* localThreads, vcl_size_t* globalThreads)
{
  //set kernel args
  cl_int status = CL_SUCCESS;
  for (unsigned int i=0; i<args_.size(); ++i) {
    cl_mem& buff = args_[i]->buffer();
    status = clSetKernelArg(kernel_, i, sizeof(cl_mem), (void *)&buff);
    if ( !check_val(status,CL_SUCCESS,error_to_string(status) + "::clSetKernelArg failed: " + args_[i]->id())) {
      vcl_cout<<"ARG number "<<i<<vcl_endl;
      return false;
    }
  }

  //set local args
  for (unsigned int i=0; i<local_args_.size(); ++i) {
    cl_int status = clSetKernelArg(kernel_,args_.size() + i, local_args_[i], 0);
    if ( !check_val(status,CL_SUCCESS,"clSetLocal Arg Failed") ) {
      vcl_cout<<"Local argument "<<i<<" failed"<<vcl_endl;
      return false;
    }
  }

  //enqueue the kernel on the command queue
  ceEvent_ = 0;
  status = clEnqueueNDRangeKernel(cmdQueue, kernel_, 2, NULL, globalThreads, localThreads, 0, NULL, &ceEvent_);
  if ( !check_val(status,CL_SUCCESS,"clEnqueueNDRangeKernel failed (" + id_ + ") " +error_to_string(status)) )
    return false;


  return true;
}

bool bocl_kernel::set_local_arg(vcl_size_t size)
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
  long tend, tstart;
  int status = clGetEventProfilingInfo(ceEvent_,CL_PROFILING_COMMAND_END,sizeof(cl_ulong),&tend,0);
  status = clGetEventProfilingInfo(ceEvent_,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),&tstart,0);
  if ( !check_val(status,CL_SUCCESS,"clFinish/ProfilingInfo failed (" + id_ + ") " +error_to_string(status)) )
    return false;

  //store execution time
  return 1.0e-6f*float(tend - tstart);   
}


//------------------------------------------------------------------------------
// Private helper functions
//------------------------------------------------------------------------------
//: load kernel into string
bool bocl_kernel::load_kernel_source(vcl_string const& path)
{
  prog_ = "";
  vcl_ifstream is(path.c_str());
  if (!is.is_open())
    return false;
  char temp[256];
  vcl_ostringstream ostr;
  while (!is.eof()) {
    is.getline(temp, 256);
    vcl_string s(temp);
    ostr << s << '\n';
  }
  prog_ =  ostr.str();
  return prog_.size() > 0;
}

bool bocl_kernel::append_process_kernels(vcl_string const& path)
{
  vcl_ifstream is(path.c_str());
  if (!is.is_open())
    return false;
  char temp[256];
  vcl_ostringstream ostr;
  while (!is.eof()) {
    is.getline(temp, 256);
    vcl_string s(temp);
    ostr << s << '\n';
  }
  prog_ += ostr.str();
  return true;
}

//: build opencl program
bool bocl_kernel::build_kernel_program(cl_program &program, vcl_string options)
{
  cl_int status = CL_SUCCESS;
  vcl_size_t sourceSize[] = { this->prog_.size() };
  if (!sourceSize[0])
    return false;
  if (program) {
    status = clReleaseProgram(program);
    program = 0;
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
                          NULL,
                          NULL);
  if (!check_val(status, CL_SUCCESS, error_to_string(status)))
  {
    vcl_size_t len;
    char buffer[2048];
    clGetProgramBuildInfo(program, (*device_),
                          CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
    vcl_printf("%s\n", buffer);
    return false;
  }
  else
    return true;
}
