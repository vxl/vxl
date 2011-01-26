#include <bocl/bocl_cl.h>
#include <boxm/ocl/boxm_ocl_utils.h>
#include <boxm/ocl/boxm_update_bit_scene_manager.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>


int main(int argc,  char** argv)
{
  vcl_cout<<"UPDATE BIT SCENE"<<vcl_endl;
  vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
  vul_arg<vcl_string> camfile("-cam", "camera filename", "");
  vul_arg<vcl_string> img("-img", "image filename", "");
  vul_arg<unsigned> ni("-ni", "Width of image", 640);
  vul_arg<unsigned> nj("-nj", "Height of image", 480);
  vul_arg<bool> use_cpu("-cpu", "Specifies to use CPU", false); 
  vul_arg<bool> use_atomic("-atom", "Specifies to use all atomics, no opt", false); 

  //// need this on some toolkit implementations to get the window up.
  vul_arg_parse(argc, argv);
  
  //1.  create ocl_scene from xml file 
  boxm_ocl_bit_scene ocl_scene(scene_file());
  vcl_cout<<"Scene Initialized... "<<vcl_endl
          <<ocl_scene<<vcl_endl;

  //2.  build the camera from file
  vcl_ifstream ifs(camfile().c_str());
  vpgl_perspective_camera<double>* pcam = new vpgl_perspective_camera<double>;
  if (!ifs.is_open()) {
      vcl_cerr << "Failed to open file " << camfile() << vcl_endl;
      return -1;
  }
  ifs >> *pcam;
  
  //3.  load image from file
  vil_image_view_base_sptr loaded_image = vil_load(img().c_str());
  vil_image_view<float> floatimg(loaded_image->ni(), loaded_image->nj(), 1);
  if (vil_image_view<vxl_byte> *img_byte = dynamic_cast<vil_image_view<vxl_byte>*>(loaded_image.ptr()))
  {
    vil_convert_stretch_range_limited(*img_byte ,floatimg, vxl_byte(0), vxl_byte(255), 0.0f, 1.0f);
  }
  else {
    vcl_cerr << "Failed to load image " << img() << vcl_endl;
    return -1;
  }
 
  //4.  initialize update manager
  boxm_update_bit_scene_manager* updt_mgr 
    = boxm_update_bit_scene_manager::instance();
  updt_mgr->set_use_gl(false); 
  if( use_cpu() ) {
    vcl_cout<<"USING CPU For Update"<<vcl_endl;
    updt_mgr->initialize_context(&updt_mgr->cpus()[0]); 
  }

  vil_image_view<float> expected(ni(),nj());
  int bundle_dim = 8;
  updt_mgr->set_bundle_ni(bundle_dim);
  updt_mgr->set_bundle_nj(bundle_dim);
  updt_mgr->init_scene(&ocl_scene, pcam, expected, 0.3f, false, use_atomic());
  if (!updt_mgr->setup_norm_data(true, 0.5f, 0.25f))
    return -1;
  updt_mgr->setup_online_processing();

  //5.  write cam, in_image and call update 
  updt_mgr->set_input_image(floatimg);
  updt_mgr->write_image_buffer();
  updt_mgr->set_persp_camera(pcam);
  updt_mgr->write_persp_camera_buffers();
  updt_mgr->update();

  //6. render for proof
  updt_mgr->rendering();
  updt_mgr->read_output_image();
  cl_float* results = updt_mgr->output_image();
  if (!results) {
    vcl_cerr << "Error : boxm_opencl_render_expected : ray_mgr->ray_results() returned NULL\n";
    return 0;
  }
  
  //convert to VXL_BYTE for PNG IMG
  //results is a float 4 image, so skip 4
  vil_image_view<vxl_byte> outimg(ni(),nj());
  cl_float *results_p = results;
  for (unsigned j = 0; j<nj(); ++j) {
    for (unsigned i = 0; i<ni(); ++i) {
      float pixel = *results_p; 
      outimg(i,j) = (vxl_byte) (pixel * 255 ); // expected intensity
      results_p += 4; 
    }
  }
  vil_save(outimg, "update.png");

  //7.  cleanup
  updt_mgr->uninit_scene();
  
  return 0;
}
