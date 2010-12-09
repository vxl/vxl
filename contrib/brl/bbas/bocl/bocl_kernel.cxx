#include "bocl_kernel.h"
#include "bocl_buffer_mgr.h"
#include <vcl_iostream.h>


bool bocl_kernel::create_kernel(cl_program const& program, vcl_string const& kernel_name, cl_int& status)
{
  status = CL_SUCCESS;
  // get a kernel object handle for a kernel with the given name
  kernel_ = clCreateKernel(program,kernel_name.c_str(),&status);

  if (!check_val(status,CL_SUCCESS,error_to_string(status)))
    return false;
  return  true;
}

bocl_kernel::~bocl_kernel()
{
  cl_int status;
  release_kernel(status);
}

void bocl_kernel::release_kernel(cl_int& status)
{
  status = true;
  if (kernel_)  {
    status = clReleaseKernel(kernel_);
  }
  kernel_ = NULL;
  if (!check_val(status,CL_SUCCESS,"clReleaseKernel failed."))
    status =  false;
  else
    status = true;
}

bool bocl_kernel::set_arg(int arg_id, vcl_string name)
{
  bocl_buffer* buffer = BOCL_BUFFER_MGR->get_buffer(name);
  if (buffer) {
    cl_int status = clSetKernelArg(kernel_,arg_id,sizeof(cl_mem),(void *)&buffer->mem());
    if (!check_val(status,false,"clSetKernelArg failed for " + name + ": " + error_to_string(status)))
      return false;
    return true;
  }
  return false;
}

bool bocl_kernel::set_local_arg(int arg_id, vcl_size_t arg_size)
{
  //  set local variable

  cl_int status = clSetKernelArg(kernel_,arg_id,arg_size,0);
  vcl_string str_status=error_to_string(status);
  //vcl_cout << "Setting local arg " << arg_id << "-->" << error_to_string(status) << vcl_endl;
  if (str_status.compare("CL_SUCCESS") != 0){
    vcl_cout << "Set local kernel arg failed\n";
    return false;
  }
  else
    return true;
}
