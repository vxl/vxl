#include <iostream>
#include <fstream>
#include <string>
#include <vul/vul_file.h>
#include <vul/vul_arg.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>
#include <vpgl/vpgl_calibration_matrix.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/algo/vpgl_camera_homographies.h>
#include <bwm/video/bwm_video_corr_processor.h>
#include <bwm/video/bwm_video_site_io.h>
#include <bwm/video/bwm_video_cam_istream.h>
#include <bwm/video/bwm_video_cam_ostream.h>

#include <vpgl/vpgl_lvcs.h>
#include <vgl/algo/vgl_h_matrix_3d_compute_linear.h>

static bool process_camera_from_photo_overlay(std::string const& params_path, std::string const& output_cam_path)
{
 if (params_path == ""||output_cam_path == "")
    return false;

 std::ifstream pfs(params_path.c_str());
 if (!pfs)
   return false;
 double lat, longit, alt, heading, tilt, roll, focal_length, ni, nj, lvcs_lat, lvcs_longit;
 std::string dummy;
 char buf[1000];
 pfs.getline(buf, 1000); // get the comment line
 pfs >> dummy; pfs >> lat; pfs >> dummy; pfs >> longit; pfs >> dummy; pfs >> alt;
 pfs >> dummy; pfs >> heading; pfs >> dummy; pfs >> tilt; pfs >> dummy; pfs >> roll; pfs >> dummy; pfs >> focal_length;
 pfs >> dummy; pfs >> ni; pfs >> dummy; pfs >> nj;
 pfs >> dummy; pfs >> lvcs_lat; pfs >> dummy; pfs >> lvcs_longit;
 std::cout.precision(15);
 std::cout << "read params: " << lat << ' ' << longit << ' ' << alt << ' '
          << heading << ' ' << tilt << ' ' << roll << ' ' << focal_length
          << ' ' << ni << ' ' << nj << '\n'
          << " CAUTION: focal length should be in pixels!\n"
          << "using LVCS: lvcs_lat " << lvcs_lat << " lvcs_long: " << lvcs_longit << std::endl;

 vpgl_lvcs_sptr lvcs = new vpgl_lvcs(lvcs_lat, lvcs_longit);
 double x,y,z;
 lvcs->global_to_local(longit, lat, alt, lvcs->get_cs_name(), x,y,z);
 vgl_homg_point_3d<double> camera_center(x,y,z);
 std::cout << "camera center in local coords: " << camera_center << std::endl;
 vgl_point_3d<double> cam_cent(x,y,z);
 //vgl_point_3d<double> look_at(x,y,0);
 lvcs->global_to_local(longit, lat+1, alt, lvcs->get_cs_name(), x,y,z);
 vgl_point_3d<double> cam_cent_up(x,y,z);
 vgl_vector_3d<double> up_vector = cam_cent_up-cam_cent; // y axis of google

 double deg2rad = vnl_math::pi_over_180;

 // now rotate up_vector by heading around Z axis
 // heading is a rotation about Z axis
 vnl_quaternion<double> q_h(0.0, 0.0, -heading*deg2rad);
 vgl_rotation_3d<double> R_h(q_h);
 up_vector = R_h*up_vector;   // this is up vector of camera

 // find x axis wrt to up, cross product of -up vector and z
 vgl_vector_3d<double> z_vector(0.0, 0.0, 1.0);
 vgl_vector_3d<double> x_vector = cross_product(-up_vector,z_vector);
 normalize(x_vector);

 // now rotate up_vector by tilt around x vector
 vnl_double_3 x_vector_fixed(x_vector.x(), x_vector.y(), x_vector.z());
 vnl_quaternion<double> q_t(x_vector_fixed, tilt*deg2rad);
 vgl_rotation_3d<double> R_t(q_t);
 vgl_vector_3d<double> look_vector = R_t*up_vector;

 // now find look at point
 vgl_point_3d<double> look_at = cam_cent + look_vector;

 vnl_double_3x3 M;
 M[0][0] = focal_length; M[0][1] = 0; M[0][2] = ni/2;
 M[1][0] = 0; M[1][1] = focal_length; M[1][2] = nj/2;
 M[2][0] = 0; M[2][1] = 0; M[2][2] = 1;
 vpgl_calibration_matrix<double> K(M);
 std::cout << "initial K:\n" << M << std::endl;

 vgl_rotation_3d<double> I; // no rotation initially
 vpgl_perspective_camera<double> camera(K, camera_center,I);
 camera.look_at(vgl_homg_point_3d<double>(look_at), up_vector);

 std::cout << "final cam:\n" << camera << std::endl;

 // write the camera out
 std::ofstream ofile(output_cam_path.c_str());
 if (ofile)
 {
   ofile<<camera.get_calibration().get_matrix()<<'\n'
        <<camera.get_rotation().as_matrix()<<'\n'
        <<camera.get_translation().x()<<' '<<camera.get_translation().y()<<' '<<camera.get_translation().z()<<'\n';
 }

 return true;
}

static bool process_conv(std::string const& site_path,
                         double latt, double longit, std::string const& output_site_path)
{
  if (site_path == ""||output_site_path == "")
    return false;
  std::cout << "site_path: " << site_path << '\n'
           << "output_site_path: " << output_site_path << std::endl;
  std::cout.precision(10);
  std::cout << " LVCS coords: latitude: " << latt << " longitude: " << longit << std::endl;

  bwm_video_corr_processor cp;
  cp.set_verbose(true);
  if (!cp.open_video_site(site_path, false))
    return false;

  vpgl_lvcs_sptr lvcs = new vpgl_lvcs(latt, longit);
  cp.convert_world_pts_to_local(lvcs);
  cp.write_video_site(output_site_path);
  return true;
}

static bool process_no_H(std::string const& site_path,
                         std::string const& cal_matrix_path,
                         double initial_depth,
                         std::string const& output_site_path,
                         std::string const& output_cam_dir)
{
  if (site_path == ""||output_site_path == "" ||output_cam_dir == "")
    return false;

  std::cout << " NOT USING H!\n"
           << "site_path: " << site_path << '\n'
           << "output_site_path: " << output_site_path << '\n'
           << "output_cam_dir: " << output_cam_dir << std::endl;

  bwm_video_corr_processor cp;
  cp.set_verbose(true);
  if (!cp.open_video_site(site_path, false))
    return false;

  unsigned min_frame, max_frame, ncameras;
  ncameras = cp.get_ncameras(min_frame, max_frame);
  vpgl_perspective_camera<double> dummy_camera;
  std::vector<vpgl_perspective_camera<double> > cameras(ncameras, dummy_camera);

  if (initial_depth == 0) {  // the passed camera is a full perspective camera, use it directly
    std::ifstream ifs(cal_matrix_path.c_str());
    ifs >> dummy_camera;
    ifs.close();
    std::cout << "using initial camera:\n" << dummy_camera << '\n';
    for (auto & camera : cameras)
      camera = dummy_camera;
  }
  else {
    // initialize the cameras
    vnl_double_3x3 M;
    if (cal_matrix_path == "")
      return false;
    std::ifstream kis(cal_matrix_path.c_str());
    kis >> M;
    vpgl_calibration_matrix<double> K(M);
    std::cout << "initial K:\n" << M << std::endl;

    vgl_rotation_3d<double> I; // no rotation initially
    vpgl_perspective_camera<double> camera(K, vgl_homg_point_3d<double>(0.0, 0.0, initial_depth),I);
    // find the first valid 3-d point and make the camera look at that point
    bool found = false;
    for (unsigned i = 0; i < cp.correspondences().size() && !found; i++) {
      if (cp.correspondences()[i]->world_pt_valid()) {
        camera.look_at(vgl_homg_point_3d<double>(cp.correspondences()[i]->world_pt()));
        found = true;
      }
    }
    if (!found)
      camera.look_at(vgl_homg_point_3d<double>(0.0, 0.0, 0.0));
    for (auto & i : cameras)
      i = camera;
  }


  // write cams as text
  std::string cam_init_dir_txt = output_cam_dir + "_initial_txt";
  vul_file::make_directory(cam_init_dir_txt);
  if (vul_file::is_directory(cam_init_dir_txt))
    cp.write_cameras_txt(cam_init_dir_txt, cameras);

  // write cams as stream
  std::string cam_init_dir = output_cam_dir + "_initial";
  vul_file::make_directory(cam_init_dir);
  if (vul_file::is_directory(cam_init_dir)) {
    bwm_video_cam_ostream_sptr cam_ostr = new bwm_video_cam_ostream(cam_init_dir);
    if (cam_ostr && cam_ostr->is_open()) {
      for (auto & camera : cameras)
        cam_ostr->write_camera(&camera);
      cam_ostr->close();
    }
  }
  else
    return false;

  cp.open_camera_istream(cam_init_dir+"\\*");
  cp.open_camera_ostream(output_cam_dir);
  if (!cp.refine_world_pts_and_cameras())
    return false;

  std::string cam_out_dir = output_cam_dir + "_txt";
  vul_file::make_directory(cam_out_dir);
  if (vul_file::is_directory(cam_out_dir))
    cp.write_cameras_txt(cam_out_dir, cp.cameras());

  cp.write_video_site(output_site_path);
  return true;
}


static bool process(std::string const& site_path,
                    std::string const& cal_matrix_path,
                    double initial_depth,
                    std::string const& output_site_path,
                    std::string const& output_cam_dir,
                    std::string const& output_proj_cam_dir)
{
  if (site_path == ""||output_site_path == "" ||output_cam_dir == "" || output_proj_cam_dir == "")
    return false;

  std::cout << "site_path: " << site_path << '\n'
           << "output_site_path: " << output_site_path << '\n'
           << "output_cam_dir: " << output_cam_dir << '\n'
           << "output_proj_cam_dir: " << output_proj_cam_dir << std::endl;

  bwm_video_corr_processor cp;
  cp.set_verbose(true);
  if (!cp.open_video_site(site_path, false))
    return false;

  // save the gt world points
  std::vector<vgl_homg_point_3d<double> > points2; // true gt points
  std::vector<unsigned> world_pt_indices;
  for (unsigned i = 0; i < cp.correspondences().size(); i++) {
    if (cp.correspondences()[i]->world_pt_valid()) {
      points2.emplace_back(cp.correspondences()[i]->world_pt());
      world_pt_indices.push_back(i);
    }
  }
  std::cout << "There are " << world_pt_indices.size() << " valid 3D points which will be used to compute H!\n";


  unsigned min_frame, max_frame, ncameras;
  ncameras = cp.get_ncameras(min_frame, max_frame);
  vpgl_perspective_camera<double> dummy_camera;
  std::vector<vpgl_perspective_camera<double> > cameras(ncameras, dummy_camera);

  if (initial_depth == 0) {  // the passed camera is a full perspective camera, use it directly
    std::ifstream ifs(cal_matrix_path.c_str());
    ifs >> dummy_camera;
    ifs.close();
    std::cout << "using initial camera:\n" << dummy_camera << '\n';
    for (auto & camera : cameras)
      camera = dummy_camera;
  }
  else {
    // initialize the cameras
    vnl_double_3x3 M;
    if (cal_matrix_path == "")
      return false;
    std::ifstream kis(cal_matrix_path.c_str());
    kis >> M;
    vpgl_calibration_matrix<double> K(M);
    std::cout << "initial K:\n" << M << std::endl;

    vgl_rotation_3d<double> I; // no rotation initially
    vpgl_perspective_camera<double> camera(K, vgl_homg_point_3d<double>(0.0, 0.0, initial_depth),I);
    // find the first valid 3-d point and make the camera look at that point
    bool found = false;
    for (unsigned i = 0; i < cp.correspondences().size() && !found; i++) {
      if (cp.correspondences()[i]->world_pt_valid()) {
        camera.look_at(vgl_homg_point_3d<double>(cp.correspondences()[i]->world_pt()));
        found = true;
      }
    }
    if (!found)
      camera.look_at(vgl_homg_point_3d<double>(0.0, 0.0, 0.0));
    for (auto & i : cameras)
      i = camera;
  }

  // write cams as text
  std::string cam_init_dir_txt = output_cam_dir + "_initial_txt";
  vul_file::make_directory(cam_init_dir_txt);
  if (vul_file::is_directory(cam_init_dir_txt))
    cp.write_cameras_txt(cam_init_dir_txt, cameras);

  // write cams as stream
  std::string cam_init_dir = output_cam_dir + "_initial";
  vul_file::make_directory(cam_init_dir);
  if (vul_file::is_directory(cam_init_dir)) {
    bwm_video_cam_ostream_sptr cam_ostr = new bwm_video_cam_ostream(cam_init_dir);
    if (cam_ostr && cam_ostr->is_open()) {
      for (auto & camera : cameras)
        cam_ostr->write_camera(&camera);
      cam_ostr->close();
    }
  }
  else
    return false;

  cp.open_camera_istream(cam_init_dir+"\\*");
  cp.open_camera_ostream(output_cam_dir);
  if (!cp.refine_world_pts_and_cameras())
    return false;

  // get the output world points
  std::vector<vgl_homg_point_3d<double> > points1;
  points1.reserve(world_pt_indices.size());
for (unsigned int i : world_pt_indices) {
    //for (unsigned i = 0; i < cp.correspondences().size(); i++) {
    points1.emplace_back(cp.correspondences()[i]->world_pt());
  }
  if (points1.size() != points2.size()) {
    std::cout << " Problem in number of world points to compute H!\n";
    return false;
  }

  // now find H that maps the output world points back to gt points
  std::cout << " will map point: " << points1[0] << " to point: " << points2[0] << std::endl;
  vgl_h_matrix_3d_compute_linear hmcl;
  vgl_h_matrix_3d<double> H = hmcl.compute(points1, points2);
  std::cout << "constructed homography:\n" << H;
  vgl_h_matrix_3d<double> H_inverse = H.get_inverse();

  if (H.is_euclidean())
    std::cout << "H is euclidean!\n";
  else
    std::cout << "H is NOT euclidean!\n";

  vgl_point_3d<double> mapped = H(vgl_homg_point_3d<double>(points1[0]));
  std::cout << " H maps point: " << points1[0] << " to point: " << mapped << std::endl;
  vgl_point_3d<double> mapped_inv = H_inverse(vgl_homg_point_3d<double>(points2[0]));
  std::cout << " H_inv maps point: " << points2[0] << " to point: " << mapped_inv << std::endl;

  // now correct the cameras using H
  std::vector<vpgl_perspective_camera<double> > cameras_mapped;
  std::vector<vpgl_proj_camera<double> > cameras_mapped_proj;
  for (unsigned k = 0; k < cp.cameras().size(); k++) {
#if 0
    vgl_point_3d<double> cent = cp.cameras()[k].get_camera_center();
    vgl_point_3d<double> cent_mapped(H(vgl_homg_point_3d<double>(cent)));
    std::cout << "cam center: " << cent << " is mapped to : " << cent_mapped << std::endl;
    vpgl_perspective_camera<double> new_cam(K, cent_mapped, I);
    new_cam.look_at(vgl_homg_point_3d<double>(points2[0]));
    cameras_mapped.push_back(new_cam);
#endif
    vpgl_perspective_camera<double> new_cam;

    vpgl_proj_camera<double> in_cam(static_cast<vpgl_proj_camera<double> >(cp.cameras()[k]));
    vpgl_proj_camera<double> new_proj = postmultiply(in_cam,H_inverse);
    cameras_mapped_proj.push_back(new_proj);
    if (vpgl_perspective_decomposition(new_proj.get_matrix(),new_cam))
    //if (postmultiply(cameras2[k], H, new_cam))
      cameras_mapped.push_back(new_cam);
  }

  cp.close_camera_istream();
  cp.close_camera_ostream();

  // write cams as stream
  cam_init_dir = output_proj_cam_dir;
  vul_file::make_directory(cam_init_dir);
  vul_file::delete_file_glob(cam_init_dir+"\\*");
  if (vul_file::is_directory(cam_init_dir)) {
    bwm_video_cam_ostream_sptr cam_ostr = new bwm_video_cam_ostream(cam_init_dir);
    if (cam_ostr && cam_ostr->is_open()) {
      for (auto & i : cameras_mapped_proj)
        cam_ostr->write_camera(&i);
      cam_ostr->close();
    }
  }
  else
    return false;

  cam_init_dir = output_cam_dir;
  vul_file::make_directory(cam_init_dir);
  vul_file::delete_file_glob(cam_init_dir+"\\*");
  if (vul_file::is_directory(cam_init_dir)) {
    bwm_video_cam_ostream_sptr cam_ostr = new bwm_video_cam_ostream(cam_init_dir);
    if (cam_ostr && cam_ostr->is_open()) {
      for (auto & i : cameras_mapped)
        cam_ostr->write_camera(&i);
      cam_ostr->close();
    }
  }
  else
    return false;

  // now reset the world points to the ones mapped by H
  for (unsigned i = 0; i < cp.correspondences().size(); i++) {
    vgl_point_3d<double> mapped = H(vgl_homg_point_3d<double>(cp.correspondences()[i]->world_pt()));
    cp.correspondences()[i]->set_world_pt(mapped);
  }

  cp.open_camera_istream(cam_init_dir+"\\*");
  cp.open_camera_ostream(output_cam_dir);
  if (!cp.refine_world_pts_and_cameras())
    return false;

  std::string cam_out_dir = output_cam_dir + "_txt";
  vul_file::make_directory(cam_out_dir);
  if (vul_file::is_directory(cam_out_dir))
    cp.write_cameras_txt(cam_out_dir, cp.cameras());
    //cp.write_cameras_txt(cam_out_dir, cameras_mapped);

  cp.write_video_site(output_site_path);

  return true;
}

int main(int argc, char** argv)
{
  vul_arg_info_list arglist;
  vul_arg<std::string> site_path(arglist, "-site_path",
                                "video site path", "");
  vul_arg<std::string> cal_matrix_path(arglist, "-cal_path",
                                      "calibration matrix(mat)", "");
  vul_arg<double> initial_depth(arglist, "-depth", "initial camera depth",
                                1000);
  vul_arg<std::string> out_site_path(arglist, "-out_site_path",
                                    "output_site", "");
  vul_arg<std::string> out_cam_dir(arglist, "-out_cam_dir",
                                  "output_cams", "");

  vul_arg<bool> dont_use_H(arglist, "-no_H", "don't find 3D to 3D transformation to map bundle adjusted world points back to gt world points", false);

  vul_arg<bool> localize(arglist, "-localize", "convert global coords to local using LVCS", false);
  vul_arg<double> lat(arglist, "-lat", "lvcs latitude, e.g. 39.91", 39);
  vul_arg<double> lon(arglist, "-lon", "lvcs longitude, e.g. 116.27", 116.27);

  vul_arg<std::string> out_proj_cam_dir(arglist, "-out_proj_cam_dir", "output projective cameras", "");

  vul_arg<bool> camera(arglist, "-camera", "create a perspective camera using photo overlay params in the file", false);

  arglist.parse(argc, argv, true);

  if (camera()) {  // run the process that reads camera parameters and creates perspective camera

    if (!process_camera_from_photo_overlay(site_path(), cal_matrix_path()))
      return -1;
  }
  else if (localize()) { // run the process that converts global coords to local coords given lvcs
    if (!process_conv(site_path(), lat(), lon(), out_site_path()))
      return -1;
  }
  else {
    if (dont_use_H()) {
      if (!process_no_H(site_path(), cal_matrix_path(), initial_depth(), out_site_path(), out_cam_dir()))
        return -1;
    }
    else {
      if (!process(site_path(), cal_matrix_path(), initial_depth(), out_site_path(), out_cam_dir(), out_proj_cam_dir()))
        return -1;
    }
  }
  return 0;
}
