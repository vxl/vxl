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
#include <boxm2/ocl/boxm2_opencl_processor.h>
#include <boxm2/ocl/boxm2_opencl_render_process.h>
#include <boxm2/ocl/boxm2_opencl_render_depth_process.h>
#include <boxm2/cpp/boxm2_cpp_processor.h>
#include <boxm2/cpp/boxm2_cpp_render_process.h>


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
  vil_image_view<unsigned int>* expimg = new vil_image_view<unsigned int>(ni(), nj()); 
  expimg->fill(0);
  vil_image_view_base_sptr expimg_sptr(expimg);// = new vil_image_view<unsigned int>(ni(), nj()); 
  brdb_value_sptr brdb_expimg = new brdb_value_t<vil_image_view_base_sptr>(expimg_sptr); 
  
  //create vis image buffer
  vil_image_view<float>* vis_img = new vil_image_view<float>(ni(), nj()); 
  vis_img->fill(1.0f); 
  brdb_value_sptr brdb_vis = new brdb_value_t<vil_image_view_base_sptr>(vis_img); 
  
  //----------------------------------------------------------------------------
  //--- BEGIN BOXM2 API EXAMPLE ------------------------------------------------
  //----------------------------------------------------------------------------
  //start out rendering with the CPU
  boxm2_scene_sptr scene = new boxm2_scene(scene_file()); 
  
  //get relevant blocks
  boxm2_nn_cache cache( scene->data_path(), vgl_vector_3d<int>(2,2,1) );
  
  //initialize gpu pro / manager
  boxm2_opencl_processor* gpu_pro = boxm2_opencl_processor::instance();
  gpu_pro->set_scene(scene.ptr()); 
  gpu_pro->set_cpu_cache(&cache); 
  gpu_pro->init();
  
  //pass in a vector of vis_orders to sequencing
  vcl_vector<boxm2_block_id> vis_order; 
  vis_order.push_back(boxm2_block_id(0,0,0));
  vis_order.push_back(boxm2_block_id(0,1,0)); 
  vis_order.push_back(boxm2_block_id(1,0,0)); 
  vis_order.push_back(boxm2_block_id(1,1,0)); 

  //set inputs
  vcl_vector<brdb_value_sptr> input; 
  input.push_back(brdb_cam);
  input.push_back(brdb_expimg); 
  input.push_back(brdb_vis); 
  
  //initoutput vector
  vcl_vector<brdb_value_sptr> output; 
  
  //initialize the GPU render process
  boxm2_opencl_render_process gpu_render; 
  gpu_render.init_kernel(gpu_pro->context(), gpu_pro->devices()[0]); 

  //run expected image like 10 times and get average
  int numTrials = 10;
  vul_timer t; 
  for(int i=0; i<numTrials; i++) {
    
    //execute process ////////////////////////////////////////////////////
    expimg->fill(0);
    vis_img->fill(1.0f); 
    gpu_pro->sequencing(vis_order, &gpu_render, input, output);
    gpu_pro->finish(); 
    //////////////////////////////////////////////////////////////////////
    
  }
  float time = t.all() / (float) numTrials;  
  vcl_cout<<"average render time: "<<time<<" ms"<<vcl_endl;

  //clean up
  gpu_render.clean(); 
  gpu_pro->finish(); 

  //----------------------------------------------------------------------------
  //------- END API EXAMPLE ----------------------------------------------------
  //----------------------------------------------------------------------------
  //save to disk
  vil_image_view<unsigned int>* expimg_view = static_cast<vil_image_view<unsigned int>* >(expimg_sptr.ptr()); 
  unsigned int min_val, max_val;
  vil_math_value_range( *expimg_view, min_val, max_val); 
  
  vil_image_view<vxl_byte> byte_img(ni(), nj()); 
  for(int i=0; i<ni(); i++) 
    for(int j=0; j<nj(); j++) 
      byte_img(i,j) = (*expimg_view)(i,j) & 0xFF;   //just grab the first byte (all foura r the same)
  vil_save( byte_img, img().c_str());
  
  vil_image_view<vxl_byte> vis_byte(ni(), nj()); 
  for(int i=0; i<ni(); i++) 
    for(int j=0; j<nj(); j++) 
      vis_byte(i,j) = (*vis_img)(i,j)*255;   //just grab the first byte (all foura r the same)
  vil_save( vis_byte, "vis_img.png");
  
  return 0;
}
    
