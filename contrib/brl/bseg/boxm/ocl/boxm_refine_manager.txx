#ifndef boxm_refine_manager_txx_
#define boxm_refine_manager_txx_
//:
// \file
#include "boxm_refine_manager.h"
#include <vcl_where_root_dir.h>
#include <boxm/ocl/boxm_ocl_utils.h>
#include <vcl_cstdio.h>
#include <vgl/vgl_box_3d.h>

//include for timing
#include <vul/vul_timer.h>


//: Initializes CPU side input buffers
//put tree structure and data into arrays
//initializes (cl_int*) cells_ and (cl_float*) cell_data_
// also initializes (cl_int*) tree_results_ and (cl_float*) data_results_
template<class T>
bool boxm_refine_manager<T>::init(tree_type *tree, float prob_thresh)
{
  //set the scene (this also allocates all host variables)
  vcl_cout<<"INITIALIZING REFINE MANAGER-----------------------------"<<vcl_endl;
  format_tree(tree);
  (*prob_thresh_) = prob_thresh;
  (*max_level_) = tree->number_levels();

  //set bounding box length for integrating alpha
  vgl_box_3d<double> bbox = tree->bounding_box();
  (*bbox_len_) = (float) (bbox.max_z() - bbox.min_z());
  return init_kernel();
}

template<class T>
bool boxm_refine_manager<T>::
init(int* cells, unsigned numcells, unsigned tree_max_size,
     float* data, unsigned data_size, unsigned data_max_size,
     float prob_thresh, unsigned max_level, float bbox_len)
{
  alloc_trees(tree_max_size, data_size);
  cells_ = cells;
  (*numcells_) = numcells;
  (*tree_max_size_) = tree_max_size;
  cell_data_ = data;
  (*numdata_) = data_size;
  (*data_max_size_) = data_max_size;
  (*prob_thresh_) = prob_thresh;
  (*max_level_) = max_level;
  (*bbox_len_) = bbox_len;
  return init_kernel();
}

template<class T>
bool boxm_refine_manager<T>::init_kernel()
{
  //load kernel source main
  if (!this->load_kernel_source(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    +"/contrib/brl/bseg/boxm/ocl/refine_main_opt.cl")) {
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
template<class T>
bool boxm_refine_manager<T>::run_tree()
{
  vcl_cout<<"REFINING TREE-----------------------------"<<vcl_endl;
  //allocate and initialize memory on gpu side
  vcl_cout<<"---setting up buffers"<<vcl_endl;
  cl_int status =  setup_tree_buffers();
  if (!this->check_val(status,CL_SUCCESS,"setup_tree_buffers failed"))
    return false;

  float numMB = (16*(*tree_max_size_)+4*16*(*data_max_size_))/(1024.0f*1024.0f);
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
  vcl_cout<<"KERNEL OUTPUT"<<vcl_endl
          <<(*output_results_)<<vcl_endl
          <<"END KERNEL OUTPUT"<<vcl_endl;

  //debug print method
  vcl_cout<<"---REFINE Stats:-----------------------------------"<<vcl_endl
          <<"---Tree Input Size (#cells) = "<<(*numcells_)<<vcl_endl
          <<"---Tree Output Size (#cells) = "<<(*tree_results_size_)<<vcl_endl;
  int numSplit = ((*tree_results_size_)-(*numcells_))/8;
  vcl_cout<<"---number of nodes that split = "<<numSplit<<vcl_endl
          <<"----------------------------------------------------"<<vcl_endl;

  //boxm_ocl_utils<T>::print_tree_array(tree_results_, (*tree_results_size_), data_results_);

  // Verify that the tree is formatted correctly
  vcl_vector<vnl_vector_fixed<int,4> > tree_vector;
  for (int i=0,j=0; j<(*tree_results_size_); i+=4,j++) {
    vnl_vector_fixed<int,4> cell;
    for (unsigned k=0; k<4; k++) 
      cell[k] = tree_results_[i+k];
    tree_vector.push_back(cell);
  }
  if(boxm_ocl_utils::verify_format(tree_vector))
    vcl_cout<<"---TREE IN CORRECT FORMAT---"<<vcl_endl;
  else
    vcl_cout<<"---TREE NOT IN CORRECT FORMAT ---"<<vcl_endl;
  

  //PROFILING INFORMATION FROM OPENCL
#if 1
  float treeSize = 4*4*(*tree_results_size_)/(1024.0f*1024.0f); //tree size in MB
  float dataSize = 4*16*(*data_results_size_)/(1024.0f*1024.0f); //data size in MBs
  vcl_cout<<"---GLOBAL MEM BANDWITH RESULTS-----------------------"<<vcl_endl
          <<"---Tree Size: "<<(*tree_results_size_)<<" blocks; "<<treeSize<<" MB"<<vcl_endl
          <<"---Data size: "<<(*data_results_size_)<<" blocks; "<<dataSize<<" MB"<<vcl_endl
          <<"---GPU Time: "<<gpu_time<<" seconds"<<vcl_endl
          <<"---Refine Bandwidth ~~ "<<(treeSize+dataSize)/gpu_time<<" MB/sec"<<vcl_endl
          <<"-----------------------------------------------------"<<vcl_endl;
#endif // 0

#if 0
  vcl_cout<<"VUL_TIMER: Global mem BANDWITH RESULTS"<<vcl_endl
          <<"Size "<<16*(*tree_max_size_)<<" bytes in "<<gpu_time<<"sec"<<vcl_endl;
  float rate = (16.0*(*tree_max_size_))/gpu_time;
  vcl_cout<<" = "<<rate<<" bytes/sec"<<vcl_endl
          <<" = "<<rate/vcl_pow(2,20)<<" megabytes/sec"<<vcl_endl;
  boxm_ocl_utils<float>::print_tree_array(tree_results_, (*tree_results_size_), data_results_);
#endif // 0

  return true;
}

//: refines the tree after initialization
template<class T>
bool boxm_refine_manager<T>::run_block()
{
  cl_int status = CL_SUCCESS;

  // ----- Set appropriate arguments to the kernel ----
  // tree buffer
  status = clSetKernelArg(kernel_, 0, sizeof(cl_mem), (void*) &cell_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (octree buffer)"))
    return SDK_FAILURE;
  // number of tree cells
  status = clSetKernelArg(kernel_, 1, sizeof(cl_mem),(void*) &cell_size_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (octree buffer)"))
    return SDK_FAILURE;
  // tree max size
  status = clSetKernelArg(kernel_, 2, sizeof(cl_mem), (void*) &cell_max_size_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (max_size buffer)"))
    return SDK_FAILURE;
  // data buffer
  status = clSetKernelArg(kernel_, 3, sizeof(cl_mem), (void*) &data_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (data buffer)"))
    return SDK_FAILURE;
  // data size
  status = clSetKernelArg(kernel_, 4, sizeof(cl_mem), (void*) &data_size_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (data_size buffer)"))
    return SDK_FAILURE;
  //data max sizeof
  status = clSetKernelArg(kernel_, 5, sizeof(cl_mem), (void*) &data_max_size_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (data_max_size buffer)"))
    return SDK_FAILURE;
  //probability threshold
  status = clSetKernelArg(kernel_, 6, sizeof(cl_mem), (void*) &prob_thresh_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (prob_thresh buffer)"))
    return SDK_FAILURE;
  //max level
  status = clSetKernelArg(kernel_, 7, sizeof(cl_mem), (void*) &max_level_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (max_level_) buffer)"))
    return SDK_FAILURE;
  //bbox length
  status = clSetKernelArg(kernel_, 8, sizeof(cl_mem), (void*) &bbox_len_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (bbox_len_) buffer)"))
    return SDK_FAILURE;
  // ----- end kernel arguments ---

  //IO ARGUMENT
  status = clSetKernelArg(kernel_, 9, sizeof(cl_mem), (void*) &output_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output_Buff buffer)"))
    return SDK_FAILURE;
  ////


  // check the local memeory
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
                                  1,NULL,
                                  globalThreads,localThreads,
                                  0,NULL,&ceEvent);
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
template<class T>
int boxm_refine_manager<T>::setup_tree_buffers()
{
  cl_int status = CL_SUCCESS;

  //---- create and initialize memory objects -----
  //tree array (cells_)
  cell_buf_ = clCreateBuffer(this->context_,  //write output_
                             CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                             (*tree_max_size_) * sizeof(cl_int4),
                             cells_,
                             &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (cell_array) failed."))
    return SDK_FAILURE;

  //tree SIZE
  cell_size_buf_ = clCreateBuffer(this->context_,
                                  CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                                  sizeof(cl_uint),
                                  numcells_,
                                  &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (tree_size_) failed."))
    return SDK_FAILURE;

  //tree max SIZE
  cell_max_size_buf_ = clCreateBuffer(this->context_,
                                      CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                                      sizeof(cl_uint),
                                      tree_max_size_,
                                      &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (tree_max_size_) failed."))
    return SDK_FAILURE;

  //data array (cell_data_)
  data_buf_ = clCreateBuffer(this->context_,
                             CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                             (*data_max_size_) * sizeof(cl_float16),
                             cell_data_,
                             &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (cell_data) failed."))
    return SDK_FAILURE;

  //data SIZE
  data_size_buf_ = clCreateBuffer(this->context_,
                                  CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                                  sizeof(cl_uint),
                                  numdata_,
                                  &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (data_size_buff_) failed."))
    return SDK_FAILURE;

  //data max size
  data_max_size_buf_ = clCreateBuffer(this->context_,
                                      CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                                      sizeof(cl_uint),
                                      data_max_size_,
                                      &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (data_max_size_buff_) failed."))
    return SDK_FAILURE;

  //probability threshold buffer
  prob_thresh_buf_ = clCreateBuffer(this->context_,
                                    CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                                    sizeof(cl_float),
                                    prob_thresh_,
                                    &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (prob_thresh_) failed."))
    return SDK_FAILURE;

  //max level buffer
  max_level_buf_ = clCreateBuffer(this->context_,
                                  CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                                  sizeof(cl_uint),
                                  max_level_,
                                  &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (max_level_) failed."))
    return SDK_FAILURE;

  //bbox len buffer
  bbox_len_buf_ = clCreateBuffer(this->context_,
                                 CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                                 sizeof(cl_float),
                                 bbox_len_,
                                 &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (bbox_len_) failed."))
    return SDK_FAILURE;

  //output buffer
  output_buf_ = clCreateBuffer(this->context_,
                               CL_MEM_READ_WRITE|CL_MEM_COPY_HOST_PTR,
                               sizeof(cl_float),
                               output_input_,
                               &status);
  if (!this->check_val(status, CL_SUCCESS, "clCreateBuffer (output_) failed."))
    return SDK_FAILURE;

  return SDK_SUCCESS;
}

/*********************************************************
 * read_tree_buffers() - reads gpu results into
 * (cl_int*) tree_results_ and (cl_float*) data_results_
 *********************************************************/
template<class T>
bool boxm_refine_manager<T>::read_tree_buffers()
{
  cl_event events[5];

  //Read cell_buf_
  int status = clEnqueueReadBuffer(command_queue_, cell_buf_, CL_TRUE,
                                   0,  (*tree_max_size_) * sizeof(cl_int4),
                                   tree_results_,
                                   0, NULL, &events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (tree_results_)failed."))
    return false;

  // read number of cells
  status = clEnqueueReadBuffer(command_queue_, cell_size_buf_, CL_TRUE,
                               0, sizeof(cl_uint),
                               tree_results_size_,
                               0, NULL, &events[1]);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (tree_results_size_)failed."))
    return false;

  //read data_size before the array
  status = clEnqueueReadBuffer(command_queue_, data_size_buf_, CL_TRUE,
                               0,  sizeof(cl_uint),
                               data_results_size_,
                               0, NULL, &events[2]);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (tree_results_)failed."))
    return false;

  //read data_buf_
  status = clEnqueueReadBuffer(command_queue_, data_buf_, CL_TRUE,
                               0, (*data_results_size_) * sizeof(cl_float16),
                               data_results_,
                               0,NULL,&events[3]);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (data_results_)failed."))
    return false;

  //read output_buf_
  status = clEnqueueReadBuffer(command_queue_, output_buf_, CL_TRUE,
                               0, sizeof(cl_float),
                               output_results_,
                               0,NULL,&events[4]);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (output_results_)failed."))
    return false;


  // Wait for the read buffer to finish execution
  status = clWaitForEvents(5, events);
  if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
    return false;

  //release events? all of em or just 0?
  for (int i=0; i<5; i++) {
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

template<class T>
int boxm_refine_manager<T>::clean_tree_buffers()
{
  // Releases OpenCL resources (Context, Memory etc.)
  cl_int status;
  status = clReleaseMemObject(cell_buf_);
  if (!this->check_val(status, CL_SUCCESS, "clReleaseMemObject failed (cell_buf_)."))
    return SDK_FAILURE;

  status = clReleaseMemObject(cell_size_buf_);
  if (!this->check_val(status, CL_SUCCESS, "clReleaseMemObject failed (cell_size_buf_)."))
    return SDK_FAILURE;

  status = clReleaseMemObject(cell_max_size_buf_);
  if (!this->check_val(status, CL_SUCCESS, "clReleaseMemObject failed (cell_max_size_buf_)."))
    return SDK_FAILURE;

  status = clReleaseMemObject(data_buf_);
  if (!this->check_val(status, CL_SUCCESS, "clReleaseMemObject failed (data_buf_)."))
    return SDK_FAILURE;

  status = clReleaseMemObject(data_size_buf_);
  if (!this->check_val(status, CL_SUCCESS, "clReleaseMemObject failed (data_size_buf_)."))
    return SDK_FAILURE;

  status = clReleaseMemObject(data_max_size_buf_);
  if (!this->check_val(status, CL_SUCCESS, "clReleaseMemObject failed (data_max_size_buf_)."))
    return SDK_FAILURE;

  status = clReleaseMemObject(prob_thresh_buf_);
  if (!this->check_val(status, CL_SUCCESS, "clReleaseMemObject failed (prob_thresh_buf_)."))
    return SDK_FAILURE;

  status = clReleaseMemObject(max_level_buf_);
  if (!this->check_val(status, CL_SUCCESS, "clReleaseMemObject failed (max_level_buf_)."))
    return SDK_FAILURE;

  status = clReleaseMemObject(bbox_len_buf_);
  if (!this->check_val(status, CL_SUCCESS, "clReleaseMemObject failed (bbox_len_buf_)."))
    return SDK_FAILURE;   
    
  //output buffer
  status = clReleaseMemObject(output_buf_);
  if (!this->check_val(status, CL_SUCCESS, "clReleaseMemObject failed (output_buf_)."))
    return SDK_FAILURE;

  return SDK_SUCCESS;
}


/*******************************************************************
 * format_tree(tree) and clean_tree()
 * takes in octree and puts it into two arrays:
 * cells_ (cl_int*) and cell_data_ (cl_float*)
 * clean tree releases memory allocated
 *******************************************************************/
template<class T>
bool boxm_refine_manager<T>::format_tree(tree_type* tree)
{
  if (!tree)
    return false;
  boct_tree_cell<short, T >* root = tree->root();
  if (!root)
    return false;

  //use vectors to build the tree up
  vcl_vector<vnl_vector_fixed<int, 4> > cell_input;
  vcl_vector<vnl_vector_fixed<float, 16>  > data_input;

  // put the root into the cell array and its data in the data array
  int cell_ptr = 0;
  vnl_vector_fixed<int, 4> root_cell(0);
  root_cell[0]=-1; // no parent
  root_cell[1]=-1; // no children at the moment
  root_cell[2]=-1; // no data at the moment
  cell_input.push_back(root_cell);
  boxm_ocl_convert<T>::copy_to_arrays(root, cell_input, data_input, cell_ptr);

  // the tree is now resident in the 1-d vectors
  // cell_input_ as vnl_vector_fixed<int, 4> and
  // data_input_ as vnl_vector_fixed<float, 2>

  //need to allocate an array with some fixed size (because the GPU cannot allocate memory)
  unsigned num_cells = cell_input.size();
  unsigned cell_max_size = (unsigned) (5*cell_input.size());
  unsigned data_max_size = (unsigned) (5*data_input.size());


  //allocate host memory
  alloc_trees(cell_max_size, data_max_size);

  //---- Transfer data into allocated memory buffers (cpu side) ----
  //Transfer data from cell_input_ vector to cells_ array
  for (unsigned i = 0, j = 0; i<cell_input.size()*4; i+=4, j++)
    for (unsigned k = 0; k<4; ++k)
      cells_[i+k]=cell_input[j][k];
  for (unsigned i=cell_input.size()*4; i<cell_max_size*4; i++)
    cells_[i] = -1; //make sure the uninitialized cells point to null values

  // note that the cell data pointer cells[i+2] does not correspond to the 1-d
  // data array location. It must be mapped as:
  //  cell_data indices = 2*cell_data_ptr, 2*cell_data_ptr +1,
  unsigned cell_data_size=16;
  for (unsigned i = 0, j = 0; i<data_input.size()*cell_data_size; i+=cell_data_size, j++)
    for (unsigned k = 0; k<cell_data_size; ++k)
      cell_data_[i+k]=data_input[j][k];
  for (unsigned i=data_input.size()*16; i<data_max_size*16; i++)
    cell_data_[i]=0;

  (*tree_max_size_) = cell_max_size;
  (*numcells_) = num_cells;
  (*numdata_) = data_input.size();
  (*data_max_size_) = data_max_size;

  return true;
}

//: allocates host memory for cells_, cell_data_, numlevels_ and tree_max_size_
template<class T>
bool boxm_refine_manager<T>::alloc_trees(int cells_size, int data_input_size)
{
  //make sure trees aren't allocated yet...
  //this->free_trees();

  //---- allocate input buffers ----//
  //allocate tree structure
  cells_ =             (cl_int*) boxm_ocl_utils::alloc_aligned(cells_size, sizeof(cl_int4), 16);
  numcells_ =         (cl_uint*) boxm_ocl_utils::alloc_aligned(1, sizeof(cl_uint), 16);
  tree_max_size_ =    (cl_uint*) boxm_ocl_utils::alloc_aligned(1, sizeof(cl_uint), 16);
  //allocate data array
  cell_data_ =       (cl_float*) boxm_ocl_utils::alloc_aligned(data_input_size, sizeof(cl_float16), 16);
  numdata_ =          (cl_uint*) boxm_ocl_utils::alloc_aligned(1, sizeof(cl_uint), 16);
  data_max_size_ =    (cl_uint*) boxm_ocl_utils::alloc_aligned(1, sizeof(cl_uint), 16);
  //output results
  tree_results_ =      (cl_int*) boxm_ocl_utils::alloc_aligned(cells_size, sizeof(cl_int4), 16);
  data_results_ =    (cl_float*) boxm_ocl_utils::alloc_aligned(data_input_size, sizeof(cl_float16), 16);
  data_results_size_ =(cl_uint*) boxm_ocl_utils::alloc_aligned(1, sizeof(cl_uint), 16);
  tree_results_size_ =(cl_uint*) boxm_ocl_utils::alloc_aligned(1, sizeof(cl_uint), 16);
  //probability threshold
  prob_thresh_ =     (cl_float*) boxm_ocl_utils::alloc_aligned(1, sizeof(cl_float), 16);
  max_level_ =        (cl_uint*) boxm_ocl_utils::alloc_aligned(1, sizeof(cl_uint), 16);
  bbox_len_ =        (cl_float*) boxm_ocl_utils::alloc_aligned(1, sizeof(cl_float),16);

  //TODO remove output stuff
  output_results_ = (cl_float*) boxm_ocl_utils::alloc_aligned(1, sizeof(cl_float), 16);
  output_input_ = (cl_float*) boxm_ocl_utils::alloc_aligned(1, sizeof(cl_float), 16);
  (*output_input_) = 9876;
  //TODO remove me ^^^

  if (cells_== NULL||cell_data_ == NULL)
  {
    vcl_cout << "Failed to allocate host memory. (tree input)\n";
    return false;
  }
  return true;
}

//: Cleans up the tree in computer memory (cells_ and cell_data_, tree_max_size_, tree_results_)
template<class T>
bool boxm_refine_manager<T>::free_trees()
{
  boxm_ocl_utils::free_aligned(cells_);
  boxm_ocl_utils::free_aligned(numcells_);
  boxm_ocl_utils::free_aligned(cell_data_);
  boxm_ocl_utils::free_aligned(data_max_size_);
  boxm_ocl_utils::free_aligned(numdata_);
  boxm_ocl_utils::free_aligned(tree_max_size_);
  boxm_ocl_utils::free_aligned(tree_results_);
  boxm_ocl_utils::free_aligned(tree_results_size_);
  boxm_ocl_utils::free_aligned(data_results_);
  boxm_ocl_utils::free_aligned(data_results_size_);
  boxm_ocl_utils::free_aligned(prob_thresh_);
  boxm_ocl_utils::free_aligned(max_level_);
  boxm_ocl_utils::free_aligned(bbox_len_);

  //TODO remove output stuff
  boxm_ocl_utils::free_aligned(output_results_);
  boxm_ocl_utils::free_aligned(output_input_);
  //TODO remove me^^^^

  return true;
}


/*****************************************
 * Cleanup refine_manager
 *****************************************/
template<class T>
bool boxm_refine_manager<T>::clean_refine()
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

/*******************************************
 * build_kernel_program - builds kernel program
 * from source (a vcl string)
 *******************************************/
template<class T>
int boxm_refine_manager<T>::build_kernel_program()
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

/*****************************************
 *macro for template instantiation
 *****************************************/
#define BOXM_REFINE_MANAGER_INSTANTIATE(T) \
  template class boxm_refine_manager<T >

#endif    //boxm_refine_manager_txx_

