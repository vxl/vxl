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

//brdb stuff
#include <brdb/brdb_value.h>

int main(int argc,  char** argv)
{
  vcl_cout<<"RENDER BIT SCENE"<<vcl_endl;
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
  
  //create scene from xml file 
  boxm2_scene scene(scene_file());
  vcl_cout<<"Scene Initialized... "<<vcl_endl
          <<scene<<vcl_endl;

  ////get relevant blocks id 0,0,0 and push em in a vector
  boxm2_block_id id(0,0,0); 
  boxm2_dumb_cache dcache(scene.data_path());
  boxm2_data<BOXM2_ALPHA>* alph = dcache.get_data<BOXM2_ALPHA>(id);
  boxm2_data<BOXM2_MOG3_GREY>* mog = dcache.get_data<BOXM2_MOG3_GREY>(id);
  
  
  boxm2_block_sptr blk = dcache.get_block(id); 
  brdb_value_sptr brdb_block = new brdb_value_t<boxm2_block_sptr>(blk);
  

  vcl_vector<brdb_value_sptr> input; 
  input.push_back(brdb_block); 
  ////input.push_back(alph);
  ////input.push_back(mog); 
  
  //initoutput vector
  vcl_vector<brdb_value_sptr> output;  
  
  //initialize a GPU processor
  boxm2_opencl_processor gpu_pro; 
  gpu_pro.init();
  
  //initialize the GPU render process
  boxm2_opencl_render_process gpu_render; 
  gpu_pro.run(&gpu_render, input, output); 
  gpu_pro.finish(); 

  //grab the output from teh gpu_pro class

  //save to disk
  vil_image_view<vxl_byte> expimg(ni(), nj()); 
  vil_save(expimg,img().c_str());
  return 0;
}
