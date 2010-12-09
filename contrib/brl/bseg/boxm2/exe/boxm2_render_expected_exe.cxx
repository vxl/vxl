#include <bocl/bocl_cl.h>

#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

//executable args
#include <vul/vul_arg.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_timer.h>

//boxm2 scene stuff
#include <boxm2/io/boxm2_dumb_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block_id.h>
#include <boxm2/boxm2_opencl_processor.h>
#include <boxm2/boxm2_opencl_render_process.h>
#include <boxm2/boxm2_cpp_processor.h>
#include <boxm2/boxm2_cpp_render_process.h>


//brdb stuff
#include <brdb/brdb_value.h>

int main(int argc,  char** argv)
{
  vcl_cout<<"Boxm2 RENDER BIT SCENE"<<vcl_endl;
  vul_arg<vcl_string> camfile("-cam", "camera filename", "");
  vul_arg<vcl_string> scene_file("-scene", "scene filename", "");
  vul_arg<unsigned> ni("-ni", "Width of image", 640);
  vul_arg<unsigned> nj("-nj", "Height of image", 480);
  vul_arg<vcl_string> img("-img", "output image filename", "expected_img.png");

  // need this on some toolkit implementations to get the window up.
  vul_arg_parse(argc, argv);

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
  vpgl_camera_double_sptr cam = pcam; 
  brdb_value_sptr brdb_cam = new brdb_value_t<vpgl_camera_double_sptr>(cam); 
  
  //create output image buffer
  vil_image_view_base_sptr expimg = new vil_image_view<vxl_byte>(ni(), nj()); 
  brdb_value_sptr brdb_expimg = new brdb_value_t<vil_image_view_base_sptr>(expimg); 
  
  //----------------------------------------------------------------------------
  //--- BEGIN BOXM2 API EXAMPLE ------------------------------------------------
  //----------------------------------------------------------------------------
  
  //start out rendering with the CPU
  boxm2_scene_sptr scene = new boxm2_scene(scene_file()); 
  brdb_value_sptr brdb_scene = new brdb_value_t<boxm2_scene_sptr>(scene); 
  
  //get relevant blocks
  boxm2_block_id id(0,0,0); 
  boxm2_dumb_cache dcache(scene->data_path());
  boxm2_block_sptr blk = dcache.get_block(id); 
  boxm2_data_base_sptr alph = dcache.get_data<BOXM2_ALPHA>(id); 
  boxm2_data_base_sptr mog  = dcache.get_data<BOXM2_MOG3_GREY>(id); 
  brdb_value_sptr brdb_block = new brdb_value_t<boxm2_block_sptr>(blk);
  brdb_value_sptr brdb_alph  = new brdb_value_t<boxm2_data_base_sptr>(alph);
  brdb_value_sptr brdb_mog   = new brdb_value_t<boxm2_data_base_sptr>(mog); 
  
  //set inputs
  vcl_vector<brdb_value_sptr> input; 
  input.push_back(brdb_scene); 
  input.push_back(brdb_block); 
  input.push_back(brdb_alph);
  input.push_back(brdb_mog); 
  input.push_back(brdb_cam);
  input.push_back(brdb_expimg); 
  
  //initoutput vector
  vcl_vector<brdb_value_sptr> output; 

  //boxm2_cpp_processor cpp_pro;
  //cpp_pro.init(); 
  //boxm2_cpp_render_process cpp_render; 
  //cpp_pro.run(&cpp_render, input, output); 
  //cpp_pro.finish(); 
  

  ////initialize a GPU processor
    //4.  initialize update manager
  boxm2_opencl_processor* gpu_pro = boxm2_opencl_processor::instance();
  
  ////initialize the GPU render process
  boxm2_opencl_render_process gpu_render; 
  gpu_pro->run(&gpu_render, input, output); 
  gpu_pro->finish(); 

  ////grab the output from teh gpu_pro class

  ////save to disk
  //vil_image_view<vxl_byte> expimg(ni(), nj()); 
  //vil_save(expimg,img().c_str());
  return 0;
}
