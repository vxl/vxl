#include <bocl/bocl_cl.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_math.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

//executable args
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>

//boxm2 scene stuff
#include <boxm2/io/boxm2_nn_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/basic/boxm2_block_id.h>
#include <boxm2/cpp/boxm2_cpp_processor.h>
#include <boxm2/cpp/pro/boxm2_cpp_render_process.h>


//brdb stuff
#include <brdb/brdb_value.h>

int main(int argc,  char** argv)
{
  vcl_cout<<"Boxm2 CPP Render "<<vcl_endl;
  vul_arg<vcl_string> camdir("-cam", "camera filename", "");
  vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
  vul_arg<vcl_string> imgname("-outimg", "output image", "");
  vul_arg<unsigned int> ni("-ni", "width", 640);
  vul_arg<unsigned int> nj("-nj", "height", 480);

  // need this on some toolkit implementations to get the window up.
  vul_arg_parse(argc, argv);

  boxm2_scene_sptr scene = new boxm2_scene(scene_file());
  brdb_value_sptr brdb_scene_sptr = new brdb_value_t<boxm2_scene_sptr>(scene);
  // get relevant blocks
  boxm2_nn_cache cache( scene.ptr() );
  boxm2_cpp_processor cpp_pro;
  cpp_pro.init();
  cpp_pro.set_scene(scene.ptr());

  boxm2_cpp_render_process cpp_render;
  cpp_render.init();
  cpp_render.set_cache(&cache);

  //load camera from file
  vcl_ifstream ifs(camdir().c_str());
  vpgl_perspective_camera<double>* pcam =new vpgl_perspective_camera<double>;
  if (!ifs.is_open()) {
      vcl_cerr << "Failed to open file " << camdir() << '\n';
      return -1;
  }
  else  {
      ifs >> *pcam;
  }
  vpgl_camera_double_sptr cam = pcam;
  brdb_value_sptr brdb_cam = new brdb_value_t<vpgl_camera_double_sptr>(cam);

  //load image from file
  vil_image_view<float>* floatimg = new vil_image_view<float>(ni(),nj(), 1);


  //create input image buffer
  brdb_value_sptr brdb_inimg = new brdb_value_t<vil_image_view_base_sptr>(floatimg);

  vcl_vector<brdb_value_sptr> input;
  input.push_back(brdb_scene_sptr);
  input.push_back(brdb_cam);
  input.push_back(brdb_inimg);

  //init output vector
  vcl_vector<brdb_value_sptr> output;

  vul_timer t;t.mark();
  cpp_pro.run(&cpp_render, input, output);
  vcl_cout<<"Time taken is :" <<t.all()<<vcl_endl;

  cpp_pro.finish();
  vil_image_view<vxl_byte> img_byte(floatimg->ni(),floatimg->nj());
  vil_convert_stretch_range_limited( *floatimg, img_byte, 0.0f, 1.0f,vxl_byte(0), vxl_byte(255));

  vil_save(img_byte,imgname().c_str());
  return 0;
}
