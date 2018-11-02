// This is brl/bpro/core/vpgl_pro/processes/vpgl_projective_to_rational_camera_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process to convert perspective camera to rational camera.

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_camera.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_rational_camera.h>

vpgl_rational_camera<double>
projective_to_rational(vpgl_proj_camera<double>& cam_pers);

//: Init function
bool vpgl_projective_to_rational_camera_process_cons(bprb_func_process& pro)
{
  //this process takes one input: the filename
  bool ok=false;
  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_camera_double_sptr");
  ok = pro.set_input_types(input_types);
  if (!ok) return ok;

  std::vector<std::string> output_types;
  output_types.emplace_back("vpgl_camera_double_sptr");  // label image
  ok = pro.set_output_types(output_types);
  if (!ok) return ok;

  return true;
}

//: Execute the process
bool vpgl_projective_to_rational_camera_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 1) {
    std::cout << "vpgl_projective_to_rational_camera_process: The input number should be 1" << std::endl;
    return false;
  }

  // get the inputs
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(0);
  if (auto* cam = dynamic_cast<vpgl_proj_camera<double>*>(camera.ptr()))
  {
    vpgl_rational_camera<double> rat_cam=projective_to_rational(*cam);

    vpgl_camera_double_sptr rat_cam_sptr=new vpgl_rational_camera<double>(rat_cam);
    pro.set_output_val<vpgl_camera_double_sptr>(0, rat_cam_sptr);
  }
  else {
    std::cout<<"Wrong type of camera input"<<std::endl;
    return false;
  }

  return true;
}

vpgl_rational_camera<double>
projective_to_rational(vpgl_proj_camera<double>& cam_proj)
{
  vnl_matrix_fixed<double,3,4> cam_pers_matrix = cam_proj.get_matrix();
  std::vector<double> neu_u,den_u,neu_v,den_v;
  double x_scale = 1.0,
         x_off = 0.0,
         y_scale = 1.0,
         y_off = 0.0,
         z_scale = 1.0,
         z_off = 0.0,
         u_scale = 1.0,
         u_off = 0.0,
         v_scale = 1.0,
         v_off = 0.0;

  for (int i=0; i<20; i++){
    neu_u.push_back(0.0);
    neu_v.push_back(0.0);
    den_u.push_back(0.0);
    den_v.push_back(0.0);
  }

  int vector_map[] = {9,15,18,19};

  for (int i=0; i<4; i++){
    neu_u[vector_map[i]] = cam_pers_matrix(0,i);
    neu_v[vector_map[i]] = cam_pers_matrix(1,i);
    den_u[vector_map[i]] = cam_pers_matrix(2,i);
    den_v[vector_map[i]] = cam_pers_matrix(2,i);
  }

  vpgl_rational_camera<double> cam_rat(neu_u,den_u,neu_v,den_v,
                                       x_scale,x_off,y_scale,y_off,z_scale,z_off,
                                       u_scale,u_off,v_scale,v_off);
  return cam_rat;
}
