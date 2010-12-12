#include <bocl/bocl_cl.h>
#include <boxm/ocl/boxm_ocl_utils.h>
#include <boxm/ocl/boxm_update_bit_scene_manager.h>

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
  vcl_cout<<"REFINE BIT SCENE BY BUFFER LENGTH"<<vcl_endl;
  vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
  vul_arg<vcl_string> camfile("-cam", "camera filename", "");
  vul_arg<unsigned> ni("-ni", "Width of image", 640);
  vul_arg<unsigned> nj("-nj", "Height of image", 480);

  //// need this on some toolkit implementations to get the window up.
  vul_arg_parse(argc, argv);

  //1.  create ocl_scene from xml file
  boxm_ocl_bit_scene ocl_scene(scene_file());
  vcl_cout<<"Scene Initialized...\n"
          <<ocl_scene<<vcl_endl;

  //2.  build the camera from file
  vcl_ifstream ifs(camfile().c_str());
  vpgl_perspective_camera<double>* pcam = new vpgl_perspective_camera<double>;
  if (!ifs.is_open()) {
    vcl_cerr << "Failed to open file " << camfile() << '\n';
    return -1;
  }
  ifs >> *pcam;

  vil_image_view<float> expimg(ni(),nj(),1);
  vil_image_view<float> maskimg(ni(),nj(),1);

  //4.  initialize update manager
  boxm_update_bit_scene_manager* updt_mgr
    = boxm_update_bit_scene_manager::instance();

  vil_image_view<float> expected(ni(),nj());
  int bundle_dim = 8;
  updt_mgr->set_bundle_ni(bundle_dim);
  updt_mgr->set_bundle_nj(bundle_dim);
  updt_mgr->init_scene(&ocl_scene, pcam, expimg, 0.3f);
  if (!updt_mgr->setup_norm_data(true, 0.5f, 0.25f))
    return -1;
  updt_mgr->setup_online_processing();

  //5.  write cam, in_image and call update
  updt_mgr->set_input_image(expimg);
  updt_mgr->write_image_buffer();
  updt_mgr->set_persp_camera(pcam);
  updt_mgr->write_persp_camera_buffers();
  updt_mgr->refine();

  vcl_cout<<"Refined scene...\n"<<ocl_scene<<vcl_endl;

  //7.  cleanup
  updt_mgr->uninit_scene();

  return 0;
}
