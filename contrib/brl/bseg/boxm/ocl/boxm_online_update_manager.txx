#ifndef boxm_online_update_manager_txx_
#define boxm_online_update_manager_txx_
//:
// \file
#include "boxm_online_update_manager.h"
#include <vcl_where_root_dir.h>
#include <boxm/ocl/boxm_ocl_utils.h>
#include <vcl_cstdio.h>
#include <vul/vul_timer.h>
#include <boxm/boxm_block.h>
#include <boxm/boxm_scene.h>
#include <boxm/util/boxm_utils.h>
#include <boxm/basic/boxm_block_vis_graph_iterator.h>
#include <vpgl/vpgl_perspective_camera.h>

//: Initializes CPU side input buffers
//put tree structure and data into arrays
template<class T>
bool boxm_online_update_manager<T>::init_update(boxm_scene<boct_tree<short,T > > *scene,
                                                vpgl_camera_double_sptr cam,
                                                vil_image_view<float> &obs)
{
  scene_ = scene;
  cam_ = cam;
  input_img_=obs;

  //: Code for Pass_0
  if (!this->load_kernel_source(vcl_string(VCL_SOURCE_ROOT_DIR)
                                +"/contrib/brl/bseg/boxm/ocl/octree_library_functions.cl") ||
      !this->append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    +"/contrib/brl/bseg/boxm/ocl/backproject.cl")||
      !this->append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    +"/contrib/brl/bseg/boxm/ocl/ray_bundle_library_functions.cl")||
      !this->append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    +"/contrib/brl/bseg/boxm/ocl/update_main.cl")) {
    vcl_cerr << "Error: boxm_ray_trace_manager : failed to load kernel source (helper functions)\n";
    return false;
  }

  return !build_kernel_program(program_pass0_);
}


//: update the tree

template<class T>
bool boxm_online_update_manager<T>::run_block()
{
  cl_int status = CL_SUCCESS;
  kernel_ = clCreateKernel(program_pass0_,"update_aux",&status);
  if (!this->check_val(status,CL_SUCCESS,error_to_string(status))) {
    return false;
  }
  // -- Set appropriate arguments to the kernel --
  int i=0;
  status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&root_level_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (root_level_buf_)"))
    return SDK_FAILURE;

  // the tree buffer
  status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&cells_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cells_buf_)"))
    return SDK_FAILURE;

  // data buffer
  status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&cell_data_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_data_buf_)"))
    return SDK_FAILURE;
  // aux data buffer
  status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&cell_aux_data_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_aux_data_buf_)"))
    return SDK_FAILURE;
  // ca,era buffer
  status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&persp_cam_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (data)"))
    return SDK_FAILURE;
  status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&tree_bbox_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (ROI dimensions)"))
    return SDK_FAILURE;
  // roi dimensions
  status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&img_dims_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (Img dimensions)"))
    return SDK_FAILURE;
  // output image buffer
  status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&image_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (input_image)"))
    return SDK_FAILURE;
  set_offset_buffers(0,0);

  //  offset buffer
  status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&offset_x_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local cam)"))
    return SDK_FAILURE;
  status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&offset_y_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local cam)"))
    return SDK_FAILURE;
  status = clSetKernelArg(kernel_,i++,sizeof(cl_float4),0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local origin)"))
    return SDK_FAILURE;
  status = clSetKernelArg(kernel_,i++,3*sizeof(cl_float16),0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local box)"))
    return SDK_FAILURE;
  status = clSetKernelArg(kernel_,i++,sizeof(cl_uint4),0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local roi)"))
    return SDK_FAILURE;
  status = clSetKernelArg(kernel_,i++,sizeof(cl_uchar4)*this->bni_*this->bnj_,0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local cache ptr bundle)"))
    return SDK_FAILURE;
  status = clSetKernelArg(kernel_,i++,sizeof(cl_short4)*this->bni_*this->bnj_,0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (loc code bundle)"))
    return SDK_FAILURE;
  status = clSetKernelArg(kernel_,i++,sizeof(cl_float16)*this->bni_*this->bnj_,0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (loc code bundle)"))
    return SDK_FAILURE;
  status = clSetKernelArg(kernel_,i++,sizeof(cl_float4)*this->bni_*this->bnj_,0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (loc code bundle)"))
    return SDK_FAILURE;
  status = clSetKernelArg(kernel_,i++,sizeof(cl_float4)*this->bni_*this->bnj_,0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (loc code bundle)"))
    return SDK_FAILURE;
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

  vcl_size_t globalThreads[]= {this->wni_,this->wnj_};
  vcl_size_t localThreads[] = {this->bni_,this->bnj_};

  if (used_local_memory > this->total_local_memory())
  {
    vcl_cout << "Unsupported: Insufficient local memory on device.\n";
    return SDK_FAILURE;
  }

  // set up a command queue
  command_queue_ = clCreateCommandQueue(this->context(),this->devices()[0],CL_QUEUE_PROFILING_ENABLE,&status);
  if (!this->check_val(status,CL_SUCCESS,"Falied in command queue creation" + error_to_string(status)))
    return false;

  cl_event ceEvent;
  status = clEnqueueNDRangeKernel(command_queue_,this->kernel_, 2,NULL,globalThreads,localThreads,0,NULL,&ceEvent);

  if (!this->check_val(status,CL_SUCCESS,"clEnqueueNDRangeKernel failed. "+error_to_string(status)))
    return SDK_FAILURE;

  status = clFinish(command_queue_);
  if (!this->check_val(status,CL_SUCCESS,"clFinish failed."+error_to_string(status)))
    return SDK_FAILURE;
  cl_ulong tstart,tend;
  status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_END,sizeof(cl_ulong),&tend,0);
  status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),&tstart,0);
  //  gpu_time_+= (double)1.0e-6 * (tend - tstart); // convert nanoseconds to milliseconds
  release_offset_buffers();

  return SDK_SUCCESS;
}

template<class T>
bool boxm_online_update_manager<T>::run_scene()
{
  cl_int status = CL_SUCCESS;
  bool good=true;
  vcl_string error_message="";
  vul_timer timer;
  good=good && set_scene_data();
  good=good && set_scene_data_buffers();
  good=good && set_input_view();
  good=good && set_input_view_buffers();

  boxm_block_vis_graph_iterator<tree_type > block_vis_iter(cam_, scene_, wni_, wnj_);
  float total_raytrace_time = 0.0f;
  float total_gpu_time = 0.0f;
  float total_load_time = 0.0f;
  while (block_vis_iter.next())
  {
    vcl_vector<vgl_point_3d<int> > block_indices = block_vis_iter.frontier_indices();
    for (unsigned i=0; i<block_indices.size(); i++) // code for each block
    {
      timer.mark();
      scene_->load_block(block_indices[i]);
      boxm_block<tree_type> * curr_block=scene_->get_active_block();
      float load_time = (float)timer.all() / 1e3f;
      vcl_cout << "loading block took " << load_time << 's' << vcl_endl;
      total_load_time += load_time;
      vcl_cout << "processing block at index (" <<block_indices[i] << ')' << vcl_endl;
      timer.mark();
      // make sure block projects to inside of image
      vgl_box_3d<double> block_bb = curr_block->bounding_box();

      if (!boxm_utils::is_visible(block_bb, cam_, wni_, wnj_))
        continue;

      tree_type * tree=curr_block->get_tree();
      good =good && set_tree(tree);
      good =good && set_tree_buffers();
      // run the raytracing for this block
      run_block();
      // release memory


      this->read_trees();
      this->print_tree();
      good =good && release_tree_buffers();
      good =good && clean_tree();


      float raytrace_time = (float)timer.all() / 1e3f;
      vcl_cout<<"processing block took " << raytrace_time << 's' << vcl_endl;
      total_raytrace_time += raytrace_time;
    }
  }

  good=good && read_output_image();

  good=good && release_input_view_buffers();
  //good=good && clean_input_view();

  good=good && release_scene_data_buffers();
  good=good && clean_scene_data();

  // release the command Queue
  status = clReleaseCommandQueue(command_queue_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseCommandQueue failed."))
    return false;

  vcl_cout << "Timing Analysis "<<vcl_endl
           << "==============="<<vcl_endl
//         <<"openCL Running time "<<gpu_time_<<" ms"<<vcl_endl
           << "Running block "<<total_gpu_time/1000<<'s'<<vcl_endl
           << "total block loading time = " << total_load_time << 's' << vcl_endl
           << "total block processing time = " << total_raytrace_time << 's' << vcl_endl;
  return true;
}

template<class T>
bool boxm_online_update_manager<T>:: read_output_image()
{
  cl_event events[2];

  // Enqueue readBuffers
  int status = clEnqueueReadBuffer(command_queue_,image_buf_,CL_TRUE,
                                   0,this->wni_*this->wnj_*sizeof(cl_float4),
                                   image_,
                                   0,NULL,&events[0]);

  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (image_)failed."))
    return false;

  // Wait for the read buffer to finish execution
  status = clWaitForEvents(1, &events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
    return false;

  status = clReleaseEvent(events[0]);
  return this->check_val(status,CL_SUCCESS,"clReleaseEvent failed.");
}

template<class T>
bool boxm_online_update_manager<T>:: read_trees()
{
  cl_event events[2];

  // Enqueue readBuffers
  int status = clEnqueueReadBuffer(command_queue_,cells_buf_,CL_TRUE,
                                   0,cells_size_*sizeof(cl_int4),
                                   cells_,
                                   0,NULL,&events[0]);

  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (cells )failed."))
    return false;
  status = clWaitForEvents(1, &events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
    return false;

  status = clEnqueueReadBuffer(command_queue_,cell_data_buf_,CL_TRUE,
                               0,cell_data_size_*sizeof(cl_float16),
                               cell_data_,
                               0,NULL,&events[0]);

  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (cell data )failed."))
    return false;
  status = clWaitForEvents(1, &events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
    return false;
  status = clEnqueueReadBuffer(command_queue_,cell_aux_data_buf_,CL_TRUE,
                               0,cell_data_size_*sizeof(cl_float4),
                               cell_aux_data_,
                               0,NULL,&events[0]);

  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (cell aux data )failed."))
    return false;
  status = clWaitForEvents(1, &events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
    return false;
  // Wait for the read buffer to finish execution

  status = clReleaseEvent(events[0]);
  return this->check_val(status,CL_SUCCESS,"clReleaseEvent failed.");
}

template<class T>
void boxm_online_update_manager<T>::print_tree()
{
  vcl_cout << "Tree Input\n";
  if (cells_)
    for (unsigned i = 0; i<cells_size_*4; i+=4) {
      int data_ptr = 16*cells_[i+2];
      int aux_data_ptr = 4*cells_[i+2];
      vcl_cout << "tree input[" << i/4 << "]("
               << cells_[i]   << ' '
               << cells_[i+1] << ' '
               << cells_[i+2] << ' '
               << cells_[i+3];
      if (data_ptr>0)
        vcl_cout << '[' << cell_data_[data_ptr] << ','
                 << cell_data_[data_ptr+1] << ','
                 << cell_data_[data_ptr+2] << ','
                 << cell_data_[data_ptr+3] << ','
                 << cell_data_[data_ptr+4] << ','
                 << cell_data_[data_ptr+5] << ','
                 << cell_data_[data_ptr+6] << ','
                 << cell_data_[data_ptr+7] << ','
                 << cell_data_[data_ptr+8] << ','
                 << cell_data_[data_ptr+9] << ','
                 << cell_data_[data_ptr+10] << ','
                 << cell_data_[data_ptr+11] << ','
                 << cell_data_[data_ptr+12] << ','
                 << cell_data_[data_ptr+13] << ','
                 << cell_data_[data_ptr+14] << ','
                 << cell_data_[data_ptr+15] << ']';

      if (aux_data_ptr>0)
        vcl_cout << '[' << cell_aux_data_[aux_data_ptr] << ','
                 << cell_aux_data_[aux_data_ptr+1] << ','
                 << cell_aux_data_[aux_data_ptr+1] << ','
                 << cell_aux_data_[aux_data_ptr+1] << ']';
      vcl_cout << ")\n";
    }
}

template<class T>
bool boxm_online_update_manager<T>::clean_update()
{
  return true;
}

/*******************************************
 * build_kernel_program - builds kernel program
 * from source (a vcl string)
 *******************************************/
template<class T>
int boxm_online_update_manager<T>::build_kernel_program(cl_program & program)
{
  cl_int status = CL_SUCCESS;
  vcl_size_t sourceSize[] = { this->prog_.size() };
  if (!sourceSize[0]) return SDK_FAILURE;
  if (program) {
    status = clReleaseProgram(program);
    program = 0;
    if (!this->check_val(status,
      CL_SUCCESS,
      "clReleaseProgram failed."))
      return SDK_FAILURE;
  }
  const char * source = this->prog_.c_str();

  program = clCreateProgramWithSource(this->context_,
                                      1,
                                      &source,
                                      sourceSize,
                                      &status);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       "clCreateProgramWithSource failed."))
    return SDK_FAILURE;

  // create a cl program executable for all the devices specified
  status = clBuildProgram(program,
                          1,
                          this->devices_,
                          "",
                          NULL,
                          NULL);
  if (!this->check_val(status,
                       CL_SUCCESS,
                       error_to_string(status)))
  {
    vcl_size_t len;
    char buffer[2048];
    clGetProgramBuildInfo(program, this->devices_[0],
                          CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
    vcl_printf("%s\n", buffer);
    return SDK_FAILURE;
  }
  else
    return SDK_SUCCESS;
}

template<class T>
bool boxm_online_update_manager<T>::set_scene_data()
{
  return set_root_level();
}

template<class T>
bool boxm_online_update_manager<T>::clean_scene_data()
{
  return clean_root_level();
}

template<class T>
bool boxm_online_update_manager<T>::set_scene_data_buffers()
{
  return set_root_level_buffers();
}

template<class T>
bool boxm_online_update_manager<T>::release_scene_data_buffers()
{
  return release_root_level_buffers();
}

template<class T>
bool boxm_online_update_manager<T>::set_root_level()
{
  if (scene_==NULL)
  {
    vcl_cout<<"Scene is Missing "<<vcl_endl;
    return false;
  }
  root_level_=scene_->max_level()-1;
  return true;
}

template<class T>
bool boxm_online_update_manager<T>::clean_root_level()
{
  root_level_=0;
  return true;
}

template<class T>
bool boxm_online_update_manager<T>::set_root_level_buffers()
{
  cl_int status;
  root_level_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                   sizeof(cl_uint),
                                   &root_level_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (root level) failed.");
}

template<class T>
bool boxm_online_update_manager<T>::release_root_level_buffers()
{
  cl_int status;
  status = clReleaseMemObject(root_level_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (root_level_buf_)."))
    return false;
  return true;
}

template<class T>
bool boxm_online_update_manager<T>::set_input_view()
{
  bool good = true;
  good = good && set_persp_camera();
  good = good && set_input_image();
  return good;
}

template<class T>
bool boxm_online_update_manager<T>::clean_input_view()
{
  bool good = true;
  good = good && clean_persp_camera();
  good = good && clean_input_image();
  return good;
}

template<class T>
bool boxm_online_update_manager<T>::set_input_view_buffers()
{
  bool good = true;
  good = good && set_persp_camera_buffers();
  good = good && set_input_image_buffers();
  return good;
}

template<class T>
bool boxm_online_update_manager<T>::release_input_view_buffers()
{
  bool good = true;
  good = good && release_persp_camera_buffers();
  good = good && release_input_image_buffers();
  return good;
}

template<class T>
bool boxm_online_update_manager<T>::set_tree(tree_type* tree)
{
  vcl_vector<vnl_vector_fixed<int, 4> > cell_input_;
  vcl_vector<vnl_vector_fixed<float, 16>  > data_input_;

  cell_type * root = tree->root();
  //nlevels_=root->level()+1;
  if (!root)
    return false;

  int cell_ptr = 0;
  // put the root into the cell array and its data in the data array
  vnl_vector_fixed<int, 4> root_cell(0);
  root_cell[0]=-1; // no parent
  root_cell[1]=-1; // no children at the moment
  root_cell[1]=-1; // no data at the moment
  cell_input_.push_back(root_cell);
  boxm_ocl_utils<T>::copy_to_arrays(root, cell_input_, data_input_, cell_ptr);

  // the tree is now resident in the 1-d vectors
  cells_size_=cell_input_.size();
  cell_data_size_=data_input_.size();

  cells_ = NULL;
  cell_data_ = NULL;
  cell_aux_data_ = NULL;

  cells_=(cl_int *)boxm_ocl_utils<T>::alloc_aligned(cell_input_.size(),sizeof(cl_int4),16);
  cell_data_=(cl_float *)boxm_ocl_utils<T>::alloc_aligned(data_input_.size(),sizeof(cl_float16),16);
  cell_aux_data_=(cl_float *)boxm_ocl_utils<T>::alloc_aligned(data_input_.size(),sizeof(cl_float4),16);

  if (cells_== NULL||cell_data_ == NULL||cell_aux_data_==NULL)
  {
    vcl_cout << "Failed to allocate host memory. (tree input)\n";
    return false;
  }

  //: copy the data from vectors to arrays
  for (unsigned i = 0, j = 0; i<cell_input_.size()*4; i+=4, j++)
    for (unsigned k = 0; k<4; ++k)
      cells_[i+k]=cell_input_[j][k];

  // note that the cell data pointer cells[i+2] does not correspond to the 1-d
  // data array location. It must be mapped as:
  //  cell_data indices = 2*cell_data_ptr, 2*cell_data_ptr +1,

  unsigned cell_data_size=16;
  unsigned aux_cell_data_size=4;
  for (unsigned i = 0, j = 0; i<data_input_.size()*cell_data_size; i+=cell_data_size, j++)
  {
    for (unsigned k = 0; k<cell_data_size; ++k)
      cell_data_[i+k]=data_input_[j][k];

    for (unsigned k = 0; k<aux_cell_data_size; ++k)
      cell_aux_data_[j*aux_cell_data_size+k]=0.0f;
  }


  tree_bbox_=(cl_float *)boxm_ocl_utils<T>::alloc_aligned(1,sizeof(cl_float4),16);

  tree_bbox_[0] = (cl_float)tree->bounding_box().min_x();
  tree_bbox_[1] = (cl_float)tree->bounding_box().min_y();
  tree_bbox_[2] = (cl_float)tree->bounding_box().min_z();
  //: Assumption is isotropic dimensions.
  tree_bbox_[3] = (cl_float)tree->bounding_box().width();

  return true;
}

template<class T>
bool boxm_online_update_manager<T>::clean_tree()
{
  if (cells_)
    boxm_ocl_utils<T>::free_aligned(cells_);
  if (cell_data_)
    boxm_ocl_utils<T>::free_aligned(cell_data_);
  if (cell_aux_data_)
    boxm_ocl_utils<T>::free_aligned(cell_aux_data_);
  if (tree_bbox_)
    boxm_ocl_utils<T>::free_aligned(tree_bbox_);
  return true;
}

template<class T>
bool boxm_online_update_manager<T>::set_tree_buffers()
{
  cl_int status;
  cells_buf_ = clCreateBuffer(this->context_,
                              CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                              cells_size_*sizeof(cl_int4),
                              cells_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (tree) failed."))
    return false;

    cell_data_buf_ = clCreateBuffer(this->context_,
                                    CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                    cell_data_size_*sizeof(cl_float16),
                                    cell_data_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (cell data) failed."))
    return false;
    cell_aux_data_buf_ = clCreateBuffer(this->context_,
                                        CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                        cell_data_size_*sizeof(cl_float4),
                                        cell_aux_data_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (cell aux data) failed."))
    return false;
    tree_bbox_buf_ = clCreateBuffer(this->context_,
                                    CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                    sizeof(cl_float4),
                                    tree_bbox_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (cell aux data) failed.");
}

template<class T>
bool boxm_online_update_manager<T>::release_tree_buffers()
{
  cl_int status;
  status = clReleaseMemObject(cells_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (cells_buf_)."))
    return false;
  status = clReleaseMemObject(cell_data_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (cell_data_buf_)."))
    return false;
  status = clReleaseMemObject(cell_aux_data_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (cell_aux_data_buf_)."))
    return false;
  status = clReleaseMemObject(tree_bbox_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (tree_bbox_buf_).");
}

template<class T>
bool boxm_online_update_manager<T>::set_persp_camera()
{
  if (vpgl_perspective_camera<double>* pcam =
      dynamic_cast<vpgl_perspective_camera<double>*>(cam_.ptr()))
  {
    vnl_svd<double>* svd=pcam->svd();

    vnl_matrix<double> Ut=svd->U().conjugate_transpose();
    vnl_matrix<double> V=svd->V();
    vnl_vector<double> Winv=svd->Winverse().diagonal();

    persp_cam_=(cl_float *)boxm_ocl_utils<T>::alloc_aligned(3,sizeof(cl_float16),16);

    int cnt=0;
    for (unsigned i=0;i<Ut.rows();i++)
    {
      for (unsigned j=0;j<Ut.cols();j++)
        persp_cam_[cnt++]=(cl_float)Ut(i,j);

      persp_cam_[cnt++]=0;
    }

    for (unsigned i=0;i<V.rows();i++)
      for (unsigned j=0;j<V.cols();j++)
        persp_cam_[cnt++]=(cl_float)V(i,j);

    for (unsigned i=0;i<Winv.size();i++)
      persp_cam_[cnt++]=(cl_float)Winv(i);

    vgl_point_3d<double> cam_center=pcam->get_camera_center();
    persp_cam_[cnt++]=(cl_float)cam_center.x();
    persp_cam_[cnt++]=(cl_float)cam_center.y();
    persp_cam_[cnt++]=(cl_float)cam_center.z();
    return true;
  }
  else {
    vcl_cerr << "Error set_persp_camera() : unsupported camera type\n";
    return false;
  }
}

template<class T>
bool boxm_online_update_manager<T>::clean_persp_camera()
{
  if (persp_cam_)
    boxm_ocl_utils<T>::free_aligned(persp_cam_);
  return true;
}

template<class T>
bool boxm_online_update_manager<T>::set_persp_camera_buffers()
{
  cl_int status;
  persp_cam_buf_ = clCreateBuffer(this->context_,
                                  CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                  3*sizeof(cl_float16),
                                  persp_cam_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (persp_cam_buf_) failed.");
}

template<class T>
bool boxm_online_update_manager<T>::release_persp_camera_buffers()
{
  cl_int status;
  status = clReleaseMemObject(persp_cam_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (persp_cam_buf_).");
}


template<class T>
bool boxm_online_update_manager<T>::set_input_image()
{
  wni_=(cl_uint)RoundUp(input_img_.ni(),bni_);
  wnj_=(cl_uint)RoundUp(input_img_.nj(),bnj_);

  image_=(cl_float *)boxm_ocl_utils<T>::alloc_aligned(wni_*wnj_,sizeof(cl_float4),16);
  img_dims_=(cl_uint *)boxm_ocl_utils<T>::alloc_aligned(1,sizeof(cl_uint4),16);

  vil_image_view<float>::iterator iter=input_img_.begin();

  // pad the image
  for (unsigned i=0;i<input_img_.ni();i++)
  {
    for (unsigned j=0;j<input_img_.nj();j++)
    {
    image_[(j*wni_+i)*4]=input_img_(i,j);
    image_[(j*wni_+i)*4+1]=0.0f;
    image_[(j*wni_+i)*4+2]=1.0f;
    image_[(j*wni_+i)*4+3]=0.0f;
    }
  }

  img_dims_[0]=0;
  img_dims_[1]=0;
  img_dims_[2]=input_img_.ni();
  img_dims_[3]=input_img_.nj();

  if (image_==NULL || img_dims_==NULL)
  {
    vcl_cerr<<"Failed allocation of image or image dimensions\n";
    return false;
  }
  return true;
}

template<class T>
bool boxm_online_update_manager<T>::clean_input_image()
{
  if (image_)
    boxm_ocl_utils<T>::free_aligned(image_);
  if (img_dims_)
    boxm_ocl_utils<T>::free_aligned(img_dims_);
  return true;
}

template<class T>
bool boxm_online_update_manager<T>::set_input_image_buffers()
{
  cl_int status;
  image_buf_ = clCreateBuffer(this->context_,
                              CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                              wni_*wnj_*sizeof(cl_float4),
                              image_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (image_buf_) failed."))
    return false;

  img_dims_buf_ = clCreateBuffer(this->context_,
                                 CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                 sizeof(cl_uint4),
                                 img_dims_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (imd_dims_buf_) failed.");
}

template<class T>
bool boxm_online_update_manager<T>::release_input_image_buffers()
{
  cl_int status;
  status = clReleaseMemObject(image_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (image_buf_)."))
    return false;

  status = clReleaseMemObject(img_dims_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (img_dims_buf_)."))
}

template<class T>
bool boxm_online_update_manager<T>::set_offset_buffers(int offset_x,int offset_y)
{
  cl_int status;
  offset_x_=offset_x;
  offset_y_=offset_y;

  offset_x_buf_ = clCreateBuffer(this->context_,
                                 CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                 sizeof(cl_int),
                                 &offset_x_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (offset_x_) failed."))
    return false;
  offset_y_buf_ = clCreateBuffer(this->context_,
                                 CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                 sizeof(cl_int),
                                 &offset_y_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (offset_y_) failed.");
}

template<class T>
bool boxm_online_update_manager<T>::release_offset_buffers()
{
  cl_int status;
  status = clReleaseMemObject(offset_x_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (offset_x_buf_)."))
    return false;

  status = clReleaseMemObject(offset_y_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (offset_y_buf_).");
}

/*****************************************
 *macro for template instantiation
 *****************************************/
#define BOXM_ONLINE_UPDATE_MANAGER_INSTANTIATE(T) \
  template class boxm_online_update_manager<T >

#endif    //boxm_online_update_manager_txx_

