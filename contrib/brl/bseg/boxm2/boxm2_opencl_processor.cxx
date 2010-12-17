#include <boxm2/boxm2_opencl_processor.h>
#include <boxm2/boxm2_opencl_process_base.h>

//boxm2 data structures
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/basic/boxm2_array_3d.h>

//cameras/images
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_vector_fixed.h>

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
  //0. cast the process to a boxm2_opencl_process
  boxm2_opencl_process_base* pro = (boxm2_opencl_process_base*) process;

  //1. execute kernel
  vcl_vector<brdb_value_sptr> pro_input;
  pro_input.push_back( new brdb_value_t<bocl_mem_sptr>(scene_info_) );
  pro_input.push_back( new brdb_value_t<bocl_mem_sptr>(trees_) );
  pro_input.push_back( new brdb_value_t<bocl_mem_sptr>(alphas_) );
  pro_input.push_back( new brdb_value_t<bocl_mem_sptr>(mogs_) );
  for (unsigned int i=0; i<input.size(); ++i)
    pro_input.push_back(input[i]);
  pro->execute(pro_input, output);

  return true;
}

bool boxm2_opencl_processor::finish()
{
  return true;
}


// set scene data for the processor
bool boxm2_opencl_processor::set_data(boxm2_scene_sptr& scene,
                                      boxm2_block_sptr& blk,
                                      boxm2_data_base_sptr& alpha,
                                      boxm2_data_base_sptr& mog)
{
    //set the processor's block pointer
    typedef vnl_vector_fixed<unsigned char, 16> uchar16;
    boxm2_array_3d<uchar16>& trees = blk->trees();
#if 0
    delete trees_;
#endif
    trees_ = new bocl_mem(this->context(), trees.data_block(), trees.size() * sizeof(uchar16), "3d trees buffer");
    trees_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    //set the processor's alpha block
#if 0
    delete alphas_;
#endif
    alphas_ = new bocl_mem(this->context(), alpha->data_buffer(), alpha->buffer_length(), "alpha buffer");
    alphas_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    //set the processor's MOG block
#if 0
    delete mogs_;
#endif
    mogs_ = new bocl_mem(this->context(), mog->data_buffer(), mog->buffer_length(), "mog buffer" );
    mogs_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    //LEAK EXISTS HERE FOR SCENE INFO
    //get scene info (hacky location, fix this)
    boxm2_scene_info* info_buffer = scene->get_blk_metadata(blk->block_id());  
    info_buffer->num_buffer = blk->num_buffers(); 
    info_buffer->tree_buffer_length = blk->tree_buff_length(); 
    info_buffer->data_buffer_length = 65536; 
    //if(scene_info_) delete scene_info_; 
    scene_info_ = new bocl_mem(this->context(), info_buffer, sizeof(boxm2_scene_info), "scene info buffer"); 
    scene_info_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    return true;
}

