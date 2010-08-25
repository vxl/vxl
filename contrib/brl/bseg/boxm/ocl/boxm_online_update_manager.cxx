//:
// \file
#include "boxm_online_update_manager.h"
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


#include <vil/vil_save.h>

void boxm_online_update_manager::init_update(vcl_string treefile,
                                             vcl_string treedatafile,
                                             vgl_point_3d<double>  origin,
                                             vgl_vector_3d<double>  block_dim,
                                             vpgl_camera_double_sptr cam,
                                             vil_image_view<float> &obs,
                                             unsigned int root_level)
 {
   treefile_=treefile;
   treedatafile_=treedatafile;
   origin_=origin;
   block_dim_=block_dim;
   cam_ = cam;
   input_img_=obs;
   root_level_=root_level;
 }

//:
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
bool boxm_online_update_manager::
build_program(vcl_string const& functor, bool use_cell_data)
{
  vcl_string root = vcl_string(VCL_SOURCE_ROOT_DIR);
  bool locc = this->load_kernel_source(root + "/contrib/brl/bseg/boxm/ocl/cl/loc_code_library_functions.cl");
  bool cell = this->append_process_kernels(root + "/contrib/brl/bseg/boxm/ocl/cl/cell_utils.cl");
  bool octr = this->append_process_kernels(root + "/contrib/brl/bseg/boxm/ocl/cl/octree_library_functions.cl");
  bool bpr  = this->append_process_kernels(root + "/contrib/brl/bseg/boxm/ocl/cl/backproject.cl");
  bool stat = this->append_process_kernels(root + "/contrib/brl/bseg/boxm/ocl/cl/statistics_library_functions.cl");
  bool rbun = this->append_process_kernels(root + "/contrib/brl/bseg/boxm/ocl/cl/ray_bundle_library_functions.cl");
  bool main = this->append_process_kernels(root + "/contrib/brl/bseg/boxm/ocl/cl/update_main.cl");

  if (!octr||!bpr||!stat||!rbun||!main||!locc||!cell) {
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


bool boxm_online_update_manager::clean_kernels()
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
bool boxm_online_update_manager::set_kernels()
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


bool boxm_online_update_manager::set_kernel_args(unsigned pass)
{
  int CHECK_SUCCESS = 1;
  cl_int status = SDK_SUCCESS;

  if (pass==4)
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
  //  offset buffer
  status = clSetKernelArg(kernel,i++,sizeof(cl_mem),(void *)&factor_buf_);
  if (this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (factor_buf_)")!=CHECK_SUCCESS)
    return false;
  status = clSetKernelArg(kernel,i++,sizeof(cl_mem),(void *)&offset_x_buf_);
  if (this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (offset_x_buf_)")!=CHECK_SUCCESS)
    return false;

  status = clSetKernelArg(kernel,i++,sizeof(cl_mem),(void *)&offset_y_buf_);
  if (this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (offset_y_buf_)")!=CHECK_SUCCESS)
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


bool boxm_online_update_manager::create_command_queue()
{
  cl_int status = SDK_SUCCESS;
  // set up a command queue
  command_queue_ = clCreateCommandQueue(this->context(),this->devices()[0],CL_QUEUE_PROFILING_ENABLE,&status);
  return this->check_val(status,CL_SUCCESS,"Falied in command queue creation" + error_to_string(status))==1;
}


bool boxm_online_update_manager::setup_app_density(bool use_uniform, float mean, float sigma)
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


bool boxm_online_update_manager::clean_app_density()
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


int boxm_online_update_manager::setup_app_density_buffer()
{
  cl_int status = CL_SUCCESS;
  app_density_buf_ = clCreateBuffer(this->context_,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                    sizeof(cl_float4),app_density_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (app density) failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}


int boxm_online_update_manager::clean_app_density_buffer()
{
  cl_int status = clReleaseMemObject(app_density_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject (app_density_buf_) failed."))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}

bool boxm_online_update_manager::setup_norm_data(bool use_uniform,
                                                 float mean,
                                                 float sigma)
{
  return this->setup_app_density(use_uniform, mean, sigma)
      && this->setup_app_density_buffer()==SDK_SUCCESS;
}


bool boxm_online_update_manager::clean_norm_data()
{
  return this->clean_app_density()
      && this->clean_app_density_buffer()==SDK_SUCCESS;
}


bool boxm_online_update_manager::run_block(unsigned pass)
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
  if (used_local_memory > this->total_local_memory())
  {
    vcl_cout << "Unsupported: Insufficient local memory on device.\n";
    return false;
  }


  vcl_size_t globalThreads[]= {this->wni_,this->wnj_};
  vcl_size_t localThreads[] = {this->bni_,this->bnj_};

  if (pass==4) // change the global threads
  {
    globalThreads[0]=RoundUp(data_array_size_[0],64);globalThreads[1]=1;
    localThreads[0]=64;                              localThreads[1]=1;
  }
  if (pass!=2 && pass!=4)
  {
    globalThreads[0]=this->wni_/2; globalThreads[1]=this->wnj_/2;
    localThreads[0] =this->bni_  ; localThreads[1] =this->bnj_  ;

    for (unsigned k=0;k<2;k++)
    {
      for (unsigned l=0;l<2;l++)
      {
        status=clEnqueueWriteBuffer(command_queue_,offset_y_buf_,0,0,sizeof(cl_uint),(void *)&k,0,0,0);
        status=clEnqueueWriteBuffer(command_queue_,offset_x_buf_,0,0,sizeof(cl_uint),(void *)&l,0,0,0);
        clFinish(command_queue_);
        cl_event ceEvent;

        status = clEnqueueNDRangeKernel(command_queue_, kernels_[pass], 2,NULL,globalThreads,localThreads,0,NULL,&ceEvent);
        if (this->check_val(status,CL_SUCCESS,"clEnqueueNDRangeKernel failed. "+error_to_string(status))!=CHECK_SUCCESS)
          return false;
        status = clFinish(command_queue_);
        if (this->check_val(status,CL_SUCCESS,"clFinish failed."+error_to_string(status))!=CHECK_SUCCESS)
          return false;
      }
    }
  }
  else
  {
    cl_event ceEvent;

    status = clEnqueueNDRangeKernel(command_queue_, kernels_[pass], 2,NULL,globalThreads,localThreads,0,NULL,&ceEvent);
    if (this->check_val(status,CL_SUCCESS,"clEnqueueNDRangeKernel failed. "+error_to_string(status))!=CHECK_SUCCESS)
      return false;
    status = clFinish(command_queue_);
  }

  if (this->check_val(status,CL_SUCCESS,"clFinish failed."+error_to_string(status))!=CHECK_SUCCESS)
    return false;
#if 0
  cl_ulong tstart,tend;
  status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_END,sizeof(cl_ulong),&tend,0);
  status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),&tstart,0);
    gpu_time_+= (double)1.0e-6 * (tend - tstart); // convert nanoseconds to milliseconds
#endif

  return true;
}


bool boxm_online_update_manager::process_block(int numpass)
{
  if (!this->set_kernels())
    return false;
  if (!this->create_command_queue())
    return false;
  vcl_string error_message="";
  vul_timer timer;
  if (!(set_block_data() &&
        set_block_data_buffers() &&
        set_offset_buffers(0,0,2)&&
        set_input_view() &&
        set_input_view_buffers()))
    return false;
  //tree_type * tree = block_->get_tree();
  if (!(set_tree() && set_tree_buffers()))
    return false;
  // run the raytracing for this block
  for (int pass = 0; pass<numpass; ++pass)
  {
    if (!run_block(pass))
      return false;
    if (pass==1)
    {
      read_output_image();
      this->save_image();
    }
  }
  // release memory
  this->read_trees();
  this->save_tree_data();
  this->archive_tree_data();
  //this->print_leaves();
  if (!(release_tree_buffers() && clean_tree()))
    return false;
  float raytrace_time = (float)timer.all() / 1e3f;
  vcl_cout<<"processing block took " << raytrace_time << 's' << vcl_endl;
  read_output_image();
  //this->print_image();

  return clean_input_view()
      && release_block_data_buffers()
      && release_offset_buffers()
      && clean_block_data()
      && clean_norm_data()
      && release_command_queue();
}


bool boxm_online_update_manager:: read_output_image()
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
  return this->check_val(status,CL_SUCCESS,"clReleaseEvent failed.")==1;
}


bool boxm_online_update_manager:: read_trees()
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


void boxm_online_update_manager::print_image()
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

void boxm_online_update_manager::save_image()
{
  if (!image_)
    return;
  vil_image_view<float> img0(this->wni_,this->wnj_);
  vil_image_view<float> img1(this->wni_,this->wnj_);
  vil_image_view<float> img2(this->wni_,this->wnj_);
  vil_image_view<float> img3(this->wni_,this->wnj_);


  for (unsigned j=0;j<this->wnj_;j++)
    for (unsigned i=0;i<this->wni_;i++)
      img0(i,j)=image_[(j*this->wni_+i)*4];
  for (unsigned j=0;j<this->wnj_;j++)
    for (unsigned i=0;i<this->wni_;i++)
      img1(i,j)=image_[(j*this->wni_+i)*4+1];
  for (unsigned j=0;j<this->wnj_;j++)
    for (unsigned i=0;i<this->wni_;i++)
      img2(i,j)=image_[(j*this->wni_+i)*4+2];
  for (unsigned j=0;j<this->wnj_;j++)
    for (unsigned i=0;i<this->wni_;i++)
      img3(i,j)=image_[(j*this->wni_+i)*4+3];


  vil_save(img0,"f:/APL/img0.tiff");
  vil_save(img1,"f:/APL/img1.tiff");
  vil_save(img2,"f:/APL/img2.tiff");
  vil_save(img3,"f:/APL/img3.tiff");
}

void boxm_online_update_manager::print_tree()
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


void boxm_online_update_manager::print_leaves()
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


void boxm_online_update_manager::clear_tree_data()
{
  tree_data_.clear();
  tree_aux_data_.clear();
}


void boxm_online_update_manager::archive_tree_data()
{
  this->clear_tree_data();
  if (cells_)
    for (unsigned i = 0; i<cells_size_*4; i+=4) {
      int child_ptr = 16*cells_[i+1];
      if (cells_[i+2]>=0)
      {
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
}

void boxm_online_update_manager::save_tree_data()
{
    boxm_ocl_utils::writetree(treefile_,cells_,(unsigned) cells_size_);
    boxm_ocl_utils::writetreedata(treedatafile_,cell_data_,(unsigned) cell_data_size_);
    //vcl_string filename="F:/APL/test/aux.tree";
    //boxm_ocl_utils::writetreeauxdata(filename,cell_aux_data_,(unsigned) cell_data_size_);
}

/*******************************************
 * build_kernel_program - builds kernel program
 * from source (a vcl string)
 *******************************************/

int boxm_online_update_manager::build_kernel_program(cl_program & program)
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


bool boxm_online_update_manager::set_block_data()
{
  return set_root_level();
}


bool boxm_online_update_manager::clean_block_data()
{
  return clean_root_level();
}


bool boxm_online_update_manager::set_block_data_buffers()
{
  return set_root_level_buffers();
}


bool boxm_online_update_manager::release_block_data_buffers()
{
  return release_root_level_buffers();
}


bool boxm_online_update_manager::set_root_level()
{
  return true;
}


bool boxm_online_update_manager::clean_root_level()
{
  root_level_=0;
  return true;
}


bool boxm_online_update_manager::set_root_level_buffers()
{
  cl_int status;
  root_level_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                   sizeof(cl_uint),
                                   &root_level_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (root level) failed.")==1;
}


bool boxm_online_update_manager::release_root_level_buffers()
{
  cl_int status;
  status = clReleaseMemObject(root_level_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (root_level_buf_).")==1;
}


bool boxm_online_update_manager::set_input_view()
{
  return set_persp_camera()
      && set_input_image();
}


bool boxm_online_update_manager::clean_input_view()
{
  return clean_persp_camera()
      && clean_input_image();
}


bool boxm_online_update_manager::set_input_view_buffers()
{
  return set_persp_camera_buffers()
      && set_input_image_buffers();
}


bool boxm_online_update_manager::release_input_view_buffers()
{
  return release_persp_camera_buffers()
        && release_input_image_buffers();
}


bool boxm_online_update_manager::set_tree()
{
  cells_ = boxm_ocl_utils::readtree(treefile_.c_str(),cells_size_);
  cell_data_ = boxm_ocl_utils::readtreedata(treedatafile_.c_str(),cell_data_size_);
  cell_aux_data_ = NULL;

  cell_aux_data_=(cl_float *)boxm_ocl_utils::alloc_aligned(cell_data_size_,sizeof(cl_float4),16);
  data_array_size_=(cl_uint *)boxm_ocl_utils::alloc_aligned(1,sizeof(cl_uint),16);

  if (cells_== NULL||cell_data_ == NULL||cell_aux_data_==NULL)
  {
    vcl_cout << "Failed to allocate host memory. (tree input)\n";
    return false;
  }

  for (unsigned i=0;i<cell_data_size_*4;)
    for (unsigned j=0;j<4;j++)
      cell_aux_data_[i++]=0.0;

  data_array_size_[0]=cell_data_size_;
  tree_bbox_=(cl_float *)boxm_ocl_utils::alloc_aligned(1,sizeof(cl_float4),16);

  tree_bbox_[0] = (cl_float)origin_.x();
  tree_bbox_[1] = (cl_float)origin_.y();
  tree_bbox_[2] = (cl_float)origin_.z();
  // Assumption: isotropic dimensions.
  tree_bbox_[3] = (cl_float)block_dim_.x();

  return true;
}


bool boxm_online_update_manager::clean_tree()
{
  if (cells_)
    boxm_ocl_utils::free_aligned(cells_);
  if (cell_data_)
    boxm_ocl_utils::free_aligned(cell_data_);
  if (cell_aux_data_)
    boxm_ocl_utils::free_aligned(cell_aux_data_);
  if (tree_bbox_)
    boxm_ocl_utils::free_aligned(tree_bbox_);
  if (data_array_size_)
    boxm_ocl_utils::free_aligned(data_array_size_);

  return true;
}


bool boxm_online_update_manager::set_tree_buffers()
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


bool boxm_online_update_manager::release_tree_buffers()
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


bool boxm_online_update_manager::set_persp_camera()
{
  if (vpgl_perspective_camera<double>* pcam =
      dynamic_cast<vpgl_perspective_camera<double>*>(cam_.ptr()))
  {
    vnl_svd<double>* svd=pcam->svd();

    vnl_matrix<double> Ut=svd->U().conjugate_transpose();
    vnl_matrix<double> V=svd->V();
    vnl_vector<double> Winv=svd->Winverse().diagonal();

    persp_cam_=(cl_float *)boxm_ocl_utils::alloc_aligned(3,sizeof(cl_float16),16);

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


bool boxm_online_update_manager::clean_persp_camera()
{
  if (persp_cam_)
    boxm_ocl_utils::free_aligned(persp_cam_);
  return true;
}


bool boxm_online_update_manager::set_persp_camera_buffers()
{
  cl_int status;
  persp_cam_buf_ = clCreateBuffer(this->context_,
                                  CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                  3*sizeof(cl_float16),
                                  persp_cam_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (persp_cam_buf_) failed.")==1;
}


bool boxm_online_update_manager::release_persp_camera_buffers()
{
  cl_int status;
  status = clReleaseMemObject(persp_cam_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (persp_cam_buf_).")==1;
}


bool boxm_online_update_manager::set_input_image()
{
  wni_=(cl_uint)RoundUp(input_img_.ni(),bni_);
  wnj_=(cl_uint)RoundUp(input_img_.nj(),bnj_);

  image_=(cl_float *)boxm_ocl_utils::alloc_aligned(wni_*wnj_,sizeof(cl_float4),16);
  img_dims_=(cl_uint *)boxm_ocl_utils::alloc_aligned(1,sizeof(cl_uint4),16);

  for (unsigned i=0;i<wni_*wnj_*4;i++)
    image_[i]=0.0f;

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


bool boxm_online_update_manager::clean_input_image()
{
  if (image_)
    boxm_ocl_utils::free_aligned(image_);
  if (img_dims_)
    boxm_ocl_utils::free_aligned(img_dims_);
  return true;
}


bool boxm_online_update_manager::set_input_image_buffers()
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


bool boxm_online_update_manager::release_input_image_buffers()
{
  cl_int status;
  status = clReleaseMemObject(image_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (image_buf_)."))
    return false;

  status = clReleaseMemObject(img_dims_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (img_dims_buf_).");
}


bool boxm_online_update_manager::set_offset_buffers(int offset_x,int offset_y,int factor)
{
  cl_int status;
  offset_x_=offset_x;
  offset_y_=offset_y;
  factor_=factor;
  factor_buf_ = clCreateBuffer(this->context_,
                               CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                               sizeof(cl_int),
                               &factor_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (factor_) failed."))
    return false;

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


bool boxm_online_update_manager::release_offset_buffers()
{
  cl_int status;
  status = clReleaseMemObject(factor_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (factor_buf_)."))
    return false;

  status = clReleaseMemObject(offset_x_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (offset_x_buf_)."))
    return false;

  status = clReleaseMemObject(offset_y_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (offset_y_buf_).")==1;
}


bool boxm_online_update_manager::release_command_queue()
{
  cl_int status = clReleaseCommandQueue(command_queue_);
  return this->check_val(status,CL_SUCCESS,"clReleaseCommandQueue failed.") == 1;
}

