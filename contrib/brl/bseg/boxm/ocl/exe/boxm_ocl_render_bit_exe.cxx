#include <bocl/bocl_cl.h>
#include <boxm/ocl/boxm_ocl_utils.h>
#include <boxm/ocl/boxm_render_bit_scene_manager.h>
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
  vcl_cout<<"RENDER BIT SCENE"<<vcl_endl;
  vul_arg<vcl_string> camfile("-cam", "camera filename", "");
  vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
  vul_arg<unsigned> ni("-ni", "Width of image", 640);
  vul_arg<unsigned> nj("-nj", "Height of image", 480);
  vul_arg<vcl_string> img("-img", "image filename", "");

  //// need this on some toolkit implementations to get the window up.
  vul_arg_parse(argc, argv);

  //create ocl_scene from xml file 
  boxm_ocl_bit_scene ocl_scene(scene_file());
  vcl_cout<<"Scene Initialized... "<<vcl_endl
          <<ocl_scene<<vcl_endl;

  //load camera from file
  vcl_ifstream ifs(camfile().c_str());
  vpgl_perspective_camera<double>* pcam =new vpgl_perspective_camera<double>;
  if (!ifs.is_open()) {
      vcl_cerr << "Failed to open file " << camfile() << vcl_endl;
      return -1;
  }
  else  {
      ifs >> *pcam;
  }
  

  vil_image_view<float> expimg(ni(),nj(),1);
  vil_image_view<float> maskimg(ni(),nj(),1);
 
  boxm_opencl_bit_scene_expected(ocl_scene,pcam,expimg,maskimg);

  vil_save(expimg,img().c_str());
  return 0;
}
