#include <boxm2/boxm2_opencl_render_process.h>

//boxm2 data structures
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>

//cameras/images
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>

//: probably don't need this... 
bool boxm2_opencl_render_process::init(){}

bool boxm2_opencl_render_process::init_kernel(cl_context& context, 
                                              cl_device_id& device)
{
  //gather all render sources... seems like a lot for rendering...
  vcl_vector<vcl_string> src_paths; 
  vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/cl/";
  src_paths.push_back(source_dir + "scene_info.cl"); 
  src_paths.push_back(source_dir + "cell_utils.cl"); 
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl"); 
  src_paths.push_back(source_dir + "backproject.cl"); 
  src_paths.push_back(source_dir + "statistics_library_functions.cl"); 
  src_paths.push_back(source_dir + "expected_functor.cl"); 
  src_paths.push_back(source_dir + "ray_bundle_library_opt.cl"); 
  src_paths.push_back(source_dir + "bit/cast_ray_bit.cl"); 
  src_paths.push_back(source_dir + "bit/render_bit_scene.cl"); 

  vcl_string options = "-D INTENSITY ";
  options += "-D NVIDIA ";
  options += "-D RENDER ";
  
  return render_kernel_.create_kernel(  &context, 
                                        &device, 
                                        src_paths, 
                                        "render_bit_scene", 
                                        options, 
                                        "boxm2 opencl render");
}


//: NEED TO FIGURE OUT HOW TO differentiate between SCENE/BLOCK/DATA arguments and Camera/Image arguments
// the scene level stuff needs to live on the processer, other 
bool boxm2_opencl_render_process::execute(vcl_vector<brdb_value_sptr>& input, vcl_vector<brdb_value_sptr>& output)
{
  vcl_cout<<"CPP RENDER!!"<<vcl_endl;
  
  int i = 0;
  //get the blocks/camera/img etc from the input vector
  //scene
  brdb_value_t<boxm2_scene_sptr>* brdb_scene = static_cast<brdb_value_t<boxm2_scene_sptr>* >( input[i++].ptr() ); 
  boxm2_scene_sptr scene = brdb_scene->value(); 
  
  //tree structure
  brdb_value_t<boxm2_block_sptr>* brdb_blk = static_cast<brdb_value_t<boxm2_block_sptr>* >( input[i++].ptr() ); 
  boxm2_block_sptr blk = brdb_blk->value(); 
  
  //alpha
  brdb_value_t<boxm2_data_base_sptr>* brdb_alpha = static_cast<brdb_value_t<boxm2_data_base_sptr>* >( input[i++].ptr() ); 
  boxm2_data_base_sptr alpha = brdb_alpha->value(); 
  
  //mog
  brdb_value_t<boxm2_data_base_sptr>* brdb_mog = static_cast<brdb_value_t<boxm2_data_base_sptr>* >( input[i++].ptr() ); 
  boxm2_data_base_sptr mog = brdb_mog->value(); 
  
  //camera
  brdb_value_t<vpgl_camera_double_sptr>* brdb_cam = static_cast<brdb_value_t<vpgl_camera_double_sptr>* >( input[i++].ptr() ); 
  vpgl_camera_double_sptr cam = brdb_cam->value(); 
  
  //exp image buffer
  brdb_value_t<vil_image_view_base_sptr>* brdb_expimg = static_cast<brdb_value_t<vil_image_view_base_sptr>* >( input[i++].ptr() ); 
  vil_image_view_base_sptr expimg = brdb_expimg->value(); 

}

