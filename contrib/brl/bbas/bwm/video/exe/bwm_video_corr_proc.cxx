#include <iostream>
#include <fstream>
#include <string>
#include <vul/vul_file.h>
#include <vul/vul_arg.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_double_3x3.h>
#include <vpl/vpl.h> // vpl_unlink()
#include <vpgl/vpgl_calibration_matrix.h>
#include <bwm/video/bwm_video_corr_processor.h>
#include <bwm/video/bwm_video_site_io.h>

static bool process(std::string const& site_path,
                    std::string const& cal_matrix_path,
                    double initial_depth,
                    unsigned search_win_radius,
                    bool use_lmq)
{
  if (site_path == "")
    return false;

  std::string dir = vul_file::dirname(site_path);
  std::string temp_site0 = dir + "/temp0.xml";
  std::string temp_site1 = dir + "/temp1.xml";
  std::string final_site = dir + "/final_corrs_cams.xml";
  bwm_video_corr_processor cp;
  cp.set_verbose(true);

  if (!cp.open_video_site(site_path, false))
    return false;

  vnl_double_3x3 M;
  if (cal_matrix_path == "")
    return false;
  std::ifstream kis(cal_matrix_path.c_str());
  kis >> M;
  vpgl_calibration_matrix<double> K(M);

  if (!cp.initialize_world_pts_and_cameras(K, initial_depth))
    return false;
  if (!cp.write_cameras_to_stream())
    return false;
  cp.write_video_site(temp_site0);
  cp.close();
#if 0
  if (!cp.open_video_site(temp_site0, true))
    return false;
  if (!cp.find_missing_correspondences(corr_win_radius,
                                       search_win_radius, use_lmq))
    return false;
  cp.write_video_site(temp_site1);
  cp.close();


  if (!cp.open_video_site(temp_site1, true))
    return false;
  //Set up output directory for refined cameras
  std::string cam_dir = dir +"/refined_cameras";
  if (vul_file::exists(cam_dir))
    vpl_rmdir(cam_dir.c_str());
  if (!vul_file::make_directory_path(cam_dir))
    return false;
  if (!cp.open_camera_ostream(cam_dir))
    return false;
  //refine the cameras and 3-d geometry
  if (!cp.refine_world_pts_and_cameras())
    return false;
  std::string camera_opath = cam_dir+"/*";
  cp.set_camera_path(camera_opath);
  cp.write_video_site(final_site);
  vpl_unlink(temp_site0.c_str());
  vpl_unlink(temp_site1.c_str());

#endif
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
  vul_arg<int> window_radius(arglist, "win_radius",
                             "correlation window radius", 2);
  vul_arg<int> search_radius(arglist, "search_radius", "search window radius",
                             4);
  vul_arg<bool> use_lmq(arglist, "use_lmq",
                        "Use levenberg_marquardt vs. amoeba", true);
  arglist.parse(argc, argv, true);

if (!process(site_path(), cal_matrix_path(), initial_depth(),
             static_cast<unsigned>(search_radius()), use_lmq()))
    return -1;
  return 0;
}
