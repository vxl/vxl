// This is brl/bpro/core/vpgl_pro/processes/vpgl_convert_local_rational_to_perspective_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process to convert a local_rational_camera to a perspective_camera, using user-defined min and max z planes.

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/algo/vpgl_camera_compute.h>
#include <vpgl/algo/vpgl_camera_convert.h>
#include <vgl/vgl_box_3d.h>

//: Init function
bool vpgl_convert_local_rational_to_perspective_process_cons(bprb_func_process& pro)
{
  //this process takes 6 inputs and has 3 outputs:
  //  0) abstract camera

  std::vector<std::string> input_types;
  input_types.emplace_back("vpgl_camera_double_sptr");
  bool ok = pro.set_input_types(input_types);


  if (!ok) return ok;

  std::vector<std::string> output_types;
  output_types.emplace_back("vpgl_camera_double_sptr");  // label image
  return pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_convert_local_rational_to_perspective_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= pro.input_types().size()) {
    std::cout << "vpgl_convert_local_rational_to_perspective_process: The number of inputs should be " << pro.input_types().size() << std::endl;
    return false;
  }
  // get the inputs
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(0);
  if (!camera) {
    std::cerr << "Null camera input\n\n";
    return false;
  }
  auto *lrat_cam = dynamic_cast<vpgl_local_rational_camera<double>*>(camera.ptr());
  if (!lrat_cam) {
    std::cerr << "Error: camera is not a vpgl_local_rational_camera\n";
    return false;
  }


    std::cout << (*lrat_cam)<<std::endl;
  vpgl_scale_offset<double> sox = lrat_cam->scl_off(vpgl_rational_camera<double>::X_INDX);
  vpgl_scale_offset<double> soy = lrat_cam->scl_off(vpgl_rational_camera<double>::Y_INDX);
  vpgl_scale_offset<double> soz = lrat_cam->scl_off(vpgl_rational_camera<double>::Z_INDX);
#if 0 // unused
  vpgl_scale_offset<double> sou = lrat_cam->scl_off(vpgl_rational_camera<double>::U_INDX);
  vpgl_scale_offset<double> sov = lrat_cam->scl_off(vpgl_rational_camera<double>::V_INDX);
#endif
  vgl_point_3d<double> pmin(sox.offset()-sox.scale(), soy.offset()-soy.scale(), 0);
  vgl_point_3d<double> pmax(sox.offset()+sox.scale(), soy.offset()+soy.scale(), soz.scale());
  vgl_box_3d<double> approx_vol(pmin, pmax);

  vpgl_perspective_camera<double> pc;

  vgl_h_matrix_3d<double> norm_trans;
  vpgl_perspective_camera_convert::convert(*lrat_cam, approx_vol, pc, norm_trans);
  vpgl_camera_double_sptr out = new vpgl_perspective_camera<double>(pc);
  pro.set_output_val<vpgl_camera_double_sptr>(0, out);

  return true;
}
