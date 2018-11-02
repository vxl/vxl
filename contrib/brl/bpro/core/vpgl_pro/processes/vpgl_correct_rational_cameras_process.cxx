// This is brl/bpro/core/vpgl_pro/processes/vpgl_correct_rational_cameras_process.cxx
#include <iostream>
#include <sstream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vul/vul_file.h>
#include <vul/vul_awk.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/algo/vpgl_rational_adjust_onept.h>

//:
//  Take a list of rational cameras and a list of 2D image correspondences of the same 3D point location,
//  find that 3D location,
//  project that point back to images and correct each camera by adjusting its 2D image offset so that they all project the 3D location to the same 2D location
bool vpgl_correct_rational_cameras_process_cons(bprb_func_process& pro)
{
  //this process takes 2 inputs: the filename, and the path for the output
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");  // a file that lists the path to a camera on each line and i and j coordinate of the 3D world point
                                    // format of the file:
                                    // full_path_cam_name_1 i_1 j_1
                                    // full_path_cam_name_2 i_2 j_2
                                    // .
                                    // .
                                    // .
  input_types.emplace_back("vcl_string"); // output path to save the corrected cams, names will be input_cam_name_corrected.rpb
  std::vector<std::string> output_types;
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_correct_rational_cameras_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 2) {
    std::cout << "lvpgl_correct_rational_cameras_process: The number of inputs should be 2" << std::endl;
    return false;
  }

  // get the inputs
  std::string input_cams = pro.get_input<std::string>(0);
  std::string output_path  = pro.get_input<std::string>(1);

  std::ifstream ifs(input_cams.c_str());
  if (!ifs) {
    std::cerr<< " cannot open file: " << input_cams << '\n';
    return false;
  }

  std::vector<vpgl_rational_camera<double> > cams;
  std::vector<vpgl_rational_camera<double> > cams_origs;
  std::vector<vpgl_local_rational_camera<double>* > cams_local;
  std::vector<vgl_point_2d<double> > corrs;

  std::vector<std::string> out_cam_names;
  std::vector<std::string> out_cam_orig_names;
  vul_awk awk(ifs);
  bool local = true;
  for (; awk; ++awk)
  {
    std::stringstream line(awk.line());
    std::string cam_path, orig_cam_path;
    line >> cam_path;
    if (cam_path.size() < 2) continue;
    double i, j;
    line >> i; line >> j;
    //ifs >> orig_cam_path;
    std::cout << "reading cam: " << cam_path << '\n'
             << "\t corr i: " << i << " j: " << j << std::endl;
    std::string img_name = vul_file::strip_directory(cam_path);
    img_name = vul_file::strip_extension(img_name);
    std::string out_cam_name = output_path + img_name + "_corrected.rpb";
    //std::string out_cam_orig_name = output_path + img_name + "_orig.rpb";
    std::cout << "out cam name: " << out_cam_name << std::endl;

    vpgl_local_rational_camera<double> *ratcam = read_local_rational_camera<double>(cam_path);
    if ( !ratcam ) {
      local = false;
      vpgl_rational_camera<double> *ratcam2 = read_rational_camera<double>(cam_path);
      if ( !ratcam2 ) {
        std::cerr << "Failed to load rational camera from file" << cam_path << '\n';
        return false;
      }
      cams.push_back(*ratcam2);
    }
    else {
      cams_local.push_back(ratcam);
      vpgl_rational_camera<double> *ratcam2 = read_rational_camera<double>(cam_path);
      cams.push_back(*ratcam2);
    }
    //vpgl_rational_camera<double> *ratcam3 = read_rational_camera<double>(orig_cam_path);
    //cams_origs.push_back(*ratcam3);

    out_cam_names.push_back(out_cam_name);
    //out_cam_orig_names.push_back(out_cam_orig_name);

    vgl_point_2d<double> cor(i,j);
    corrs.push_back(cor);
  }

  ifs.close();
  std::cout << "cams size: " << cams.size() << " corrs size: " << corrs.size() << std::endl;

  std::cout << "Executing adjust image offsets\n";
  std::vector<vgl_vector_2d<double> > cam_trans;
  vgl_point_3d<double> intersection;
  if (!vpgl_rational_adjust_onept::adjust(cams, corrs, cam_trans,
                                          intersection))
  {
    std::cerr<< "In vpgl_correct_rational_cameras_process - adjustment failed\n";
    return false;
  }

  std::cout << "after adjustment 3D intersection point: " << intersection << std::endl;

  for (unsigned i = 0; i < cams.size(); i++) {
    if (local) {
      std::cout << "correcting LOCAL cam!!\n";
      double u_off,v_off;
      cams_local[i]->image_offset(u_off,v_off);
      cams_local[i]->set_image_offset(u_off + cam_trans[i].x(), v_off + cam_trans[i].y());
      cams_local[i]->save(out_cam_names[i]);
    }
    else {
      double u_off,v_off;
      cams[i].image_offset(u_off,v_off);
      cams[i].set_image_offset(u_off + cam_trans[i].x(), v_off + cam_trans[i].y());
      cams[i].save(out_cam_names[i]);
    }
    //double u_off,v_off;
    //cams_origs[i].image_offset(u_off,v_off);
    //cams_origs[i].set_image_offset(u_off + cam_trans[i].x(), v_off + cam_trans[i].y());
    //cams_origs[i].save(out_cam_orig_names[i]);
  }
  return true;
}
