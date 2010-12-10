#include <boxm2/boxm2_opencl_processor.h>
#include <boxm2/boxm2_opencl_process_base.h>

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

bool boxm2_opencl_processor::init()
{
  //need to initialize an instance of the manager here - setup the device/context
  return true;
}

// NEED TO FIGURE OUT HOW TO differentiate between SCENE/BLOCK/DATA arguments and Camera/Image arguments
// the scene level stuff needs to live on the processor, other
bool boxm2_opencl_processor::run(boxm2_process * process, vcl_vector<brdb_value_sptr> & input, vcl_vector<brdb_value_sptr> & output)
{
  boxm2_opencl_process_base* pro = (boxm2_opencl_process_base*) process;

  //1. initialize the process (pass it a context and a device so it can compile the kernel)
  pro->init_kernel(this->context(), this->devices()[0]);

  //2. execute kernel
  pro->execute(input, output);
#if 0
  //3. get the blocks/camera/img etc from the input vector
  int i = 0;
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
#endif // 0
  return true;
}

bool boxm2_opencl_processor::finish()
{
  return true;
}
