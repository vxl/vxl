// This is brl/bpro/core/vpgl_pro/processes/vpgl_export_cameras_to_nvm_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process to KRT cameras to a NVM file ( input for Viosual SFM)

#include <vcl_iostream.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vnl/vnl_quaternion.h>
#include <vul/vul_file_iterator.h>
#include <bpgl/bpgl_camera_utils.h>
//: Init function
bool vpgl_export_cameras_to_nvm_process_cons(bprb_func_process& pro)
{
  //this process takes 6 inputs and has 3 outputs:

  vcl_vector<vcl_string> input_types;
  input_types.push_back("vcl_string"); // folder for cameras
  input_types.push_back("vcl_string"); // folder for images

  input_types.push_back("vcl_string");// output nvm file
  bool ok = pro.set_input_types(input_types);


  if (!ok) return ok;

  vcl_vector<vcl_string> output_types;
  return pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_export_cameras_to_nvm_process(bprb_func_process& pro)
{
  if (pro.n_inputs()!= pro.input_types().size()) {
    vcl_cout << "vpgl_export_cameras_to_nvm_process: The number of inputs should be " << pro.input_types().size() << vcl_endl;
    return false;
  }
  int i=0;
  vcl_string cam_dir = pro.get_input<vcl_string>(i++);
  vcl_string img_dir = pro.get_input<vcl_string>(i++);
  vcl_string output_nvm = pro.get_input<vcl_string>(i++);

  //populate vector of cameras
  //: returns a list of cameras from specified directory
  vcl_vector<vpgl_perspective_camera<double> *> cams = bpgl_camera_utils::cameras_from_directory(cam_dir);

  vcl_ofstream ofile(output_nvm.c_str());
  if(!ofile)
  {
      vcl_cout<<"Cannot open "<< output_nvm<<" file "<<vcl_endl;
      return false;
  }

  vul_file_iterator img_iter(img_dir+"/*.??g");
  ofile<<"NVM_V3"<<vcl_endl;
  ofile<<vcl_endl;
  ofile<<cams.size()<<vcl_endl;
  for(unsigned k = 0 ; k <cams.size(); k++)
  {
      double f = cams[k]->get_calibration().focal_length()
                *cams[k]->get_calibration().x_scale();
      vnl_quaternion<double> q = cams[k]->get_rotation().as_quaternion();
      vgl_point_3d<double> cc = cams[k]->get_camera_center();
      
      ofile<<img_iter()<<" "<<f<<" "<<q.r()<<" "<<q.x()<<" "<<q.y()<<" "<<q.z()<<" "<<cc.x()<<" "<<cc.y()<<" "<<cc.z()<<" "<<0<<" "<<0<<vcl_endl;

      ++img_iter;
  }

  ofile.close();
  return true;
}

