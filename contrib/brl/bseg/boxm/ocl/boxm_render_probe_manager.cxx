#include "boxm_render_probe_manager.h"
//:
// \file
#include <vcl_where_root_dir.h>
#include <boxm/ocl/boxm_ocl_utils.h>
#include <vcl_cstdio.h>

//: Initializes CPU side input buffers
//put tree structure and data into arrays
bool boxm_render_probe_manager::init_ray_trace(boxm_ocl_scene *scene,
                                               vpgl_camera_double_sptr cam,
                                               unsigned i,
                                               unsigned j,
                                               float intensity=0.0f)
{
  scene_ = scene;
  cam_ = cam;
  i_=(int)i;
  j_=(int)j;
  intensity_=intensity;

  // Code for Pass_0
  if (!this->load_kernel_source(vcl_string(VCL_SOURCE_ROOT_DIR)
                                + "/contrib/brl/bseg/boxm/ocl/cl/loc_code_library_functions.cl") ||
      !this->append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    + "/contrib/brl/bseg/boxm/ocl/cl/cell_utils.cl") ||
      !this->append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    +"/contrib/brl/bseg/boxm/ocl/cl/octree_library_functions.cl") ||
      !this->append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    +"/contrib/brl/bseg/boxm/ocl/cl/backproject.cl")||
      !this->append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    +"/contrib/brl/bseg/boxm/ocl/cl/statistics_library_functions.cl")||
      !this->append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    +"/contrib/brl/bseg/boxm/ocl/cl/expected_functor.cl")||
      !this->append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    +"/contrib/brl/bseg/boxm/ocl/cl/ray_bundle_library_functions.cl")||
      !this->append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    +"/contrib/brl/bseg/boxm/ocl/cl/ray_trace_ocl_scene.cl")) {
    vcl_cerr << "Error: boxm_render_ocl_scene_manager : failed to load kernel source (helper functions)\n";
    return false;
  }

  return !build_kernel_program(program_);
}


bool boxm_render_probe_manager::setup_online_processing()
{
  int bundle_dim = 1;
  this->set_bundle_ni(bundle_dim);
  this->set_bundle_nj(bundle_dim);
  bool good=true;
  good = good && this->set_scene_data()
              && this->set_all_blocks()
              && this->set_scene_data_buffers()
              && this->set_tree_buffers()
              && this->set_rayoutput()
              && this->set_rayoutput_buffers();
  if (vpgl_perspective_camera<double> * cam=dynamic_cast<vpgl_perspective_camera<double>* >(cam_.ptr()))
  {
    // run the raytracing
    good = good && this->set_persp_camera(cam)
                && this->set_persp_camera_buffers();

    good=good && this->set_kernel();
    this->set_args();
    this->set_commandqueue();
    this->set_workspace();

    vcl_cout<<"Setup Successful";

    return good;
  }
  else
    return false;
}

bool boxm_render_probe_manager::online_processing(vpgl_camera_double_sptr & camera)
{
  if (vpgl_perspective_camera<double> * cam=dynamic_cast<vpgl_perspective_camera<double>* >(camera.ptr()))
  {
      this->set_persp_camera(cam);
      this->write_persp_camera_buffers();
      this->run();
      return true;
  }
  return false;
}

void boxm_render_probe_manager::getoutputarray(vcl_vector< vcl_vector<float> >& out)
{
    out.resize(10);
    for (unsigned j=0;j<10;j++)
        for (unsigned i=0;i<raydepth_;i++)
            if (rayoutput_[j][i]>-1)
                out[j].push_back(rayoutput_[j][i]);
}


bool boxm_render_probe_manager::finish_online_processing()
{
  bool good=true;

  good=good && this->release_scene_data_buffers();
  good=good && this->clean_scene_data()
            && this->release_rayoutput_buffers()
            && this->clean_rayoutput();

  // release the command Queue
  this->release_kernel();
  this->release_commandqueue();

  return good;
}

bool boxm_render_probe_manager::set_kernel()
{
  cl_int status = CL_SUCCESS;
  kernel_ = clCreateKernel(program_,"single_ray_probe_opt",&status);
  if (!check_val(status,CL_SUCCESS,error_to_string(status))) {
    return false;
  }
  return true;
}


bool boxm_render_probe_manager::release_kernel()
{
    cl_int status = clReleaseKernel(kernel_);
    if (!check_val(status,CL_SUCCESS,error_to_string(status)))
        return false;
    return true;
}


bool boxm_render_probe_manager::set_args()
{
    cl_int status = CL_SUCCESS;
    int i=0;

    status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&scene_dims_buf_);
    if (!check_val(status,CL_SUCCESS,"clSetKernelArg failed. (scene_dims_buf_)"))
        return 0;
    status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&scene_origin_buf_);
    if (!check_val(status,CL_SUCCESS,"clSetKernelArg failed. (scene_orign_buf_)"))
        return 0;
    status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&block_dims_buf_);
    if (!check_val(status,CL_SUCCESS,"clSetKernelArg failed. (block_dims_buf_)"))
        return 0;
    status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&block_ptrs_buf_);
    if (!check_val(status,CL_SUCCESS,"clSetKernelArg failed. (block_ptrs_buf_)"))
        return 0;
    // root level buffer
    status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&root_level_buf_);
    if (!check_val(status,CL_SUCCESS,"clSetKernelArg failed. (root_level_buf_)"))
        return 0;
    // the length of buffer
    status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&numbuffer_buf_);
    if (!check_val(status,CL_SUCCESS,"clSetKernelArg failed. (lenbuffer_buf_)"))
        return 0;

    // the length of buffer
    status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&lenbuffer_buf_);
    if (!check_val(status,CL_SUCCESS,"clSetKernelArg failed. (lenbuffer_buf_)"))
        return 0;
    // the tree buffer
    status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&cells_buf_);
    if (!check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cells_buf_)"))
        return 0;
    // alpha buffer
    status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&cell_alpha_buf_);
    if (!check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_data_buf_)"))
        return 0;
    //mixture buffer
    status = clSetKernelArg(kernel_, i++, sizeof(cl_mem), (void *)&cell_mixture_buf_);
    if (!check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mixture_buf)"))
        return 0;

    // camera buffer
    status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&persp_cam_buf_);
    if (!check_val(status,CL_SUCCESS,"clSetKernelArg failed. (data)"))
        return 0;
    //// output image buffer
    status = clSetKernelArg(kernel_,i++,3*sizeof(cl_float16),0);
    if (!check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local cam)"))
        return 0;
    status = clSetKernelArg(kernel_, i++, sizeof(cl_int),&i_);
    if (!check_val(status,CL_SUCCESS,"clSetKernelArg failed. (i) buffer)"))
        return SDK_FAILURE;
    status = clSetKernelArg(kernel_, i++, sizeof(cl_int),&j_);
    if (!check_val(status,CL_SUCCESS,"clSetKernelArg failed. (j) buffer)"))
        return SDK_FAILURE;
    status = clSetKernelArg(kernel_, i++, sizeof(cl_float),&intensity_);
    if (!check_val(status,CL_SUCCESS,"clSetKernelArg failed. (intensity) buffer)"))
        return SDK_FAILURE;
    for (unsigned cnt=0;cnt<10;cnt++)
    {
    status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&rayoutput_buf_[cnt]);
    if (!check_val(status,CL_SUCCESS,"clSetKernelArg failed. (input_image)"))
        return 0;
    }
    status = clSetKernelArg(kernel_, i++, sizeof(cl_mem),(void*)&output_buf_);
    if (!check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output) buffer)"))
        return SDK_FAILURE;

    return true;
}


bool boxm_render_probe_manager::set_commandqueue()
{
  cl_int status = CL_SUCCESS;
  command_queue_ = clCreateCommandQueue(this->context(),this->devices()[0],CL_QUEUE_PROFILING_ENABLE,&status);
  if (!check_val(status,CL_SUCCESS,"Falied in command queue creation" + error_to_string(status)))
    return false;

  return true;
}


bool boxm_render_probe_manager::release_commandqueue()
{
  if (command_queue_)
  {
    cl_int status = clReleaseCommandQueue(command_queue_);
    if (!check_val(status,CL_SUCCESS,"clReleaseCommandQueue failed."))
      return false;
  }
  return true;
}


bool boxm_render_probe_manager::set_workspace()
{
  globalThreads[0]=this->bni_;
  globalThreads[1]=this->bnj_;

  localThreads[0]=this->bni_;
  localThreads[1]=this->bnj_;

  return true;
}


bool boxm_render_probe_manager::run()
{
  cl_int status = CL_SUCCESS;

  // set up a command queue

  cl_event ceEvent;
  status = clEnqueueNDRangeKernel(command_queue_,this->kernel_, 2,NULL,globalThreads,localThreads,0,NULL,&ceEvent);

  if (!check_val(status,CL_SUCCESS,"clEnqueueNDRangeKernel failed. "+error_to_string(status)))
    return SDK_FAILURE;

  status = clFinish(command_queue_);
  if (!check_val(status,CL_SUCCESS,"clFinish failed."+error_to_string(status)))
    return SDK_FAILURE;
  cl_ulong tstart,tend;
  status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_END,sizeof(cl_ulong),&tend,0);
  status = clGetEventProfilingInfo(ceEvent,CL_PROFILING_COMMAND_START,sizeof(cl_ulong),&tstart,0);
  gpu_time_= 1e-6f * float(tend - tstart); // convert nanoseconds to milliseconds
  vcl_cout<<"GPU time is "<<gpu_time_<<vcl_endl;

  return SDK_SUCCESS;
}

bool boxm_render_probe_manager::run_scene()
{
  bool good=true;
  vcl_string error_message="";
  good=good && set_scene_data()
            && set_all_blocks()
            && set_scene_data_buffers()
            && set_tree_buffers();
  this->set_kernel();
  this->set_args();
  this->set_commandqueue();
  this->set_workspace();

  this->run();

  good =good && release_tree_buffers()
             && clean_tree()
             && release_scene_data_buffers()
             && clean_scene_data();

  // release the command Queue
  this->release_kernel();
  this->release_commandqueue();
  if (program_) {
    vcl_cout<<"release: program"<<vcl_endl;
    clReleaseProgram(program_);
  }
  clReleaseContext(this->context_);
#if 0
  vcl_cout << "Timing Analysis\n"
           << "===============\n"
           <<"openCL Running time "<<gpu_time_<<" ms\n"
           << "Running block "<<total_gpu_time/1000<<"s\n"
           << "total block loading time = " << total_load_time << "s\n"
           << "total block processing time = " << total_raytrace_time << 's' << vcl_endl;
#endif
  return good;
}

bool boxm_render_probe_manager::set_rayoutput()
{
  raydepth_=1000;
  for (unsigned j=0;j<10;j++)
  {
    rayoutput_[j]=(float*)boxm_ocl_utils::alloc_aligned(raydepth_,sizeof(float),16);

    for (unsigned i=0;i<raydepth_;i++)
      rayoutput_[j][i]=-1.0;
  }

  output_=(float*)boxm_ocl_utils::alloc_aligned(1,sizeof(float),16);
  output_[0]=0.0f;
  return true;
}

bool boxm_render_probe_manager::set_rayoutput_buffers()
{
  cl_int status;
  for (unsigned i=0;i<10;i++)
  {
    rayoutput_buf_[i] = clCreateBuffer(this->context_,
                                       CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                       raydepth_*sizeof(cl_float),
                                       rayoutput_[i],&status);

    output_buf_=clCreateBuffer(this->context_,
                               CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                               sizeof(cl_float),output_,&status);
  }
  return check_val(status,CL_SUCCESS,"clCreateBuffer (rayoutput_) failed.")==1;
}

bool boxm_render_probe_manager::release_rayoutput_buffers()
{
    cl_int status;
    for (unsigned i=0;i<10;i++)
        status = clReleaseMemObject(rayoutput_buf_[i]);

    status = clReleaseMemObject(output_buf_);

    return check_val(status,CL_SUCCESS,"clReleaseMemObject failed (rayoutput_buf_).")==1;
}

bool boxm_render_probe_manager::clean_rayoutput()
{
    if (rayoutput_)
        for (unsigned i=0;i<10;i++)
            boxm_ocl_utils::free_aligned(rayoutput_[i]);
    return true;
}

bool boxm_render_probe_manager::read_output_array()
{
  cl_event events[2];
  int status =-1;
  for (unsigned i=0;i<10;i++)
  {
      // Enqueue readBuffers
      status= clEnqueueReadBuffer(command_queue_,rayoutput_buf_[i],CL_TRUE,
                                  0,this->raydepth_*sizeof(cl_float),
                                  rayoutput_[i], 0,NULL,&events[0]);

      if (!check_val(status,CL_SUCCESS,"clEnqueueBuffer (image_)failed."))
          return false;

      // Wait for the read buffer to finish execution
      status = clWaitForEvents(1, &events[0]);
      if (!check_val(status,CL_SUCCESS,"clWaitForEvents failed."))
          return false;

      status = clReleaseEvent(events[0]);
  }
  status= clEnqueueReadBuffer(command_queue_,output_buf_,CL_TRUE,
                              0,sizeof(cl_float),
                              output_, 0,NULL,NULL);

  if (!check_val(status,CL_SUCCESS,"clEnqueueBuffer (image_)failed."))
      return false;

  return check_val(status,CL_SUCCESS,"clReleaseEvent failed.")==1;
}


int boxm_render_probe_manager::build_kernel_program(cl_program & program)
{
  cl_int status = CL_SUCCESS;
  vcl_size_t sourceSize[] = { this->prog_.size() };
  if (!sourceSize[0]) return SDK_FAILURE;
  if (program) {
    status = clReleaseProgram(program);
    program = 0;
    if (!check_val(status,
                         CL_SUCCESS,
                         "clReleaseProgram failed."))
      return SDK_FAILURE;
  }
  const char * source = this->prog_.c_str();

  vcl_string options="";
  options+="-D INTENSITY";


  if (vcl_strstr(this->platform_name().c_str(),"ATI"))
    options+="-D ATI";
  if (vcl_strstr(this->platform_name().c_str(),"NVIDIA"))
    options+="-D NVIDIA";

  program = clCreateProgramWithSource(this->context_,
                                      1,
                                      &source,
                                      sourceSize,
                                      &status);
  if (!check_val(status,
                       CL_SUCCESS,
                       "clCreateProgramWithSource failed."))
    return SDK_FAILURE;

  // create a cl program executable for all the devices specified
  status = clBuildProgram(program,
                          1,
                          this->devices(),
                          "",
                          NULL,
                          NULL);
  if (!check_val(status,
                       CL_SUCCESS,
                       error_to_string(status)))
  {
    vcl_size_t len;
    char buffer[2048];
    clGetProgramBuildInfo(program, this->devices()[0],
                          CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
    vcl_printf("%s\n", buffer);
    return SDK_FAILURE;
  }
  else
    return SDK_SUCCESS;
}

bool boxm_render_probe_manager::set_scene_data()
{
    return set_scene_dims()
        && set_scene_origin()
        && set_block_dims()
        && set_block_ptrs()
        && set_root_level();
}


bool boxm_render_probe_manager::clean_scene_data()
{
    return clean_scene_dims()
        && clean_scene_origin()
        && clean_block_dims()
        && clean_block_ptrs()
        && clean_root_level();
}


bool boxm_render_probe_manager::set_scene_data_buffers()
{
    return set_scene_dims_buffers()
        && set_scene_origin_buffers()
        && set_block_dims_buffers()
        && set_block_ptrs_buffers()
        && set_root_level_buffers();
}


bool boxm_render_probe_manager::release_scene_data_buffers()
{
    return release_scene_dims_buffers()
        && release_scene_origin_buffers()
        && release_block_dims_buffers()
        && release_block_ptrs_buffers()
        && release_root_level_buffers();
}


bool boxm_render_probe_manager::set_root_level()
{
  if (scene_==NULL)
  {
    vcl_cout<<"Scene is Missing"<<vcl_endl;
    return false;
  }
  root_level_=scene_->max_level()-1;
  return true;
}


bool boxm_render_probe_manager::clean_root_level()
{
  root_level_=0;
  return true;
}


bool boxm_render_probe_manager::set_scene_origin()
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


bool boxm_render_probe_manager::set_scene_origin_buffers()
{
  cl_int status;
  scene_origin_buf_ = clCreateBuffer(this->context_,
                                     CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                     4*sizeof(cl_float),
                                     scene_origin_,&status);
  return check_val(status,CL_SUCCESS,"clCreateBuffer (scene_origin_) failed.")==1;
}


bool boxm_render_probe_manager::release_scene_origin_buffers()
{
  cl_int status;
  status = clReleaseMemObject(scene_origin_buf_);
  return check_val(status,CL_SUCCESS,"clReleaseMemObject failed (scene_origin_buf_).")==1;
}


bool boxm_render_probe_manager::clean_scene_origin()
{
  if (scene_origin_)
    boxm_ocl_utils::free_aligned(scene_origin_);
  return true;
}


bool boxm_render_probe_manager::set_scene_dims()
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


bool boxm_render_probe_manager::set_scene_dims_buffers()
{
  cl_int status;
  scene_dims_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                   sizeof(cl_int4),
                                   scene_dims_,&status);
  return check_val(status,CL_SUCCESS,"clCreateBuffer (scene_dims_) failed.")==1;
}


bool boxm_render_probe_manager::release_scene_dims_buffers()
{
  cl_int status;
  status = clReleaseMemObject(scene_dims_buf_);
  return check_val(status,CL_SUCCESS,"clReleaseMemObject failed (scene_dims_buf_).")==1;
}


bool boxm_render_probe_manager::clean_scene_dims()
{
  if (scene_dims_)
    boxm_ocl_utils::free_aligned(scene_dims_);
  return true;
}


bool boxm_render_probe_manager::set_block_dims()
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


bool boxm_render_probe_manager::set_block_dims_buffers()
{
  cl_int status;
  block_dims_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                   4*sizeof(cl_float),
                                   block_dims_,&status);
  return check_val(status,CL_SUCCESS,"clCreateBuffer (block_dims_) failed.")==1;
}


bool boxm_render_probe_manager::release_block_dims_buffers()
{
  cl_int status;
  status = clReleaseMemObject(block_dims_buf_);
  return check_val(status,CL_SUCCESS,"clReleaseMemObject failed (block_dims_buf_).")==1;
}


bool boxm_render_probe_manager::clean_block_dims()
{
  if (block_dims_)
    boxm_ocl_utils::free_aligned(block_dims_);
  return true;
}


bool boxm_render_probe_manager::set_block_ptrs()
{
  if (scene_==NULL)
  {
    vcl_cout<<"Scene is Missing"<<vcl_endl;
    return false;
  }
  scene_->block_num(scene_x_,scene_y_,scene_z_);
  int numblocks=scene_x_*scene_y_*scene_z_;
  vcl_cout<<"Block size "<<(float)numblocks*16/1024.0/1024.0<<"MB"<<vcl_endl;

  block_ptrs_=(cl_int*)boxm_ocl_utils::alloc_aligned(numblocks,sizeof(cl_int4),16);
  scene_->get_block_ptrs(block_ptrs_);
  return true;
}


bool boxm_render_probe_manager::set_block_ptrs_buffers()
{
  cl_int status;
  block_ptrs_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                   scene_x_*scene_y_*scene_z_*sizeof(cl_int4),
                                   block_ptrs_,&status);
  return check_val(status,CL_SUCCESS,"clCreateBuffer (block_ptrs_) failed.")==1;
}


bool boxm_render_probe_manager::release_block_ptrs_buffers()
{
  cl_int status;
  status = clReleaseMemObject(block_ptrs_buf_);
  return check_val(status,CL_SUCCESS,"clReleaseMemObject failed (block_ptrs_buf_).")==1;
}


bool boxm_render_probe_manager::clean_block_ptrs()
{
  if (block_ptrs_)
    boxm_ocl_utils::free_aligned(block_ptrs_);
  return true;
}


bool boxm_render_probe_manager::set_root_level_buffers()
{
  cl_int status;
  root_level_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                   sizeof(cl_uint),
                                   &root_level_,&status);
  return check_val(status,CL_SUCCESS,"clCreateBuffer (root level) failed.")==1;
}


bool boxm_render_probe_manager::release_root_level_buffers()
{
  cl_int status;
  status = clReleaseMemObject(root_level_buf_);
  if (!check_val(status,CL_SUCCESS,"clReleaseMemObject failed (root_level_buf_)."))
    return false;
  return true;
}


bool boxm_render_probe_manager::set_all_blocks()
{
  if (!scene_)
    return false;

  cells_ = NULL;
  cell_alpha_=NULL;
  cell_mixture_ = NULL;
  scene_->tree_buffer_shape(numbuffer_,lenbuffer_);
  cells_size_=numbuffer_*lenbuffer_;
  cell_data_size_=numbuffer_*lenbuffer_;

  /******* debug print **************/
  int cellBytes = sizeof(cl_int2);
  int dataBytes = sizeof(cl_uchar8)+sizeof(cl_float);
  vcl_cout<<"Optimized sizes:\n"
          <<"    cells: "<<(float)cells_size_*cellBytes/1024.0/1024.0<<"MB\n"
          <<"    data:  "<<(float)cells_size_*dataBytes/1024.0/1024.0<<"MB"<<vcl_endl;
  /**********************************/

  //allocate and initialize tree cells
  cells_=(cl_int *)boxm_ocl_utils::alloc_aligned(cells_size_,sizeof(cl_int2),16);
  scene_->get_tree_cells(cells_);

  //allocate and initialize alphas
  cell_alpha_=(cl_float *)boxm_ocl_utils::alloc_aligned(cell_data_size_,sizeof(cl_float),16);
  scene_->get_alphas(cell_alpha_);

  //allocate and initialize mix components
  cell_mixture_ = (cl_uchar *) boxm_ocl_utils::alloc_aligned(cell_data_size_,sizeof(cl_uchar8),16);
  scene_->get_mixture(cell_mixture_);

  if (cells_== NULL|| cell_mixture_ == NULL || cell_alpha_==NULL) {
    vcl_cout << "Failed to allocate host memory. (tree input)\n";
    return false;
  }
  return true;
}

bool boxm_render_probe_manager::clean_tree()
{
  if (cells_)
    boxm_ocl_utils::free_aligned(cells_);
  if (cell_mixture_)
    boxm_ocl_utils::free_aligned(cell_mixture_);
  if (cell_alpha_)
    boxm_ocl_utils::free_aligned(cell_alpha_);

  lenbuffer_=0;
  numbuffer_=0;
  return true;
}


bool boxm_render_probe_manager::set_tree_buffers()
{
  cl_int status;
  cells_buf_ = clCreateBuffer(this->context_,
                              CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                              cells_size_*sizeof(cl_int2),
                              cells_,&status);
  if (!check_val(status,CL_SUCCESS,"clCreateBuffer (tree) failed."))
    return false;

  cell_alpha_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                   cell_data_size_*sizeof(cl_float),
                                   cell_alpha_,&status);
  if (!check_val(status,CL_SUCCESS,"clCreateBuffer (cell data) failed."))
    return false;

  cell_mixture_buf_ = clCreateBuffer(this->context_,
                                     CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                                     cell_data_size_*sizeof(cl_uchar8),
                                     cell_mixture_,&status);
  if (!check_val(status,CL_SUCCESS,"clCreateBuffer (cell mixture) failed."))
    return false;

  numbuffer_buf_=clCreateBuffer(this->context_,
                                CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                sizeof(cl_uint),
                                &numbuffer_,&status);
  if (!check_val(status,CL_SUCCESS,"clCreateBuffer (numbuffer_) failed."))
    return false;

  lenbuffer_buf_=clCreateBuffer(this->context_,
                                CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                sizeof(cl_uint),
                                &lenbuffer_,&status);
  if (!check_val(status,CL_SUCCESS,"clCreateBuffer (lenbuffer_) failed."))
    return false;

  /******* debug print ***************/
  int cellBytes = cells_size_*sizeof(cl_int2);
  int alphaBytes = cell_data_size_*sizeof(cl_float);
  int mixBytes = cell_data_size_*sizeof(cl_uchar8);
  int blockBytes = scene_x_*scene_y_*scene_z_*sizeof(cl_int4);
  float MB = (cellBytes + alphaBytes + mixBytes + blockBytes)/1024.0f/1024.0f;
  vcl_cout<<"GPU Mem allocated:\n"
          <<"   cells: "<<cellBytes<<" bytes\n"
          <<"   alpha: "<<alphaBytes<<" bytes\n"
          <<"   mix  : "<<mixBytes<<" bytes\n"
          <<"   block: "<<blockBytes<<" bytes\n"
          <<"TOTAL: "<<MB<<"MB"<<vcl_endl;
  /************************************/

  return true;
}


bool boxm_render_probe_manager::release_tree_buffers()
{
  cl_int status;
  status = clReleaseMemObject(lenbuffer_buf_);
  if (!check_val(status,CL_SUCCESS,"clReleaseMemObject failed (lenbuffer_buf_)."))
    return false;
  status = clReleaseMemObject(numbuffer_buf_);
  if (!check_val(status,CL_SUCCESS,"clReleaseMemObject failed (numbuffer_buf_)."))
    return false;
  status = clReleaseMemObject(cells_buf_);
  if (!check_val(status,CL_SUCCESS,"clReleaseMemObject failed (cells_buf_)."))
    return false;
  status = clReleaseMemObject(cell_mixture_buf_);
  if (!check_val(status,CL_SUCCESS,"clReleaseMemObject failed (cell_mixture_buf_)."))
    return false;
  status = clReleaseMemObject(cell_alpha_buf_);
  return check_val(status,CL_SUCCESS,"clReleaseMemObject failed (cell_alpha_buf_).");
}


bool boxm_render_probe_manager::set_persp_camera(vpgl_perspective_camera<double> * pcam)
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


bool boxm_render_probe_manager::set_persp_camera()
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


bool boxm_render_probe_manager::clean_persp_camera()
{
  if (persp_cam_)
    boxm_ocl_utils::free_aligned(persp_cam_);
  return true;
}


bool boxm_render_probe_manager::set_persp_camera_buffers()
{
  cl_int status;
  persp_cam_buf_ = clCreateBuffer(this->context_,
                                  CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                  3*sizeof(cl_float16),
                                  persp_cam_,&status);
  return check_val(status,CL_SUCCESS,"clCreateBuffer (persp_cam_buf_) failed.")==1;
}


bool boxm_render_probe_manager::write_persp_camera_buffers()
{
  cl_int status;
  status=clEnqueueWriteBuffer(command_queue_,persp_cam_buf_,CL_FALSE, 0,3*sizeof(cl_float16), persp_cam_, 0, 0, 0);
  if (!check_val(status,CL_SUCCESS,"clEnqueueWriteBuffer (persp_cam_buf_) failed."))
    return false;
  clFinish(command_queue_);

  return true;
}


bool boxm_render_probe_manager::release_persp_camera_buffers()
{
  cl_int status;
  status = clReleaseMemObject(persp_cam_buf_);
  return check_val(status,CL_SUCCESS,"clReleaseMemObject failed (persp_cam_buf_).")==1;
}


//: Binary write multi_tracker scene to stream
void vsl_b_write(vsl_b_ostream& /*os*/, boxm_render_probe_manager const& /*multi_tracker*/)
{
}


//: Binary load boxm scene from stream.
void vsl_b_read(vsl_b_istream& /*is*/, boxm_render_probe_manager& /*multi_tracker*/)
{
}

//: Binary write boxm scene pointer to stream
void vsl_b_read(vsl_b_istream& /*is*/, boxm_render_probe_manager* /*ph*/)
{
}

//: Binary write boxm scene pointer to stream
void vsl_b_write(vsl_b_ostream& /*os*/, boxm_render_probe_manager* const& /*ph*/)
{
}
