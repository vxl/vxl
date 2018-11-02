#include <iostream>
#include <fstream>
#include <string>
#include <vul/vul_file.h>
#include <vul/vul_arg.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_double_4.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <bwm/video/bwm_video_registration.h>
#include <vidl/vidl_image_list_istream.h>
#include <vidl/vidl_image_list_ostream.h>
#include <vidl/vidl_frame.h>
#include <bwm/video/bwm_video_cam_istream.h>

static bool planar_reg(std::string const& video_input_glob,
                       std::string const& camera_input_glob,
                       std::string const& video_output_dir,
                       std::string const& world_plane_path,
                       std::string const& homography_path,
                       std::string const& preserve_float)
{
  vnl_double_4 pv;
  unsigned skip_frames = 0;
  if (world_plane_path == "")
    return false;
  std::ifstream wis(world_plane_path.c_str());
  wis >> pv;
  vgl_plane_3d<double> world_plane(pv[0], pv[1], pv[2], pv[3]);

  if (video_input_glob == ""||
      camera_input_glob == ""||
      video_output_dir == "")
    return false;
  bool pre_flt = true;
  if (preserve_float == "false")
    pre_flt = false;
  if (!vul_file::exists(video_output_dir))
    if (!vul_file::make_directory_path(video_output_dir))
      return false;

  vidl_istream_sptr video_istr =
    new vidl_image_list_istream(video_input_glob);
  bwm_video_cam_istream_sptr cam_istr =
    new bwm_video_cam_istream(camera_input_glob);

  vidl_ostream_sptr video_ostr =
    new vidl_image_list_ostream(video_output_dir);

  if (!(cam_istr&&cam_istr->seek_camera(0)))
    return false;
  if (!(video_istr&&video_istr->seek_frame(0)))
    return false;

  vidl_frame_sptr f0 = video_istr->current_frame();
  unsigned input_ni = f0->ni(), input_nj = f0->nj();

#if 0
  vgl_point_3d<double> world_point(69.014420,-3.638463, 13.361106);
  if (!bwm_video_registration::measure_stability(cam_istr, world_plane,
                                                 world_point))
     return false;
#endif
  vsol_box_2d_sptr bounds;
  double sample_distance = 0;

  if (!bwm_video_registration::output_frame_bounds_planar(cam_istr,
                                                          world_plane,
                                                          input_ni, input_nj,
                                                          bounds,
                                                          sample_distance))
    return false;

  if (video_istr)
    if (!bwm_video_registration::register_image_stream_planar(video_istr,
                                                              cam_istr,
                                                              world_plane,
                                                              bounds,
                                                              sample_distance,
                                                              video_ostr,
                                                              skip_frames,
                                                              pre_flt))
      return false;

  if (homography_path != "")
    if (!bwm_video_registration::register_planar_homographies(cam_istr,
                                                            world_plane,
                                                            bounds,
                                                            sample_distance,
                                                            homography_path,
                                                             skip_frames))
      return false;
  return true;
}

int main(int argc, char** argv)
{
  vul_arg_info_list arglist;
  vul_arg<std::string> video_input_glob(arglist, "-video_input_glob",
                                       "video input file glob", "");
  vul_arg<std::string> camera_input_glob(arglist, "-camera_input_glob",
                                        "camera input file glob", "");
  vul_arg<std::string> video_output_dir(arglist, "-video_output_dir",
                                       "video output file directory", "");
  vul_arg<std::string> world_plane_path(arglist, "-world_plane",
                                       "world plane (4 element vector)", "");
  vul_arg<std::string> homography_path(arglist, "-homg_dir", "homography dir", "");
  vul_arg<std::string> preserve_float(arglist, "-preserve_float", "keep float format", "");
  arglist.parse(argc, argv, true);

  if (!planar_reg(video_input_glob(), camera_input_glob(),
                  video_output_dir(), world_plane_path(),
                  homography_path(), preserve_float()))
    return -1;
  return 0;
}
