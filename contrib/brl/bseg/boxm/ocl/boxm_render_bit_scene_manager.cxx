#include "boxm_render_bit_scene_manager.h"
//:
// \file
#include <vcl_where_root_dir.h>
#include <boxm/ocl/boxm_ocl_utils.h>
#include <vcl_cstdio.h>
#include <boxm/boxm_block.h>
#include <boxm/boxm_scene.h>
#include <boxm/util/boxm_utils.h>
#include <boxm/basic/boxm_block_vis_graph_iterator.h>
#include <bsta/bsta_histogram.h>
#include <vil/vil_save.h>
#include <vcl_cstdlib.h>
//: Initializes CPU side input buffers
//put tree structure and data into arrays

bool boxm_render_bit_scene_manager::init_ray_trace(boxm_ocl_bit_scene *scene,
                                                   vpgl_camera_double_sptr cam,
                                                   vil_image_view<float> &obs,
                                                   bool render_depth)
{
  scene_ = scene;
  cam_ = cam;
  output_img_=obs;

  // Code for Pass_0
  if (!this->load_kernel_source(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    + "/contrib/brl/bseg/boxm/ocl/cl/scene_info.cl") ||
      //!this->append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
      //                              + "/contrib/brl/bseg/boxm/ocl/cl/float3.cl") ||
      !this->append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    + "/contrib/brl/bseg/boxm/ocl/cl/cell_utils.cl") ||
      !this->append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    +"/contrib/brl/bseg/boxm/ocl/cl/bit_tree_library_functions.cl") ||
      !this->append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    +"/contrib/brl/bseg/boxm/ocl/cl/backproject.cl")||
      !this->append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    +"/contrib/brl/bseg/boxm/ocl/cl/statistics_library_functions.cl")||
      !this->append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    +"/contrib/brl/bseg/boxm/ocl/cl/expected_functor.cl")||
      !this->append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    +"/contrib/brl/bseg/boxm/ocl/cl/ray_bundle_library_functions.cl")||
      !this->append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    +"/contrib/brl/bseg/boxm/ocl/cl/ray_trace_bit_scene_float3.cl")) {
    vcl_cerr << "Error: boxm_render_bit_scene_manager : failed to load kernel source (helper functions)\n";
    return false;
  }

  vcl_string patt = "/*$$step_cell$$*/";
  // transfer cell data from global to local memory if use_cell_data_ == true

  vcl_string functor="";
  if (render_depth) {
    vcl_cout<<"Using Functor step_cell_render_depth ="<<vcl_endl;
    functor="step_cell_render_depth(alpha_array,data_ptr,d,global_depth,&data_return);";
  }
  else {
    vcl_cout<<"Using functor step_cell_render_opt"<<vcl_endl;
    functor = "step_cell_render_opt(mixture_array,alpha_array,data_ptr,d,&data_return);";
    // was: functor="step_cell_render(sample_array,alpha_array,data_ptr,d,&data_return);";
  }

  // assign the functor calling signature
  vcl_string::size_type pos_start = this->prog_.find(patt);
  vcl_string::size_type n1 = patt.size();
  if (pos_start < this->prog_.size()) {
    vcl_string::size_type n2 = functor.size();
    if (!n2)
      return false;
    this->prog_ = this->prog_.replace(pos_start, n1, functor.c_str(), n2);
    return this->build_kernel_program(program_)==SDK_SUCCESS;
  }
  return !build_kernel_program(program_,render_depth);
}


bool boxm_render_bit_scene_manager::set_kernel()
{
  cl_int status = CL_SUCCESS;  kernels_.clear();
  vcl_cout<<"create: kernel0"<<vcl_endl;
  cl_kernel kernel0 = clCreateKernel(program_,"ray_trace_bit_scene_opt",&status);
  if (!this->check_val(status,CL_SUCCESS,error_to_string(status))) {
    return false;
  }
  kernels_.push_back(kernel0);

#if 0
  vcl_cout<<"create: kernel1"<<vcl_endl;
  cl_kernel kernel1 = clCreateKernel(program_,"rerender_ocl_scene_opt",&status);
  if (!this->check_val(status,CL_SUCCESS,error_to_string(status))) {
    return false;
  }
  kernels_.push_back(kernel1);
#endif
  return true;
}


bool boxm_render_bit_scene_manager::release_kernel()
{
  if (kernels_.size()>0)
  {
    for (unsigned i=0;i<kernels_.size();i++)
    {
      vcl_cout<<"release kernel"<<i<<vcl_endl;
      cl_int status = clReleaseKernel(kernels_[i]);
      if (!this->check_val(status,CL_SUCCESS,error_to_string(status)))
        return false;
    }
  }
  return true;
}


bool boxm_render_bit_scene_manager::set_args(unsigned kernel_index=0)
{
  if (kernels_.size()<=0 && kernel_index>=kernels_.size())
    return false;
  cl_int status = CL_SUCCESS;

  if (kernel_index==0)
  {
    //Temporary hack for scene info - to be put in another method
    //create a render scene info
    RenderSceneInfo* info = new RenderSceneInfo;
    info->scene_origin = *((cl_float4*)scene_origin_);  // scene origin (point)
    // was: info->scene_origin = (cl_float4) { scene_origin_[0], scene_origin_[1], scene_origin_[2], scene_origin_[3] };
    info->scene_dims   = *((cl_int4*)scene_dims_);      // number of blocks in each dimension
    // was: info->scene_dims   = (cl_int4) { scene_dims_[0], scene_dims_[1],scene_dims_[2],scene_dims_[3] };
    info->block_len    = block_dims_[0];    // size of each block (can only be 1 number now that we've established blocks are cubes)

    //tree meta information
    info->root_level   = root_level_;                // root_level of trees
    info->num_buffer   = numbuffer_;                 // number of buffers (both data and tree)
    info->tree_buffer_length = tree_buffer_length_;  // length of tree buffer (number of cells/trees)
    info->data_buffer_length = data_buffer_length_;

    scene_info_buf_ = clCreateBuffer(this->context_,
                                     CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                     sizeof(RenderSceneInfo),
                                     info,&status);
    if (!this->check_val(status,CL_SUCCESS,"scene info buff failed."))
      return 0;

    int i=0;
    status = clSetKernelArg(kernels_[0],i++,sizeof(cl_mem), (void *)&scene_info_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (render scene info)"))
      return 0;    
      
    //local copy of the scene info
    status = clSetKernelArg(kernels_[0],i++,sizeof(RenderSceneInfo),0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local scene info)"))
      return 0;
    
    //block pointers
    status = clSetKernelArg(kernels_[0],i++,sizeof(cl_mem),(void *)&block_ptrs_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (block_ptrs_buf_)"))
      return 0;

    // the tree buffer
    status = clSetKernelArg(kernels_[0],i++,sizeof(cl_mem),(void *)&trees_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cells_buf_)"))
      return 0;

    // alpha buffer
    status = clSetKernelArg(kernels_[0],i++,sizeof(cl_mem),(void *)&data_alpha_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_data_buf_)"))
      return 0;
    //mixture buffer
    status = clSetKernelArg(kernels_[0], i++, sizeof(cl_mem), (void *)&data_mixture_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mixture_buf)"))
      return 0;

    // camera buffer
    status = clSetKernelArg(kernels_[0],i++,sizeof(cl_mem),(void *)&persp_cam_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (data)"))
      return 0;
    // roi dimensions
    status = clSetKernelArg(kernels_[0],i++,sizeof(cl_mem),(void *)&img_dims_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (Img dimensions)"))
      return 0;
    //local copy of the tree (one for each thread/ray)
    status = clSetKernelArg(kernels_[0],i++,this->bni_*this->bnj_*sizeof(cl_uchar16),0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local tree)"))
      return 0;

    //// output image buffer
    status = clSetKernelArg(kernels_[0],i++,3*sizeof(cl_float16),0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local cam)"))
      return 0;
    status = clSetKernelArg(kernels_[0],i++,sizeof(cl_uint4),0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local image dimensions)"))
      return 0;

    status = clSetKernelArg(kernels_[0],i++,sizeof(cl_mem),(void *)&image_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (input_image)"))
      return 0;
    status = clSetKernelArg(kernels_[0],i++,sizeof(cl_mem),(void *)&image_gl_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (gl_image)"))
      return 0;
    //output buffer
    //status = clSetKernelArg(kernels_[0],i++,sizeof(cl_mem),(void *)&output_buf_);
    //if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output)"))
      //return 0;
    //bit lookup buffer
    status = clSetKernelArg(kernels_[0],i++,sizeof(cl_mem),(void *)&bit_lookup_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output)"))
      return 0;

  }
  else if (kernel_index==1)
  {
    //: Set args for second kernel
    int i=0;
    // depth image buffer
    status = clSetKernelArg(kernels_[1],i++,sizeof(cl_mem),(void *)&image_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (block_ptrs_buf_)"))
      return 0;
    //status = clSetKernelArg(kernels_[1],i++,sizeof(cl_mem),(void *)&ext_image_buf_);
    //if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (orig_image_buf_)"))
      //return 0;
    //status = clSetKernelArg(kernels_[1],i++,sizeof(cl_mem),(void *)&ext_cam_buf_);
    //if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (orig_cam_buf_)"))
      //return 0;
    status = clSetKernelArg(kernels_[1],i++,sizeof(cl_mem),(void *)&scene_dims_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (scene_dims_buf_)"))
      return 0;
    status = clSetKernelArg(kernels_[1],i++,sizeof(cl_mem),(void *)&scene_origin_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (scene_orign_buf_)"))
      return 0;
    status = clSetKernelArg(kernels_[1],i++,sizeof(cl_mem),(void *)&block_dims_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (block_dims_buf_)"))
      return 0;
    status = clSetKernelArg(kernels_[1],i++,sizeof(cl_mem),(void *)&block_ptrs_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (block_ptrs_buf_)"))
      return 0;
    // root level buffer
    status = clSetKernelArg(kernels_[1],i++,sizeof(cl_uint), &root_level_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (root_level_buf_)"))
      return 0;
    // the number of buffers
    status = clSetKernelArg(kernels_[1],i++,sizeof(cl_int), &numbuffer_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (lenbuffer_buf_)"))
      return 0;
    // the length of the tree buffer
    status = clSetKernelArg(kernels_[1],i++,sizeof(cl_int), &tree_buffer_length_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (lenbuffer_buf_)"))
      return 0;
    // the tree buffer
    status = clSetKernelArg(kernels_[1],i++,sizeof(cl_mem),(void *)&trees_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cells_buf_)"))
      return 0;
    // the length of the tree buffer
    status = clSetKernelArg(kernels_[1],i++,sizeof(cl_int), &data_buffer_length_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (lenbuffer_buf_)"))
      return 0;
    // alpha buffer
    status = clSetKernelArg(kernels_[1],i++,sizeof(cl_mem),(void *)&data_alpha_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_data_buf_)"))
      return 0;
    //mixture buffer !!IS THIS NEEDED HERE?!!

    // camera buffer
    status = clSetKernelArg(kernels_[1],i++,sizeof(cl_mem),(void *)&persp_cam_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (data)"))
      return 0;
    // roi dimensions
    status = clSetKernelArg(kernels_[1],i++,sizeof(cl_mem),(void *)&img_dims_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (Img dimensions)"))
      return 0;
    //// output image buffer
    status = clSetKernelArg(kernels_[1],i++,3*sizeof(cl_float16),0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local cam)"))
      return 0;
    status = clSetKernelArg(kernels_[1],i++,sizeof(cl_uint4),0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local image dimensions)"))
      return 0;
    status = clSetKernelArg(kernels_[1],i++,sizeof(cl_float4),0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local image dimensions)"))
      return 0;

    //status = clSetKernelArg(kernels_[1],i++,sizeof(cl_mem),(void *)&rerender_image_buf_);
    //if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (input_image)"))
      //return 0;
#if 0
    image_gl_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                   wni_*wnj_*sizeof(cl_uint),
                                   image_gl_,&status);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (gl_image)"))
      return 0;
#endif

    status = clSetKernelArg(kernels_[1],i++,sizeof(cl_mem),(void *)&image_gl_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (gl_image)"))
      return 0;
  }
  return 1;
}


bool boxm_render_bit_scene_manager::set_commandqueue()
{
  cl_int status = CL_SUCCESS;
  vcl_cout<<"create: command_queue"<<vcl_endl;
  command_queue_ = clCreateCommandQueue(this->context(),this->devices()[0],CL_QUEUE_PROFILING_ENABLE,&status);
  if (!this->check_val(status,CL_SUCCESS,"Failed in command queue creation" + error_to_string(status)))
    return false;
  else
    return true;
}


bool boxm_render_bit_scene_manager::release_commandqueue()
{
  if (command_queue_)
  {
    vcl_cout<<"release: commandqueue"<<vcl_endl;
    cl_int status = clReleaseCommandQueue(command_queue_);
    if (!this->check_val(status,CL_SUCCESS,"clReleaseCommandQueue failed."))
      return false;
  }
  return true;
}


bool boxm_render_bit_scene_manager::set_workspace()
{
  globalThreads[0]=this->wni_;
  globalThreads[1]=this->wnj_;

  localThreads[0]=this->bni_;
  localThreads[1]=this->bnj_;

  return true;
}


bool boxm_render_bit_scene_manager::start(bool set_gl_buffer)
{
  bool good=true;
  good = good && this->set_scene_data()
              && this->set_all_blocks()
              && this->set_scene_data_buffers()
              && this->set_tree_buffers()
              && this->set_persp_camera()
              && this->set_input_image();

 if (set_gl_buffer)
    this->set_gl_buffer();

  good = good && this->set_persp_camera_buffers()
              && this->set_input_image_buffers()
              && this->set_image_dims_buffers();

  good= good  && this->set_kernel()
              && this->set_args(0);

   good=good  && this->set_commandqueue()
              && this->set_workspace();

  return good;
}


bool boxm_render_bit_scene_manager::run(bool rerender)
{
  cl_int status = CL_SUCCESS;

  // set up a command queue

  cl_event ceEvent;
  status = clEnqueueNDRangeKernel(command_queue_,this->kernels_[0], 2,NULL,globalThreads,localThreads,0,NULL,&ceEvent);

  if (!this->check_val(status,CL_SUCCESS,"clEnqueueNDRangeKernel failed. "+error_to_string(status)))
    return SDK_FAILURE;

  status = clFinish(command_queue_);
  if (!this->check_val(status,CL_SUCCESS,"clFinish failed."+error_to_string(status)))
    return SDK_FAILURE;
  cl_ulong tstart,tend;
  status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_END,sizeof(cl_ulong),&tend,0);
  status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),&tstart,0);
  gpu_time_= (double)1.0e-6 * (tend - tstart); // convert nanoseconds to milliseconds
  vcl_cout<<"GPU time is "<<gpu_time_<<vcl_endl;
  status = clReleaseEvent(ceEvent);

#ifdef DEBUG
  //-------- DEBUG READ AND PRINT --------------------------------------
  cl_event events[2];
  status = clEnqueueReadBuffer(command_queue_,output_buf_,CL_TRUE,
                               0,numbuffer_*sizeof(cl_float4),
                               output_,
                               0,NULL,&events[0]);

  // Wait for the read buffer to finish execution
  status = clWaitForEvents(1, &events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
    return false;
  status = clReleaseEvent(events[0]);
  vcl_cout<<"KERNEL OUTPUT:"<<vcl_endl;
  for (int i=0; i<1; i++)
    vcl_cout<<output_[4*i]<<' '
            <<output_[4*i+1]<<' '
            <<output_[4*i+2]<<' '
            <<output_[4*i+3]<<'\n';
  vcl_cout<<"MATCH?\n"
          <<(int)scene_->tree_buffers_(4, 10)[0]<<' '<<(int)scene_->tree_buffers_(33,11)[0]<<vcl_endl;
  //--------------------------------------------------------------------
#endif // DEBUG

  if (rerender)
  {
    status = clEnqueueNDRangeKernel(command_queue_,this->kernels_[1], 2,NULL,globalThreads,localThreads,0,NULL,&ceEvent);
    if (!this->check_val(status,CL_SUCCESS,"clEnqueueNDRangeKernel failed. "+error_to_string(status)))
      return SDK_FAILURE;

    status = clFinish(command_queue_);
    if (!this->check_val(status,CL_SUCCESS,"clFinish failed."+error_to_string(status)))
      return SDK_FAILURE;
  }
  return SDK_SUCCESS;
}

bool boxm_render_bit_scene_manager::finish()
{
  bool good=true;
  good =good && release_tree_buffers()
             && read_output_image()
             && release_input_view_buffers()
             && release_scene_data_buffers();

  this->release_kernel();
  return good;
}

bool boxm_render_bit_scene_manager::set_gl_buffer()
{
    cl_int status=0;
    vcl_cout<<"create: gl_buffer"<<vcl_endl;
    image_gl_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                   wni_*wnj_*sizeof(cl_uint),
                                   image_gl_,&status);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (gl_image)"))
        return false;
    return true;
}

bool boxm_render_bit_scene_manager::release_gl_buffer()
{
  vcl_cout<<"release gl_buffer called"<<vcl_endl;
  cl_int status;
  status = clReleaseMemObject(image_gl_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (gl_buffer).")==1;
}

bool boxm_render_bit_scene_manager::run_scene()
{
  //set up image gl buf for arguments sake

  bool good=true;
  vcl_string error_message="";
  good=good && set_scene_data()
            && set_all_blocks()
            && set_scene_data_buffers()
            && set_tree_buffers();
  // run the raytracing
  good=good && set_input_view()
            && set_input_view_buffers();
  this->set_kernel();
  this->set_gl_buffer();
  this->set_args();
  this->set_commandqueue();
  this->set_workspace();

  this->run();

  good =good && release_tree_buffers()
             && clean_tree()
             && read_output_image();
#ifdef DEBUG
  this->print_image();
#endif
  good=good && release_input_view_buffers()
  //        && clean_input_view()
            && release_scene_data_buffers()
            && release_gl_buffer()
            && clean_scene_data();

  // release the command Queue
  this->release_kernel();
  this->release_commandqueue();
  if (program_){
    vcl_cout<<"release: program"<<vcl_endl;
    clReleaseProgram(program_);
  }
  clReleaseContext(this->context_);
#ifdef DEBUG
  vcl_cout << "Timing Analysis\n"
           << "===============\n"
           << "openCL Running time "<<gpu_time_<<" ms\n"
           << "Running block "<<total_gpu_time/1000<<"s\n"
           << "total block loading time = " << total_load_time << "s\n"
           << "total block processing time = " << total_raytrace_time << 's' << vcl_endl;
#endif
  return good;
}

bool boxm_render_bit_scene_manager::read_output_image()
{
  vcl_cout<<"READING OUTPUT IMAGE "<<vcl_endl;
  cl_event events[1];

  // Enqueue readBuffers
  int status = clEnqueueReadBuffer(command_queue_,image_buf_,CL_TRUE,
                                   0,this->wni_*this->wnj_*sizeof(cl_float),
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

void boxm_render_bit_scene_manager::save_image(vcl_string img_filename)
{
    vil_image_view<float> oimage(output_img_.ni(),output_img_.nj());
    for (unsigned i=0;i<output_img_.ni();i++)
        for (unsigned j=0;j<output_img_.nj();j++)
            oimage(i,j)=image_[(j*wni_+i)];
    vil_save(oimage,img_filename.c_str());
}


bool boxm_render_bit_scene_manager::read_trees()
{
  cl_event events[2];

  // Enqueue readBuffers
  int status = clEnqueueReadBuffer(command_queue_,trees_buf_,CL_TRUE,
                                   0,trees_size_*sizeof(cl_uchar16),
                                   trees_,
                                   0,NULL,&events[0]);

  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (cells )failed."))
    return false;

  status = clWaitForEvents(1, &events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
    return false;

  status = clEnqueueReadBuffer(command_queue_,data_alpha_buf_,CL_TRUE,
                               0,data_size_*sizeof(cl_float),
                               data_alpha_,
                               0,NULL,&events[0]);

  if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (cell alpha )failed."))
    return false;
  status = clWaitForEvents(1, &events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
    return false;

  status = clWaitForEvents(1, &events[0]);
  if (!this->check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
    return false;
  // Wait for the read buffer to finish execution

  status = clReleaseEvent(events[0]);
  return this->check_val(status,CL_SUCCESS,"clReleaseEvent failed.")==1;
}


void boxm_render_bit_scene_manager::print_tree()
{
  vcl_cout << "Tree Input\n";
  if (trees_)
    for (unsigned i = 0; i<trees_size_*16; i+=16) {
      vcl_cout << "tree input[" << i/16 << "]("
               << trees_[i]   << ' '
               << trees_[i+1] << ' '
               << trees_[i+2] << ' '
               << trees_[i+3];

#ifdef DEBUG
      int data_ptr = 16*cells_[i+2];
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
#endif // 0
      vcl_cout << ")\n";
    }
}


void boxm_render_bit_scene_manager::print_image()
{
  vcl_cout << "IMage Output\n";
  if (image_)
  {
    vcl_cout<<"Plane 0"<<vcl_endl;
    for (unsigned j=0;j<img_dims_[3];j++)
    {
      for (unsigned i=0;i<img_dims_[2];i++)
        vcl_cout<<image_[(j*img_dims_[2]+i)]<<' ';
      vcl_cout<<vcl_endl;
    }
#ifdef DEBUG
    vcl_cout<<"Plane 1"<<vcl_endl;
    for (unsigned j=0;j<img_dims_[3];j++)
    {
      for (unsigned i=0;i<img_dims_[2];i++)
        vcl_cout<<image_[(j*img_dims_[2]+i)*4+1]<<' ';
      vcl_cout<<vcl_endl;
    }
    vcl_cout<<"Plane 2"<<vcl_endl;
    for (unsigned j=0;j<img_dims_[3];j++)
    {
      for (unsigned i=0;i<img_dims_[2];i++)
        vcl_cout<<image_[(j*img_dims_[2]+i)*4+2]<<' ';
      vcl_cout<<vcl_endl;
    }
    vcl_cout<<"Plane 3"<<vcl_endl;
    for (unsigned j=0;j<img_dims_[3];j++)
    {
      for (unsigned i=0;i<img_dims_[2];i++)
        vcl_cout<<image_[(j*img_dims_[2]+i)*4+3]<<' ';
      vcl_cout<<vcl_endl;
    }
#endif // 0
  }
}


bool boxm_render_bit_scene_manager::clean_update()
{
  return true;
}

/*******************************************
 * build_kernel_program - builds kernel program
 * from source (a vcl string)
 *******************************************/

int boxm_render_bit_scene_manager::build_kernel_program(cl_program & program, bool render_depth)
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

  vcl_string options="";
  if (render_depth)
    options+="-D DEPTH ";
  else
    options+="-D INTENSITY ";

  if (vcl_strstr(this->platform_name,"ATI"))
    options+="-D ATI ";
  if (vcl_strstr(this->platform_name,"NVIDIA"))
    options+="-D NVIDIA ";
    
  options += "cl-opt-disable";

  vcl_cout<<"create: program"<<vcl_endl;
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


bool boxm_render_bit_scene_manager::set_scene_data()
{
  return set_scene_dims()
      && set_scene_origin()
      && set_block_dims()
      && set_block_ptrs()
      && set_root_level();
}


bool boxm_render_bit_scene_manager::clean_scene_data()
{
  return clean_scene_dims()
      && clean_scene_origin()
      && clean_block_dims()
      && clean_block_ptrs()
      && clean_root_level();
}


bool boxm_render_bit_scene_manager::set_scene_data_buffers()
{
  return set_scene_dims_buffers()
      && set_scene_origin_buffers()
      && set_block_dims_buffers()
      && set_block_ptrs_buffers();
}


bool boxm_render_bit_scene_manager::release_scene_data_buffers()
{
  return release_scene_dims_buffers()
      && release_scene_origin_buffers()
      && release_block_dims_buffers()
      && release_block_ptrs_buffers();
}


bool boxm_render_bit_scene_manager::set_root_level()
{
  if (scene_==NULL)
  {
    vcl_cout<<"Scene is Missing"<<vcl_endl;
    return false;
  }
  root_level_=scene_->max_level()-1;
  return true;
}


bool boxm_render_bit_scene_manager::clean_root_level()
{
  root_level_=0;
  return true;
}


bool boxm_render_bit_scene_manager::set_scene_origin()
{
  if (scene_==NULL)
  {
    vcl_cout<<"Scene is Missing"<<vcl_endl;
    return false;
  }
  vgl_point_3d<double> orig=scene_->origin();

  scene_origin_=(cl_float *)boxm_ocl_utils::alloc_aligned(1,sizeof(cl_float4),16);

  scene_origin_[0]=(float)orig.x();
  scene_origin_[1]=(float)orig.y();
  scene_origin_[2]=(float)orig.z();
  scene_origin_[3]=0.0f;

  return true;
}


bool boxm_render_bit_scene_manager::set_scene_origin_buffers()
{
  cl_int status;
  vcl_cout<<"create: scene_origin_buf"<<vcl_endl;
  scene_origin_buf_ = clCreateBuffer(this->context_,
                                     CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                     4*sizeof(cl_float),
                                     scene_origin_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (scene_origin_) failed.")==1;
}


bool boxm_render_bit_scene_manager::release_scene_origin_buffers()
{
  vcl_cout<<"release scene_origin called"<<vcl_endl;
  cl_int status;
  status = clReleaseMemObject(scene_origin_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (scene_origin_buf_).")==1;
}


bool boxm_render_bit_scene_manager::clean_scene_origin()
{
  if (scene_origin_)
    boxm_ocl_utils::free_aligned(scene_origin_);
  return true;
}


bool boxm_render_bit_scene_manager::set_scene_dims()
{
  if (scene_==NULL)
  {
    vcl_cout<<"Scene is Missing"<<vcl_endl;
    return false;
  }
  scene_->block_num(scene_x_,scene_y_,scene_z_);
  scene_dims_=(cl_int *)boxm_ocl_utils::alloc_aligned(1,sizeof(cl_int4),16);

  scene_dims_[0]=scene_x_;
  scene_dims_[1]=scene_y_;
  scene_dims_[2]=scene_z_;
  scene_dims_[3]=0;

  return true;
}


bool boxm_render_bit_scene_manager::set_scene_dims_buffers()
{
  cl_int status;
  vcl_cout<<"create: scene_dims_buf"<<vcl_endl;
  scene_dims_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                   sizeof(cl_int4),
                                   scene_dims_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (scene_dims_) failed.")==1;
}


bool boxm_render_bit_scene_manager::release_scene_dims_buffers()
{
  vcl_cout<<"release scene_dims called"<<vcl_endl;
  cl_int status;
  status = clReleaseMemObject(scene_dims_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (scene_dims_buf_).")==1;
}


bool boxm_render_bit_scene_manager::clean_scene_dims()
{
  if (scene_dims_)
    boxm_ocl_utils::free_aligned(scene_dims_);
  return true;
}


bool boxm_render_bit_scene_manager::set_block_dims()
{
  if (scene_==NULL)
  {
    vcl_cout<<"Scene is Missing"<<vcl_endl;
    return false;
  }
  double x,y,z;
  scene_->block_dim(x,y,z);

  block_dims_=(cl_float *)boxm_ocl_utils::alloc_aligned(1,sizeof(cl_float4),16);

  block_dims_[0]=(float)x;
  block_dims_[1]=(float)y;
  block_dims_[2]=(float)z;
  block_dims_[3]=0;

  return true;
}


bool boxm_render_bit_scene_manager::set_block_dims_buffers()
{
  cl_int status;
  vcl_cout<<"create: block_dims_buf"<<vcl_endl;
  block_dims_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                   4*sizeof(cl_float),
                                   block_dims_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (block_dims_) failed.")==1;
}


bool boxm_render_bit_scene_manager::release_block_dims_buffers()
{
  vcl_cout<<"release block_dims called"<<vcl_endl;
  cl_int status;
  status = clReleaseMemObject(block_dims_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (block_dims_buf_).")==1;
}


bool boxm_render_bit_scene_manager::clean_block_dims()
{
  if (block_dims_)
    boxm_ocl_utils::free_aligned(block_dims_);
  return true;
}


bool boxm_render_bit_scene_manager::set_block_ptrs()
{
  if (scene_==NULL)
  {
    vcl_cout<<"Scene is Missing"<<vcl_endl;
    return false;
  }
  scene_->block_num(scene_x_,scene_y_,scene_z_);
  int numblocks=scene_x_*scene_y_*scene_z_;
  vcl_cout<<"Block size "<<(float)numblocks*16/1024.0/1024.0<<"MB"<<vcl_endl;

  block_ptrs_=(cl_ushort*) boxm_ocl_utils::alloc_aligned(numblocks,sizeof(cl_ushort2),16);
  scene_->get_block_ptrs(block_ptrs_);
  return true;
}


bool boxm_render_bit_scene_manager::set_block_ptrs_buffers()
{
  cl_int status;
  vcl_cout<<"create: block_ptrs_buf_"<<vcl_endl;
  block_ptrs_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                   scene_x_*scene_y_*scene_z_*sizeof(cl_ushort2),
                                   block_ptrs_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (block_ptrs_) failed.")==1;
}


bool boxm_render_bit_scene_manager::release_block_ptrs_buffers()
{
  vcl_cout<<"release block_ptrs called"<<vcl_endl;
  cl_int status;
  status = clReleaseMemObject(block_ptrs_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (block_ptrs_buf_).")==1;
}


bool boxm_render_bit_scene_manager::clean_block_ptrs()
{
  if (block_ptrs_)
    boxm_ocl_utils::free_aligned(block_ptrs_);
  return true;
}

bool boxm_render_bit_scene_manager::set_input_view()
{
  return set_persp_camera()
      && set_input_image();
}


bool boxm_render_bit_scene_manager::clean_input_view()
{
  return clean_persp_camera()
      && clean_input_image();
}


bool boxm_render_bit_scene_manager::set_input_view_buffers()
{
  return set_persp_camera_buffers()
      && set_input_image_buffers()
      && set_image_dims_buffers();
}


bool boxm_render_bit_scene_manager::release_input_view_buffers()
{
  return release_persp_camera_buffers()
      && release_input_image_buffers();
}


bool boxm_render_bit_scene_manager::set_all_blocks()
{
  if (!scene_)
    return false;
  trees_        = NULL;
  data_alpha_   = NULL;
  data_mixture_ = NULL;
  bit_lookup_   = NULL;
  scene_->tree_buffer_shape(numbuffer_, tree_buffer_length_);
  scene_->data_buffer_shape(numbuffer_, data_buffer_length_);
  trees_size_   = numbuffer_*tree_buffer_length_;
  data_size_    = numbuffer_*data_buffer_length_;

  /******* debug print **************/
  int cellBytes = sizeof(cl_uchar16);
  int dataBytes = sizeof(cl_uchar8)+sizeof(cl_float);
  vcl_cout<<"Optimized sizes:\n"
          <<"    cells: "<<(float)trees_size_*cellBytes/1024.0/1024.0<<" MB\n"
          <<"    data:  "<<(float)data_size_*dataBytes/1024.0/1024.0<<" MB"<<vcl_endl;
  /**********************************/

  //allocate and initialize tree cells
  trees_ = (cl_uchar*) boxm_ocl_utils::alloc_aligned(trees_size_,sizeof(cl_uchar16),16);
  scene_->get_tree_cells(trees_);

  //allocate and initialize alphas
  data_alpha_ = (cl_float *)boxm_ocl_utils::alloc_aligned(data_size_,sizeof(cl_float),16);
  scene_->get_alphas(data_alpha_);

  //allocate and initialize mix components
  data_mixture_ = (cl_uchar *) boxm_ocl_utils::alloc_aligned(data_size_,sizeof(cl_uchar8),16);
  scene_->get_mixture(data_mixture_);

  //allocate bit lookup hash table
  bit_lookup_ = (cl_uchar *) boxm_ocl_utils::alloc_aligned(256,sizeof(cl_uchar),16);
  unsigned char bits[] = { 0,   1,   1,   2,   1,   2,   2,   3,   1,   2,   2,   3,   2,   3,   3,   4,
                           1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5 ,
                           1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7  ,
                           1,   2,   2,   3,   2,   3,   3,   4,   2,   3,   3,   4,   3,   4,   4,   5  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7  ,
                           2,   3,   3,   4,   3,   4,   4,   5,   3,   4,   4,   5,   4,   5,   5,   6  ,
                           3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7  ,
                           3,   4,   4,   5,   4,   5,   5,   6,   4,   5,   5,   6,   5,   6,   6,   7  ,
                           4,   5,   5,   6,   5,   6,   6,   7,   5,   6,   6,   7,   6,   7,   7,   8 };
  for (int i=0; i<256; i++) bit_lookup_[i] = bits[i];

  //---- debug alloc --------------------------------------------------
  output_ = NULL;
  output_ = (cl_float*) boxm_ocl_utils::alloc_aligned(numbuffer_,sizeof(cl_float4),16);
  for (int i=0; i<numbuffer_*4; i++) output_[i] = 0.0;
  //-------------------------------------------------------------------

  if (trees_== NULL|| data_mixture_ == NULL || data_alpha_==NULL) {
    vcl_cout << "Failed to allocate host memory. (tree input)\n";
    return false;
  }
  return true;
}

bool boxm_render_bit_scene_manager::clean_tree()
{
  if (trees_)
    boxm_ocl_utils::free_aligned(trees_);
  if (data_mixture_)
    boxm_ocl_utils::free_aligned(data_mixture_);
  if (data_alpha_)
    boxm_ocl_utils::free_aligned(data_alpha_);
  if (output_)
    boxm_ocl_utils::free_aligned(output_);
  if (bit_lookup_)
    boxm_ocl_utils::free_aligned(bit_lookup_);
  data_buffer_length_=0;
  tree_buffer_length_=0;
  numbuffer_=0;
  output_ = 0;
  bit_lookup_ = 0;
  return true;
}


bool boxm_render_bit_scene_manager::set_tree_buffers()
{
  vcl_cout<<"create: trees, data_alpha, data_mixture, bit_lookup, output"<<vcl_endl;
  cl_int status;
  trees_buf_ = clCreateBuffer(this->context_,
                              CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                              trees_size_*sizeof(cl_uchar16),
                              trees_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (tree) failed."))
    return false;

  data_alpha_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                   data_size_*sizeof(cl_float),
                                   data_alpha_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (cell data) failed."))
    return false;

  data_mixture_buf_ = clCreateBuffer(this->context_,
                                     CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                     data_size_*sizeof(cl_uchar8),
                                     data_mixture_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (cell mixture) failed."))
    return false;

  bit_lookup_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                   256*sizeof(cl_uchar),
                                   bit_lookup_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (bit_lookup) failed."))
    return false;

  output_buf_ = clCreateBuffer(this->context_,
                               CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                               numbuffer_*sizeof(cl_float4),
                               output_,&status);
  if (!this->check_val(status,CL_SUCCESS,"clCreateBuffer (tree) failed."))
    return false;

  /******* debug print ***************/
  int cellBytes  = trees_size_*sizeof(cl_uchar16);
  int alphaBytes = data_size_*sizeof(cl_float);
  int mixBytes   = data_size_*sizeof(cl_uchar8);
  int blockBytes = scene_x_*scene_y_*scene_z_*sizeof(cl_ushort2);
  float MB = (cellBytes + alphaBytes + mixBytes + blockBytes)/1024.0/1024.0;
  vcl_cout<<"GPU Mem allocated:\n"
          <<"   cells: "<<cellBytes<<" bytes\n"
          <<"   alpha: "<<alphaBytes<<" bytes\n"
          <<"   mix  : "<<mixBytes<<" bytes\n"
          <<"   block: "<<blockBytes<<" bytes\n"
          <<"TOTAL: "<<MB<<"MB"<<vcl_endl;
  /************************************/

 return true;
}


bool boxm_render_bit_scene_manager::release_tree_buffers()
{
  vcl_cout<<"release output, bit_lookup, trees, data_mixture, data_alpha called"<<vcl_endl;
  cl_int status;
  status = clReleaseMemObject(output_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (output_buf_)."))
    return false;
  status = clReleaseMemObject(bit_lookup_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (bit_lookup_buf_)."))
    return false;
  status = clReleaseMemObject(trees_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (cells_buf_)."))
    return false;
  status = clReleaseMemObject(data_mixture_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (cell_mixture_buf_)."))
    return false;
  status = clReleaseMemObject(data_alpha_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (cell_alpha_buf_).");
}


bool boxm_render_bit_scene_manager::set_persp_camera(vpgl_perspective_camera<double> * pcam)
{
  if (pcam)
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
    vcl_cerr << "Error set_persp_camera() : Missing camera\n";
    return false;
  }
}


bool boxm_render_bit_scene_manager::set_persp_camera()
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


bool boxm_render_bit_scene_manager::clean_persp_camera()
{
  if (persp_cam_)
    boxm_ocl_utils::free_aligned(persp_cam_);
  return true;
}


bool boxm_render_bit_scene_manager::set_persp_camera_buffers()
{
  cl_int status;
  vcl_cout<<"create: persp_cam_buf"<<vcl_endl;
  persp_cam_buf_ = clCreateBuffer(this->context_,
                                  CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                  3*sizeof(cl_float16),
                                  persp_cam_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (persp_cam_buf_) failed.")==1;
}


bool boxm_render_bit_scene_manager::write_persp_camera_buffers()
{
  cl_int status;
  status=clEnqueueWriteBuffer(command_queue_,persp_cam_buf_,CL_FALSE, 0,3*sizeof(cl_float16), persp_cam_, 0, 0, 0);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueWriteBuffer (persp_cam_buf_) failed."))
    return false;
  clFinish(command_queue_);

  return true;
}


bool boxm_render_bit_scene_manager::release_persp_camera_buffers()
{
  vcl_cout<<"release persp cam buffers called"<<vcl_endl;
  cl_int status;
  status = clReleaseMemObject(persp_cam_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (persp_cam_buf_).")==1;
}


bool boxm_render_bit_scene_manager::set_input_image()
{
  wni_=(cl_uint)RoundUp(output_img_.ni(),bni_);
  wnj_=(cl_uint)RoundUp(output_img_.nj(),bnj_);

  image_=(cl_float *)boxm_ocl_utils::alloc_aligned(wni_*wnj_,sizeof(cl_float),16);
  image_gl_=(cl_uint*)boxm_ocl_utils::alloc_aligned(wni_*wnj_,sizeof(cl_uint),16);

  img_dims_=(cl_uint *)boxm_ocl_utils::alloc_aligned(1,sizeof(cl_uint4),16);

  // pad the image
  for (unsigned i=0;i<output_img_.ni();i++)
  {
    for (unsigned j=0;j<output_img_.nj();j++)
    {
      image_[(j*wni_+i)]=0;
      image_gl_[(j*wni_+i)]=0;
    }
  }

  img_dims_[0]=0;
  img_dims_[1]=0;
  img_dims_[2]=output_img_.ni();
  img_dims_[3]=output_img_.nj();

  if (image_==NULL || img_dims_==NULL)
  {
    vcl_cerr<<"Failed allocation of image or image dimensions\n";
    return false;
  }
  else
    return true;
}

bool boxm_render_bit_scene_manager::clean_input_image()
{
  if (image_)
    boxm_ocl_utils::free_aligned(image_);
  if (img_dims_)
    boxm_ocl_utils::free_aligned(img_dims_);
  return true;
}


bool boxm_render_bit_scene_manager::set_input_image_buffers()
{
  cl_int status;
  vcl_cout<<"create: image_buf"<<vcl_endl;
  image_buf_ = clCreateBuffer(this->context_,
                              CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                              wni_*wnj_*sizeof(cl_float),
                              image_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (image_buf_) failed.")==1;
}


bool boxm_render_bit_scene_manager::set_image_dims_buffers()
{
  cl_int status;
  vcl_cout<<"create: img_dims_buf"<<vcl_endl;
  img_dims_buf_ = clCreateBuffer(this->context_,
                                 CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                 sizeof(cl_uint4),
                                 img_dims_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (imd_dims_buf_) failed.")==1;
}


bool boxm_render_bit_scene_manager::release_input_image_buffers()
{
  vcl_cout<<"release image_buf, image_dims called"<<vcl_endl;
  cl_int status;
  status = clReleaseMemObject(image_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (image_buf_)."))
    return false;

  status = clReleaseMemObject(img_dims_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (img_dims_buf_).")==1;
}

