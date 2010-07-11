// Main to run opencl implentation of onlineupdate
#include <bocl/bocl_cl.h>
#include <boxm/ocl/boxm_ocl_utils.h>
#include <boxm/ocl/boxm_render_ocl_scene_manager.h>
#include <boxm/ocl/boxm_ocl_render_expected.h>

#include <boxm/boxm_scene.h>
#include <boxm/algo/boxm_init_scene.h>
#include <boxm/boxm_scene_parser.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_block.h>
#include <boct/boct_tree.h>


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
  vul_arg<vcl_string> cam_file("-cam", "camera filename", "");
  vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
  vul_arg<vcl_string> img("-img", "image filename", "");
  vul_arg<unsigned> ni("-ni", "Image width", 640);
  vul_arg<unsigned> nj("-nj", "Image height", 480);
  vul_arg_parse(argc, argv);
  // load the camera;
  vcl_ifstream ifs(cam_file().c_str());
  vpgl_perspective_camera<double>* pcam =new vpgl_perspective_camera<double>;
  if (!ifs.is_open()) {
    vcl_cerr << "Failed to open file " << cam_file() << vcl_endl;
    return -1;
  }
  else {
    ifs >> *pcam;
  }

  vil_image_view<float> expimg(ni(),nj(),1);
  vil_image_view<float> maskimg(ni(),nj(),1);
 
  boxm_ocl_scene ocl_scene(scene_file());
  boxm_opencl_ocl_scene_expected(ocl_scene,pcam,expimg,maskimg);

  vil_save(expimg,img().c_str());
  return 0;
}

