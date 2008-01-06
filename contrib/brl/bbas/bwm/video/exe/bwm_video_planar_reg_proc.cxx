#include <vcl_string.h>
#include <vcl_cstdlib.h>
#include <vul/vul_file.h>
#include <vul/vul_arg.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vnl/vnl_double_4.h>
#include <bwm/video/bwm_video_registration.h>
#include <vidl2/vidl2_image_list_istream.h>
#include <vidl2/vidl2_image_list_ostream.h>
#include <vidl2/vidl2_frame.h>
#include <bwm/video/bwm_video_cam_istream.h>

static bool planar_reg(vcl_string const& video_input_glob,
                       vcl_string const& camera_input_glob,
                       vcl_string const& video_output_dir,
                       vcl_string const& world_plane_path)
                       
{
  vnl_double_4 pv;

  if(world_plane_path == "")
    return false;
  vcl_ifstream wis(world_plane_path.c_str());
  wis >> pv;
  vgl_plane_3d<double> world_plane(pv[0], pv[1], pv[2], pv[3]);
  
  if(video_input_glob == ""||camera_input_glob == ""||
     video_output_dir == "")
    return false;
  
  if(!vul_file::exists(video_output_dir))
    if(!vul_file::make_directory_path(video_output_dir))
      return false;

  vidl2_istream_sptr video_istr =
    new vidl2_image_list_istream(video_input_glob);
  bwm_video_cam_istream_sptr cam_istr = 
    new bwm_video_cam_istream(camera_input_glob);

  vidl2_ostream_sptr video_ostr =
    new vidl2_image_list_ostream(video_output_dir);

  if(!(cam_istr&&cam_istr->seek_camera(0)))
    return false;
  if(!(video_istr&&video_istr->seek_frame(0)))
    return false;

  vidl2_frame_sptr f0 = video_istr->current_frame();
  unsigned input_ni = f0->ni(), input_nj = f0->nj();
  unsigned output_ni = 0, output_nj = 0;    

  vsol_box_2d_sptr bounds;
  double sample_distance = 0;
   if(!bwm_video_registration::output_frame_bounds_planar(cam_istr,
                                                world_plane,
                                                input_ni, input_nj,
                                                bounds,
                                                sample_distance))
												return false;

  if(!bwm_video_registration::register_image_stream_planar(video_istr,
                                                       cam_istr,
                                                       world_plane,
                                                       bounds,
                                                       sample_distance,
                                                       video_ostr))
													   return false;
  return true;
}

int main(int argc, char** argv)
{
  vul_arg_info_list arglist;
  vul_arg<vcl_string> video_input_glob(arglist, "-video_input_glob",
                                       "video input file glob", "");
  vul_arg<vcl_string> camera_input_glob(arglist, "-camera_input_glob",
                                       "camera input file glob", "");
  vul_arg<vcl_string> video_output_dir(arglist, "-video_output_dir",
                                       "video output file directory", "");
  vul_arg<vcl_string> world_plane_path(arglist, "-world_plane",
                                       "world plane (4 element vector)", "");
  arglist.parse(argc, argv, true);

  if(!planar_reg(video_input_glob(), camera_input_glob(),
                 video_output_dir(), world_plane_path()))
    return -1;
  return 0;
}
