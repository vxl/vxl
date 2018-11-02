#include <iostream>
#include <fstream>
#include <string>
#include <vul/vul_file.h>
#include <vul/vul_arg.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_double_4.h>
#include <vpl/vpl.h> // vpl_unlink()
#include <vpgl/vpgl_perspective_camera.h>
#include <bpgl/algo/bpgl_camera_homographies.h>
#include <bwm/video/bwm_video_corr_processor.h>
#include <bwm/video/bwm_video_site_io.h>
#include <bwm/video/bwm_video_cam_istream.h>
#include <bwm/video/bwm_video_cam_ostream.h>

static bool process(std::string const& site_path,
                    std::string const& trans_site_path,
                    std::string const& trans_cam_dir,
                    std::string const& plane_path)
{
  if (site_path == ""||trans_site_path == "" ||trans_cam_dir == "")
    return false;
  vnl_double_4 pv;
  std::ifstream wis(plane_path.c_str());
  wis >> pv;
  vgl_plane_3d<double> world_plane(pv[0], pv[1], pv[2], pv[3]);

  bwm_video_corr_processor cp;
  cp.set_verbose(true);
  if (!cp.open_video_site(site_path, true))
    return false;
  std::string cam_path = cp.camera_path();
  std::vector<vgl_point_3d<double> > pts = cp.world_pts();
  bwm_video_cam_istream cam_istr(cam_path);
  bool open = cam_istr.is_open();
  if (open)
    cam_istr.seek_camera(0);
  else return false;
  vpgl_perspective_camera<double>* cam0 = cam_istr.current_camera();
  vgl_homg_point_3d<double> hcen = cam0->camera_center();
  vgl_homg_point_3d<double> cen(hcen);
  //Set up output directory for transformed cameras
  if (vul_file::exists(trans_cam_dir))
    vpl_rmdir(trans_cam_dir.c_str());
  if (!vul_file::make_directory_path(trans_cam_dir))
    return false;
  bwm_video_cam_ostream cam_ostr(trans_cam_dir);
  while (true) {
    vpgl_perspective_camera<double>* cam = cam_istr.current_camera();
    vpgl_perspective_camera<double> tr_cam =
      bpgl_camera_homographies::transform_camera_to_plane(*cam, world_plane);
    if (! cam_ostr.write_camera(&tr_cam))
      return false;
    if (!cam_istr.advance())
      break;
  }
  //transform the points
  std::vector<vgl_point_3d<double> > tr_pts =
    bpgl_camera_homographies::transform_points_to_plane(world_plane, cen, pts );
  cp.set_world_pts(tr_pts);
  std::string camera_opath = trans_cam_dir+"/*";
  cp.set_camera_path(camera_opath);
  cp.write_video_site(trans_site_path);
  return true;
}

int main(int argc, char** argv)
{
  vul_arg_info_list arglist;
  vul_arg<std::string> site_path(arglist, "-site_path",
                                "video site path", "");
  vul_arg<std::string> trans_site_path(arglist, "-trans_site_path",
                                      "transformed_site", "");
  vul_arg<std::string> trans_cam_dir(arglist, "-trans_cam_dir",
                                    "transformed_cams", "");

  vul_arg<std::string> plane_path(arglist, "-plane",
                                 "path to 3d plane", "");
  arglist.parse(argc, argv, true);

  if (!process(site_path(), trans_site_path(), trans_cam_dir(),
               plane_path()))
    return -1;
  else
    return 0;
}
