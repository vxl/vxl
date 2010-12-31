#include <boxm2/boxm2_opencl_processor.h>
#include <boxm2/boxm2_opencl_process_base.h>

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
  //set scene data to null
  for (int i=0; i<NUM_QUEUES; ++i) {
    scene_info_[i]  = 0;
    trees_[i]       = 0;
    alphas_[i]      = 0;
    mogs_[i]        = 0;
  }

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

  return true;
}

// NEED TO FIGURE OUT HOW TO differentiate between SCENE/BLOCK/DATA arguments and Camera/Image arguments
// the scene level stuff needs to live on the processor, other
bool boxm2_opencl_processor::run(boxm2_process * process, vcl_vector<brdb_value_sptr> & input, vcl_vector<brdb_value_sptr> & output)
{
  //0. cast the process to a boxm2_opencl_process
  boxm2_opencl_process_base* pro = (boxm2_opencl_process_base*) process;

  //1. set process args and execute process (for each block in the queue)
  int curr = 0;   //refers to current queue index
  vul_timer t;
  this->setup_pinned_buffers(scene_,
                             blocks_to_process_[0],
                             alphas_to_process_[0],
                             mogs_to_process_[0]);

  //debugging for number of blocks (currently 2 works without issue)
  int NUM_BLOCKS = 2;
  vul_timer t1;
  for (int i=0; i<=NUM_BLOCKS; ++i) { // was: i<blocks_to_process_.size()

    t1.mark();
    if(i<NUM_BLOCKS)
    {
    this->enqueue_write_scene(scene_,blocks_to_process_[i],
                              alphas_to_process_[i],mogs_to_process_[i],
                                0,i%2);

    }

    //vcl_cout<<"T1 :"<<t1.all()<<vcl_endl;
    //
    //t1.mark();
    //set up argument vector, execute kernel
    if(i>0)
    {
        vcl_vector<brdb_value_sptr> pro_input;
        pro_input.push_back( new brdb_value_t<bocl_mem_sptr>(scene_info_[(i+1)%2]) );
        pro_input.push_back( new brdb_value_t<bocl_mem_sptr>(trees_[(i+1)%2]) );
        pro_input.push_back( new brdb_value_t<bocl_mem_sptr>(alphas_[(i+1)%2]) );
        pro_input.push_back( new brdb_value_t<bocl_mem_sptr>(mogs_[(i+1)%2]) );
        for (unsigned int j=0; j<input.size(); ++j)
            pro_input.push_back(input[j]);
        pro->set_command_queue(&queues_[1]);
        pro->execute(pro_input, output);
    }
    clFinish(queues_[0]);
    clFinish(queues_[1]);
   // clFinish(queues_[0]);

    vcl_cout<<"T2 :"<<t1.all()<<vcl_endl;
    //calculate the next kernel index
    //if (i+1 < NUM_BLOCKS) // < blocks_to_process_.size()
    //{
    //  int next = (curr + 1) % NUM_QUEUES;
    //  vcl_cout<<"NEXT QUEUE INDEX: "<<next<<vcl_endl;

    //  this->enqueue_write_scene(scene_,
    //                            blocks_to_process_[i+1],
    //                            alphas_to_process_[i+1],
    //                            mogs_to_process_[i+1],
    //                            next);
    //  curr = next;
    //}

  }
  vcl_cout<<"Wall Clock Execution: "<<t.all()<<" ms"<<vcl_endl;

  return true;
}

bool boxm2_opencl_processor::finish()
{
  return true;
}

// pushes block and data into a BOCL_mem, using QUEUE index
bool boxm2_opencl_processor::enqueue_write_scene( boxm2_scene* scene,
                                                  boxm2_block* blk,
                                                  boxm2_data_base* alpha,
                                                  boxm2_data_base* mog,
                                                  int queue_index, int buffer_index)
{
  //if ( loaded_[queue_index] != blk->block_id() )
  {
    loaded_[queue_index] = blk->block_id();

    //write tree information
    typedef vnl_vector_fixed<unsigned char, 16> uchar16;
    uchar16* trees = blk->trees().data_block();
    int treeSize = blk->trees().size()*sizeof(uchar16);
    vcl_memcpy(trees_pin[buffer_index], trees, treeSize);
    clEnqueueWriteBuffer( queues_[queue_index], trees_[buffer_index]->buffer(),
                          CL_FALSE, 0, treeSize,
                          (void*) trees_pin[buffer_index], 0, NULL, NULL);

    //write alpha
    vcl_memcpy(alphas_pin[buffer_index], alpha->data_buffer(), alpha->buffer_length());
    clEnqueueWriteBuffer( queues_[queue_index], alphas_[buffer_index]->buffer(),
                          CL_FALSE, 0, alpha->buffer_length(),
                          (void*) alphas_pin[buffer_index], 0, NULL, NULL);

    //write MOG
    vcl_memcpy(mogs_pin[buffer_index], mog->data_buffer(), mog->buffer_length());
    clEnqueueWriteBuffer( queues_[queue_index], mogs_[buffer_index]->buffer(),
                          CL_FALSE, 0, mog->buffer_length(),
                          (void*) mogs_pin[buffer_index], 0, NULL, NULL);

    //copy over scene info
    boxm2_scene_info* info_buffer = scene->get_blk_metadata(blk->block_id());
    info_buffer->num_buffer = blk->num_buffers();
    info_buffer->tree_buffer_length = blk->tree_buff_length();
    info_buffer->data_buffer_length = 65536;
    vcl_memcpy(info_pin[buffer_index], info_buffer, sizeof(boxm2_scene_info));
    clEnqueueWriteBuffer( queues_[queue_index], scene_info_[buffer_index]->buffer(),
                          CL_FALSE, 0, sizeof(boxm2_scene_info),
                          (void*)info_pin[buffer_index], 0, NULL, NULL);
  }

  return true;
}

// initializes scene buffers (Both of em)
//TODO MAKE THIS NOT WRITE TO THE SECOND BUFFER - IT'S UNNECESSARY
bool boxm2_opencl_processor::setup_pinned_buffers( boxm2_scene*     scene,
                                                   boxm2_block*     blk,
                                                   boxm2_data_base* alpha,
                                                   boxm2_data_base* mog )
{
  //set up pinned memory
  for (int i=0; i<NUM_QUEUES; ++i)
  {
    //set loaded
    loaded_[i] = blk->block_id();

    //setup pinned trees
    typedef vnl_vector_fixed<unsigned char, 16> uchar16;
    boxm2_array_3d<uchar16>& trees = blk->trees();
    trees_[i] = new bocl_mem(this->context(), NULL, trees.size()*sizeof(uchar16), "3d trees buffer ");
    trees_[i]->create_buffer(CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR );

    //set up alpha
    alphas_[i] = new bocl_mem(this->context(), NULL, alpha->buffer_length(), "alpha buffer");
    alphas_[i]->create_buffer(CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR );

    //set the processor's MOG block
    mogs_[i] = new bocl_mem(this->context(), NULL, mog->buffer_length(), "mog buffer" );
    mogs_[i]->create_buffer(CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR );

    //LEAK EXISTS HERE FOR SCENE INFO
    //get scene info (hacky location, fix this)
    boxm2_scene_info* info_buffer = scene->get_blk_metadata(blk->block_id());
    info_buffer->num_buffer = blk->num_buffers();
    info_buffer->tree_buffer_length = blk->tree_buff_length();
    info_buffer->data_buffer_length = 65536;
    scene_info_[i] = new bocl_mem(this->context(), NULL, sizeof(boxm2_scene_info), "scene info buffer");
    scene_info_[i]->create_buffer(CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR );

    //MAP OPENCL ALLOCATED BUFFERS TO A C++ POINTER
    trees_pin[i] = (uchar16*) clEnqueueMapBuffer( queues_[i], trees_[i]->buffer(), CL_TRUE,
                                                        CL_MAP_WRITE, 0, trees.size()*sizeof(uchar16), 0,
                                                        NULL, NULL, NULL);
    alphas_pin[i] = (float*) clEnqueueMapBuffer( queues_[i], alphas_[i]->buffer(), CL_TRUE,
                                               CL_MAP_WRITE, 0, alpha->buffer_length(), 0,
                                               NULL, NULL, NULL);
    mogs_pin[i]   = (char*)  clEnqueueMapBuffer( queues_[i], mogs_[i]->buffer(), CL_TRUE,
                                               CL_MAP_WRITE, 0, mog->buffer_length(), 0,
                                               NULL, NULL, NULL);
    info_pin[i]   = (char*)  clEnqueueMapBuffer( queues_[i], scene_info_[i]->buffer(), CL_TRUE,
                                                        CL_MAP_WRITE, 0, sizeof(boxm2_scene_info), 0,
                                                        NULL, NULL, NULL);

    //now set each BOCL_MEM to point to the above, pinned C++ pointer (for the future)
    //trees_[i]->set_cpu_buffer(trees_pin);
    //alphas_[i]->set_cpu_buffer(alphas_pin);
    //mogs_[i]->set_cpu_buffer(mogs_pin);
    //scene_info_[i]->set_cpu_buffer(info_pin);
  }
  return true;
}

// sets scene pointers (in their correct order)
bool boxm2_opencl_processor::push_scene_data( boxm2_block* blk,
                                              boxm2_data_base* alph,
                                              boxm2_data_base* mog )
{
  blocks_to_process_.push_back(blk);
  alphas_to_process_.push_back(alph);
  mogs_to_process_.push_back(mog);
  return true;
}
