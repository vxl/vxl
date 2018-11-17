// This is brl/bbas/volm/pro/processes/volm_correct_rational_cameras_process.cxx
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <bprb/bprb_func_process.h>
//:
// \file
//         Take a list of rational cameras and a list of 2D image correspondences to geo-register all cameras
//         some of the correspondences may be very poor/wrong but a majority is of good quality (i.e. corresponds to the same 3D point)
//         Use a RANSAC scheme to find offsets for each camera using "inlier" correspondences
//         A track file that contains the name of all cameras and associated 2-d image correspondences is used as an input.  Format of the file is:
//           # The number of correspondences for each frame,
//           # [satellite camera name 1] [i_11] [j_11] [i_12] [j_12] ... [i_1n] [j_1n]
//           # [satellite camera name 2] [i_21] [j_21] [i_22] [j_22] ... [i_2n] [j_2n]
//           # [satellite camera name 3] [i_31] [j_31] [i_32] [j_32] ... [i_3n] [j_3n]
//           # ...
//         To improve the accuracy, a 3-d initial guessing point is given.  The initial point can be defined based on the overlapped region of cameras
//
// \verbatim
//  Modifications
//    none yet
// \endverbatim
//
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
#include <vgl/vgl_box_2d.h>
#include <vgl/vgl_intersection.h>
#include <vpgl/algo/vpgl_rational_adjust_onept.h>
#include <vul/vul_file_iterator.h>
#include <volm/volm_satellite_resources.h>
#include <volm/volm_satellite_resources_sptr.h>
#include <volm/volm_io_tools.h>
#include <vil/vil_load.h>
#include <vil/file_formats/vil_nitf2_image.h>
#include <vpgl/file_formats/vpgl_nitf_rational_camera.h>
#include <vgl/vgl_distance.h>
#include <vpgl/algo/vpgl_rational_adjust_multipt.h>

namespace volm_correct_rational_cameras_ransac_with_initial_process_globals
{
  unsigned int n_inputs_  = 6;
  unsigned int n_outputs_ = 0;

  //: return the overlapped region of multiple 2-d bounding box
  vgl_box_2d<double> intersection(std::vector<vgl_box_2d<double> > const& boxes);

  //: load rational camera from nitf file path
  vpgl_rational_camera<double>* load_cam_from_nitf(std::string const& nitf_img_path);

  //: calculate the relative diameter used in back-projection
  //  Relative diameter is used to define the initial search range in Amoeba algorithm (check vnl/algo/vnl_amoeba.h for more details)
  bool obtain_relative_diameter(volm_satellite_resources_sptr const& sat_res,
                                std::vector<std::string> const& camera_names,
                                double& relative_diameter);

  //: calculate the 3-d initial point from the overlapped region of satellite images
  bool initial_point_by_overlap_region(volm_satellite_resources_sptr const& sat_res,
                                       std::vector<std::string> const& camera_names,
                                       std::string const& dem_folder,
                                       vgl_point_3d<double>& init_pt,
                                       double& zmin, double& zmax);
}

bool volm_correct_rational_cameras_ransac_with_intial_process_cons(bprb_func_process& pro)
{
  using namespace volm_correct_rational_cameras_ransac_with_initial_process_globals;
  std::vector<std::string> input_types(n_inputs_);
  input_types[0] = "volm_satellite_resources_sptr";  // satellite resource to fetch the full path of the satellite images
  input_types[1] = "vcl_string";                     // a file that lists the name to a camera on each line and i and j coordinate of the 3D world point
                                                     // format of the file:
                                                     // n  # number of correspondences for each frame,
                                                     // full_path_cam_name_1 i_11 j_11 i_12 j_12 ... i_1n j_1n
                                                     // full_path_cam_name_2 i_21 j_21 i_22 j_22 ... i_2n j_2n
  input_types[2] = "vcl_string";                     // ASTER DEM folder to retrieve the elevation values
  input_types[3] = "vcl_string";                     // output folder
  input_types[4] = "float";                          // pixel radius for the disagreement among inliers, e.g. 2 pixels
  input_types[5] = "bool";                           // option to enforce having at least 2 existing corrected cameras
  std::vector<std::string> output_types(n_outputs_);
  return pro.set_input_types(input_types) && pro.set_output_types(output_types);
}

//: execute the process
bool volm_correct_rational_cameras_ransac_with_intial_process(bprb_func_process& pro)
{
  using namespace volm_correct_rational_cameras_ransac_with_initial_process_globals;
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong Inputs!!!\n";
    return false;
  }
  // get the inputs
  unsigned in_i = 0;
  volm_satellite_resources_sptr res = pro.get_input<volm_satellite_resources_sptr>(in_i++);
  std::string input_txt   = pro.get_input<std::string>(in_i++);
  std::string dem_folder  = pro.get_input<std::string>(in_i++);
  std::string output_path = pro.get_input<std::string>(in_i++);
  auto pix_rad          = pro.get_input<float>(in_i++);
  bool enforce_existing  = pro.get_input<bool>(in_i++);

  if (enforce_existing)
    std::cout << "!!!!!!! enforce to have at least 2 existing images!\n";
  else
    std::cout << "!!!!!!! DO NOT enforce to have at least 2 existing images!\n";

  // read the track file
  std::ifstream ifs(input_txt.c_str());
  if (!ifs) {
    std::cerr << pro.name() << ": can not open file: " << input_txt << "!!\n";
    return false;
  }
  unsigned n;
  ifs >> n;
  if (!n) {
    std::cerr << pro.name() << ": 0 correspondences in file: " << input_txt << "! returning without correcting any cams!\n";
    return false;
  }

  std::cout << "will read: " << n << " correspondences for each frame from " << input_txt << std::endl;
  std::vector<std::string> cam_names;
  std::vector<std::string> in_cam_files;
  std::vector<std::vector<vgl_point_2d<double> > > corrs;
  std::vector<std::string> out_cam_files;

  while (!ifs.eof())
  {
    std::string cam_name;
    ifs >> cam_name;
    if (cam_name.size() < 2) break;
    std::cout << "reading camera: " << cam_name << std::endl;
    std::string out_cam_file = output_path + cam_name + "_corrected.rpb";
    std::cout << "output camera file: " << out_cam_file << std::endl;

    // locate the original cameras in the resources
    std::pair<std::string, std::string> img_path = res->full_path(cam_name);
    if (img_path.first.compare("") == 0) {
      std::cerr << pro.name() << ": Can not locate " << cam_name << " in satellite resources! exiting!\n";
      return false;
    }

    cam_names.push_back(cam_name);
    in_cam_files.push_back(img_path.first);
    out_cam_files.push_back(out_cam_file);

    std::vector<vgl_point_2d<double> > corrs_frame;
    for (unsigned ii = 0; ii < n; ii++) {
      double i, j;
      ifs >> i;  ifs >> j;
      vgl_point_2d<double> cor(i,j);
      corrs_frame.push_back(cor);
    }
    corrs.push_back(corrs_frame);
  }
  ifs.close();

  // define camera weights
  std::vector<float> cam_weights;
  std::vector<vpgl_rational_camera<double> > cams;
  std::vector<std::vector<vgl_point_2d<double> > > new_corrs;

  unsigned cnt_exist = 0;

  // now determine which cameras already exist
  for (unsigned i = 0; i < out_cam_files.size(); i++) {
    if (vul_file::exists(out_cam_files[i]))
    {
      cnt_exist++;
      // load corrected camera
      vpgl_rational_camera<double> *ratcam = read_rational_camera<double>(out_cam_files[i]);
      if (!ratcam) {
        std::cerr << pro.name() << ": Failed to load rational camera from file " << out_cam_files[i] << "\n";
        return false;
      }
      cams.push_back(*ratcam);
      cam_weights.push_back(1.0f);
      new_corrs.push_back(corrs[i]);
    }
    else // load original camera and start correction
    {
      vpgl_rational_camera<double> *ratcam = load_cam_from_nitf(in_cam_files[i]);
      if (!ratcam) {
        std::cerr << pro.name() << ": Failed to load original camera from original image path " << in_cam_files[i] << "\n";
        return false;
      }
      cams.push_back(*ratcam);
      cam_weights.push_back(0.0f);
      new_corrs.push_back(corrs[i]);
    }
  }
  if (enforce_existing && cnt_exist < 2) {
    std::cerr << pro.name() << ": Enforcing condition to have 2 pre-existing corrected cameras! EXITING since there is: " << cnt_exist << " cameras.\n";
    return false;
  }

  // re-distribute weight if there is no corrected camera
  if (cnt_exist == 0) {
    cam_weights.assign(cam_weights.size(), 1.0f/cams.size());
  }
  else {
    if (cnt_exist < 2) {
      std::cerr << pro.name() << ": If pre-existing camera exists, there should be at least 2 corrected cameras!\n";
      return false;
    }
  }
  std::cout << " assigned camera weights: \n";
  for (unsigned i = 0; i < cams.size(); i++)
    std::cout << cam_names[i] << " weight: " << cam_weights[i] << std::endl;
  std::cout << "camera size: " << cams.size() << " corrs size: " << new_corrs.size() << std::endl;
  std::cout.flush();

  // calculate the initial guessing point
  vgl_point_3d<double> initial_pt;
  double zmin, zmax;
  if (!initial_point_by_overlap_region(res, cam_names, dem_folder, initial_pt, zmin, zmax)) {
    std::cerr << pro.name() << ": Evaluating initial point failed!\n";
    return false;
  }
  // calculate the relative diameter given the fact that all correspondence must be inside the overlapped region of all cameras
  double relative_diameter;
  if (!obtain_relative_diameter(res, cam_names, relative_diameter)) {
    std::cerr << pro.name() << ": Evaluating relative diameter failed!\n";
    return false;
  }

  // adjust using each correspondence and save the offsets
  std::cout << "Executing adjust image offsets..." << std::endl;
  std::cout << "initial 3-d point for back projection: " << initial_pt << std::endl;
  std::cout << "height range: [" << zmin << ',' << zmax << ']' << std::endl;
  std::cout << "relative diameter: " << relative_diameter << std::endl;
  std::vector<std::vector<vgl_vector_2d<double> > > cam_trans;
  std::vector<unsigned> corrs_ids;
  for (unsigned i = 0; i < n; i++)
  {
    // rearrange the correspondence
    std::vector<vgl_point_2d<double> > corrs_i;
    corrs_i.reserve(new_corrs.size());
for (auto & new_corr : new_corrs)
      corrs_i.push_back(new_corr[i]);
    std::vector<vgl_vector_2d<double> > cam_trans_i;
    vgl_point_3d<double> intersection;
    if (!vpgl_rational_adjust_onept::adjust_with_weights(cams, cam_weights, corrs_i, initial_pt, zmin, zmax, cam_trans_i, intersection, relative_diameter))
    {
#if 1
      std::cout << "correspondence adjustment failed for correspondence: " << std::endl;
      for (auto & ii : corrs_i)
        std::cout << "[" << ii.x() << "," << ii.y() << "]\t";
      std::cout << '\n';
#endif
      continue;
    }
    cam_trans.push_back(cam_trans_i);
    corrs_ids.push_back(i);

#if 1
    std::cout << i << " --> correspondence: ";
    for (auto & i : corrs_i) {
        std::cout << "[" << i.x() << "," << i.y() << "]\t";
    }
    std::cout << " --> project to 3D intersection point: [" << std::setprecision(12) << intersection.y()
                                                         << "," << std::setprecision(12) << intersection.x()
                                                         << "," << std::setprecision(12) << intersection.z()
                                                         << "], giving offset: ";
    std::cout << " --> camera translation: ";
    for (auto & i : cam_trans_i) {
      std::cout << "[" << i.x() << "," << i.y() << "]\t";
    }
    std::cout << '\n';
#endif
  }
  std::cout << "out of " << n << " correspondences " << cam_trans.size() << " of them back-projected to 3-d world point successfully:";
  for (unsigned int corrs_id : corrs_ids)
    std::cout << ' ' << corrs_id;
  std::cout << '\n';

  if (!cam_trans.size()) {
    std::cout << "out of " << n << " correspondences " << cam_trans.size() << " of them yielded corrections! exit without any correction!\n";
    return false;
  }

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
      if (dif < pix_rad) {
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
  std::cout << "out of " << cam_trans.size() << " valid correspondences, " << max << " of them yield constant translations using " << pix_rad << " pixel radius" << std::endl;
  // check whether the inliers count is sufficient
  double inlier_ratio = (double)max / cam_trans.size();
  if (inlier_ratio < 0.1) {
    std::cout << pro.name() << ": less than 10% of correspondence yield constant translations due to bad correspondence, correction failed" << std::endl;
    return false;
  }

#if 1
  std::cout << "correspondence that provides inliers: " << std::endl;
  for (unsigned int j : inliers[max_i])
    std::cout << j << ' ';
  std::cout << '\n';
#endif
  // use the correspondence with the most number of inliers to correct the cameras
  std::cout << "correction offset: " << std::endl;
  for (unsigned k = 0; k < cams.size(); k++)
    std::cout << "camera " << k << " --> offset_u: " << cam_trans[max_i][k].x() << " offset_v: " << cam_trans[max_i][k].y() << std::endl;
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
  //std::vector<float> cam_weights_equal(cams.size(), 1.0f/cams.size());
  //if (!vpgl_rational_adjust_multiple_pts::adjust_lev_marq(cams, cam_weights_equal, corrs_inliers, cam_trans_inliers, intersections))
  //if (!vpgl_rational_adjust_multiple_pts::adjust_lev_marq(cams, cam_weights, corrs_inliers, cam_trans_inliers, intersections))
  if (!vpgl_rational_adjust_multiple_pts::adjust_lev_marq(cams, cam_weights, corrs_inliers, initial_pt, zmin, zmax, cam_trans_inliers, intersections, relative_diameter))
  {
    std::cerr << "In vpgl_correct_rational_cameras_process - adjustment failed\n";
    return false;
  }

#if 1
  std::cout << " after refinement: \n";
  for (auto & intersection : intersections)
    std::cout << "after adjustment 3D intersection point: " << std::setprecision(12) << intersection.y() << "," << std::setprecision(12) << intersection.x()
                                                           << "," << std::setprecision(12) << intersection.z()
                                                           << std::endl;
#endif
  std::cout << "correction offset from refinement:" << std::endl;
  for (auto & cam_trans_inlier : cam_trans_inliers)   // for each correction find how many inliers are there for it
    std::cout << "offset_u: " << cam_trans_inlier.x() << " v: " << cam_trans_inlier.y() << std::endl;

  for (unsigned i = 0; i < cams.size(); i++) {
    double u_off,v_off;
    cams[i].image_offset(u_off,v_off);
    cams[i].set_image_offset(u_off + cam_trans_inliers[i].x(), v_off + cam_trans_inliers[i].y());
    cams[i].save(out_cam_files[i]);
  }

  return true;
}

vpgl_rational_camera<double>* volm_correct_rational_cameras_ransac_with_initial_process_globals::load_cam_from_nitf(std::string const& nitf_img_path)
{
  vil_image_resource_sptr image = vil_load_image_resource(nitf_img_path.c_str());
  if (!image)
  {
    return nullptr;
  }
  std::string format = image->file_format();
  std::string prefix = format.substr(0,4);
  if (prefix != "nitf") {
    return nullptr;
  }
  // cast to an nitf2_image
  auto *nitf_image = static_cast<vil_nitf2_image*>(image.ptr());
  auto *nitf_cam = new vpgl_nitf_rational_camera(nitf_image, true);
  return dynamic_cast<vpgl_rational_camera<double>*>(nitf_cam);
}

bool volm_correct_rational_cameras_ransac_with_initial_process_globals::obtain_relative_diameter(volm_satellite_resources_sptr const& sat_res,
                                                                                                 std::vector<std::string> const& camera_names,
                                                                                                 double& relative_diameter)
{
  relative_diameter = 1.0;
  // obtain the overlap region
  std::vector<vgl_box_2d<double> > img_footprints;
  auto cit = camera_names.begin();
  for (; cit != camera_names.end(); ++cit)
  {
    std::pair<std::string, std::string> img_path = sat_res->full_path(*cit);
    if (img_path.first.compare("") == 0)
      return false;
    brad_image_metadata meta(img_path.first);
    double ll_lon = meta.lower_left_.x();
    double ll_lat = meta.lower_left_.y();
    double ur_lon = meta.upper_right_.x();
    double ur_lat = meta.upper_right_.y();
    vgl_box_2d<double> img_box(ll_lon, ur_lon, ll_lat, ur_lat);
    img_footprints.push_back(img_box);
  }

  vgl_box_2d<double> overlap_region = volm_correct_rational_cameras_ransac_with_initial_process_globals::intersection(img_footprints);
  double width  = overlap_region.width();
  double height = overlap_region.height();
  // calculate the diameter by the diagonal
  double diagonal = std::sqrt(width*width + height*height);
  if (overlap_region.centroid_x() > overlap_region.centroid_y())
    relative_diameter = 0.5*diagonal/overlap_region.centroid_y();
  else
    relative_diameter = 0.5*diagonal/overlap_region.centroid_x();
  return true;
}

bool volm_correct_rational_cameras_ransac_with_initial_process_globals::initial_point_by_overlap_region(volm_satellite_resources_sptr const& sat_res,
                                                                                                        std::vector<std::string> const& camera_names,
                                                                                                        std::string const& dem_folder,
                                                                                                        vgl_point_3d<double>& init_pt,
                                                                                                        double& zmin, double& zmax)
{
  std::vector<volm_img_info> infos;
  volm_io_tools::load_aster_dem_imgs(dem_folder, infos);
  if (infos.empty()) {
    std::cerr << "can not find any height map in the DEM folder " << dem_folder << '\n';
    return false;
  }
  // obtain the overlap region
  std::vector<vgl_box_2d<double> > img_footprints;
  auto cit = camera_names.begin();
  for (; cit != camera_names.end(); ++cit)
  {
    std::pair<std::string, std::string> img_path = sat_res->full_path(*cit);
    if (img_path.first.compare("") == 0)
      return false;
    brad_image_metadata meta(img_path.first);
    double ll_lon = meta.lower_left_.x();
    double ll_lat = meta.lower_left_.y();
    double ur_lon = meta.upper_right_.x();
    double ur_lat = meta.upper_right_.y();
    vgl_box_2d<double> img_box(ll_lon, ur_lon, ll_lat, ur_lat);
    img_footprints.push_back(img_box);
  }
  vgl_box_2d<double> overlap_region = volm_correct_rational_cameras_ransac_with_initial_process_globals::intersection(img_footprints);

  // find the min and max height of the overlapped region
  double min_elev = 10000.0, max_elev = -10000.0;
  vgl_point_2d<double> lower_left = overlap_region.min_point();
  vgl_point_2d<double> upper_right = overlap_region.max_point();
  if (!volm_io_tools::find_min_max_height(lower_left, upper_right, infos, min_elev, max_elev)){
    std::cerr << "can not find elevation for the overlap region " << overlap_region.min_point() << ", " << overlap_region.max_point() << '\n';
    return false;
  }
  zmin = min_elev - 10;
  zmax = max_elev + 10;
  init_pt.set(overlap_region.centroid_x(), overlap_region.centroid_y(), zmin);
  return true;
}

vgl_box_2d<double> volm_correct_rational_cameras_ransac_with_initial_process_globals::intersection(std::vector<vgl_box_2d<double> > const& boxes)
{
  if (boxes.size() == 2) {
    return vgl_intersection(boxes[0], boxes[1]);
  }
  std::vector<vgl_box_2d<double> > new_boxes;
  vgl_box_2d<double> box = vgl_intersection(boxes[0], boxes[1]);
  if (box.is_empty())
    return box;
  new_boxes.push_back(box);
  for (unsigned i = 2; i < boxes.size(); i++)
    new_boxes.push_back(boxes[i]);
  return volm_correct_rational_cameras_ransac_with_initial_process_globals::intersection(new_boxes);
}
