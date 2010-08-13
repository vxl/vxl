// Main to run opencl implentation of onlineupdate
#include <bocl/bocl_cl.h>
#include <boxm/ocl/boxm_ocl_utils.h>
#include <boxm/ocl/boxm_render_ocl_scene_manager.h>
#include <boxm/ocl/boxm_ocl_render_expected.h>


#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>

#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>






int main(int argc,  char** argv)
{
  vcl_cout<<"RENDER "<<vcl_endl;
  vul_arg<vcl_string> cam_dir("-camdir", "camera filename", "");
  vul_arg<vcl_string> img_dir("-imgdir", "image filename", "");
  vul_arg<vcl_string> out_dir("-outdir", "Output directory", "");
  vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
  vul_arg_parse(argc, argv);

  vcl_string camglob=cam_dir()+"/*.txt";
  vcl_string imgglob=img_dir()+"/*.tiff";
  vul_file_iterator file_it(camglob.c_str());
  vul_file_iterator img_file_it(imgglob.c_str());
  vcl_vector<vcl_string> cam_files;
  vcl_vector<vcl_string> img_files;
  while (file_it && img_file_it) {
      vcl_string camName(file_it());
      vcl_string imgName(img_file_it());
      cam_files.push_back(camName);
      img_files.push_back(imgName);
      ++file_it; ++img_file_it;
  }
  vcl_sort(cam_files.begin(), cam_files.end());
  vcl_sort(img_files.begin(), img_files.end());
  if (cam_files.size() != img_files.size()) {
      vcl_cerr<<"Image files and cam files not one to one\n";
      return -1;
  } 
  boxm_ocl_scene ocl_scene(scene_file());
  boxm_ocl_scene_rerender(ocl_scene,cam_files,img_files,out_dir());
  return 0;
}

