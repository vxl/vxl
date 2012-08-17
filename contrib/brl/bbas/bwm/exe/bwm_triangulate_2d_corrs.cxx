#include <bwm/bwm_observer_cam.h>
#include <bwm/bwm_observer_mgr.h>
#include <bwm/bwm_3d_corr.h>
#include <bwm/bwm_3d_corr_sptr.h>
#include <vcl_vector.h>
#include <vcl_set.h>
#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vul/vul_arg.h>
#include <vgl/vgl_point_3d.h>

#include <bwm/video/bwm_video_corr_processor.h>
#include <bwm/video/bwm_video_site_io.h>
#include <bwm/video/bwm_video_cam_istream.h>

#include <vpgl/algo/vpgl_triangulate_points.h>


// this executable takes a site file as input with a set of 2d image to image correspondences
// and triangulates the 3d point for each correspondence using the cameras of the corresponding views

int main(int argc, char** argv)
{
  //Get Inputs

  vul_arg<vcl_string> site_file   ("-site", "site file",  "");
  vul_arg<vcl_string> out_site_file   ("-out", "out site file",  "");

  if (argc != 5) {
    vcl_cout << "usage: bwm_triangulate_2d_corrs -site <site file> -out <site file>\n";
    return -1;
  }

  vul_arg_parse(argc, argv);

  bwm_video_corr_processor cp;
  cp.set_verbose(true);
  if (!cp.open_video_site(site_file().c_str(), true))
    return false;
  vcl_string cam_path = cp.camera_path();
  bwm_video_cam_istream_sptr cstr = new bwm_video_cam_istream(cam_path);

  vcl_vector<vpgl_perspective_camera<double> > cams;

  do {
    vpgl_perspective_camera<double>* cam = cstr->read_camera();
    if (cam) {
      cams.push_back(*cam);
    }
    else
      break;
  } while (true);

  vcl_cout << "found: " << cams.size() << vcl_endl;
  vcl_vector<bwm_video_corr_sptr> corrs = cp.correspondences();
  vcl_cout << "there are: " << corrs.size() << " corrs in the file\n";

  for (unsigned i = 0; i < corrs.size(); i++)
  {
    vcl_vector<vgl_point_2d<double> > points;
    vcl_vector<vpgl_perspective_camera<double> > cameras;

    bwm_video_corr_sptr corr = corrs[i];
    vcl_map<unsigned, vgl_point_2d<double> > matches = corr->matches();
    for (vcl_map<unsigned, vgl_point_2d<double> >::iterator iter = matches.begin(); iter != matches.end(); iter++) {
      points.push_back(iter->second);
      cameras.push_back(cams[iter->first]);
    }

    vgl_point_3d<double> point_3d;
    vpgl_triangulate_points::triangulate(points,cameras,point_3d);
    vcl_cout << "output 3d point:\n"
             << point_3d.x() << '\t' << point_3d.y() << '\t' << point_3d.z() << vcl_endl;
    corr->set_world_pt(point_3d);
  }
  cp.write_video_site(out_site_file().c_str());

  return 0;
}
