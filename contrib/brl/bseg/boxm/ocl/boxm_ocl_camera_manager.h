#ifndef boxm_ocl_camera_manager_h_
#define boxm_ocl_camera_manager_h_
//:
// \file
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <bocl/bocl_cl.h>
#include <vnl/vnl_vector_fixed.h>
#include <vbl/vbl_array_2d.h>
#include <bocl/bocl_manager.h>
#include <bocl/bocl_utils.h>
#include <boxm/sample/boxm_sample.h>
#include <boxm/boxm_apm_traits.h>
#include "boxm_ocl_utils.h"
#include <vpgl/vpgl_perspective_camera.h>


class boxm_ocl_camera_manager : public bocl_manager<boxm_ocl_camera_manager>
{
 public:
  boxm_ocl_camera_manager(): program_(0),
                             cam_(0),
                             cam_inv_(0),
                             point_2d_(0),
                             point_3d_(0)
  {}

  ~boxm_ocl_camera_manager(){if (program_)clReleaseProgram(program_);}

  bool set_input_cam(vpgl_perspective_camera<double> * cam);
  bool clean_cam();
  bool setup_point_data();
  bool clean_point_data();
  bool set_project_args();
  bool set_backproject_args();

  bool run();
  void set_point2d(float u, float v);
  void set_point3d(float x, float y,float z);
  cl_float* point2d(){return point_2d_;}
  cl_float* point3d(){return point_3d_;}

  cl_program program() {return program_;}

  int setup_cam_buffer();
  int release_cam_buffer();
  int setup_cam_inv_buffer();
  int release_cam_inv_buffer();

  int set_point_buffers();
  int release_point_buffers();

  int build_kernel_program();
  int create_kernel(vcl_string const& name);
  int release_kernel();

  cl_kernel kernel() {return kernel_;}
 protected:
  cl_program program_;
  cl_command_queue command_queue_;
  cl_kernel kernel_;
  cl_float* cam_;
  cl_float* cam_inv_;
  cl_float* point_2d_;
  cl_float* point_3d_;

  cl_mem   cam_buf_;
  cl_mem   cam_inv_buf_;
  cl_mem   point_2d_buf_;
  cl_mem   point_3d_buf_;
};

#endif // boxm_ocl_camera_manager_h_
