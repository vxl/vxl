#ifndef online_update_test_manager_txx_
#define online_update_test_manager_txx_
//:
// \file
#include "online_update_test_manager.h"
#include <vcl_where_root_dir.h>
#include <boxm/ocl/boxm_ocl_utils.h>
#include <bocl/bocl_utils.h>
#include <vcl_cstdio.h>
#include <vcl_string.h>
#include <vul/vul_timer.h>
#include <boxm/boxm_block.h>
#include <boxm/boxm_scene.h>
#include <boxm/util/boxm_utils.h>
#include <boxm/basic/boxm_block_vis_graph_iterator.h>
#include <vpgl/vpgl_perspective_camera.h>

template<class T>
void online_update_test_manager<T>::
set_block_items(boxm_block<boct_tree<short,T> > *block,
                vpgl_camera_double_sptr cam,
                vil_image_view<float> &obs)
{
  if (block_)
    delete block_;
  block_ = block;
  cam_ = cam;
  input_img_=obs;
}

// This function enables a kind of "functor" capability where a token
// in the ray trace main program is replaced with an appropriate function
// signature. There are two aspects that have to be modified:
// 1) Whether or not cached_cell_data is used in the functor. If cell_data
//    (as opposed to cell_aux_data) is not used then the cost of
//    transferring it from global memory is avoided. This choice is
//    enabled by the %% token in update_main. If "1" is substituted then
//    global memory is transferred.
// 2) The functor name and argument list is controlled by the token
//    $$step_cell$$. The token is replaced by the desired functor signature
//
//  To do - should split prog_ into two parts: prog_libraries_ and prog_main_.
//  Then the functor modification is restricted to just one file and
//  the library files need to be loaded only once.
//
template<class T>
bool online_update_test_manager<T>::
build_program(vcl_string const& functor, bool use_cell_data)
{
  vcl_string root = vcl_string(VCL_SOURCE_ROOT_DIR);
  bool octr = this->load_kernel_source(root + "/contrib/brl/bseg/boxm/ocl/octree_library_functions.cl");
  bool bpr  = this->append_process_kernels(root + "/contrib/brl/bseg/boxm/ocl/backproject.cl");
  bool stat = this->append_process_kernels(root + "/contrib/brl/bseg/boxm/ocl/statistics_library_functions.cl");
  bool rbun = this->append_process_kernels(root + "/contrib/brl/bseg/boxm/ocl/ray_bundle_library_functions.cl");
  bool main = this->append_process_kernels(root + "/contrib/brl/bseg/boxm/ocl/update_main.cl");

  if (!octr||!bpr||!stat||!rbun||!main) {
    vcl_cerr << "Error: boxm_ray_trace_manager : failed to load kernel source (helper functions)\n";
    return false;
  }
  vcl_string patt = "$$step_cell$$", empty = "", zero = "0", one = "1";
  // transfer cell data from global to local memory if use_cell_data_ == true
  vcl_string use = "%%";
  vcl_string::size_type use_start = this->prog_.find(use);
  if (use_start < this->prog_.size()) {
    if (use_cell_data)
      this->prog_ = this->prog_.replace (use_start, 2, one.c_str(), 1);
    else
      this->prog_ = this->prog_.replace (use_start, 2, zero.c_str(), 1);
  }
  else
    return false;
  // assign the functor calling signature
  vcl_string::size_type pos_start = this->prog_.find(patt);
  vcl_string::size_type n1 = patt.size();
  if (pos_start < this->prog_.size()) {
    vcl_string::size_type n2 = functor.size();
    if (!n2)
      return false;
    this->prog_ = this->prog_.replace(pos_start, n1, functor.c_str(), n2);
    //write_program("c:/mundy/local-software/junk.cl");
    return this->build_kernel_program(program_)==SDK_SUCCESS;
  }
  return false;
}

template<class T>
bool online_update_test_manager<T>::clean_kernels()
{
  cl_int status = CL_SUCCESS;
  int CHECK_SUCCESS = 1;
  for (unsigned i = 0; i<kernels_.size(); ++i) {
    if (kernels_[i]) {
      status = clReleaseKernel(kernels_[i]);
      if (this->check_val(status,CL_SUCCESS,"clReleaseKernel failed.")!=CHECK_SUCCESS)
        return false;
    }
  }
  kernels_.clear();
  return true;
}

//: update the tree
template<class T>
bool online_update_test_manager<T>::set_kernels()
{
  cl_int status = CL_SUCCESS;
  int CHECK_SUCCESS = 1;
  if (!this->clean_kernels())
    return false;
  // pass 0
  if (!this->build_program("seg_len_obs(d,image_vect,ray_bundle_array,cached_aux_data)", false))
    return false;
  cl_kernel kernel = clCreateKernel(program_,"update_aux",&status);
  if (this->check_val(status,CL_SUCCESS,error_to_string(status))!=CHECK_SUCCESS)
    return false;
  kernels_.push_back(kernel);
  // pass 1
  if (!this->build_program("pre_infinity(d,image_vect,ray_bundle_array, cached_data, cached_aux_data)", true))
    return false;
  kernel = clCreateKernel(program_,"update_aux",&status);
  if (this->check_val(status,CL_SUCCESS,error_to_string(status))!=CHECK_SUCCESS)    return false;
  kernels_.push_back(kernel);
  //pass 2 norm image (pre-requisite to C++ pass 2)
  kernel = clCreateKernel(program_,"proc_norm_image",&status);
  if (this->check_val(status,CL_SUCCESS,error_to_string(status))!=CHECK_SUCCESS)
    return false;
  kernels_.push_back(kernel);
  // pass 3 Bayes ratio (pass 2 in C++)
  if (!this->build_program("bayes_ratio(d,image_vect,ray_bundle_array, cached_data, cached_aux_data)", true))
    return false;
  kernel = clCreateKernel(program_,"update_aux",&status);
  if (this->check_val(status,CL_SUCCESS,error_to_string(status))!=CHECK_SUCCESS)
    return false;
  kernels_.push_back(kernel);

  kernel = clCreateKernel(program_,"update_main",&status);
  if (this->check_val(status,CL_SUCCESS,error_to_string(status))!=CHECK_SUCCESS)
      return false;
  kernels_.push_back(kernel);
  return true;
}

template<class T>
bool online_update_test_manager<T>::set_kernel_args(unsigned pass)
{
  int CHECK_SUCCESS = 1;
  cl_int status = SDK_SUCCESS;

  if(pass==4)
  {
    status = clSetKernelArg(kernels_[pass], 0,
                            sizeof(cl_mem), (void *)&cell_data_buf_);
    if (this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (cell_data_buf_)")!=CHECK_SUCCESS)
      return false;

    status = clSetKernelArg(kernels_[pass], 1,
                            sizeof(cl_mem), (void *)&cell_aux_data_buf_);
    if (this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (cell_aux_data_buf_)")!=CHECK_SUCCESS)
      return false;
    status = clSetKernelArg(kernels_[pass], 2,
                            sizeof(cl_mem), (void *)&data_array_size_buf_);
    return this->check_val(status, CL_SUCCESS,
                           "clSetKernelArg failed. (data array size)")==CHECK_SUCCESS;

  }
  if (pass == 2) { // norm image process //
    status = clSetKernelArg(kernels_[pass], 0,
                            sizeof(cl_mem), (void *)&image_buf_);
    if (this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (image array)")!=CHECK_SUCCESS)
      return false;

    status = clSetKernelArg(kernels_[pass], 1,
                            sizeof(cl_mem), (void *)&app_density_buf_);

    return this->check_val(status, CL_SUCCESS,
                           "clSetKernelArg failed. (remote surface appearance)")==CHECK_SUCCESS;
  }
  cl_kernel kernel = kernels_[pass];
  // -- Set appropriate arguments to the kernel for ray tracing--
  int i=0;
  status = clSetKernelArg(kernel,i++,sizeof(cl_mem),(void *)&root_level_buf_);
  if (this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (root_level_buf_)")!=CHECK_SUCCESS)
    return false;

  // the tree buffer
  status = clSetKernelArg(kernel,i++,sizeof(cl_mem),(void *)&cells_buf_);
  if (this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cells_buf_)")
      != CHECK_SUCCESS)
    return false;

  // data buffer
  status = clSetKernelArg(kernel,i++,sizeof(cl_mem),(void *)&cell_data_buf_);
  if (this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_data_buf_)") != CHECK_SUCCESS)
    return false;
  // aux data buffer
  status = clSetKernelArg(kernel,i++,sizeof(cl_mem),(void *)&cell_aux_data_buf_);
  if (this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_aux_data_buf_)")!=CHECK_SUCCESS)
    return false;
  // camera buffer
  status = clSetKernelArg(kernel,i++,sizeof(cl_mem),(void *)&persp_cam_buf_);
  if (this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (data)")!=CHECK_SUCCESS)
    return false;
  status = clSetKernelArg(kernel,i++,sizeof(cl_mem),(void *)&tree_bbox_buf_);
  if (this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (ROI dimensions)")!=CHECK_SUCCESS)
    return false;
  // roi dimensions
  status = clSetKernelArg(kernel,i++,sizeof(cl_mem),(void *)&img_dims_buf_);
  if (this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (Img dimensions)")!=CHECK_SUCCESS)
    return false;
  // output image buffer
  status = clSetKernelArg(kernel,i++,sizeof(cl_mem),(void *)&image_buf_);
  if (this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (input_image)")!=CHECK_SUCCESS)
    return false;
  set_offset_buffers(0,0);

  //  offset buffer
  status = clSetKernelArg(kernel,i++,sizeof(cl_mem),(void *)&offset_x_buf_);
  if (this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local cam)")!=CHECK_SUCCESS)
    return false;
  status = clSetKernelArg(kernel,i++,sizeof(cl_mem),(void *)&offset_y_buf_);
  if (this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local cam)")!=CHECK_SUCCESS)
    return false;
  status = clSetKernelArg(kernel,i++,sizeof(cl_float4),0);
  if (this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local origin)")!=CHECK_SUCCESS)
    return false;
  status = clSetKernelArg(kernel,i++,3*sizeof(cl_float16),0);
  if (this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local box)")!=CHECK_SUCCESS)
    return false;
  status = clSetKernelArg(kernel,i++,sizeof(cl_uint4),0);
  if (this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local roi)")!=CHECK_SUCCESS)
    return false;
  status = clSetKernelArg(kernel,i++,sizeof(cl_uchar4)*this->bni_*this->bnj_,0);

  if (this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local cache ptr bundle)")!=CHECK_SUCCESS)
    return false;
  status = clSetKernelArg(kernel,i++,sizeof(cl_short4)*this->bni_*this->bnj_,0);
  if (this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (loc code bundle)")!=CHECK_SUCCESS)
    return false;
  status = clSetKernelArg(kernel,i++,sizeof(cl_float16)*this->bni_*this->bnj_,0);
  if (this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (loc code bundle)")!=CHECK_SUCCESS)
    return false;
  status = clSetKernelArg(kernel,i++,sizeof(cl_float4)*this->bni_*this->bnj_,0);

  if (this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (loc code bundle)")!=CHECK_SUCCESS)
    return false;
  status = clSetKernelArg(kernel,i++,sizeof(cl_float4)*this->bni_*this->bnj_,0);
  return this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (loc code bundle)")==CHECK_SUCCESS;
}

template<class T>
bool online_update_test_manager<T>::create_command_queue()
{
  cl_int status = SDK_SUCCESS;
  // set up a command queue
  command_queue_ = clCreateCommandQueue(this->context(),this->devices()[0],CL_QUEUE_PROFILING_ENABLE,&status);
  return this->check_val(status,CL_SUCCESS,"Falied in command queue creation" + error_to_string(status))==1;
}

template<class T>
bool online_update_test_manager<T>::setup_app_density(bool use_uniform, float mean, float sigma)
{
#if defined (_WIN32)
  app_density_ =  (cl_float*)_aligned_malloc( sizeof(cl_float4), 16);
#elif defined(__APPLE__)
  app_density_ =  (cl_float*)malloc( sizeof(cl_float4));
#else
  app_density_ =  (cl_float*)memalign(16, sizeof(cl_float4));
#endif
  if (use_uniform) {
    app_density_[0]=1.0f;
    app_density_[1]=0.0f;
    app_density_[2]=0.0f;
    app_density_[3]=0.0f;
  }
  else {
    app_density_[0]=0.0f;
    app_density_[1]=mean;
    app_density_[2]=sigma;
    app_density_[3]=0.0f;
  }
  return true;
}

template<class T>
bool online_update_test_manager<T>::clean_app_density()
{
  if (app_density_) {
#ifdef _WIN32
    _aligned_free(app_density_);
#else
    free(app_density_);
#endif
    app_density_ = NULL;
    return true;
  }
  else
    return false;
}

template<class T>
int online_update_test_manager<T>::setup_app_density_buffer()
{
  cl_int status = CL_SUCCESS;
  app_density_buf_ = clCreateBuffer(this->context_,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                    sizeof(cl_float4),app_density_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (app density) failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template<class T>
int online_update_test_manager<T>::clean_app_density_buffer()
{
  cl_int status = clReleaseMemObject(app_density_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject (app_density_buf_) failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

template <class T>
bool online_update_test_manager<T>::setup_norm_data(bool use_uniform,
                                                    float mean,
                                                    float sigma)
{
  return this->setup_app_density(use_uniform, mean, sigma)
      && this->setup_app_density_buffer()==SDK_SUCCESS;
}


template <class T>
bool online_update_test_manager<T>::clean_norm_data()
{
  return this->clean_app_density()
      && this->clean_app_density_buffer()==SDK_SUCCESS;
}

template<class T>
bool online_update_test_manager<T>::run_block(unsigned pass)
{
  int CHECK_SUCCESS = 1;
  cl_int status = SDK_SUCCESS;
  this->set_kernel_args(pass);
  // check the local memeory
  cl_ulong used_local_memory;
  status = clGetKernelWorkGroupInfo(kernels_[pass],this->devices()[0],CL_KERNEL_LOCAL_MEM_SIZE,
                                    sizeof(cl_ulong),&used_local_memory,NULL);
  if (this->check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_LOCAL_MEM_SIZE failed.")!=CHECK_SUCCESS)
    return false;

  // determine the work group size
  cl_ulong kernel_work_group_size;
  status = clGetKernelWorkGroupInfo(kernels_[pass],this->devices()[0],CL_KERNEL_WORK_GROUP_SIZE,
                                    sizeof(cl_ulong),&kernel_work_group_size,NULL);
  if (this->check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_WORK_GROUP_SIZE, failed.")!=CHECK_SUCCESS)
    return false;


  vcl_size_t globalThreads[]= {this->wni_,this->wnj_};
  vcl_size_t localThreads[] = {this->bni_,this->bnj_};

  if(pass==4) //: change the gloabal threads
  {
    globalThreads[0]=RoundUp(data_array_size_[0],64);globalThreads[1]=1;
    localThreads[0]=64;localThreads[1]=1;

  }
  if (used_local_memory > this->total_local_memory())
  {
    vcl_cout << "Unsupported: Insufficient local memory on device.\n";
    return false;
  }

  cl_event ceEvent;
  status = clEnqueueNDRangeKernel(command_queue_, kernels_[pass], 2,NULL,globalThreads,localThreads,0,NULL,&ceEvent);

  if (this->check_val(status,CL_SUCCESS,"clEnqueueNDRangeKernel failed. "+error_to_string(status))!=CHECK_SUCCESS)
    return false;

  status = clFinish(command_queue_);
  if (this->check_val(status,CL_SUCCESS,"clFinish failed."+error_to_string(status))!=CHECK_SUCCESS)
    return false;
  cl_ulong tstart,tend;
  status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_END,sizeof(cl_ulong),&tend,0);
  status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),&tstart,0);
  //  gpu_time_+= (double)1.0e-6 * (tend - tstart); // convert nanoseconds to milliseconds
  //  release_offset_buffers();

  return true;
}

template<class T>
bool online_update_test_manager<T>::process_block()
{
  cl_int status = CL_SUCCESS;
  if (!this->set_kernels())
    return false;
  if (!this->create_command_queue())
    return false;
  vcl_string error_message="";
  vul_timer timer;
  if (!block_)
    return false;
  if (!(set_block_data() &&
        set_block_data_buffers() &&
        set_input_view() &&
        set_input_view_buffers()))
    return false;
  float total_raytrace_time = 0.0f;
  float total_gpu_time = 0.0f;
  float total_load_time = 0.0f;
  tree_type * tree = block_->get_tree();
  if (!(set_tree(tree) && set_tree_buffers()))
    return false;
  // run the raytracing for this block
  for (unsigned pass = 0; pass<4; pass++)
    if (!run_block(pass))
      return false;
  // release memory
  this->read_trees();
  this->archive_tree_data();
  //this->print_leaves();
  if (!(release_tree_buffers() && clean_tree()))
    return false;
  float raytrace_time = (float)timer.all() / 1e3f;
  vcl_cout<<"processing block took " << raytrace_time << 's' << vcl_endl;
  this->print_image() ;
  return read_output_image()    && clean_input_view()
      && release_block_data_buffers()
      && clean_block_data()
      && clean_norm_data()
      && release_command_queue();
}

template<class T>
bool online_update_test_manager<T>:: read_output_image()
{
  cl_event events[2];
  // clear image
  for (unsigned i = 0; i<this->wni_*this->wnj_*4; ++i)
    image_[i]=0.0f;

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
  return this->check_val(status,CL_SUCCESS,"clReleaseEvent failed.")==1;
}

template<class T>
bool online_update_test_manager<T>:: read_trees()
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
  return this->check_val(status,CL_SUCCESS,"clReleaseEvent failed.")==1;
}

template<class T>
void online_update_test_manager<T>::print_image()
{
  if (!image_)
    return;
  vcl_cout<<"Plane 0"<<vcl_endl;
  for (unsigned j=0;j<this->wnj_;j++)
  {
    for (unsigned i=0;i<this->wni_;i++)
      vcl_cout<<image_[(j*this->wni_+i)*4]<<' ';
    vcl_cout<<vcl_endl;
  }
  vcl_cout<<"Plane 1"<<vcl_endl;
  for (unsigned j=0;j<this->wnj_;j++)
  {
    for (unsigned i=0;i<this->wni_;i++)
      vcl_cout<<image_[(j*this->wni_+i)*4+1]<<' ';
    vcl_cout<<vcl_endl;
  }
  vcl_cout<<"Plane 2"<<vcl_endl;
  for (unsigned j=0;j<this->wnj_;j++)
  {
    for (unsigned i=0;i<this->wni_;i++)
      vcl_cout<<image_[(j*this->wni_+i)*4+2]<<' ';
    vcl_cout<<vcl_endl;
  }
  vcl_cout<<"Plane 3"<<vcl_endl;
  for (unsigned j=0;j<this->wnj_;j++)
  {
    for (unsigned i=0;i<this->wni_;i++)
      vcl_cout<<image_[(j*this->wni_+i)*4+3]<<' ';
    vcl_cout<<vcl_endl;
  }
}

template<class T>
void online_update_test_manager<T>::print_tree()
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
                 << cell_aux_data_[aux_data_ptr+2] << ','
                 << cell_aux_data_[aux_data_ptr+3] << ']';
      vcl_cout << ")\n";
    }
}

template<class T>
void online_update_test_manager<T>::print_leaves()
{
  vcl_cout << "Tree Leaves\n";
  if (cells_)
    for (unsigned i = 0; i<cells_size_*4; i+=4) {
      int child_ptr = 16*cells_[i+1];
      int data_ptr = 16*cells_[i+2];
      int aux_data_ptr = 4*cells_[i+2];
      if (child_ptr<0&&data_ptr>0)
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
                 << cell_data_[data_ptr+15] << "]\n";

      if (child_ptr<0&&aux_data_ptr>0)
        vcl_cout << '[' << cell_aux_data_[aux_data_ptr] << ','
                 << cell_aux_data_[aux_data_ptr+1] << ','
                 << cell_aux_data_[aux_data_ptr+2] << ','
                 << cell_aux_data_[aux_data_ptr+3] << "]\n\n";
    }
}

template<class T>
void online_update_test_manager<T>::clear_tree_data()
{
  tree_data_.clear();
  tree_aux_data_.clear();
}

template<class T>
void online_update_test_manager<T>::archive_tree_data()
{
  this->clear_tree_data();
  if (cells_)
    for (unsigned i = 0; i<cells_size_*4; i+=4) {
      int child_ptr = 16*cells_[i+1];
      int data_ptr = 16*cells_[i+2];
      int aux_data_ptr = 4*cells_[i+2];
      if (child_ptr<0&&data_ptr>=0) {
        vnl_vector_fixed<float, 16> cdata;

        for (unsigned k = 0; k<16; ++k)
          cdata[k] = cell_data_[data_ptr+k];
        tree_data_.push_back(cdata);
      }
      if (child_ptr<0&&aux_data_ptr>=0) {
        vnl_vector_fixed<float, 4> caux_data;
        for (unsigned k = 0; k<4; ++k)
          caux_data[k] = cell_aux_data_[aux_data_ptr+k];
        tree_aux_data_.push_back(caux_data);
      }
    }
}


/*******************************************
 * build_kernel_program - builds kernel program
 * from source (a vcl string)
 *******************************************/
template<class T>
int online_update_test_manager<T>::build_kernel_program(cl_program & program)
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
bool online_update_test_manager<T>::set_block_data()
{
  return set_root_level();
}

template<class T>
bool online_update_test_manager<T>::clean_block_data()
{
  return clean_root_level();
}

template<class T>
bool online_update_test_manager<T>::set_block_data_buffers()
{
  return set_root_level_buffers();
}

template<class T>
bool online_update_test_manager<T>::release_block_data_buffers()
{
  return release_root_level_buffers();
}

template<class T>
bool online_update_test_manager<T>::set_root_level()
{
  if (block_==NULL)
  {
    vcl_cout<<"Block is Missing "<<vcl_endl;
    return false;
  }
  else {
    root_level_=block_->get_tree()->root_level();
    return true;
  }
}

template<class T>
bool online_update_test_manager<T>::clean_root_level()
{
  root_level_=0;
  return true;
}

template<class T>
bool online_update_test_manager<T>::set_root_level_buffers()
{
  cl_int status;
  root_level_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                   sizeof(cl_uint),
                                   &root_level_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (root level) failed.")==1;
}

template<class T>
bool online_update_test_manager<T>::release_root_level_buffers()
{
  cl_int status;
  status = clReleaseMemObject(root_level_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (root_level_buf_).");
}

template<class T>
bool online_update_test_manager<T>::set_input_view()
{
  return set_persp_camera()
      && set_input_image();
}

template<class T>
bool online_update_test_manager<T>::clean_input_view()
{
  bool good = true;
  return clean_persp_camera()
      && clean_input_image();
}

template<class T>
bool online_update_test_manager<T>::set_input_view_buffers()
{
  return set_persp_camera_buffers()
      && set_input_image_buffers();
}

template<class T>
bool online_update_test_manager<T>::release_input_view_buffers()
{
  return release_persp_camera_buffers()    
        && release_input_image_buffers();
}

template<class T>
bool online_update_test_manager<T>::set_tree(tree_type* tree)
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
  data_array_size_=(cl_uint *)boxm_ocl_utils<T>::alloc_aligned(1,sizeof(cl_uint),16);
 
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


  data_array_size_[0]=cell_data_size_;

  tree_bbox_=(cl_float *)boxm_ocl_utils<T>::alloc_aligned(1,sizeof(cl_float4),16);

  tree_bbox_[0] = (cl_float)tree->bounding_box().min_x();
  tree_bbox_[1] = (cl_float)tree->bounding_box().min_y();
  tree_bbox_[2] = (cl_float)tree->bounding_box().min_z();
  //: Assumption is isotropic dimensions.
  tree_bbox_[3] = (cl_float)tree->bounding_box().width();

  return true;
}

template<class T>
bool online_update_test_manager<T>::clean_tree()
{
  if (cells_)
    boxm_ocl_utils<T>::free_aligned(cells_);
  if (cell_data_)
    boxm_ocl_utils<T>::free_aligned(cell_data_);
  if (cell_aux_data_)
    boxm_ocl_utils<T>::free_aligned(cell_aux_data_);
  if (tree_bbox_)
    boxm_ocl_utils<T>::free_aligned(tree_bbox_);
  if(data_array_size_)
    boxm_ocl_utils<T>::free_aligned(data_array_size_);

  return true;
}

template<class T>
bool online_update_test_manager<T>::set_tree_buffers()
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
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (tree_bbox_buf_) failed."))
    return false;
  data_array_size_buf_ = clCreateBuffer(this->context_,
                                  CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                  sizeof(cl_uint),
                                  data_array_size_,&status);

  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (data_array_size_) failed.")==1;
}

template<class T>
bool online_update_test_manager<T>::release_tree_buffers()
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
    if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (tree_bbox_buf_)."))
    return false;
  status = clReleaseMemObject(data_array_size_buf_);

  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (tree_bbox_buf_).")==1;
}

template<class T>
bool online_update_test_manager<T>::set_persp_camera()
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
bool online_update_test_manager<T>::clean_persp_camera()
{
  if (persp_cam_)
    boxm_ocl_utils<T>::free_aligned(persp_cam_);
  return true;
}

template<class T>
bool online_update_test_manager<T>::set_persp_camera_buffers()
{
  cl_int status;
  persp_cam_buf_ = clCreateBuffer(this->context_,
                                  CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                  3*sizeof(cl_float16),
                                  persp_cam_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (persp_cam_buf_) failed.")==1;
}

template<class T>
bool online_update_test_manager<T>::release_persp_camera_buffers()
{
  cl_int status;
  status = clReleaseMemObject(persp_cam_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (persp_cam_buf_).")==1;
}


template<class T>
bool online_update_test_manager<T>::set_input_image()
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
  else
    return true;
}

template<class T>
bool online_update_test_manager<T>::clean_input_image()
{
  if (image_)
    boxm_ocl_utils<T>::free_aligned(image_);
  if (img_dims_)
    boxm_ocl_utils<T>::free_aligned(img_dims_);
  return true;
}

template<class T>
bool online_update_test_manager<T>::set_input_image_buffers()
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
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (imd_dims_buf_) failed.")==1;
}

template<class T>
bool online_update_test_manager<T>::release_input_image_buffers()
{
  cl_int status;
  status = clReleaseMemObject(image_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (image_buf_)."))
    return false;

  status = clReleaseMemObject(img_dims_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (img_dims_buf_).");
}

template<class T>
bool online_update_test_manager<T>::set_offset_buffers(int offset_x,int offset_y)
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
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (offset_y_) failed.")==1;
}

template<class T>
bool online_update_test_manager<T>::release_offset_buffers()
{
  cl_int status;
  status = clReleaseMemObject(offset_x_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (offset_x_buf_)."))
    return false;

  status = clReleaseMemObject(offset_y_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (offset_y_buf_).")==1;
}

template<class T>
bool online_update_test_manager<T>::release_command_queue()
{
  cl_int status = clReleaseCommandQueue(command_queue_);
  return this->check_val(status,CL_SUCCESS,"clReleaseCommandQueue failed.") == 1;
}

/*****************************************
 *macro for template instantiation
 *****************************************/
#define ONLINE_UPDATE_TEST_MANAGER_INSTANTIATE(T) \
  template class online_update_test_manager<T >

#endif    //online_update_test_manager_txx_
