#include "boxm2_opencl_processor.h"
#include "boxm2_opencl_process_base.h"

//boxm2 data structures


//cameras/images
#include <vpgl/vpgl_perspective_camera.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_vector_fixed.h>
#include <vul/vul_timer.h>

//brdb stuff
#include <brdb/brdb_value.h>

bool boxm2_opencl_processor::init()
{
  // set up 2 command queues
  int status;
  for (int i=0; i<NUM_QUEUES; ++i) {
    queues_[i] = clCreateCommandQueue(this->context(),
                                      this->devices()[0],
                                      CL_QUEUE_PROFILING_ENABLE,
                                      &status);
    if (!check_val(status,CL_SUCCESS,"Failed in command queue creation" + error_to_string(status)))
      return false;
  }
  
  //set up our cache
  gpu_cache_ = new boxm2_opencl_cache(cpu_cache_, 
                                      &(this->context()), 
                                      &queues_[0], 
                                      scene_); 
  return true;
}


    
//INCORPORATE A SEQUENCING METHOD
// bool sequencing(vector<boxm2_block_id> ids)
// for each ID
//    vector<brdb_vals> args. push back all args and id
//    pro.execute(args)
//    cache->load(next id) //for streaming load
// 
// ALL OF THE BUFFER stuff will end up in the opencl_cache, making it more modular
bool boxm2_opencl_processor::sequencing(vcl_vector<boxm2_block_id> ids,
                                        boxm2_process* process,
                                        vcl_vector<brdb_value_sptr>& input,
                                        vcl_vector<brdb_value_sptr>& output)
{
  boxm2_opencl_process_base* pro = (boxm2_opencl_process_base*) process;
  pro->set_gpu_cache(gpu_cache_); 
  
  // for each BLOCK id execute
  for(int i=0; i<ids.size(); i++)
  {
    vcl_vector<brdb_value_sptr> pro_input;
    pro_input.push_back( new brdb_value_t<boxm2_block_id_sptr>( new boxm2_block_id(ids[i]) ) ); 
    for (unsigned int j=0; j<input.size(); ++j)
        pro_input.push_back(input[j]);
    pro->set_command_queue(&queues_[1]);          //always executing with queue 1
    pro->execute(pro_input, output);
  }
  
}

bool boxm2_opencl_processor::run(boxm2_process * process, vcl_vector<brdb_value_sptr> & input, vcl_vector<brdb_value_sptr> & output)
{
  return true;
}

//: clean up allocated buffers
bool boxm2_opencl_processor::finish()
{
   return true;
}
