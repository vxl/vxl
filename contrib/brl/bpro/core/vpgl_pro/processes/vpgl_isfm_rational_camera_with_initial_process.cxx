// This is contrib/brl/bpro/core/vpgl_pro/processes
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vil/vil_config.h>
#if HAS_GEOTIFF
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
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vpgl/algo/vpgl_rational_adjust_onept.h>
#include <vpgl/algo/vpgl_rational_adjust_multipt.h>
#include <vul/vul_file_iterator.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_crop.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_intersection.h>
#include <vul/vul_file.h>
#include <brad/brad_image_metadata.h>

namespace vpgl_isfm_rational_camera_with_initial_process_globals
{
  unsigned n_inputs_  = 8;
  unsigned n_outputs_ = 3;

  //: find the min and max height in a given region from height map resources
  bool find_min_max_height(double const& ll_lon, double const& ll_lat, double const& ur_lon, double const& ur_lat,
                           std::vector<std::pair<vil_image_view_base_sptr, vpgl_geo_camera*> >& infos,
                           double& min, double& max);
  void crop_and_find_min_max(std::vector<std::pair<vil_image_view_base_sptr, vpgl_geo_camera*> >& infos,
                             unsigned const& img_id, int const& i0, int const& j0, int const& crop_ni, int const& crop_nj,
                             double& min, double& max);

  //: return the overlapped region of multiple 2-d bounding box
  vgl_box_2d<double> intersection(std::vector<vgl_box_2d<double> > const& boxes);

  //: calculate the relative diameter used in back-projection
  //  Relative diameter is used to define the initial search range in Amoeba algorithm (check vnl/algo/vnl_amoeba.h for more details)
  bool obtain_relative_diameter(double const& ll_lon, double const& ll_lat,
                                double const& ur_lon, double const& ur_lat,
                                double& relative_diameter);

  //: calculate the 3-d initial point from the overlapped region of satellite images
  bool initial_point_by_overlap_region(double const& ll_lon, double const& ll_lat, double const& ur_lon, double const& ur_lat,
                                       std::vector<std::pair<vil_image_view_base_sptr, vpgl_geo_camera*> >& dem_infos,
                                       vgl_point_3d<double>& init_pt,
                                       double& zmin, double& zmax,
                                       double const& height_diff = 20.0);
}

bool vpgl_isfm_rational_camera_with_initial_process_cons(bprb_func_process& pro)
{
  using namespace vpgl_isfm_rational_camera_with_initial_process_globals;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";  // track file
  input_types_[1] = "vcl_string";  // ASTER DEM height map folder
  input_types_[2] = "double";      // lower left longitude
  input_types_[3] = "double";      // lower right latitude
  input_types_[4] = "double";      // upper left longitude
  input_types_[5] = "double";      // upper right latitude
  input_types_[6] = "double";      // extra elevation value added onto height search space
  input_types_[7] = "float";       // radius in pixels for the disagreement among inliers, e.g. 2 pixels
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vpgl_camera_double_sptr";  // corrected camera
  output_types_[1] = "float";                    // projection error
  output_types_[2] = "float";                    // inlier in percentage

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
  std::string trackfile   = pro.get_input<std::string>(in_i++);
  std::string dem_folder  = pro.get_input<std::string>(in_i++);
  auto lower_left_lon  = pro.get_input<double>(in_i++);
  auto lower_left_lat  = pro.get_input<double>(in_i++);
  auto upper_right_lon = pro.get_input<double>(in_i++);
  auto upper_right_lat = pro.get_input<double>(in_i++);
  auto height_diff     = pro.get_input<double>(in_i++);
  auto pixel_radius     = pro.get_input<float>(in_i++);

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
    for (unsigned int j = 1; j < numfeatures; j++) {
      int id; float u, v;
      ifs >> id >> u >> v;
      pts.emplace_back(u, v);
      ids.push_back(id);
    }
    trackimgids.push_back(ids);
    tracks.push_back(pts);
  }

  std::cout << "correcting camera: " << uncorrectedcam << std::endl;
  std::cout << "number of tracks: " << n_track << std::endl;
  std::cout << "number of seed cameras: " << n_seeds << std::endl;

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

  // load aster dem resource
  std::vector<std::pair<vil_image_view_base_sptr, vpgl_geo_camera*> > dem_infos;
  std::string file_glob = dem_folder + "/*.tif";
  for (vul_file_iterator fn = file_glob.c_str(); fn; ++fn)
  {
    std::string filename = fn();
    vil_image_view_base_sptr img_r = vil_load(filename.c_str());
    vpgl_geo_camera* cam;
    vpgl_lvcs_sptr lvcs_dummy = new vpgl_lvcs;
    vil_image_resource_sptr img_res = vil_load_image_resource(filename.c_str());
    if (!vpgl_geo_camera::init_geo_camera(img_res, lvcs_dummy, cam)) {
      std::cerr << pro.name() << ": Given height map " << filename << " is NOT a GeoTiff!\n";
      return false;
    }
    dem_infos.emplace_back(img_r, cam);
  }
  if (dem_infos.empty()) {
    std::cerr << pro.name() << ": No image in the folder: " << dem_folder << std::endl;
    return false;
  }

  // define overlap region by input coordinates
  vgl_box_2d<double> overlap_region(lower_left_lon, upper_right_lon, lower_left_lat, upper_right_lat);
  if (overlap_region.is_empty()) {
    std::cerr << pro.name() << ": Missing input search region!\n";
    return false;
  }
  // compute translations
  std::vector<vgl_vector_2d<double> > cam_trans;
  std::vector<vgl_point_3d<double> > reconstructed_pts;
  std::vector<vgl_point_2d<double> > img_points;
  std::vector<vgl_point_3d<double> > init_pts;
  std::vector<double> zmin_list;
  std::vector<double> zmax_list;
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

    // find the initial guess point
    vgl_point_3d<double> initial_pt;
    double zmin, zmax;
    if (!initial_point_by_overlap_region(lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, dem_infos, initial_pt, zmin, zmax, height_diff)) {
      std::cerr << pro.name() << ": Evaluating initial point for correspondence " << i << " failed!\n";
      return false;
    }
    // find the searching diameter
    double relative_diameter;
    if (!obtain_relative_diameter(lower_left_lon, lower_left_lat, upper_right_lon, upper_right_lat, relative_diameter)) {
      std::cerr << pro.name() << ": Evaluating relative diameter for correspondence " << i << " failed!\n";
      return false;
    }
    // search for 3-d intersection
    vgl_point_3d<double> intersection;
    if (!vpgl_rational_adjust_onept::adjust_with_weights(cams, weights, corrs, initial_pt, zmin, zmax, cam_trans_i, intersection, relative_diameter)) {
      continue;
#if 1
      std::cout << "correspondence adjustment failed for: " << std::endl;
      for (auto & corr : corrs)
        std::cout << "[" << corr.x() << "," << corr.y() << "]\t";
      std::cout << '\n';
#endif
    }
    img_points.push_back(currpts[i]);
    cam_trans.push_back(cam_trans_i[0]);
    reconstructed_pts.push_back(intersection);
    init_pts.push_back(initial_pt);
    zmin_list.push_back(zmin);
    zmax_list.push_back(zmax);
  }

#if 0
  std::cout << " ================== back-projection ======================= " << std::endl;
  for (unsigned k = 0; k < cam_trans.size(); k++) {
    std::cout << "image point: [" << img_points[k].x() << ',' << img_points[k].y()
             << "], initial pt: [" << init_pts[k].x() << ',' << init_pts[k].y() << ',' << init_pts[k].z()
             << "], height range: [" << zmin_list[k] << ',' << zmax_list[k]
             << "], world point: [" << std::setprecision(10) << reconstructed_pts[k].x() << ',' << std::setprecision(10) << reconstructed_pts[k].y() << ','
             << std::setprecision(10) << reconstructed_pts[k].z()
             << "], camera translations: " << cam_trans[k].x() << ',' << cam_trans[k].y() << ']' << std::endl;
  }
#endif

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

  std::cout << "camera: " << uncorrectedcam
           << " out of " << n_track << " correspondences, inlier cnts using pixel radius: " << pixel_radius << ", "
           << " number of valid translation: " << cam_trans_new.size()
           << " inliers size: " << inliers[max_i].size() << "(" << max << ")"
           << " average offset: " << sum
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
  std::cout << "Final Translation is: " << sum << std::endl;

  // generate output
  pro.set_output_val<vpgl_camera_double_sptr>(0, cam);
  pro.set_output_val<float>(1, error);
  pro.set_output_val<float>(2, inlierpercent);
  return true;
}

bool vpgl_isfm_rational_camera_with_initial_process_globals::find_min_max_height(double const& ll_lon, double const& ll_lat, double const& ur_lon, double const& ur_lat,
                                                                                 std::vector<std::pair<vil_image_view_base_sptr, vpgl_geo_camera*> >& infos,
                                                                                 double& min, double& max)
{
  // find the corner points
  std::vector<std::pair<unsigned, std::pair<int, int> > > corners;
  std::vector<vgl_point_2d<double> > pts;
  pts.emplace_back(ll_lon, ur_lat);
  pts.emplace_back(ur_lon, ll_lat);
  pts.emplace_back(ll_lon, ll_lat);
  pts.emplace_back(ur_lon, ur_lat);
  for (auto & pt : pts)
  {
    // find the image
    for (unsigned j = 0; j < (unsigned)infos.size(); j++)
    {
      double u, v;
      infos[j].second->global_to_img(pt.x(), pt.y(), 0, u, v);
      int uu = (int)std::floor(u+0.5);
      int vv = (int)std::floor(v+0.5);
      if (uu < 0 || vv < 0 || uu >= (int)infos[j].first->ni() || vv >= (int)infos[j].first->nj())
        continue;
      std::pair<unsigned, std::pair<int, int> > pp(j, std::pair<int, int>(uu, vv));
      corners.push_back(pp);
      break;
    }
  }
  if (corners.size() != 4) {
    std::cerr << "Cannot locate all 4 corners among given DEM tiles!\n";
    return false;
  }
  // case 1 all corners are in the same image
  if (corners[0].first == corners[1].first) {
    // crop the image
    int i0 = corners[0].second.first;
    int j0 = corners[0].second.second;
    int crop_ni = corners[1].second.first-corners[0].second.first+1;
    int crop_nj = corners[1].second.second-corners[0].second.second+1;
    vpgl_isfm_rational_camera_with_initial_process_globals::crop_and_find_min_max(infos, corners[0].first, i0, j0, crop_ni, crop_nj, min, max);
    return true;
  }
  // case 2: two corners are in the same image
  if (corners[0].first == corners[2].first && corners[1].first == corners[3].first) {
    // crop the first image
    int i0 = corners[0].second.first;
    int j0 = corners[0].second.second;
    int crop_ni = infos[corners[0].first].first->ni() - corners[0].second.first;
    int crop_nj = corners[2].second.second-corners[0].second.second+1;
    vpgl_isfm_rational_camera_with_initial_process_globals::crop_and_find_min_max(infos, corners[0].first, i0, j0, crop_ni, crop_nj, min, max);

    // crop the second image
    i0 = 0;
    j0 = corners[3].second.second;
    crop_ni = corners[3].second.first + 1;
    crop_nj = corners[1].second.second-corners[3].second.second+1;
    vpgl_isfm_rational_camera_with_initial_process_globals::crop_and_find_min_max(infos, corners[1].first, i0, j0, crop_ni, crop_nj, min, max);
    return true;
  }
  // case 3: two corners are in the same image
  if (corners[0].first == corners[3].first && corners[1].first == corners[2].first) {
    // crop the first image
    int i0 = corners[0].second.first;
    int j0 = corners[0].second.second;
    int crop_ni = corners[3].second.first - corners[0].second.first + 1;
    int crop_nj = infos[corners[0].first].first->nj() - corners[0].second.second;
    vpgl_isfm_rational_camera_with_initial_process_globals::crop_and_find_min_max(infos, corners[0].first, i0, j0, crop_ni, crop_nj, min, max);

    // crop the second image
    i0 = corners[2].second.first;
    j0 = 0;
    crop_ni = corners[1].second.first - corners[2].second.first + 1;
    crop_nj = corners[2].second.second + 1;
    vpgl_isfm_rational_camera_with_initial_process_globals::crop_and_find_min_max(infos, corners[1].first, i0, j0, crop_ni, crop_nj, min, max);
    return true;
  }
  // case 4: all corners are in a different image
  // crop the first image, image of corner 0
  int i0 = corners[0].second.first;
  int j0 = corners[0].second.second;
  int crop_ni = infos[corners[0].first].first->ni() - corners[0].second.first;
  int crop_nj = infos[corners[0].first].first->nj() - corners[0].second.second;
  vpgl_isfm_rational_camera_with_initial_process_globals::crop_and_find_min_max(infos, corners[0].first, i0, j0, crop_ni, crop_nj, min, max);

  // crop the second image, image of corner 1
  i0 = 0;
  j0 = 0;
  crop_ni = corners[1].second.first + 1;
  crop_nj = corners[1].second.second + 1;
  vpgl_isfm_rational_camera_with_initial_process_globals::crop_and_find_min_max(infos, corners[1].first, i0, j0, crop_ni, crop_nj, min, max);

  // crop the third image, image of corner 2
  i0 = corners[2].second.first;
  j0 = 0;
  crop_ni = infos[corners[2].first].first->ni() - corners[2].second.first;
  crop_nj = corners[2].second.second + 1;
  vpgl_isfm_rational_camera_with_initial_process_globals::crop_and_find_min_max(infos, corners[2].first, i0, j0, crop_ni, crop_nj, min, max);

  // crop the fourth image, image of corner 3
  i0 = 0;
  j0 = corners[3].second.second;
  crop_ni = corners[3].second.first + 1;
  crop_nj = infos[corners[3].first].first->nj() - corners[3].second.second;
  vpgl_isfm_rational_camera_with_initial_process_globals::crop_and_find_min_max(infos, corners[3].first, i0, j0, crop_ni, crop_nj, min, max);
  return true;
}

void vpgl_isfm_rational_camera_with_initial_process_globals::crop_and_find_min_max(std::vector<std::pair<vil_image_view_base_sptr, vpgl_geo_camera*> >& infos,
                                                                                   unsigned const& img_id, int const& i0, int const& j0, int const& crop_ni, int const& crop_nj,
                                                                                   double& min, double& max)
{
  if (auto* img = dynamic_cast<vil_image_view<vxl_int_16>*>(infos[img_id].first.ptr()))
  {
    vil_image_view<vxl_int_16> img_crop = vil_crop(*img, i0, crop_ni, j0, crop_nj);
    for (unsigned ii = 0; ii < img_crop.ni(); ii++) {
      for (unsigned jj = 0; jj < img_crop.nj(); jj++) {
        if (min > img_crop(ii, jj)) min = img_crop(ii,jj);
        if (max < img_crop(ii, jj)) max = img_crop(ii,jj);
      }
    }
  }
  else if (auto* img = dynamic_cast<vil_image_view<float>*>(infos[img_id].first.ptr()))
  {
    vil_image_view<float> img_crop = vil_crop(*img, i0, crop_ni, j0, crop_nj);
    for (unsigned ii = 0; ii < img_crop.ni(); ii++) {
      for (unsigned jj = 0; jj < img_crop.nj(); jj++) {
        if (min > img_crop(ii, jj)) min = img_crop(ii,jj);
        if (max < img_crop(ii, jj)) max = img_crop(ii,jj);
      }
    }
  }
  return;
}

bool vpgl_isfm_rational_camera_with_initial_process_globals::initial_point_by_overlap_region(double const& ll_lon, double const& ll_lat, double const& ur_lon, double const& ur_lat,
                                                                                             std::vector<std::pair<vil_image_view_base_sptr, vpgl_geo_camera*> >& dem_infos,
                                                                                             vgl_point_3d<double>& init_pt,
                                                                                             double& zmin, double& zmax,
                                                                                             double const& height_diff)
{
  vgl_box_2d<double> overlap_region(ll_lon, ur_lon, ll_lat, ur_lat);
  if (overlap_region.is_empty())
    return false;
  // find the min and max elevation value of the overlap region
  double min_elev = 10000.0, max_elev = -10000.0;
  if (!vpgl_isfm_rational_camera_with_initial_process_globals::find_min_max_height(overlap_region.min_x(), overlap_region.min_y(), overlap_region.max_x(), overlap_region.max_y(),
                                                                                   dem_infos, min_elev, max_elev))
    return false;
  zmin = min_elev - 10 - height_diff;
  zmax = max_elev + 10 + height_diff;
  init_pt.set(overlap_region.centroid_x(), overlap_region.centroid_y(), zmin);
  return true;
}

bool vpgl_isfm_rational_camera_with_initial_process_globals::obtain_relative_diameter(double const& ll_lon, double const& ll_lat,
                                                                                      double const& ur_lon, double const& ur_lat,
                                                                                      double& relative_diameter)
{
  vgl_box_2d<double> overlap_region(ll_lon, ur_lon, ll_lat, ur_lat);
  if (overlap_region.is_empty())
    return false;
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

vgl_box_2d<double> vpgl_isfm_rational_camera_with_initial_process_globals::intersection(std::vector<vgl_box_2d<double> > const& boxes)
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
  return vpgl_isfm_rational_camera_with_initial_process_globals::intersection(new_boxes);
}
#endif
