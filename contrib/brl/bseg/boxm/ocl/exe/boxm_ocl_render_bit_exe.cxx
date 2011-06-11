#include <bocl/bocl_cl.h>
#include <boxm/ocl/boxm_ocl_utils.h>
#include <boxm/ocl/boxm_render_bit_scene_manager.h>
#include <boxm/ocl/boxm_ocl_render_expected.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view.h>

#include <vul/vul_arg.h>

int main(int argc,  char** argv)
{
  vcl_cout<<"RENDER BIT SCENE"<<vcl_endl;
  vul_arg<vcl_string> camfile("-cam", "camera filename", "");
  vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
  vul_arg<unsigned> ni("-ni", "Width of output image", 640);
  vul_arg<unsigned> nj("-nj", "Height of output image", 480);
  vul_arg<vcl_string> img("-img", "image filename", "out.png");
  vul_arg<bool> use_cpu("-cpu", "Specifies to use CPU", false);

  //// need this on some toolkit implementations to get the window up.
  vul_arg_parse(argc, argv);

  //create ocl_scene from xml file
  boxm_ocl_bit_scene ocl_scene(scene_file());
  vcl_cout<<"Scene Initialized...\n" <<ocl_scene<<vcl_endl;

  //load camera from file
  vcl_ifstream ifs(camfile().c_str());
  vpgl_perspective_camera<double>* pcam =new vpgl_perspective_camera<double>;
  if (!ifs.is_open()) {
    vcl_cerr << "Failed to open file " << camfile() << '\n';
    return -1;
  }
  else  {
    ifs >> *pcam;
  }

  //image to write out
  vil_image_view<float> expimg(ni(),nj(),1);

  // render the image using the opencl raytrace manager
  boxm_render_bit_scene_manager* ray_mgr = boxm_render_bit_scene_manager::instance();
#if 0 // boxm_update_bit_scene_manager has no method initialize_context() ?!?!? - FIXME
  if (use_cpu()) {
    vcl_cout<<"USING CPU"<<vcl_endl;
    ray_mgr->initialize_context(&ray_mgr->cpus()[0]);
  }
#endif

  int bundle_dim=8;
  ray_mgr->set_bundle_ni(bundle_dim);
  ray_mgr->set_bundle_nj(bundle_dim);
  ray_mgr->init_ray_trace(&ocl_scene, pcam, expimg, false);
  ray_mgr->run_scene();

  // extract expected image and mask from OpenCL output data
  cl_float* results = ray_mgr->output_image();
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
  vil_save(outimg, img().c_str());
  return 0;
}
