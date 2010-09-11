//:
// \file
#include "boxm_refine_scene_manager.h"
#include <vcl_where_root_dir.h>
#include <boxm/ocl/boxm_ocl_utils.h>
#include <vnl/vnl_vector_fixed.h>
#include <vbl/vbl_array_3d.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_array_1d.h>

//include for timing
#include <vul/vul_timer.h>


//: Initializes CPU side input buffers
// Put tree structure and data into arrays
// Initializes (cl_int*) cells_ and (cl_float*) cell_data_
// also initializes (cl_int*) tree_results_ and (cl_float*) data_results_
bool boxm_refine_scene_manager::
init_refine(boxm_ocl_scene* scene, float prob_thresh)
{
  //keep track of the scene
  scene_ = scene;

  //store scene information, numbuffer_ length of buffer,
  tree_cell_size_ = 4;  //four ints for now
  data_cell_size_ = 16;  //8 floats for now
  scene->tree_buffer_shape(numbuffer_, lenbuffer_);
  max_level_ = scene->max_level();      //this is really the same as max level...

  //stuff refine needs
  prob_thresh_ = prob_thresh;
  double x, y, z; scene->block_dim(x, y, z);
  bbox_len_ = (float) x;

  bool good = setup_scene_data() &&
              setup_scene_data_buffers() &&
              init_kernel();
  return good;
}

//: copies scene data into these member variable buffers
bool boxm_refine_scene_manager::setup_scene_data()
{
  typedef vnl_vector_fixed<int, 4> int4;
  typedef vnl_vector_fixed<float, 16> float16;
  typedef vnl_vector_fixed<int, 2> int2;

  //number of blocks along each dimension
  block_nums_   = (cl_int*)   boxm_ocl_utils::alloc_aligned(1, sizeof(cl_int4), 16);
  int numX, numY, numZ;
  scene_->block_num(numX,numY,numZ);
  block_nums_[0] = numX;
  block_nums_[1] = numY;
  block_nums_[2] = numZ;
  block_nums_[3] = 0;

  //3d array of block pointers
  int numblocks = block_nums_[0]*block_nums_[1]*block_nums_[2];
  vcl_cout<<"Block size "<<(float)numblocks*16/1024.0/1024.0<<"MB"<<vcl_endl;
  block_ptrs_   = (cl_int*)   boxm_ocl_utils::alloc_aligned(numblocks,sizeof(cl_int4),16);
  int index=0;
  vbl_array_3d<int4>::iterator iter;
  for (iter = scene_->blocks_.begin(); iter != scene_->blocks_.end(); iter++) {
    block_ptrs_[index++]=(*iter)[0];
    block_ptrs_[index++]=(*iter)[1];
    block_ptrs_[index++]=(*iter)[2];
    block_ptrs_[index++]=(*iter)[3];
  }

  for (int i=0; i<4; i++)
    vcl_cout<<block_ptrs_[i]<<',';
  vcl_cout<<vcl_endl;

  //2d array of tree cells
  vcl_cout<<"Cells "<<(float)(numbuffer_*lenbuffer_)*16/1024.0/1024.0<<"MB"<<vcl_endl;
  tree_cells_   = (cl_int*)   boxm_ocl_utils::alloc_aligned(numbuffer_*lenbuffer_, sizeof(cl_int4), 16);
  index = 0;
  vbl_array_2d<int4>::iterator tree_iter;
  for ( tree_iter = scene_->tree_buffers_.begin(); tree_iter != scene_->tree_buffers_.end(); tree_iter++) {
    tree_cells_[index++]=(*tree_iter)[0];
    tree_cells_[index++]=(*tree_iter)[1];
    tree_cells_[index++]=(*tree_iter)[2];
    tree_cells_[index++]=(*tree_iter)[3];
  }

  //2d array of data cells
  vcl_cout<<"Data "<<(float)(numbuffer_*lenbuffer_)*16*4/1024.0/1024.0<<"MB"<<vcl_endl;
  data_cells_   = (cl_float*) boxm_ocl_utils::alloc_aligned(numbuffer_*lenbuffer_, sizeof(cl_float16), 16);
  vbl_array_2d<float16>::iterator data_iter;
  int datIndex = 0;
  for (data_iter=scene_->data_buffers_.begin(); data_iter!=scene_->data_buffers_.end(); data_iter++) {
    if (datIndex == 6614*16) {
      vcl_cout<<"data item 6614 = "<<(*data_iter)<<vcl_endl;
    }
    for (int j=0; j<16; j++) {
      data_cells_[datIndex++]=(*data_iter)[j];
    }
  }
  vcl_cout<<"datIndex after = "<<datIndex<<vcl_endl;

  //1d array of memory pointers
  mem_ptrs_     = (cl_int*)   boxm_ocl_utils::alloc_aligned(numbuffer_, sizeof(cl_int2), 16);
  vbl_array_1d<int2>::iterator mem_iter;
  index = 0;
  for (mem_iter = scene_->mem_ptrs_.begin(); mem_iter != scene_->mem_ptrs_.end(); mem_iter++) {
    mem_ptrs_[index++] = (*mem_iter)[0];
    mem_ptrs_[index++] = (*mem_iter)[1];
  }

  //scene_dims_   = (cl_int*)   boxm_ocl_utils::alloc_aligned(1, sizeof(cl_int4), 16);
  //scene_origin_ = (cl_float*) boxm_ocl_utils::alloc_aligned(1, sizeof(cl_int4), 16);

  //output buffer
  output_       = (cl_float*) boxm_ocl_utils::alloc_aligned(10, sizeof(cl_float), 16);
  for (int i=0; i<10; i++)
    output_[i] = 0;
  return true;
}

//: Setup tree buffers and clean tree buffers
//  Sets up and cleans tree buffers sent to GPU
bool boxm_refine_scene_manager::setup_scene_data_buffers()
{
  cl_int status = CL_SUCCESS;

  //block pointers
  int numblocks = block_nums_[0]*block_nums_[1]*block_nums_[2];
  block_ptrs_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                                   numblocks * sizeof(cl_int4),
                                   block_ptrs_,
                                   &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (block_ptrs_) failed."))
    return false;

  //number of blocks in each dimension
  block_nums_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                                   sizeof(cl_int4),
                                   block_nums_,
                                   &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (block_nums_) failed."))
    return false;

  //tree cells
  tree_cells_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                                   numbuffer_*lenbuffer_*sizeof(cl_int4),
                                   tree_cells_,
                                   &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (tree_cells_) failed."))
    return false;

  //data cells buffer
  data_cells_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                                   numbuffer_*lenbuffer_*sizeof(cl_float16),
                                   data_cells_,
                                   &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (data_cells_) failed."))
    return false;

  //memory pointers for each tree buffer
  mem_ptrs_buf_ = clCreateBuffer(this->context_,
                                 CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                                 numbuffer_*sizeof(cl_int2),
                                 mem_ptrs_,
                                 &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (mem_ptrs_) failed."))
    return false;

  //OUTPUT
  output_buf_ = clCreateBuffer(this->context_,
                               CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                               sizeof(cl_float)*10,
                               output_,
                               &status);

  return this->check_val(status, CL_SUCCESS, "clCreateBuffer (cell_data) failed.");
}


//: Cleans up the tree in computer memory (cells_ and cell_data_, tree_max_size_, tree_results_)
bool boxm_refine_scene_manager::clean_scene_data()
{
  boxm_ocl_utils::free_aligned(block_ptrs_);
  boxm_ocl_utils::free_aligned(block_nums_);
  boxm_ocl_utils::free_aligned(tree_cells_);
  boxm_ocl_utils::free_aligned(data_cells_);
  boxm_ocl_utils::free_aligned(mem_ptrs_);
  //boxm_ocl_utils::free_aligned(scene_dims_);
  //boxm_ocl_utils::free_aligned(scene_origin_);
  boxm_ocl_utils::free_aligned(output_);
  return true;
}

bool boxm_refine_scene_manager::clean_scene_data_buffers()
{
  // Releases OpenCL resources (Context, Memory etc.)
  cl_int status;
  status = clReleaseMemObject(block_ptrs_buf_);
  if (!this->check_val(status, CL_SUCCESS, "clReleaseMemObject failed (block_ptrs_buf_)."))
    return SDK_FAILURE;

  status = clReleaseMemObject(block_nums_buf_);
  if (!this->check_val(status, CL_SUCCESS, "clReleaseMemObject failed (block_nums_buf_)."))
    return SDK_FAILURE;

  status = clReleaseMemObject(tree_cells_buf_);
  if (!this->check_val(status, CL_SUCCESS, "clReleaseMemObject failed (tree_cells_buf_)."))
    return SDK_FAILURE;

  status = clReleaseMemObject(data_cells_buf_);
  if (!this->check_val(status, CL_SUCCESS, "clReleaseMemObject failed (data_cells_buf_)."))
    return SDK_FAILURE;

  status = clReleaseMemObject(mem_ptrs_buf_);
  if (!this->check_val(status, CL_SUCCESS, "clReleaseMemObject failed (mem_ptrs_buf_)."))
    return SDK_FAILURE;

  //output
  status = clReleaseMemObject(output_buf_);
  if (!this->check_val(status, CL_SUCCESS, "clReleaseMemObject failed (output_buf_)."))
    return SDK_FAILURE;

  return SDK_SUCCESS;
}


//: Runs refine on gpu
bool boxm_refine_scene_manager::run_refine()
{
  vcl_cout<<"REFINING OCL SCENE:--------------------------------"<<vcl_endl;

  //set kernel arguments
  int status = CL_SUCCESS;
  status = this->set_kernel_args();

  //make sure adequate local mem space
  cl_ulong used_local_memory;
  status = clGetKernelWorkGroupInfo(this->kernel_,this->devices()[0],CL_KERNEL_LOCAL_MEM_SIZE,
                                    sizeof(cl_ulong),&used_local_memory,NULL);
  if (!this->check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_LOCAL_MEM_SIZE failed."))
    return SDK_FAILURE;
  vcl_cout<<"---used local mem space: "<<used_local_memory<<" bytes"<<vcl_endl;
  if (used_local_memory > this->total_local_memory()) {
    vcl_cout << "Unsupported: Insufficient local memory on device.\n";
    return false;
  }

  // determine the work group size
  cl_ulong kernel_work_group_size;
  status = clGetKernelWorkGroupInfo(this->kernel_,this->devices()[0],CL_KERNEL_WORK_GROUP_SIZE,
                                    sizeof(cl_ulong),&kernel_work_group_size,NULL);
  if (!this->check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_WORK_GROUP_SIZE, failed."))
    return SDK_FAILURE;
  vcl_cout<<"KERNEL work group size..."<<kernel_work_group_size<<vcl_endl;

  //Global size and local size is just 8 for now (serial)
  vcl_size_t globalThreads[1], localThreads[1];
  localThreads[0] = 1; //64;
  globalThreads[0] = 1*numbuffer_; //(*tree_max_size_);
  vcl_cout<<"(global, local) threads: "<<globalThreads[0]<<", "<<localThreads[0]<<vcl_endl;

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


  //get scene information back and reinitialize the ocl_scene
  this->read_buffers();
  scene_->set_blocks(block_ptrs_);
  scene_->set_tree_buffers(tree_cells_);
  scene_->set_mem_ptrs(mem_ptrs_);
  scene_->set_data_values(data_cells_);

  //profiling information
  cl_ulong tstart,tend;
  status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),&tstart,0);
  status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_END,sizeof(cl_ulong),&tend,0);
  gpu_time = (tend-tstart)/1e9f;  //gpu time in seconds

  //opencl output_
  vcl_cout<<"---OPENCL KERNEL OUTPUT: " << output_[0];
  for (int i=1; i<10; ++i)
    vcl_cout<<','<<output_[i];
  vcl_cout<<vcl_endl;


  //PROFILING INFORMATION FROM OPENCL
#if 0
  float treeSize = 4*4*tree_results_size_/(1024.0f*1024.0f); //tree size in MB
  float dataSize = 4*16*data_results_size_/(1024.0f*1024.0f); //data size in MBs
  vcl_cout<<"---GLOBAL MEM BANDWITH RESULTS-----------------------\n"
          <<"---Tree Size: "<<(*tree_results_size_)<<" blocks; "<<treeSize<<" MB\n"
          <<"---Data size: "<<(*data_results_size_)<<" blocks; "<<dataSize<<" MB\n"
          <<"---GPU Time: "<<gpu_time<<" seconds\n"
          <<"---Refine Bandwidth ~~ "<<(treeSize+dataSize)/gpu_time<<" MB/sec\n"
          <<"-----------------------------------------------------"<<vcl_endl;
#endif // 0

  return true;
}


//: reads output buffers to cpu memory
bool boxm_refine_scene_manager::read_buffers()
{
  int numEvents = 5, eventI = 0, status = CL_SUCCESS;
  cl_event events[5];

  //read block pointers
  int numblocks = block_nums_[0]*block_nums_[1]*block_nums_[2];
  status = clEnqueueReadBuffer(command_queue_, block_ptrs_buf_, CL_TRUE,
                               0, numblocks * sizeof(cl_int4),
                               block_ptrs_,
                               0, NULL, &events[eventI++]);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (block_ptrs)failed."))
    return false;

  //Read tree_cells
  status = clEnqueueReadBuffer(command_queue_, tree_cells_buf_, CL_TRUE,
                               0,  numbuffer_*lenbuffer_*sizeof(cl_int4),
                               tree_cells_,
                               0, NULL, &events[eventI++]);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (tree_results_)failed."))
    return false;

  //read data_cells
  status = clEnqueueReadBuffer(command_queue_, data_cells_buf_, CL_TRUE,
                               0, numbuffer_*lenbuffer_*sizeof(cl_float16),
                               data_cells_,
                               0, NULL, &events[eventI++]);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (block_ptrs_results_)failed."))
    return false;

  //read mem_ptrs_
  status = clEnqueueReadBuffer(command_queue_, mem_ptrs_buf_, CL_TRUE,
                               0, numbuffer_*sizeof(cl_int2),
                               mem_ptrs_,
                               0,NULL,&events[eventI++]);

  //read output_buf_
  status = clEnqueueReadBuffer(command_queue_, output_buf_, CL_TRUE,
                               0, sizeof(cl_float)*10,
                               output_,
                               0,NULL,&events[eventI++]);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (output_results_)failed."))
    return false;


  // Wait for the read buffer to finish execution
  status = clWaitForEvents(numEvents, events);
  if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
    return false;

  //release events? all of em or just 0?
  for (int i=0; i<numEvents; i++) {
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


/*****************************************
 * Cleanup refine_manager
 *****************************************/
bool boxm_refine_scene_manager::clean_refine()
{
  // release host memory and gpu memory
  bool good = clean_scene_data() && clean_scene_data_buffers();

  // release kernel
  cl_int status = CL_SUCCESS;
  status = clReleaseKernel(kernel_);
  good = static_cast<bool>(this->check_val(status,CL_SUCCESS,"clReleaseKernel failed."));
  return good;
}


/************************ OpenCL Helpers ******************************/

//: Sets kernel arguments
int boxm_refine_scene_manager::set_kernel_args()
{
  //Refine args:
  // block_ptrs
  // num blocksn
  // num buffers
  // length buffers
  // tree cells
  // data cells
  // mem pointers
  // prob thresh
  // max level
  // bbox len
  cl_int status = CL_SUCCESS;
  int i = 0;


  // ----- Set appropriate arguments to the kernel ----
  //block pointers and block nums in each dimension
  status = clSetKernelArg(kernel_, i++, sizeof(cl_mem), (void*) &block_ptrs_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (block_ptrs buffer)"))
    return SDK_FAILURE;
  status = clSetKernelArg(kernel_, i++, sizeof(cl_mem), (void*) &block_nums_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (block_nums buffer)"))
    return SDK_FAILURE;

  //num buffers, length of buffers
  status = clSetKernelArg(kernel_, i++, sizeof(cl_int), &numbuffer_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (numbuffer_ buffer)"))
    return SDK_FAILURE;
  status = clSetKernelArg(kernel_, i++, sizeof(cl_int), &lenbuffer_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (lenbuffer_ buffer)"))
    return SDK_FAILURE;

  //tree cells, data cells cells
  status = clSetKernelArg(kernel_, i++, sizeof(cl_mem), (void*) &tree_cells_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (tree_cells_buf_ buffer)"))
    return SDK_FAILURE;
  status = clSetKernelArg(kernel_, i++, sizeof(cl_mem), (void*) &data_cells_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (data_cells_buf_ buffer)"))
    return SDK_FAILURE;

  //mem pointers for the tree cells
  status = clSetKernelArg(kernel_, i++, sizeof(cl_mem), (void*) &mem_ptrs_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (data buffer)"))
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
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local tree buffer)"))
    return false;
  //---- end local memory arguments

  //IO ARGUMENT
  status = clSetKernelArg(kernel_, i++, sizeof(cl_mem), (void*) &output_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output_Buff buffer)"))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

bool boxm_refine_scene_manager::init_kernel()
{
  //load kernel source main
  if (!this->load_kernel_source(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    +"/contrib/brl/bseg/boxm/ocl/cl/refine_blocks.cl")) {
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

//: build_kernel_program - builds kernel program from source (a vcl string)
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
