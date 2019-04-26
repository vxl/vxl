// This is contrib/brl/bpro/core/vpgl_pro/processes
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
//       Take a list of rational caemras and a list of 2-d image correspondences
//       some of the correspondences may be very poor/wrong but a majority is of good quality (i.e. corresponds to the same 3D point)
//       Use a RANSAC scheme to find offsets for each camera using "inlier" correspondences
//       To improve back-projection accuracy, each correspondence will have its own initial guessing and search space
//
//       This code was developed under contract #FA8650-13-C-1613 and is approved for public release.
//       The case number for the approval is 88ABW-2014-1143.
//
// \verbatim
//  Modifications
//    Yi Dong, Aug, 2015, remove the track file to avoid using satellite original image
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
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vpgl/algo/vpgl_rational_adjust_onept.h>
#include <vpgl/algo/vpgl_rational_adjust_multipt.h>


namespace vpgl_isfm_rational_camera_with_initial_process_globals
{
  unsigned n_inputs_  = 9;
  unsigned n_outputs_ = 3;
}


bool vpgl_isfm_rational_camera_with_initial_process_cons(bprb_func_process& pro)
{
  using namespace vpgl_isfm_rational_camera_with_initial_process_globals;

  // inputs
  unsigned i = 0;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[i++] = "vcl_string"; // track file
  input_types_[i++] = "double";     // lower left longitude
  input_types_[i++] = "double";     // lower right latitude
  input_types_[i++] = "double";     // lower right elevation (meters)
  input_types_[i++] = "double";     // upper left longitude
  input_types_[i++] = "double";     // upper right latitude
  input_types_[i++] = "double";     // upper right elevation (meters)
  input_types_[i++] = "double";     // radius in pixels for the disagreement among inliers, e.g. 2 pixels
  input_types_[i++] = "bool";       // verbose

  // outputs
  i = 0;
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[i++] = "vpgl_camera_double_sptr";   // corrected camera
  output_types_[i++] = "double";                    // projection error
  output_types_[i++] = "double";                    // inlier in percentage

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


bool vpgl_isfm_rational_camera_with_initial_process(bprb_func_process& pro)
{
  using namespace vpgl_isfm_rational_camera_with_initial_process_globals;

  // verify inputs
  if (!pro.verify_inputs()) {
    std::cerr << pro.name() << ": Wrong inputs" << std::endl;
    return false;
  }

  // get the inputs
  unsigned in_i = 0;
  std::string trackfile  = pro.get_input<std::string>(in_i++);
  auto lower_left_lon    = pro.get_input<double>(in_i++);
  auto lower_left_lat    = pro.get_input<double>(in_i++);
  auto lower_left_elev   = pro.get_input<double>(in_i++);
  auto upper_right_lon   = pro.get_input<double>(in_i++);
  auto upper_right_lat   = pro.get_input<double>(in_i++);
  auto upper_right_elev  = pro.get_input<double>(in_i++);
  auto pixel_radius      = pro.get_input<double>(in_i++);
  auto verbose           = pro.get_input<bool>(in_i++);

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
  std::ifstream ifs(trackfile.c_str());
  if (!ifs) {
    std::cerr << pro.name() << ": can not open track input file: " << trackfile << "!!\n";
    return false;
  }
  std::string uncorrectedcam;
  unsigned n_track;
  unsigned n_seeds;
  ifs >> uncorrectedcam >> n_track >> n_seeds;
  vpgl_local_rational_camera<double> *cam = read_local_rational_camera<double>(uncorrectedcam);
  if (!cam) {
    std::cerr << pro.name() << ": failed to load local rational camera from file " << uncorrectedcam << "!!\n";
    return false;
  }

  // read seed camera/image files
  std::vector<std::string> seed_cam_files;
  for (unsigned i = 0; i < n_seeds; i++) {
    unsigned id;
    std::string seed_cam_file;
    ifs >> id >> seed_cam_file;
    seed_cam_files.push_back(seed_cam_file);
    if (!vul_file::exists(seed_cam_file)) {
      std::cerr << pro.name() << ": can not find seed camera file: " << seed_cam_file << "!!\n";
      return false;
    }
  }

  // read tracks
  std::vector<std::vector<vgl_point_2d<double> > > tracks;
  std::vector<std::vector<int> > trackimgids;
  std::vector<vgl_point_2d<double> > currpts;
  for (unsigned i = 0; i < n_track; i++)
  {
    int numfeatures = 0;
    ifs >> numfeatures;
    int id; float u, v;
    ifs >> id >> u >> v;  // id is always -1 for uncorrected camera
    currpts.emplace_back(u,v);
    std::vector<int> ids;
    std::vector<vgl_point_2d<double> > pts;
    for (int j = 1; j < numfeatures; j++) {
      int id; float u, v;
      ifs >> id >> u >> v;
      pts.emplace_back(u, v);
      ids.push_back(id);
    }
    trackimgids.push_back(ids);
    tracks.push_back(pts);
  }

  std::cout << "correcting camera: " << uncorrectedcam << "\n"
            << "  number of tracks = " << n_track << "\n"
            << "  number of seed cameras = " << n_seeds << std::endl;

  // load all seed cameras
  std::vector<vpgl_local_rational_camera<double>*> lcams;
  for (unsigned i = 0; i < n_seeds; i++) {
    vpgl_local_rational_camera<double> *ratcam = read_local_rational_camera<double>(seed_cam_files[i]);
    if (!ratcam) {
      std::cerr << pro.name() << ": failed to load local seed rational camera " << i << " from file: "<< seed_cam_files[i] << "!!\n";
      return false;
    }
    lcams.push_back(ratcam);
  }

  // compute translations
  std::vector<vgl_vector_2d<double> > cam_trans;
  std::vector<vgl_point_3d<double> > reconstructed_pts;
  std::vector<vgl_point_2d<double> > img_points;
  for (unsigned i = 0; i < n_track; i++)
  {
    std::vector<vgl_vector_2d<double> > cam_trans_i;
    std::vector<vgl_point_2d<double> > corrs;
    std::vector<float> weights;
    std::vector<vpgl_rational_camera<double> > cams;
    cams.push_back(*cam);
    weights.push_back(0.0f);  // the back-projection from un-corrected camera is ignored due to zero weight
    corrs.push_back(currpts[i]);
    for (unsigned k = 0; k < tracks[i].size(); k++) {
      cams.push_back(*lcams[trackimgids[i][k]]);
      weights.push_back(1.0f);
      corrs.push_back(tracks[i][k]);
    }

    // search for 3-d intersection
    vgl_point_3d<double> intersection;
    if (!vpgl_rational_adjust_onept::adjust_with_weights(cams, weights, corrs, initial_pt, zmin, zmax, cam_trans_i, intersection, relative_diameter)) {
      // if (verbose) {
      //   std::cout << "correspondence adjustment failed for: " << std::endl;
      //   for (auto & corr : corrs)
      //     std::cout << "[" << corr.x() << "," << corr.y() << "]\t";
      //   std::cout << '\n';
      // }
      continue;
    }

    img_points.push_back(currpts[i]);
    cam_trans.push_back(cam_trans_i[0]);
    reconstructed_pts.push_back(intersection);
  }

  // if (verbose) {
  //   std::cout << " ================== back-projection ======================= " << cam_trans.size() << std::endl;
  //   for (unsigned k = 0; k < cam_trans.size(); k++) {
  //     std::cout << "image: [" << img_points[k].x() << ',' << img_points[k].y() << "], "
  //               << "world: [" << reconstructed_pts[k].x() << ',' << reconstructed_pts[k].y() << reconstructed_pts[k].z() << "], "
  //               << "camera translation: " << cam_trans[k].x() << ',' << cam_trans[k].y() << "]" << std::endl
  //               ;
  //   }
  // }

  // remove unrealistic trans
  std::vector<vgl_vector_2d<double> > cam_trans_new;
  for (auto & cam_tran : cam_trans)
    if (cam_tran.x() >= -1000 && cam_tran.y() >= -1000)
      cam_trans_new.push_back(cam_tran);
  if (cam_trans_new.empty()) {
    std::cerr << pro.name() << ": can not find any valid translations, exit without correction!\n";
    return false;
  }

  // find the inliers from valid camera translations
  std::vector<unsigned> inlier_cnts(cam_trans_new.size(), 0);
  std::vector<std::vector<unsigned> > inliers;
  std::vector<std::vector<unsigned> > inlier_track_ids;
  for (unsigned i = 0; i < cam_trans_new.size(); i++)
  {
    std::vector<unsigned> inliers_i;
    std::vector<unsigned> inlier_track_id;
    // first push itself
    inliers_i.push_back(i);
    inlier_cnts[i]++;
    for (unsigned j = 0; j < cam_trans_new.size(); j++) {
      if (i == j || cam_trans_new[i].x() < -1000) continue;
      double dif = (cam_trans_new[i] - cam_trans_new[j]).length();
      if (dif < pixel_radius) {
        inlier_cnts[i]++;
        inliers_i.push_back(j);
      }
    }
    inliers.push_back(inliers_i);
    inlier_track_ids.push_back(inlier_track_id);
  }
  unsigned max = 0; unsigned max_i = 0;
  for (unsigned i = 0; i < cam_trans_new.size(); i++) {
    if (max < inlier_cnts[i]) {
      max = inlier_cnts[i];
      max_i = i;
    }
  }

  // use the average correspondence with the most number of inliers to correct camera
  vgl_vector_2d<double> sum(0.0, 0.0);
  for (unsigned int k : inliers[max_i])
    sum += cam_trans_new[k];
  sum /= (double)inliers[max_i].size();

  std::cout << "camera: " << uncorrectedcam << "\n"
            << "  " << n_track << " correspondences for " << pixel_radius << " pixel radius\n"
            << "  " << cam_trans_new.size() << " valid translations, "
                    << inliers[max_i].size() << " inliers (max = " << max << ")\n"
            << "  " << sum << " final translation"
            << std::endl;

  // evaluate the correction error and inlier percentage
  double error = 0.0;
  vpgl_rational_camera<double> rcam = (*cam);
  for (unsigned int k : inliers[max_i]) {
    vgl_point_2d<double> uvp = rcam.project(reconstructed_pts[k]);
    error += ((img_points[k] - uvp) = sum).sqr_length();
  }
  error = std::sqrt(error) / (double)inliers[max_i].size();
  double inlierpercent = (double)max / (double)cam_trans_new.size() * 100;

  double u, v;
  cam->image_offset(u, v);
  cam->set_image_offset(u+sum.x(), v+sum.y());

  // generate output
  pro.set_output_val<vpgl_camera_double_sptr>(0, cam);
  pro.set_output_val<double>(1, error);
  pro.set_output_val<double>(2, inlierpercent);
  return true;
}
