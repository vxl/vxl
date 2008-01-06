#include <vcl_string.h>
#include <vcl_cstdlib.h>
#include <vul/vul_file.h>
#include <vul/vul_arg.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vnl/vnl_double_3x3.h>
#include <vpl/vpl.h> // vpl_unlink()
#include <vpgl/vpgl_calibration_matrix.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <bwm/video/bwm_video_corr_processor.h>
#include <bwm/video/bwm_video_site_io.h>

static bool process(vcl_string const& site_path,
                    vcl_string const& cal_matrix_path,
                    double initial_depth,
                    unsigned corr_win_radius,
                    unsigned search_win_radius,
                    bool use_lmq)
{

  if(site_path == "")
    return false;
  
  vcl_string dir = vul_file::dirname(site_path);
  vcl_string temp_site0 = dir + "/temp0.xml";
  vcl_string temp_site1 = dir + "/temp1.xml";
  {
  bwm_video_corr_processor cp;
  cp.set_verbose(true);
  if(!cp.open_video_site(site_path, false)) return false;

  vnl_double_3x3 M;
  if(cal_matrix_path == "")
    return false;
  vcl_ifstream kis(cal_matrix_path.c_str());
  kis >> M;
  vpgl_calibration_matrix<double> K(M);

  if(!cp.initialize_world_pts_and_cameras(K, initial_depth))
    return false;
  if(!cp.write_cameras_to_stream())
    return false;
  cp.write_video_site(temp_site0);
  cp.close();

  if(!cp.open_video_site(temp_site0, true)) return false;
  if(!cp.find_missing_correspondences(corr_win_radius,
                                     search_win_radius, use_lmq))
    return false;
  cp.write_video_site(temp_site1);
  cp.close();

  if(!cp.open_video_site(temp_site1, true)) return false;
  //Set up output directory for refined cameras
  vcl_string cam_dir = dir +"/refined_cameras";
  if(vul_file::exists(cam_dir))
    vpl_rmdir(cam_dir.c_str());
  if(!vul_file::make_directory_path(cam_dir))
    return false;
  if(!cp.open_camera_ostream(cam_dir))
    return false;
  //refine the cameras and 3-d geometry
  if(!cp.refine_world_pts_and_cameras())
    return false;
  vcl_string final_site = dir + "/final_corrs_cams.xml";
  vcl_string camera_opath = cam_dir+"/*";
  cp.set_camera_path(camera_opath);
  cp.write_video_site(final_site);
  }// exit scope 
  vpl_unlink(temp_site0.c_str());
  vpl_unlink(temp_site1.c_str());
  return true;
}

int main(int argc, char** argv)
{
  vul_arg_info_list arglist;
  vul_arg<vcl_string> site_path(arglist, "-site_path",
                                 "video site path", "");
  vul_arg<vcl_string> cal_matrix_path(arglist, "-cal_path", 
                                      "calibration matrix(mat)", "");
  vul_arg<double> initial_depth(arglist, "-depth", "initial camera depth",
                                1000);
  vul_arg<int> window_radius(arglist, "win_radius", 
                             "correlation window radius", 7);
  vul_arg<int> search_radius(arglist, "search_radius", "search window radius",
                             10);
  vul_arg<bool> use_lmq(arglist, "use_lmq",
                        "Use levenberg_marquardt vs. amoeba", true);
  arglist.parse(argc, argv, true);

if(!process(site_path(), cal_matrix_path(), initial_depth(),
              static_cast<unsigned>(window_radius()), 
              static_cast<unsigned>(search_radius()), use_lmq()))
    return -1;
  return 0;
}
