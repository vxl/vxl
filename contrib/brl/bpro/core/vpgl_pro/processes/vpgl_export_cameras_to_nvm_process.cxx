// This is brl/bpro/core/vpgl_pro/processes/vpgl_export_cameras_to_nvm_process.cxx
#include <iostream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process to KRT cameras to a NVM file ( input for Viosual SFM)

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_perspective_camera.h>
#include <vnl/vnl_quaternion.h>
#include <vul/vul_file_iterator.h>
#include <bpgl/bpgl_camera_utils.h>
//: Init function
bool vpgl_export_cameras_to_nvm_process_cons(bprb_func_process& pro)
{
  //this process takes 6 inputs and has 3 outputs:

  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string"); // folder for cameras
  input_types.emplace_back("vcl_string"); // folder for images

  input_types.emplace_back("vcl_string");// output nvm file
  bool ok = pro.set_input_types(input_types);


  if (!ok) return ok;

  std::vector<std::string> output_types;
  return pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_export_cameras_to_nvm_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= pro.input_types().size()) {
    std::cout << "vpgl_export_cameras_to_nvm_process: The number of inputs should be " << pro.input_types().size() << std::endl;
    return false;
  }
  int i=0;
  std::string cam_dir = pro.get_input<std::string>(i++);
  std::string img_dir = pro.get_input<std::string>(i++);
  std::string output_nvm = pro.get_input<std::string>(i++);

  //populate vector of cameras
  //: returns a list of cameras from specified directory
  std::vector<vpgl_perspective_camera<double> *> cams = bpgl_camera_utils::cameras_from_directory(cam_dir);

  std::ofstream ofile(output_nvm.c_str());
  if(!ofile)
  {
      std::cout<<"Cannot open "<< output_nvm<<" file "<<std::endl;
      return false;
  }

  vul_file_iterator img_iter(img_dir+"/*.??g");
  ofile<<"NVM_V3"<<std::endl;
  ofile<<std::endl;
  ofile<<cams.size()<<std::endl;
  for(auto & cam : cams)
  {
      double f = cam->get_calibration().focal_length()
                *cam->get_calibration().x_scale();
      vnl_quaternion<double> q = cam->get_rotation().as_quaternion();
      vgl_point_3d<double> cc = cam->get_camera_center();

      ofile<<img_iter()<<" "<<f<<" "<<q.r()<<" "<<q.x()<<" "<<q.y()<<" "<<q.z()<<" "<<cc.x()<<" "<<cc.y()<<" "<<cc.z()<<" "<<0<<" "<<0<<std::endl;

      ++img_iter;
  }

  ofile.close();
  return true;
}
