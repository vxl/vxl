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
#include <boxm2/io/boxm2_dumb_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block_id.h>
#include <boxm2/boxm2_opencl_processor.h>
#include <boxm2/boxm2_opencl_render_process.h>
#include <boxm2/boxm2_opencl_render_depth_process.h>
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
  vil_image_view_base_sptr expimg = new vil_image_view<float>(ni(), nj()); 
  brdb_value_sptr brdb_expimg = new brdb_value_t<vil_image_view_base_sptr>(expimg); 
  
  //----------------------------------------------------------------------------
  //--- BEGIN BOXM2 API EXAMPLE ------------------------------------------------
  //----------------------------------------------------------------------------
  
  //start out rendering with the CPU
  boxm2_scene_sptr scene = new boxm2_scene(scene_file()); 
  
  //get relevant blocks
  boxm2_block_id id(0,0,0); 
  boxm2_dumb_cache dcache(scene->data_path());
  boxm2_block_sptr blk      = dcache.get_block(id); 
  boxm2_data_base_sptr alph = dcache.get_data<BOXM2_ALPHA>(id); 
  boxm2_data_base_sptr mog  = dcache.get_data<BOXM2_MOG3_GREY>(id); 
  
  //set inputs
  vcl_vector<brdb_value_sptr> input; 
  input.push_back(brdb_cam);
  input.push_back(brdb_expimg); 
  
  //initoutput vector
  vcl_vector<brdb_value_sptr> output; 

  //initialize gpu pro / manager
  boxm2_opencl_processor* gpu_pro = boxm2_opencl_processor::instance();
  gpu_pro->set_data(scene, blk, alph, mog);
  
  //////initialize the GPU render process
  boxm2_opencl_render_process gpu_render; 
  gpu_pro->run(&gpu_render, input, output); 
  gpu_pro->finish(); 



  //----------------------------------------------------------------------------
  //------- END API EXAMPLE ----------------------------------------------------
  //----------------------------------------------------------------------------
  ///save to disk
  vil_image_view<float>* expimg_view = static_cast<vil_image_view<float>* >(expimg.ptr()); 
  float min_val, max_val;
  vil_math_value_range( *expimg_view, min_val, max_val); 
  
  vil_image_view<vxl_byte> byte_img(ni(), nj()); 
  for(int i=0; i<ni(); i++) {
    for(int j=0; j<nj(); j++) {
      float norm = ((*expimg_view)(i,j) - min_val) / (max_val-min_val); 
      byte_img(i,j) = (vxl_byte) (255 * norm); 
    }
  }
  vil_save( byte_img, img().c_str());
  
  
  //render depth image
  vcl_vector<brdb_value_sptr> input2;
  input2.push_back(brdb_cam);
  input2.push_back(brdb_expimg); 
  
  
  //////initialize the GPU render process
  //boxm2_opencl_render_depth_process gpu_depth; 
  gpu_pro->run(&gpu_render, input2, output); 
  gpu_pro->finish(); 
  
  ///save to disk
  expimg_view = static_cast<vil_image_view<float>* >(expimg.ptr()); 
  vil_math_value_range( *expimg_view, min_val, max_val); 
  for(int i=0; i<ni(); i++) {
    for(int j=0; j<nj(); j++) {
      float norm = ((*expimg_view)(i,j) - min_val) / (max_val-min_val); 
      byte_img(i,j) = (vxl_byte) (255 * norm); 
    }
  }
  vcl_string img2 = "depth_" + img(); 
  vil_save( byte_img, img2.c_str());
  
  
  return 0;
}
