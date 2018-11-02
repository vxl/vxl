// This is brl/bpro/core/vpgl_pro/processes/vpgl_correct_rational_cameras_multi_corr_process.cxx
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
#include <vul/vul_file.h>
#include <vul/vul_awk.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/algo/vpgl_rational_adjust_multipt.h>

//:
//  Take a list of rational cameras and a list of 2D image correspondences of multiple 3D point locations,
//  find those 3D locations and camera adjustments by optimizing all of their projections back to the images,
bool vpgl_correct_rational_cameras_mult_corr_process_cons(bprb_func_process& pro)
{
  //this process takes 4 inputs and has no outputs
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");  // a file that lists the path to a camera on each line and i and j coordinate of the 3D world point
                                    // format of the file:
                                    // n  # number of correspondences for each frame,
                                    // full_path_cam_name_1 i_11 j_11 i_12 j_12 ... i_1n j_1n
                                    // full_path_cam_name_2 i_21 j_21 i_22 j_22 ... i_2n j_2n
                                    // .
                                    // .
                                    // .
  input_types.emplace_back("vcl_string"); // output path to save the corrected cams, names will be input_cam_name_corrected.rpb
  input_types.emplace_back("float"); // radius in terms of pixels to search for camera translations
  input_types.emplace_back("int");   // number of intervals to break the radius into to generate the search space
  std::vector<std::string> output_types;
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_correct_rational_cameras_mult_corr_process(bprb_func_process& pro)
{
  if (pro.n_inputs()< 4) {
    std::cout << "lvpgl_correct_rational_cameras_process: The number of inputs should be 4" << std::endl;
    return false;
  }

  // get the inputs
  std::string input_cams = pro.get_input<std::string>(0);
  std::string output_path  = pro.get_input<std::string>(1);
  auto radius  = pro.get_input<float>(2);
  int interval_n  = pro.get_input<int>(3);

  std::ifstream ifs(input_cams.c_str());
  if (!ifs) {
    std::cerr<< " cannot open file: " << input_cams << '\n';
    return false;
  }
  unsigned int n; ifs >> n;
  std::cout << "will read: " << n << " correspondences for each frame from "<< input_cams << std::endl;

  std::vector<vpgl_rational_camera<double> > cams;
  std::vector<std::vector<vgl_point_2d<double> > > corrs;

  std::vector<std::string> out_cam_names;
  vul_awk awk(ifs);
  for (; awk; ++awk)
  {
    std::stringstream line(awk.line());
    std::string cam_path;
    ifs >> cam_path;
    if (cam_path.size() < 2) continue;
    std::cout << "reading cam: " << cam_path << std::endl;
    std::string img_name = vul_file::strip_directory(cam_path);
    img_name = vul_file::strip_extension(img_name);
    std::string out_cam_name = output_path + img_name + "_corrected.rpb";
    std::cout << "out cam name: " << out_cam_name << std::endl;
    vpgl_rational_camera<double> *ratcam = read_rational_camera<double>(cam_path);

    if ( !ratcam ) {
      std::cerr << "Failed to load rational camera from file" << cam_path << '\n';
      return false;
    }
    cams.push_back(*ratcam);
    out_cam_names.push_back(out_cam_name);

    std::vector<vgl_point_2d<double> > corrs_frame;
    for (unsigned int ii = 0; ii < n; ++ii) {
      double i, j;
      ifs >> i; ifs >> j;
      vgl_point_2d<double> cor(i,j);
      corrs_frame.push_back(cor);
    }
    corrs.push_back(corrs_frame);
  }

  ifs.close();
  std::cout << "cams size: " << cams.size() << " corrs size: " << corrs.size() << std::endl;

  std::cout << "Executing adjust image offsets\n";
  std::vector<vgl_vector_2d<double> > cam_trans;
  std::vector<vgl_point_3d<double> > intersections;
  std::vector<float> cam_weights(cams.size(), 1.0f/cams.size());
  if (!vpgl_rational_adjust_multiple_pts::adjust(cams, cam_weights, corrs, radius, interval_n, cam_trans, intersections))
  {
    std::cerr << "In vpgl_correct_rational_cameras_process - adjustment failed\n";
    return false;
  }

  if (intersections.size() != n) return false;
  for (const auto & intersection : intersections)
    std::cout << "after adjustment 3D intersection point: " << intersection << std::endl;

  for (unsigned i = 0; i < cams.size(); i++) {
    double u_off,v_off;
    cams[i].image_offset(u_off,v_off);
    cams[i].set_image_offset(u_off + cam_trans[i].x(), v_off + cam_trans[i].y());
    cams[i].save(out_cam_names[i]);
  }

  return true;
}

//:
//  Take a list of rational cameras and a list of 2D image correspondences of multiple 3D point locations,
//  find those 3D locations and camera adjustments by optimizing all of their projections back to the images, use Lev-Marq, needs a good initialization within one pixel error
bool vpgl_correct_rational_cameras_mult_corr_refine_process_cons(bprb_func_process& pro)
{
  //this process takes 2 inputs and has no outputs
  std::vector<std::string> input_types;
  input_types.emplace_back("vcl_string");  // a file that lists the path to a camera on each line and i and j coordinate of the 3D world point
                                    // format of the file:
                                    // n  # number of correspondences for each frame,
                                    // full_path_cam_name_1 i_11 j_11 i_12 j_12 ... i_1n j_1n
                                    // full_path_cam_name_2 i_21 j_21 i_22 j_22 ... i_2n j_2n
                                    // .
                                    // .
                                    // .
  input_types.emplace_back("vcl_string"); // output path to save the corrected cams, names will be input_cam_name_corrected.rpb
  std::vector<std::string> output_types;
  return pro.set_input_types(input_types)
      && pro.set_output_types(output_types);
}

//: Execute the process
bool vpgl_correct_rational_cameras_mult_corr_refine_process(bprb_func_process& pro)
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
  unsigned int n; ifs >> n;
  std::cout << "will read: " << n << " correspondences for each frame from "<< input_cams << std::endl;

  std::vector<vpgl_rational_camera<double> > cams;
  std::vector<std::vector<vgl_point_2d<double> > > corrs;

  std::vector<std::string> out_cam_names;
  vul_awk awk(ifs);
  for (; awk; ++awk)
  {
    std::stringstream line(awk.line());
    std::string cam_path;
    ifs >> cam_path;
    if (cam_path.size() < 2) continue;
    std::cout << "reading cam: " << cam_path << std::endl;
    std::string img_name = vul_file::strip_directory(cam_path);
    img_name = vul_file::strip_extension(img_name);
    std::string out_cam_name = output_path + img_name + "_corrected.rpb";
    std::cout << "out cam name: " << out_cam_name << std::endl;
    vpgl_rational_camera<double> *ratcam = read_rational_camera<double>(cam_path);

    if ( !ratcam ) {
      std::cerr << "Failed to load rational camera from file" << cam_path << '\n';
      return false;
    }
    cams.push_back(*ratcam);
    out_cam_names.push_back(out_cam_name);

    std::vector<vgl_point_2d<double> > corrs_frame;
    for (unsigned int ii = 0; ii < n; ++ii) {
      double i, j;
      ifs >> i; ifs >> j;
      vgl_point_2d<double> cor(i,j);
      corrs_frame.push_back(cor);
    }
    corrs.push_back(corrs_frame);
  }

  ifs.close();
  std::cout << "cams size: " << cams.size() << " corrs size: " << corrs.size() << std::endl;

  std::cout << "Executing adjust image offsets\n";
  std::vector<vgl_vector_2d<double> > cam_trans;
  std::vector<vgl_point_3d<double> > intersections;
  std::vector<float> cam_weights(cams.size(), 1.0f/cams.size());
  if (!vpgl_rational_adjust_multiple_pts::adjust_lev_marq(cams, cam_weights, corrs, cam_trans, intersections))
  {
    std::cerr << "In vpgl_correct_rational_cameras_process - adjustment failed\n";
    return false;
  }

  if (intersections.size() != n) return false;
  for (const auto & intersection : intersections)
    std::cout << "after adjustment 3D intersection point: " << intersection << std::endl;

  for (unsigned i = 0; i < cams.size(); i++) {
    double u_off,v_off;
    cams[i].image_offset(u_off,v_off);
    cams[i].set_image_offset(u_off + cam_trans[i].x(), v_off + cam_trans[i].y());
    cams[i].save(out_cam_names[i]);
  }

  return true;
}
