// This is brl/bbas/bocl/bocl_manager.txx
#ifndef bocl_manager_txx_
#define bocl_manager_txx_

#include "bocl_manager.h"
//:
// \file

#include <vcl_utility.h>
#include <vcl_fstream.h>
#include <vcl_sstream.h>
#include <bocl/bocl_utils.h>
#include <vcl_cstdio.h>
#if !defined (_WIN32) && !defined(__APPLE__)
#include <malloc.h> // for memalign()
#endif

//: Ensure only one instance is created
template <class T>
T* bocl_manager<T>::instance()
{
  if (!instance_) {
    instance_ = new T();
    instance_->initialize_cl();
  }
  return bocl_manager::instance_;
}

//: clears all opencl created objects
template <class T>
void bocl_manager<T>::clear_cl()
{
}

//: Destructor
template <class T>
bocl_manager<T>::~bocl_manager()
{
  this->clear_cl();
}

template <class T>
bool bocl_manager<T>::initialize_cl()
{
  cl_int status = CL_SUCCESS;

  //////////////////////////////////////////////////////////////////////////////
  // Check the number of available platforms
  //////////////////////////////////////////////////////////////////////////////
  cl_uint num_platforms = 0;
  status = clGetPlatformIDs(0, NULL, &num_platforms);
  if (status != CL_SUCCESS) {
    vcl_cerr << "bocl_manager: clGetPlatformIDs (call 1) returned " << status << '\n';
    return false;
  }
  if (num_platforms == 0) {
    vcl_cerr << "bocl_manager: 0 OpenCL platforms found!\n";
    return false;
  }
  if (num_platforms > 1) {
    vcl_cerr << "bocl_manager: warning: found " << num_platforms << " OpenCL platforms. Using the first\n";
  }
  // Get the first platform ID
  cl_platform_id* platform_id = new cl_platform_id[num_platforms];
  status = clGetPlatformIDs (num_platforms, platform_id, NULL);
  if (status != CL_SUCCESS) {
    vcl_cerr << "bocl_manager: clGetPlatformIDs (call 2) returned " << status << '\n';
    delete [] platform_id;
    return false;
  }

  //////////////////////////////////////////////////////////////////////////////
  // Get devices from platforms
  //////////////////////////////////////////////////////////////////////////////
  const vcl_size_t MAX_GPUS = 16;
  const vcl_size_t MAX_CPUS = 16;
  char platform_name[256];
  vcl_size_t ret_size;

  // First checking for GPU
  bool gpu_found=false;
  for (unsigned i=0;i<num_platforms;i++)
  {
    vcl_cout<<"PLatform number; "<<i<<vcl_endl;

    //grab device id's for type GPU
    cl_device_id gpu_ids[MAX_GPUS];
    cl_uint numGpus;
    if ( clGetDeviceIDs(platform_id[i], CL_DEVICE_TYPE_GPU, MAX_GPUS, gpu_ids, &numGpus)== CL_SUCCESS)
    {
      clGetPlatformInfo(platform_id[i],CL_PLATFORM_NAME,sizeof(platform_name),platform_name,&ret_size);
      gpu_found=true;
      vcl_cout<<"Found "<<numGpus<<" GPUs"<<vcl_endl;

      //create device objects, push them onto gpu list
      for (unsigned int i=0; i<numGpus; ++i) {
        bocl_device* gpu = new bocl_device(gpu_ids[i]);
        gpus_.push_back(gpu);
      }

      break;
    }
  }
  // now check for CPUs
  bool cpu_found=false;
  for (unsigned i=0;i<num_platforms;i++)
  {
    vcl_cout<<"PLatform number; "<<i<<vcl_endl;
    cl_device_id cpu_ids[MAX_CPUS];
    cl_uint numCpus;
    if ( clGetDeviceIDs(platform_id[i], CL_DEVICE_TYPE_CPU, MAX_CPUS, cpu_ids, &numCpus)== CL_SUCCESS)
    {
      vcl_cout<<"FOUND "<<numCpus<<" CPUs"<<vcl_endl;
      cpu_found=true;

      //create device objects, push them onto gpu list
      for (unsigned int i=0; i<numCpus; ++i) {
        bocl_device* cpu = new bocl_device(cpu_ids[i]);
        cpus_.push_back(cpu);
      }
      break;
    }
  }
  if (!gpu_found && !cpu_found) {
    vcl_cout<<"bocl_manager:: No devices (GPU or CPU) found, manager is invalid"<<vcl_endl;
    delete [] platform_id;
    return false;
  }

  //////////////////////////////////////////////////////////////////////////////
  //store current_device_/context for older functions that use bocl_manager
  if (gpu_found)
    curr_device_ = gpus_[0];
  else if (cpu_found)
    curr_device_ = cpus_[0];
  else
    return false;
  context_ = curr_device_->context();
#ifdef VERBOSE
  vcl_cout<<"Default device: "<<*curr_device_<<vcl_endl;
#endif
  //////////////////////////////////////////////////////////////////////////////
  delete [] platform_id;
  return true;
}


////////////////////////////////////////////////////////////////////////////////
// Old kernel/program/alloc methods that have been
// replaced by bocl_mem, bocl_kernel
////////////////////////////////////////////////////////////////////////////////
template <class T>
cl_context bocl_manager<T>::create_context(cl_device_id* device, int num_devices)
{
  //create device info for this device
  bocl_device_info info(device);
  vcl_cout<<"creating context on device: "<<info<<vcl_endl;

  //Create a context from the device ID
  int status = 1;
  cl_context context = clCreateContext(0, num_devices, device, NULL, NULL, &status);
  if (!check_val(status,CL_SUCCESS,"clCreateContextFromType failed: " + error_to_string(status))) {
    return 0;
  }

#if 0 //below is the old method of storing devices - seemed roundabout
  vcl_size_t device_list_size = 0;

  // First, get the size of device list data
  status = clGetContextInfo(context,
                            CL_CONTEXT_DEVICES,
                            0,
                            NULL,
                            &device_list_size);
  if (!check_val(status,CL_SUCCESS,"clGetContextInfo failed."))
    return false;
  number_devices_ = device_list_size/sizeof(cl_device_id);

  // Now allocate memory for device list based on the size we got earlier
  devices_ = (cl_device_id *)malloc(device_list_size);
  if (devices_==NULL) {
    vcl_cout << "Failed to allocate memory (devices).\n";
    return false;
  }
  // Now, get the device list data
  status = clGetContextInfo(context_,
                            CL_CONTEXT_DEVICES,
                            device_list_size,
                            devices_,
                            NULL);
  if (!check_val(status, CL_SUCCESS, "clGetGetContextInfo failed."))
    return false;
#endif

  return context;
}

template<class T>
bool bocl_manager<T>::load_kernel_source(vcl_string const& path)
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

template<class T>
bool bocl_manager<T>::append_process_kernels(vcl_string const& path)
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

template<class T>
bool bocl_manager<T>::write_program(vcl_string const& path)
{
  vcl_ofstream os(path.c_str());
  if (!os.is_open())
    return false;
  os << prog_;
  return true;
}

template <class T>
void* bocl_manager<T>::allocate_host_mem(vcl_size_t size)
{
#if defined (_WIN32)
  return _aligned_malloc(size, 16);
#elif defined(__APPLE__)
  return malloc(size);
#else
  return memalign(16, size);
#endif
}

template<class T>
int bocl_manager<T>::build_kernel_program(cl_program & program, vcl_string options)
{
  cl_int status = CL_SUCCESS;
  vcl_size_t sourceSize[] = { this->prog_.size() };
  if (!sourceSize[0]) return SDK_FAILURE;
  if (program) {
    status = clReleaseProgram(program);
    program = 0;
    if (!check_val(status, CL_SUCCESS, "clReleaseProgram failed."))
      return SDK_FAILURE;
  }
  const char * source = this->prog_.c_str();

  program = clCreateProgramWithSource(this->context(),
                                      1,
                                      &source,
                                      sourceSize,
                                      &status);
  if (!check_val(status,CL_SUCCESS,"clCreateProgramWithSource failed."))
    return SDK_FAILURE;

  // create a cl program executable for all the devices specified
  status = clBuildProgram(program,
                          1,
                          this->devices(),
                          options.c_str(),
                          NULL,
                          NULL);
  if (!check_val(status, CL_SUCCESS, error_to_string(status)))
  {
    vcl_size_t len;
    char buffer[2048];
    clGetProgramBuildInfo(program, this->devices()[0],
                          CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
    vcl_printf("%s\n", buffer);
    return SDK_FAILURE;
  }
  else
    return SDK_SUCCESS;
}

template<class T>
bool bocl_manager<T>::free_buffer(void* buffer)
{
  if (buffer) {
#ifdef _WIN32
    _aligned_free(buffer);
#elif defined(__APPLE__)
    free(buffer);
#else
    buffer = NULL;
#endif
    return true;
  }
  return false;
}

template<class T>
bool bocl_manager<T>::create_buffer(void** buffer,vcl_string type, int elm_size, int length)
{
  if (type.compare("cl_int") == 0) {
#if defined (_WIN32)
  *buffer=(cl_int*)_aligned_malloc( sizeof(cl_int)*elm_size, length);
#elif defined(__APPLE__)
  *buffer = (cl_int*)malloc(sizeof(cl_int)*elm_size);
#else
  *buffer = (cl_int*)memalign(length, sizeof(cl_int)*elm_size);
#endif
  }
  else if (type.compare("cl_uint") == 0) {
#if defined (_WIN32)
    *buffer = (cl_uint*)_aligned_malloc(elm_size*sizeof(cl_uint),length);
#elif defined(__APPLE__)
    *buffer =(cl_uint*)malloc(sizeof(cl_uint)*elm_size);
#else
    *buffer =(cl_uint*)memalign(length,sizeof(cl_uint)*elm_size);
#endif
  }
  else if (type.compare("cl_float") == 0) {
#if defined (_WIN32)
    *buffer = (cl_float*)_aligned_malloc(elm_size*sizeof(cl_float),length);
#elif defined(__APPLE__)
    *buffer =(cl_float*)malloc(sizeof(cl_float)*elm_size);
#else
    *buffer =(cl_float*)memalign(length,sizeof(cl_float)*elm_size);
#endif
  }
  else if (type.compare("cl_float4") == 0) {
#if defined (_WIN32)
    *buffer = (cl_float*)_aligned_malloc(elm_size*sizeof(cl_float)*4,length);
#elif defined(__APPLE__)
    *buffer =(cl_float*)malloc(sizeof(cl_float)*elm_size*4);
#else
    *buffer =(cl_float*)memalign(length,sizeof(cl_float)*elm_size*4);
#endif
  }
  else {
    vcl_cout << "Buffer of type (" << type << "*) is not defined yet!" << vcl_endl;
    return false;
  }
  return true;
}

#undef BOCL_MANAGER_INSTANTIATE
#define BOCL_MANAGER_INSTANTIATE(T) \
template <class T > T* bocl_manager<T >::instance_ = 0; \
template class bocl_manager<T >


#endif // bocl_manager_txx_
