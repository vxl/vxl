#include "bocl_kernel.h"
//:
// \file
#include "bocl_device_info.h"
#include "bocl_mem.h"
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
      vcl_cerr<<"bocl_kernel::couldn't append source from "<<src_paths[i]<<'\n';
      return false;
    }
  }

  bocl_device_info info(device);
  if (info.is_nvidia_device_) {
    vcl_cout<<"Compiling kernel ["<<id_<<"] on an NVIDIA device"<<vcl_endl;
    options += " -cl-nv-verbose";
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
  //else
  vcl_cout<<this->build_log()<<vcl_endl;
  return true;
}

bool bocl_kernel::create_kernel(const cl_context& context,
                                cl_device_id* device,
                                vcl_string const& src,
                                vcl_string const& kernel_name,
                                vcl_string options,
                                vcl_string id )
{
  id_ = id;
  device_ = device;

  //dev info
  bocl_device_info info(device);
  if (info.is_nvidia_device_) {
    vcl_cout<<"Compiling kernel ["<<id_<<"] on an NVIDIA device"<<vcl_endl;
    options += " -cl-nv-verbose";
  }

  //build program from string source
  int status;
  if (program_) {
    status = clReleaseProgram(program_);
    program_ = 0;
    if (!check_val(status, CL_SUCCESS, "clReleaseProgram failed."))
      return false;
  }

  //build program with source
  const char* source = src.c_str();
  vcl_size_t size = src.size();
  program_ = clCreateProgramWithSource(context, 1, &source, &size, &status);
  if (!check_val(status,CL_SUCCESS,"clCreateProgramWithSource failed.")) {
    vcl_cout<<"clCreate Program With Source Failed..."<<id<<vcl_endl;
    return SDK_FAILURE;
  }

  // create a cl program executable for all the devices specified
  status = clBuildProgram(program_, 1, device, options.c_str(), NULL, NULL);
  if (!check_val(status, CL_SUCCESS, error_to_string(status))){
    vcl_cout<<"BUILD ERROR: "<<this->build_log()<<vcl_endl;
    return false;
  }

  //create cl_kernel object
  kernel_ = clCreateKernel(program_, kernel_name.c_str(), &status);
  if ( !check_val(status,CL_SUCCESS,error_to_string(status)) ) {
    vcl_cerr<<"bocl_kernel:: couldn't build program "<<id_<<'\n';
    return false;
  }
  //else
  vcl_cout<<this->build_log()<<vcl_endl;
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
    vcl_cout<<" release failed in bocl_kernel destructor"<<vcl_endl;

  if (program_) {
    status = clReleaseProgram(program_);
  }
  program_ = 0;
  if ( !check_val(status,CL_SUCCESS,"clReleaseProgram failed: " + this->id_) )
    vcl_cout<<" release failed in bocl_kernel destructor"<<vcl_endl;
}

bool bocl_kernel::execute(const cl_command_queue& cmd_queue,
                          cl_uint dim,
                          vcl_size_t* local_threads,
                          vcl_size_t* global_threads,
                          vcl_size_t* global_offsets)
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
    cl_int status = (cl_int)clSetKernelArg(kernel_,args_.size() + i, local_args_[i], 0);
    if ( !check_val(status,CL_SUCCESS,"clSetLocal Arg Failed") ) {
      vcl_cout<<"Local argument "<<i<<" failed"<<vcl_endl;
      return false;
    }
  }

  //enqueue the kernel on the command queue
  ceEvent_ = 0;

  status = clEnqueueNDRangeKernel(cmd_queue, kernel_, dim, global_offsets, global_threads, local_threads, 0, NULL, &ceEvent_);
  if ( !check_val(status,CL_SUCCESS,"clEnqueueNDRangeKernel failed (" + id_ + ") " +error_to_string(status)) )
    return false;
  else
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
  cl_ulong tend=0, tstart=0;
  int status = clGetEventProfilingInfo(ceEvent_,CL_PROFILING_COMMAND_END,sizeof(cl_ulong),&tend,0);
  status = clGetEventProfilingInfo(ceEvent_,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),&tstart,0);
  if ( !check_val(status,CL_SUCCESS,"clFinish/ProfilingInfo failed (" + id_ + ") " +error_to_string(status)) )
    return false;

  //store execution time
  unsigned long diff = (unsigned long)(tend - tstart);
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
  int status = clGetKernelWorkGroupInfo(kernel_, (*device_), CL_KERNEL_LOCAL_MEM_SIZE, sizeof(size), (void*) &size, NULL);
  if ( !check_val(status,CL_SUCCESS,"bocl_kernel::local_mem_size() failed (" + id_ + ") " +error_to_string(status)) )
    return 0;
  return size;
}

vcl_size_t bocl_kernel::workgroup_size()
{
  vcl_size_t size;
  int status = clGetKernelWorkGroupInfo(kernel_, (*device_), CL_KERNEL_WORK_GROUP_SIZE, sizeof(size), (void*) &size, NULL);
  if ( !check_val(status,CL_SUCCESS,"bocl_kernel::private_mem_size()  failed (" + id_ + ") " +error_to_string(status)) )
    return 0;
  else
    return size;
}

vcl_string bocl_kernel::build_log()
{
  char log[40*1024];
  int status = clGetProgramBuildInfo( program_, (*device_), CL_PROGRAM_BUILD_LOG, sizeof(log), (void*) log, NULL);
  if ( !check_val(status,CL_SUCCESS,"bocl_kernel::build_log()  failed (" + id_ + ") " +error_to_string(status)) )
    return "";
  else
    return vcl_string(log);
}

vcl_string bocl_kernel::program_binaries()
{
  //get num devices
  cl_uint numDevices;
  cl_int status = clGetProgramInfo( program_, CL_PROGRAM_NUM_DEVICES, sizeof(numDevices), (void*) &numDevices, NULL);
  if ( !check_val(status,CL_SUCCESS,"bocl_kernel::program_binaries() numDevices failed (" + id_ + ") " +error_to_string(status)) )
    return "";

  //get binary sizes for each device
  vcl_size_t* sizes = new vcl_size_t[numDevices];
  status = clGetProgramInfo( program_, CL_PROGRAM_BINARY_SIZES, numDevices*sizeof(vcl_size_t), (void*) sizes, NULL);
  if ( !check_val(status,CL_SUCCESS,"bocl_kernel::program_binaries() binary_sizes failed (" + id_ + ") " +error_to_string(status)) )
    return "";

  //get binary for each device
  vcl_size_t numBytes = 0;
  unsigned char** binaries = new unsigned char*[numDevices];
  for (unsigned int i=0; i<numDevices; ++i) {
    numBytes += sizes[i];
    binaries[i] = new unsigned char[sizes[i]];
  }
  status = clGetProgramInfo( program_, CL_PROGRAM_BINARIES, numBytes, (void*) binaries, NULL);
  if ( !check_val(status,CL_SUCCESS,"bocl_kernel::program_binaries()  failed (" + id_ + ") " +error_to_string(status)) )
    return "";

  //Stitch together binary string
  vcl_string toReturn = "";
  for (unsigned int i=0; i<numDevices; ++i)
    toReturn += vcl_string((char*)binaries[i]);

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
    vcl_cout<<"BUILD ERROR: "<<this->build_log()<<vcl_endl;
    return false;
  }
  else
    return true;
}
