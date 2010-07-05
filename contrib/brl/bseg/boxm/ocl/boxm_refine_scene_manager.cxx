//:
// \file
#include "boxm_refine_scene_manager.h"
#include <vcl_where_root_dir.h>
#include <boxm/ocl/boxm_ocl_utils.h>
#include <vcl_cstdio.h>
#include <vgl/vgl_box_3d.h>
#include <vcl_cstring.h> //(memcpy)

//include for timing
#include <vul/vul_timer.h>


//: Initializes CPU side input buffers
//put tree structure and data into arrays
//initializes (cl_int*) cells_ and (cl_float*) cell_data_
// also initializes (cl_int*) tree_results_ and (cl_float*) data_results_
bool boxm_refine_scene_manager::
init(int* cells, int* block_ptrs, int num_blocks, int tree_max_size, 
     int start_ptr, int end_ptr,
     float* data, int data_size, int data_max_size,
     float prob_thresh, int max_level, float bbox_len)
{
  //allocate memory for the trees
  alloc_trees(tree_max_size, data_max_size, num_blocks);
  
  //copy tree structure
  vcl_memcpy(cells_, cells, sizeof(cl_int4)*tree_max_size);
  vcl_memcpy(block_ptrs_, block_ptrs, sizeof(cl_int2)*num_blocks);
  tree_max_size_ = tree_max_size;
  num_blocks_ = num_blocks;
  start_ptr_ = start_ptr;
  end_ptr_ = end_ptr;
  
  //copy data
  vcl_memcpy(cell_data_, data, sizeof(cl_float16)*data_max_size);
  numdata_ = data_size;
  data_max_size_ = data_max_size;
  
  //copy necessary data
  prob_thresh_ = prob_thresh;
  max_level_ = max_level;
  bbox_len_ = bbox_len;
  return init_kernel();
}

bool boxm_refine_scene_manager::init_kernel()
{
  //load kernel source main
  if (!this->load_kernel_source(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    +"/contrib/brl/bseg/boxm/ocl/refine_blocks.cl")) {
    vcl_cerr << "Error: boxm_refine_manager : failed to load kernel source (main function)\n";
    return false;
  }
  if (this->build_kernel_program()) {
    return false;
  }
  //create the kernel
  cl_int status = CL_SUCCESS;
  kernel_ = clCreateKernel(program_, "refine_main", &status);
  if (!this->check_val(status, CL_SUCCESS, error_to_string(status))) {
    return false;
  }
  return true;
}


/***************************************************************
 * RUN_SCENE() and RUN_TREE() - refines a scene after manager
 * is initialized with a scene must call boxm_refine_manager.init(scene)
 ***************************************************************/
bool boxm_refine_scene_manager::run_scene()
{
  vcl_cout<<"REFINING Scene-----------------------------"<<vcl_endl;
  
  //allocate and initialize memory on gpu side
  vcl_cout<<"---setting up buffers"<<vcl_endl;
  cl_int status =  setup_tree_buffers();

  float numMB = (16*tree_max_size_+4*16*data_max_size_)/(1024.0f*1024.0f);
  vcl_cout<<"---total global mem allocated: "<<numMB<<vcl_endl;

  //run this block on the GPU
  vcl_cout<<"---running on gpu "<<vcl_endl;
  vul_timer timer; timer.mark();
  status = run_block();
  float gpu_time = (float) timer.all()/1e3f;
  if (!this->check_val(status,CL_SUCCESS,"run_block failed"))
    return false;

  //read output from GPU
  vcl_cout<<"---reading buffers"<<vcl_endl;
  if (!read_tree_buffers())
    return false;

  //opencl output_
  vcl_cout<<"---KERNEL OUTPUT: "<<output_results_<<vcl_endl;

  //debug print method - print each Blocks' Result here
  vcl_cout<<"---REFINE Stats:-----------------------------------"<<vcl_endl
          <<"---Scene Input Size (#blocks) = "<<num_blocks_<<vcl_endl
          <<"----------------------------------------------------"<<vcl_endl;

  //boxm_ocl_utils<T>::print_tree_array(tree_results_, (*tree_results_size_), data_results_);

  // Verify that the tree is formatted correctly

  //PROFILING INFORMATION FROM OPENCL
#if 0
  float treeSize = 4*4*tree_results_size_/(1024.0f*1024.0f); //tree size in MB
  float dataSize = 4*16*data_results_size_/(1024.0f*1024.0f); //data size in MBs
  vcl_cout<<"---GLOBAL MEM BANDWITH RESULTS-----------------------"<<vcl_endl
          <<"---Tree Size: "<<(*tree_results_size_)<<" blocks; "<<treeSize<<" MB"<<vcl_endl
          <<"---Data size: "<<(*data_results_size_)<<" blocks; "<<dataSize<<" MB"<<vcl_endl
          <<"---GPU Time: "<<gpu_time<<" seconds"<<vcl_endl
          <<"---Refine Bandwidth ~~ "<<(treeSize+dataSize)/gpu_time<<" MB/sec"<<vcl_endl
          <<"-----------------------------------------------------"<<vcl_endl;
#endif // 0


  return true;
}

//: refines the tree after initialization
bool boxm_refine_scene_manager::run_block()
{
  //set kernel arguments
  int status = CL_SUCCESS;
  status = this->set_kernel_args();

  //make sure adequate local mem space
  cl_ulong used_local_memory;
  status = clGetKernelWorkGroupInfo(this->kernel(),this->devices()[0],CL_KERNEL_LOCAL_MEM_SIZE,
                                    sizeof(cl_ulong),&used_local_memory,NULL);
  if (!this->check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_LOCAL_MEM_SIZE failed."))
    return SDK_FAILURE;

  // determine the work group size
  cl_ulong kernel_work_group_size;
  status = clGetKernelWorkGroupInfo(this->kernel(),this->devices()[0],CL_KERNEL_WORK_GROUP_SIZE,
                                    sizeof(cl_ulong),&kernel_work_group_size,NULL);
  if (!this->check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_WORK_GROUP_SIZE, failed."))
    return SDK_FAILURE;

  vcl_cout<<"KERNEL work group size..."<<kernel_work_group_size<<vcl_endl;

  //Global size and local size is just 1 for now (serial)
  vcl_size_t globalThreads[1], localThreads[1];
  globalThreads[0] = 8; //(*tree_max_size_);
  localThreads[0] = 8; //64;

  if (used_local_memory > this->total_local_memory())
  {
    vcl_cout << "Unsupported: Insufficient local memory on device.\n";
    return SDK_FAILURE;
  }

  // set up a command queue
  command_queue_ = clCreateCommandQueue(this->context(), 
                                        this->devices()[0], 
                                        CL_QUEUE_PROFILING_ENABLE, 
                                        &status);
  if (!this->check_val(status,CL_SUCCESS,"Failed in command queue creation" + error_to_string(status)))
    return false;

  // pop the kernel onto the queue to execute
  cl_event ceEvent = 0;
  status = clEnqueueNDRangeKernel(command_queue_, this->kernel_,
                                  1, NULL, globalThreads, localThreads,
                                  0, NULL, &ceEvent);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueNDRangeKernel failed. "+error_to_string(status)))
    return SDK_FAILURE;

  //clFinish makes sure the command_queue_ is flushed before probing memory..?
  status = clFinish(command_queue_);
  if (!this->check_val(status,CL_SUCCESS,"clFinish failed."+error_to_string(status)))
    return SDK_FAILURE;

  //profiling information
  cl_ulong tstart,tend;
  status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),&tstart,0);
  status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_END,sizeof(cl_ulong),&tend,0);
  gpu_time = (tend-tstart)/1e9f;  //gpu time in seconds
 
  return SDK_SUCCESS;
}

/**********************************************************
 * Setup tree bufers and clean tree buffers
 * sets up and cleans tree buffers sent to GPU
 **********************************************************/
int boxm_refine_scene_manager::setup_tree_buffers()
{
  cl_int status = CL_SUCCESS;

  //---- create and initialize memory objects -----
  //tree array (cells_)
  cell_buf_ = clCreateBuffer(this->context_,  //write output_
                             CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                             tree_max_size_ * sizeof(cl_int4),
                             cells_,
                             &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (cell_array) failed."))
    return SDK_FAILURE;

  //tree SIZE
  block_ptrs_buf_ = clCreateBuffer(this->context_,
                                  CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                                  sizeof(cl_int2)*num_blocks_,
                                  block_ptrs_,
                                  &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (block_ptrs_) failed."))
    return SDK_FAILURE;

  //data array (cell_data_)
  data_buf_ = clCreateBuffer(this->context_,
                             CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                             data_max_size_ * sizeof(cl_float16),
                             cell_data_,
                             &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (cell_data) failed."))
    return SDK_FAILURE;
    
  
  //memory pointer buffers
  start_ptr_buf_ = clCreateBuffer(this->context_,
                             CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                             sizeof(cl_uint),
                             &start_ptr_,
                             &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (start_ptr_buf_) failed."))
    return SDK_FAILURE;
    
  end_ptr_buf_ = clCreateBuffer(this->context_,
                             CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                             sizeof(cl_uint),
                             &end_ptr_,
                             &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (memory end buf ) failed."))
    return SDK_FAILURE;  
    
  //OUTPUT
  output_buf_ = clCreateBuffer(this->context_,
                             CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                             sizeof(cl_float),
                             &output_results_,
                             &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (cell_data) failed."))
    return SDK_FAILURE;

  return SDK_SUCCESS;
}

/*********************************************************
 * read_tree_buffers() - reads gpu results into
 * (cl_int*) tree_results_ and (cl_float*) data_results_
 *********************************************************/
bool boxm_refine_scene_manager::read_tree_buffers()
{
  cl_event events[5];

  //Read cell_buf_
  int status = clEnqueueReadBuffer(command_queue_, cell_buf_, CL_TRUE,
                                   0,  tree_max_size_ * sizeof(cl_int4),
                                   tree_results_,
                                   0, NULL, &events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (tree_results_)failed."))
    return false;

  // read number of cells
  vcl_cout<<"size = " <<num_blocks_*sizeof(cl_int2)<<vcl_endl;
  status = clEnqueueReadBuffer(command_queue_, block_ptrs_buf_, CL_TRUE,
                               0, num_blocks_*sizeof(cl_int2),
                               block_ptrs_results_,
                               0, NULL, &events[1]);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (block_ptrs_results_)failed."))
    return false;

  //read data_buf_
  status = clEnqueueReadBuffer(command_queue_, data_buf_, CL_TRUE,
                               0, data_max_size_ * sizeof(cl_float16),
                               data_results_,
                               0,NULL,&events[2]);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (data_results_)failed."))
    return false;

  //read output_buf_
  status = clEnqueueReadBuffer(command_queue_, output_buf_, CL_TRUE,
                               0, sizeof(cl_float),
                               &output_results_,
                               0,NULL,&events[3]);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (output_results_)failed."))
    return false;


  // Wait for the read buffer to finish execution
  status = clWaitForEvents(4, events);
  if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
    return false;

  //release events? all of em or just 0?
  for (int i=0; i<4; i++) {
    status = clReleaseEvent(events[i]);
    if (!this->check_val(status,CL_SUCCESS,"clReleaseEvent failed."))
      return false;
  }

  // release the command Queue
  status = clReleaseCommandQueue(command_queue_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseCommandQueue failed."))
    return false;

  return true;
}

int boxm_refine_scene_manager::clean_tree_buffers()
{
  // Releases OpenCL resources (Context, Memory etc.)
  cl_int status;
  status = clReleaseMemObject(cell_buf_);
  if (!this->check_val(status, CL_SUCCESS, "clReleaseMemObject failed (cell_buf_)."))
    return SDK_FAILURE;

  status = clReleaseMemObject(block_ptrs_buf_);
  if (!this->check_val(status, CL_SUCCESS, "clReleaseMemObject failed (block_ptrs_buf_)."))
    return SDK_FAILURE;

  status = clReleaseMemObject(data_buf_);
  if (!this->check_val(status, CL_SUCCESS, "clReleaseMemObject failed (data_buf_)."))
    return SDK_FAILURE;

  status = clReleaseMemObject(start_ptr_buf_);
  if (!this->check_val(status, CL_SUCCESS, "clReleaseMemObject failed (start_ptr_buf_)."))
    return SDK_FAILURE;
    
  status = clReleaseMemObject(end_ptr_buf_);
  if (!this->check_val(status, CL_SUCCESS, "clReleaseMemObject failed (end_ptr_buf_)."))
    return SDK_FAILURE;

  //output
  status = clReleaseMemObject(output_buf_);
  if (!this->check_val(status, CL_SUCCESS, "clReleaseMemObject failed (output_buf_)."))
    return SDK_FAILURE;

  return SDK_SUCCESS;
}


//: allocates host memory for cells_, cell_data_, numlevels_ and tree_max_size_
bool boxm_refine_scene_manager::alloc_trees(int cells_size, int data_input_size, int num_blocks)
{
  //---- allocate input buffers ----//
  //allocate tree structure, data array
  cells_ =             (cl_int*) boxm_ocl_utils::alloc_aligned(cells_size, sizeof(cl_int4), 16);
  cell_data_ =       (cl_float*) boxm_ocl_utils::alloc_aligned(data_input_size, sizeof(cl_float16), 16);
  block_ptrs_ =        (cl_int*) boxm_ocl_utils::alloc_aligned(num_blocks, sizeof(cl_int2), 16);

  //output results
  tree_results_ =       (cl_int*) boxm_ocl_utils::alloc_aligned(cells_size, sizeof(cl_int4), 16);
  data_results_ =     (cl_float*) boxm_ocl_utils::alloc_aligned(data_input_size, sizeof(cl_float16), 16);
  block_ptrs_results_ = (cl_int*) boxm_ocl_utils::alloc_aligned(num_blocks, sizeof(cl_int2), 16);

  if (cells_== NULL||cell_data_ == NULL)
  {
    vcl_cout << "Failed to allocate host memory. (tree input)\n";
    return false;
  }
  return true;
}

//: Cleans up the tree in computer memory (cells_ and cell_data_, tree_max_size_, tree_results_)
bool boxm_refine_scene_manager::free_trees()
{
  boxm_ocl_utils::free_aligned(cells_);
  boxm_ocl_utils::free_aligned(cell_data_);
  boxm_ocl_utils::free_aligned(block_ptrs_);
  boxm_ocl_utils::free_aligned(tree_results_);
  boxm_ocl_utils::free_aligned(data_results_);
  boxm_ocl_utils::free_aligned(block_ptrs_results_);
  return true;
}


/*****************************************
 * Cleanup refine_manager
 *****************************************/
bool boxm_refine_scene_manager::clean_refine()
{
  // release host memory
  bool good = free_trees();

  // clean up GPU side memory
  cl_int status = CL_SUCCESS;
  status = clean_tree_buffers();
  good = good && this->check_val(status, CL_SUCCESS, "clReleaseMemObject failed");

  // release kernel
  status = clReleaseKernel(kernel_);
  good = static_cast<bool>(this->check_val(status,CL_SUCCESS,"clReleaseKernel failed."));
  return good;
}


/**************************************************
 * Set kernel arguments
 **************************************************/
int boxm_refine_scene_manager::set_kernel_args()
{
  cl_int status = CL_SUCCESS;
  int i = 0;
  
  // ----- Set appropriate arguments to the kernel ----
  // scene buffer
  status = clSetKernelArg(kernel_, i++, sizeof(cl_mem), (void*) &cell_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (octree buffer)"))
    return SDK_FAILURE;
  // block pointers
  status = clSetKernelArg(kernel_, i++, sizeof(cl_mem),(void*) &block_ptrs_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (block_ptrs buffer)"))
    return SDK_FAILURE;
  // num blocks
  status = clSetKernelArg(kernel_, i++, sizeof(cl_uint), &num_blocks_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (num_blocks buffer)"))
    return SDK_FAILURE;
  // max tree size
  status = clSetKernelArg(kernel_, i++, sizeof(cl_uint), &tree_max_size_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (tree_max_size_ buffer)"))
    return SDK_FAILURE;
  //mem start and end
  status = clSetKernelArg(kernel_, i++, sizeof(cl_mem), (void*) &start_ptr_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (start_ptr_ buffer)"))
    return SDK_FAILURE;
  status = clSetKernelArg(kernel_, i++, sizeof(cl_mem), (void*) &end_ptr_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (end_ptr_ buffer)"))
    return SDK_FAILURE;
  
  // data buffer
  status = clSetKernelArg(kernel_, i++, sizeof(cl_mem), (void*) &data_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (data buffer)"))
    return SDK_FAILURE;
  // data size
  status = clSetKernelArg(kernel_, i++, sizeof(cl_uint), &numdata_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (data_size buffer)"))
    return SDK_FAILURE;
  //data max sizeof
  status = clSetKernelArg(kernel_, i++, sizeof(cl_uint), &data_max_size_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (data_max_size buffer)"))
    return SDK_FAILURE;
  //probability threshold
  status = clSetKernelArg(kernel_, i++, sizeof(cl_float),  &prob_thresh_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (prob_thresh buffer)"))
    return SDK_FAILURE;
  //max level
  status = clSetKernelArg(kernel_, i++, sizeof(cl_uint), (void*) &max_level_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (max_level_) buffer)"))
    return SDK_FAILURE;
  //bbox length
  status = clSetKernelArg(kernel_, i++, sizeof(cl_float), (void*) &bbox_len_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (bbox_len_) buffer)"))
    return SDK_FAILURE;
  // ----- end kernel arguments ---

  // ---- create local memory arguments for caching whole blocks ----
  //local tree copy
  status = clSetKernelArg(kernel_, i++, sizeof(cl_int4)*585, 0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (loc code bundle)"))
    return false;
  //---- end local memory arguments
  
  //IO ARGUMENT
  status = clSetKernelArg(kernel_, i++, sizeof(cl_mem), (void*) &output_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output_Buff buffer)"))
    return SDK_FAILURE; 
}

/*******************************************
 * build_kernel_program - builds kernel program
 * from source (a vcl string)
 *******************************************/
int boxm_refine_scene_manager::build_kernel_program()
{
  cl_int status = CL_SUCCESS;
  vcl_size_t sourceSize[] = { this->prog_.size() };
  if (!sourceSize[0]) return SDK_FAILURE;
  if (program_) {
    status = clReleaseProgram(program_);
    program_ = 0;
    if (!this->check_val(status,
      CL_SUCCESS,
      "clReleaseProgram failed."))
      return SDK_FAILURE;
  }
  const char * source = this->prog_.c_str();

  program_ = clCreateProgramWithSource(this->context_,
                                       1,
                                       &source,
                                       sourceSize,
                                       &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateProgramWithSource failed."))
    return SDK_FAILURE;

  // create a cl program executable for all the devices specified
  status = clBuildProgram(program_,
                          1,
                          this->devices_,
                          NULL,
                          NULL,
                          NULL);
  if (!this->check_val(status, CL_SUCCESS, error_to_string(status)))
  {
    vcl_size_t len;
    char buffer[2048];
    clGetProgramBuildInfo(program_, this->devices_[0],
                          CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
    vcl_printf("%s\n", buffer);
    return SDK_FAILURE;
  }
  else
    return SDK_SUCCESS;
}


