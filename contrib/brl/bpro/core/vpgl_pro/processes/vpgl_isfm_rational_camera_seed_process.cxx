// This is contrib/brl/bpro/core/vpgl_pro/processes/vpgl_isfm_rational_camera_seed_process.cxx
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <bprb/bprb_func_process.h>
//:
// \file
//      Take a list of rational cameras and a list of 2-d image correspondences
//      Use a RANSAC scheme to find offsets for each camera using "inlier" correspondence
//      Note that the input and output cameras are local rational cameras
//
//      This code was developed under contract #FA8650-13-C-1613 and is approved for public release.
//      The case number for the approval is 88ABW-2014-1143.
//
// \verbatim
//  Modifications
//  Yi Dong, Aug, 2015, remove image dependency
// \endverbatim
//
#include <bprb/bprb_parameters.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vul/vul_file.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_distance.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/algo/vpgl_rational_adjust_onept.h>
#include <vpgl/algo/vpgl_rational_adjust_multipt.h>


//: take a list of rational cameras and a list of 2-d image correspondences of the same 3-d point location
//  find that 3-d location using back-projection and project that point back to images such that each camera can
//  be corrected by adjusting its 2-d image offsets, in a RANSAC scheme
namespace vpgl_isfm_rational_camera_seed_process_globals
{
  unsigned n_inputs_  = 11;
  unsigned n_outputs_ = 0;
}


bool vpgl_isfm_rational_camera_seed_process_cons(bprb_func_process& pro)
{
  using namespace vpgl_isfm_rational_camera_seed_process_globals;

  // inputs
  unsigned i = 0;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[i++] = "vcl_string"; // a file that lists the name to a input camera, file path of the satellite
                                    // image on each line and i and j coordinate of the 3D world point
                                    // format of the file:
                                    // n  # number of correspondences for each frame,
                                    // full_path_cam_name_1 i_11 j_11 i_12 j_12 ... i_1n j_1n
                                    // full_path_cam_name_2 i_21 j_21 i_22 j_22 ... i_2n j_2n
  input_types_[i++] = "vcl_string"; // output folder
  input_types_[i++] = "double";     // lower left longitude
  input_types_[i++] = "double";     // lower right latitude
  input_types_[i++] = "double";     // lower right elevation (meters)
  input_types_[i++] = "double";     // upper left longitude
  input_types_[i++] = "double";     // upper right latitude
  input_types_[i++] = "double";     // upper right elevation (meters)
  input_types_[i++] = "double";     // pixel radius for the disagreement among inliers, e.g., 2 pixels
  input_types_[i++] = "bool";       // option to enforce having at least 2 existing corrected cameras in output folder
  input_types_[i++] = "bool";       // verbose

  // outputs
  std::vector<std::string> output_types_(n_outputs_);

  // return
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//: execute the process
bool vpgl_isfm_rational_camera_seed_process(bprb_func_process& pro)
{
  using namespace vpgl_isfm_rational_camera_seed_process_globals;
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!!!!\n";
    return false;
  }

  // get the inputs
  unsigned in_i = 0;
  std::string input_txt  = pro.get_input<std::string>(in_i++);
  std::string out_folder = pro.get_input<std::string>(in_i++);
  auto lower_left_lon    = pro.get_input<double>(in_i++);
  auto lower_left_lat    = pro.get_input<double>(in_i++);
  auto lower_left_elev   = pro.get_input<double>(in_i++);
  auto upper_right_lon   = pro.get_input<double>(in_i++);
  auto upper_right_lat   = pro.get_input<double>(in_i++);
  auto upper_right_elev  = pro.get_input<double>(in_i++);
  auto pixel_radius      = pro.get_input<double>(in_i++);
  auto enforce_existing  = pro.get_input<bool>(in_i++);
  auto verbose           = pro.get_input<bool>(in_i++);

  if (enforce_existing)
    std::cout << pro.name() << ": require more than 2 pre-existing corrected cameras" << std::endl;
  else
    std::cout << pro.name() << ": DO NOT require pre-existing corrected cameras" << std::endl;

  // 3D region from inputs
  vgl_box_3d<double> region_3d(lower_left_lon, lower_left_lat, lower_left_elev,
                           upper_right_lon, upper_right_lat, upper_right_elev);
  if (region_3d.is_empty()) {
    std::cerr << pro.name() << ": invalid 3D extents" << std::endl;
    return false;
  }

  // min/max elevation
  double zmin = region_3d.min_z();
  double zmax = region_3d.max_z();

  // initial guess point (bottom center of 3D region)
  vgl_point_3d<double> initial_pt(region_3d.centroid_x(), region_3d.centroid_y(), region_3d.min_z());

  // searching diameter
  double width  = region_3d.width();
  double height = region_3d.height();
  double diagonal = std::sqrt(width*width + height*height);

  double relative_diameter;
  if (region_3d.centroid_x() > region_3d.centroid_y())
    relative_diameter = 0.5*diagonal/region_3d.centroid_y();
  else
    relative_diameter = 0.5*diagonal/region_3d.centroid_x();

  // verbose report
  if (verbose) {
    std::cout << "initial 3D point for back projection: " << initial_pt << "\n"
              << "elevation range: [" << zmin << ',' << zmax << ']' << "\n"
              << "relative diameter: " << relative_diameter << std::endl;
  }

  // read the track file
  std::ifstream ifs(input_txt.c_str());
  if (!ifs) {
    std::cerr << pro.name() << ": can not open track input file: " << input_txt << "!!\n";
    return false;
  }
  unsigned n;
  ifs >> n;
  if (!n) {
    std::cerr << pro.name() << ": 0 correspondences in file: " << input_txt << "! returning without correcting any camera!\n";
    return false;
  }
  std::cout << "will read " << n << " correspondences for each frame from " << input_txt << std::endl;
  std::vector<std::string> in_cam_files;
  std::vector<std::vector<vgl_point_2d<double> > > corrs;
  std::vector<std::string> out_cam_files;

  while (!ifs.eof())
  {
    std::string cam_file;
    ifs >> cam_file;
    if (cam_file.size() < 2)
      break;
    std::string out_cam_file = out_folder + vul_file::strip_extension(vul_file::basename(cam_file)) + "_local_corrected.rpb";
    std::vector<vgl_point_2d<double> > corrs_frame;
    for (unsigned ii = 0; ii < n; ii++) {
      double i, j;
      ifs >> i;  ifs >> j;
      vgl_point_2d<double> cor(i,j);
      corrs_frame.push_back(cor);
    }
    in_cam_files.push_back(cam_file);
    out_cam_files.push_back(out_cam_file);
    corrs.push_back(corrs_frame);
  }
  ifs.close();

  // print out
  if (verbose) {
    for (unsigned i = 0; i < in_cam_files.size(); i++)
    {
      std::cout << "============ " << i << " ============" << std::endl;
      std::cout << in_cam_files[i] << std::endl;
      std::cout << out_cam_files[i] << std::endl;
      for (unsigned k = 0; k < n; k++) {
        std::cout << '[' << corrs[i][k].x() << ',' << corrs[i][k].y() << "] ";
      }
      std::cout << '\n' << std::flush;
    }
  }

  // define camera weight
  std::vector<float> cam_weights;
  std::vector<vpgl_rational_camera<double> > cams;
  std::vector<vpgl_local_rational_camera<double> > local_cams;
  std::vector<std::vector<vgl_point_2d<double> > > new_corrs;
  unsigned cnt_exist = 0;

  // now determine which cameras already exist
  for (unsigned i = 0; i < out_cam_files.size(); i++)
  {
    if (vul_file::exists(out_cam_files[i]))
    {
      cnt_exist++;
      // load the corrected cameras
      vpgl_local_rational_camera<double> *ratcam = read_local_rational_camera<double>(out_cam_files[i]);
      if (!ratcam) {
        std::cerr << pro.name() << ": failed to load rational camera from file " << out_cam_files[i] << "!\n";
        return false;
      }
      cams.push_back(*ratcam);
      local_cams.push_back(*ratcam);
      cam_weights.push_back(1.0f);
      new_corrs.push_back(corrs[i]);
    }
    else // load original camera and start correction
    {
      vpgl_local_rational_camera<double> *ratcam = read_local_rational_camera<double>(in_cam_files[i]);
      if (!ratcam) {
        std::cerr << pro.name() << ": failed to load rational camera from file " << in_cam_files[i] << "!\n";
        return false;
      }
      cams.push_back(*ratcam);
      local_cams.push_back(*ratcam);
      cam_weights.push_back(0.0f);
      new_corrs.push_back(corrs[i]);
    }
  }
  if (enforce_existing && cnt_exist < 2) {
    std::cerr << pro.name() << ": enforcing condition to have at least 2 pre-existing corrected cameras! EXITING since there is: " << cnt_exist << " cameras.\n";
    return false;
  }

  // re-distribute weight parameters if there is no corrected camera
  if (cnt_exist == 0) {
    cam_weights.assign(cam_weights.size(), 1.0f/cams.size());
  }
  else {
    if (cnt_exist < 2) {
      std::cerr << pro.name() << ": If pre-existing camera exists, there should be at least 2 corrected cameras!\n";
      return false;
    }
  }

  if (verbose) {
    std::cout << cams.size() << " cameras, " << new_corrs.size() << " correspondences" << "\n";
    std::cout << "Assigned camera weights: \n";
    for (unsigned i = 0; i < cams.size(); i++)
      std::cout << "--" << in_cam_files[i] << " weight: " << cam_weights[i] << "\n";
    std::cout.flush();
  }

  // adjust using each correspondence and save the offsets
  std::vector<std::vector<vgl_vector_2d<double> > > cam_trans;
  std::vector<unsigned> corrs_ids;
  for (unsigned i = 0; i < n; i++)
  {
    // re-arrange the correspondence
    std::vector<vgl_point_2d<double> > corrs_i;
    corrs_i.reserve(new_corrs.size());
    for (auto & new_corr : new_corrs)
      corrs_i.push_back(new_corr[i]);
    std::vector<vgl_vector_2d<double> > cam_trans_i;
    vgl_point_3d<double> intersection;
    if (!vpgl_rational_adjust_onept::adjust_with_weights(cams, cam_weights, corrs_i, initial_pt, zmin, zmax, cam_trans_i, intersection, relative_diameter))
    {
      // if (verbose) {
      //   std::cout << "correspondence adjustment failed for correspondence: " << std::endl;
      //   for (auto & ii : corrs_i)
      //     std::cout << "[" << ii.x() << "," << ii.y() << "]\t";
      //   std::cout << '\n';
      // }
      continue;
    }
    cam_trans.push_back(cam_trans_i);
    corrs_ids.push_back(i);
  }

  if (!cam_trans.size()) {
    std::cerr << pro.name() << ": No valid corrections" << std::endl;
    return false;
  }

  std::cout << cam_trans.size() << " of " << n << " correspondences back-projected successfully:\n";
  for (unsigned int corrs_id : corrs_ids)
    std::cout << ' ' << corrs_id;
  std::cout << std::endl;

  // find the inliers
  std::vector<unsigned> inlier_cnts(cam_trans.size(), 0);
  std::vector<std::vector<unsigned> > inliers;
  for (unsigned i = 0; i < cam_trans.size(); i++) {  // for each correction find how many inliers are there for it
    std::vector<unsigned> inliers_i;
    inliers_i.push_back(corrs_ids[i]); // first push itself
    inlier_cnts[i]++;
    for (unsigned j = 0; j < cam_trans.size(); j++) {
      if (i == j) continue;
      double dif = 0;
      for (unsigned k = 0; k < cam_trans[i].size(); k++) {
        vgl_point_2d<double> trans1(cam_trans[i][k].x(), cam_trans[i][k].y());
        vgl_point_2d<double> trans2(cam_trans[j][k].x(), cam_trans[j][k].y());
        dif += vgl_distance(trans1, trans2);
      }
      dif /= cam_trans[i].size();
      if (dif < pixel_radius) {
        inlier_cnts[i]++;
        inliers_i.push_back(corrs_ids[j]);
      }
    }
    inliers.push_back(inliers_i);
  }
  unsigned max = 0;
  unsigned max_i = 0;
  for (unsigned i = 0; i < cam_trans.size(); i++) {
    if (max < inlier_cnts[i]) {
      max = inlier_cnts[i];
      max_i = i;
    }
  }

  // check whether the inliers count is sufficient
  double inlier_ratio = (double)max / cam_trans.size();
  if (inlier_ratio < 0.1) {
    std::cerr << pro.name() << ": correction failed (less than 10%% of correspondence yield constant translation)" << std::endl;
    return false;
  }

  std::cout << max << " of " << cam_trans.size() << " valid correspondences yield constant translation for " << pixel_radius << " pixel radius" << std::endl;
  for (unsigned int j : inliers[max_i])
    std::cout << ' ' << j;
  std::cout << std::endl;

  // use the correspondence with the most number of inliers to correct the cameras
  std::cout << "\nCOARSE SEED TRANSLATION\n";
  for (unsigned k = 0; k < cams.size(); k++) {
    std::cout << "  camera " << k << " [u,v] --> ["
              << cam_trans[max_i][k].x() << ","
              << cam_trans[max_i][k].y() << "]\n";
  }
  std::cout << std::endl;

  for (unsigned k = 0; k < cams.size(); k++) {
    double u_off, v_off;
    cams[k].image_offset(u_off, v_off);
    cams[k].set_image_offset(u_off + cam_trans[max_i][k].x(), v_off + cam_trans[max_i][k].y());
  }

  // refine the cameras using all the inliers of this correspondence
  std::vector<std::vector<vgl_point_2d<double> > > corrs_inliers;
  for (auto & new_corr : new_corrs) {
    std::vector<vgl_point_2d<double> > vec;
    for (unsigned int j : inliers[max_i])
      vec.push_back(new_corr[j]);
    corrs_inliers.push_back(vec);
  }
  std::vector<vgl_vector_2d<double> > cam_trans_inliers;
  std::vector<vgl_point_3d<double> > intersections;
  if (!vpgl_rational_adjust_multiple_pts::adjust_lev_marq(cams, cam_weights, corrs_inliers, initial_pt, zmin, zmax, cam_trans_inliers, intersections, relative_diameter))
  {
    std::cerr << "In vpgl_correct_rational_cameras_process - adjustment failed\n";
    return false;
  }

  std::cout << "\nREFINED SEED TRANSLATION\n";
  for (unsigned k = 0; k < cams.size(); k++) {
    std::cout << "  camera " << k << " [u,v] --> ["
              << cam_trans[max_i][k].x() + cam_trans_inliers[k].x() << ","
              << cam_trans[max_i][k].y() + cam_trans_inliers[k].y() << "]\n";
  }
  std::cout << std::endl;

  // further correction using refined offset values
  for (unsigned i = 0; i < cams.size(); i++)
  {
    double u_off, v_off;
    cams[i].image_offset(u_off, v_off);
    local_cams[i].set_image_offset(u_off+cam_trans_inliers[i].x(), v_off+cam_trans_inliers[i].y());
    local_cams[i].save(out_cam_files[i]);
  }
  return true;
}
