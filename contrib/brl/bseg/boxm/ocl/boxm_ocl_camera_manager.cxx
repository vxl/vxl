// This is brl/bseg/boxm/ocl/boxm_ocl_camera_manager.cxx
#include "boxm_ocl_camera_manager.h"

#include <vcl_fstream.h>
#include <vcl_iomanip.h>
#include <vcl_sstream.h>
#include <vcl_ctime.h>
#include <vcl_cmath.h>
#include <vcl_cstdlib.h>
#include <vcl_cstdio.h>
#include <vcl_cassert.h>
#include <vbl/io/vbl_io_array_2d.h>
#include <boxm/util/boxm_utils.h>
#include <vul/vul_timer.h>
#include "boxm_ocl_utils.h"

bool boxm_ocl_camera_manager::run()
{
    cl_int status = CL_SUCCESS;
    command_queue_ = clCreateCommandQueue(this->context(),this->devices()[0],CL_QUEUE_PROFILING_ENABLE,&status);
    if (!this->check_val(status,CL_SUCCESS,"Falied in command queue creation" + error_to_string(status)))
        return false;
    vcl_size_t globalThreads[1];vcl_size_t localThreads[1];

    globalThreads[0]=1;
    localThreads[0]=1;
    status = clEnqueueNDRangeKernel(command_queue_,this->kernel_, 1,NULL,globalThreads,localThreads,0,NULL,NULL);

    if (!this->check_val(status,CL_SUCCESS,"clEnqueueNDRangeKernel failed. "+error_to_string(status)))
        return SDK_FAILURE;

    status = clFinish(command_queue_);
    if (!this->check_val(status,CL_SUCCESS,"clFinish failed."+error_to_string(status)))
        return SDK_FAILURE;
  // Enqueue readBuffers
    status = clEnqueueReadBuffer(command_queue_,point_2d_buf_,CL_TRUE,
                                 0,sizeof(cl_float2),point_2d_,0,NULL,NULL);
    if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (image_)failed."))
        return false;
    status = clEnqueueReadBuffer(command_queue_,point_3d_buf_,CL_TRUE,
                                 0,sizeof(cl_float4),point_3d_,0,NULL,NULL);
    if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (image_)failed."))
        return false;

    status = clReleaseCommandQueue(command_queue_);
    if (!this->check_val(status,CL_SUCCESS,"clReleaseCommandQueue failed."))
        return false;
    return true;
}

bool boxm_ocl_camera_manager::setup_point_data()
{
    point_2d_ =(cl_float*) boxm_ocl_utils::alloc_aligned(1,sizeof(cl_float2), 16);
    point_3d_ =(cl_float*) boxm_ocl_utils::alloc_aligned(1,sizeof(cl_float4), 16);

    return true;
}

void boxm_ocl_camera_manager::set_point2d(float u, float v)
{
    point_2d_[0]=u;point_2d_[1]=v;
}

void boxm_ocl_camera_manager::set_point3d(float x, float y,float z)
{
    point_3d_[0]=x;point_3d_[1]=y;point_3d_[2]=z;
}

bool boxm_ocl_camera_manager::clean_point_data()
{
    if (point_2d_)
        boxm_ocl_utils::free_aligned(point_2d_);
    if (point_3d_)
        boxm_ocl_utils::free_aligned(point_3d_);
    return true;
}

int boxm_ocl_camera_manager::set_point_buffers()
{
  cl_int status = CL_SUCCESS;
  point_2d_buf_ = clCreateBuffer(this->context_,CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(cl_mem),point_2d_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (stat input) failed."))
    return SDK_FAILURE;
  point_3d_buf_ = clCreateBuffer(this->context_,CL_MEM_READ_WRITE| CL_MEM_COPY_HOST_PTR, sizeof(cl_mem),point_3d_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (stat input) failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

int boxm_ocl_camera_manager::release_point_buffers()
{
  cl_int status = clReleaseMemObject(point_2d_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject (point_2d_buf_) failed."))
    return SDK_FAILURE;
  status = clReleaseMemObject(point_3d_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject (point_3d_buf_) failed."))
    return SDK_FAILURE;

  return SDK_SUCCESS;
}

int boxm_ocl_camera_manager::setup_cam_buffer()
{
  cl_int status = CL_SUCCESS;
  cam_buf_ = clCreateBuffer(this->context_,CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(cl_mem),cam_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (stat input) failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}


int boxm_ocl_camera_manager::release_cam_buffer()
{
  cl_int status = clReleaseMemObject(cam_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject (input_cam_buf_) failed."))
    return SDK_FAILURE;
  else {
    cam_buf_=0;
    return SDK_SUCCESS;
  }
}

int boxm_ocl_camera_manager::setup_cam_inv_buffer()
{
  cl_int status = CL_SUCCESS;
  cam_inv_buf_ = clCreateBuffer(this->context_,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(cl_mem),cam_inv_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (cam inverse) failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

int boxm_ocl_camera_manager::release_cam_inv_buffer()
{
  cl_int status = clReleaseMemObject(cam_inv_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject (cam_inv_buf_) failed."))
    return SDK_FAILURE;
  else {
    cam_inv_buf_=0;
    return SDK_SUCCESS;
  }
}

bool boxm_ocl_camera_manager::set_input_cam(vpgl_perspective_camera<double> * pcam)
{
  if (pcam)
  {
    vnl_svd<double>* svd=pcam->svd();
    vnl_matrix<double> Ut=svd->U().conjugate_transpose();
    vnl_matrix<double> V=svd->V();
    vnl_vector<double> Winv=svd->Winverse().diagonal();
    cam_inv_=(cl_float *)boxm_ocl_utils::alloc_aligned(3,sizeof(cl_float16),16);

    int cnt=0;
    for (unsigned i=0;i<Ut.rows();i++)
    {
      for (unsigned j=0;j<Ut.cols();j++)
        cam_inv_[cnt++]=(cl_float)Ut(i,j);

      cam_inv_[cnt++]=0;
    }

    for (unsigned i=0;i<V.rows();i++)
      for (unsigned j=0;j<V.cols();j++)
        cam_inv_[cnt++]=(cl_float)V(i,j);

    for (unsigned i=0;i<Winv.size();i++)
      cam_inv_[cnt++]=(cl_float)Winv(i);

    vgl_point_3d<double> cam_center=pcam->get_camera_center();
    cam_inv_[cnt++]=(cl_float)cam_center.x();
    cam_inv_[cnt++]=(cl_float)cam_center.y();
    cam_inv_[cnt++]=(cl_float)cam_center.z();

    cam_=(cl_float *)boxm_ocl_utils::alloc_aligned(1,sizeof(cl_float16),16);
    vnl_matrix<double> projection_matrix=pcam->get_matrix();
    cnt=0;
    for (unsigned i=0;i<projection_matrix.rows();i++)
      for (unsigned j=0;j<projection_matrix.cols();j++)
        cam_[cnt++]=(cl_float)projection_matrix(i,j);
    return true;
  }
  else {
    vcl_cerr << "Error set_persp_camera() : Missing camera\n";
    return false;
  }
}

bool boxm_ocl_camera_manager::set_project_args()
{
  if (!kernel_)
    return false;
  cl_int status = CL_SUCCESS;
  int i=0;
  status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&cam_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cam_buf_)"))
    return SDK_FAILURE;
  status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&point_3d_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (point_3d_buf_)"))
      return SDK_FAILURE;
  status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&point_2d_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (point_2d_buf_)"))
    return SDK_FAILURE;
  return SDK_SUCCESS;
}

bool boxm_ocl_camera_manager::set_backproject_args()
{
  if (!kernel_)
    return false;
  cl_int status = CL_SUCCESS;
  int i=0;
  status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&cam_inv_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cam_inv_buf_)"))
    return SDK_FAILURE;
  status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&point_3d_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (point_3d_buf_)"))
      return SDK_FAILURE;
  status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&point_2d_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (point_2d_buf_)"))
    return SDK_FAILURE;
  return SDK_SUCCESS;
}

bool boxm_ocl_camera_manager::clean_cam()
{
  if (cam_inv_)
    boxm_ocl_utils::free_aligned(cam_inv_);
  if (cam_)
    boxm_ocl_utils::free_aligned(cam_);

  return true;
}


int boxm_ocl_camera_manager::build_kernel_program()
{
  cl_int status = CL_SUCCESS;
  vcl_size_t sourceSize[] = { this->prog_.size() };
  if (!sourceSize[0]) return SDK_FAILURE;
  if (program_) {
    status = clReleaseProgram(program_);
    program_ = 0;
    if (!this->check_val(status,
                         CL_SUCCESS,
                         "clReleaseProgram failed."))
      return SDK_FAILURE;
  }
  const char * source = this->prog_.c_str();

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


int boxm_ocl_camera_manager::create_kernel(vcl_string const& kernel_name)
{
  cl_int status = CL_SUCCESS;
  // get a kernel object handle for a kernel with the given name
  kernel_ = clCreateKernel(program_,kernel_name.c_str(),&status);
  if (!this->check_val(status,CL_SUCCESS,error_to_string(status)))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}


int boxm_ocl_camera_manager::release_kernel()
{
  cl_int status = SDK_SUCCESS;
  if (kernel_)  {
    status = clReleaseKernel(kernel_);
  }
  kernel_ = NULL;
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clReleaseKernel failed."))
    return SDK_FAILURE;
  return SDK_SUCCESS;
}

