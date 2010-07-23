#ifndef boxm_render_single_block_manager_txx_
#define boxm_render_single_block_manager_txx_
//:
// \file
#include "boxm_render_single_block_manager.h"
#include <vcl_where_root_dir.h>
#include <boxm/ocl/boxm_ocl_utils.h>
#include <vcl_cstdio.h>
#include <vul/vul_timer.h>
#include <boxm/boxm_block.h>
#include <boxm/boxm_scene.h>
#include <boxm/util/boxm_utils.h>
#include <boxm/basic/boxm_block_vis_graph_iterator.h>
#include <vil/vil_save.h>


//: Initializes CPU side input buffers
//put tree structure and data into arrays

bool boxm_render_single_block_manager::init_render(vcl_string treefile,
                                                   vcl_string treedatafile,
                                                   vgl_point_3d<double>  origin,
                                                   vgl_vector_3d<double>  block_dim,
                                                   vpgl_camera_double_sptr cam,
                                                   vil_image_view<float> &obs,
                                                   unsigned int root_level)
{
  cam_ = cam;
  output_img_=obs;
  treefile_=treefile;
  treedatafile_=treedatafile;
  origin_=origin;
  block_dim_=block_dim;
  root_level_=root_level;
#if 0 // unused variables
  vcl_string extensions_supported((char*)this->extensions_supported_);
  vcl_size_t found=extensions_supported.find("gl_sharing");
#endif

  // Code for Pass_0
  if (!this->load_kernel_source(vcl_string(VCL_SOURCE_ROOT_DIR)
                                +"/contrib/brl/bseg/boxm/ocl/octree_library_functions.cl") ||
      !this->append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    +"/contrib/brl/bseg/boxm/ocl/backproject.cl")||
      !this->append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    +"/contrib/brl/bseg/boxm/ocl/expected_functor.cl")||
      !this->append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    +"/contrib/brl/bseg/boxm/ocl/statistics_library_functions.cl")||
      !this->append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    +"/contrib/brl/bseg/boxm/ocl/ray_bundle_library_functions.cl")||
      !this->append_process_kernels(vcl_string(VCL_SOURCE_ROOT_DIR)
                                    +"/contrib/brl/bseg/boxm/ocl/ray_trace_all_blocks.cl")) {
    vcl_cerr << "Error: boxm_ray_trace_manager : failed to load kernel source (helper functions)\n";
    return false;
  }

  return !build_kernel_program(program_);
}


//: update the tree

bool boxm_render_single_block_manager::set_kernel()
{
  cl_int status = CL_SUCCESS;
  kernel_ = clCreateKernel(program_,"ray_trace_single_block",&status);
  if (!this->check_val(status,CL_SUCCESS,error_to_string(status))) {
    return false;
  }

  return true;
}


bool boxm_render_single_block_manager::release_kernel()
{
  if (kernel_)
  {
    cl_int status = clReleaseKernel(kernel_);
    if (!this->check_val(status,CL_SUCCESS,error_to_string(status)))
      return false;
  }
  return true;
}


bool boxm_render_single_block_manager::set_args()
{
  if (!kernel_)
    return false;
  cl_int status = CL_SUCCESS;
  int i=0;
  status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&scene_dims_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (scene_dims_buf_)"))
    return SDK_FAILURE;
  status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&scene_origin_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (scene_orign_buf_)"))
      return SDK_FAILURE;
  status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&block_dims_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (block_dims_buf_)"))
    return SDK_FAILURE;
  //status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&block_ptrs_buf_);
  //if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (block_ptrs_buf_)"))
  //  return SDK_FAILURE;
  // root level buffer
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
  //status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&cell_alpha_buf_);
  //if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_data_buf_)"))
  //    return SDK_FAILURE;

  // camera buffer
  status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&persp_cam_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (data)"))
    return SDK_FAILURE;
  // roi dimensions
  status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&img_dims_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (Img dimensions)"))
    return SDK_FAILURE;
  //// output image buffer
  status = clSetKernelArg(kernel_,i++,3*sizeof(cl_float16),0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local cam)"))
    return SDK_FAILURE;
  status = clSetKernelArg(kernel_,i++,sizeof(cl_uint4),0);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local image dimensions)"))
    return SDK_FAILURE;

  status = clSetKernelArg(kernel_,i++,sizeof(cl_mem),(void *)&image_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (input_image)"))
    return SDK_FAILURE;
  else
    return SDK_SUCCESS;
}


bool boxm_render_single_block_manager::set_commandqueue()
{
  cl_int status = CL_SUCCESS;
  command_queue_ = clCreateCommandQueue(this->context(),this->devices()[0],CL_QUEUE_PROFILING_ENABLE,&status);
  if (!this->check_val(status,CL_SUCCESS,"Falied in command queue creation" + error_to_string(status)))
    return false;

  return true;
}


bool boxm_render_single_block_manager::release_commandqueue()
{
  if (command_queue_)
  {
    cl_int status = clReleaseCommandQueue(command_queue_);
    if (!this->check_val(status,CL_SUCCESS,"clReleaseCommandQueue failed."))
      return false;
  }
  return true;
}


bool boxm_render_single_block_manager::set_workspace()
{
  cl_int status = CL_SUCCESS;

  // check the local memeory
  cl_ulong used_local_memory;
  status = clGetKernelWorkGroupInfo(this->kernel(),this->devices()[0],
                                    CL_KERNEL_LOCAL_MEM_SIZE,
                                    sizeof(cl_ulong),&used_local_memory,NULL);
  if (!this->check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_LOCAL_MEM_SIZE failed."))
    return 0;

  // determine the work group size
  cl_ulong kernel_work_group_size;
  status = clGetKernelWorkGroupInfo(this->kernel(),this->devices()[0],CL_KERNEL_WORK_GROUP_SIZE,
                                    sizeof(cl_ulong),&kernel_work_group_size,NULL);
  if (!this->check_val(status,CL_SUCCESS,"clGetKernelWorkGroupInfo CL_KERNEL_WORK_GROUP_SIZE, failed."))
    return 0;

  globalThreads[0]=this->wni_;
  globalThreads[1]=this->wnj_;

  localThreads[0]=this->bni_;
  localThreads[1]=this->bnj_;

  if (used_local_memory > this->total_local_memory())
  {
    vcl_cout << "Unsupported: Insufficient local memory on device.\n";
    return 0;
  }
  else
    return 1;
}


bool boxm_render_single_block_manager::run()
{
  cl_int status = CL_SUCCESS;

  // set up a command queue

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
  float  gpu_time_= (double)1.0e-6 * (tend - tstart); // convert nanoseconds to milliseconds
  vcl_cout<<"GPU time is "<<gpu_time_<<vcl_endl;
  return SDK_SUCCESS;
}


bool boxm_render_single_block_manager::run_scene()
{
  bool good=true;
  vcl_string error_message="";
  vul_timer timer;
  good=good && set_scene_data();
  good=good && set_tree();

  good=good && set_scene_data_buffers();
  good=good && set_tree_buffers();
  // run the raytracing
  good=good && set_input_view();
  good=good && set_input_view_buffers();
  this->set_kernel();
  this->set_args();
  this->set_commandqueue();
  this->set_workspace();

  this->run();

  good =good && release_tree_buffers();
  good =good && clean_tree();
  good=good && read_output_image();
  //this->print_image();
  good=good && release_input_view_buffers();
  //good=good && clean_input_view();
  good=good && release_scene_data_buffers();
  good=good && clean_scene_data();

  // release the command Queue

  this->release_kernel();
  vcl_cout << "Timing Analysis\n"
           << "===============" << vcl_endl
#ifdef DEBUG
           <<"openCL Running time "<<gpu_time_<<" ms\n"
           << "Running block "<<total_gpu_time/1000<<"s\n"
           << "total block loading time = " << total_load_time << "s\n"
           << "total block processing time = " << total_raytrace_time << 's' << vcl_endl
#endif
  ;
  return true;
}


bool boxm_render_single_block_manager:: read_output_image()
{
  cl_event events[2];

  // Enqueue readBuffers
  int status = clEnqueueReadBuffer(command_queue_,image_buf_,CL_TRUE,
                                   0,this->wni_*this->wnj_*sizeof(cl_uint),
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


bool boxm_render_single_block_manager:: read_trees()
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
  //status = clEnqueueReadBuffer(command_queue_,cell_alpha_buf_,CL_TRUE,
  //                             0,cell_data_size_*sizeof(cl_float),
  //                             cell_alpha_,
  //                             0,NULL,&events[0]);

  //if (!this->check_val(status,CL_SUCCESS,"clEnqueueBuffer (cell data )failed."))
  //  return false;
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


void boxm_render_single_block_manager::print_tree()
{
  vcl_cout << "Tree Input\n";
  if (cells_)
    for (unsigned i = 0; i<cells_size_*4; i+=4) {
      int data_ptr = 16*cells_[i+2];
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

      vcl_cout << ")\n";
    }
}


void boxm_render_single_block_manager::print_image()
{
  vcl_cout << "IMage Output\n";
  if (image_)
  {
    vcl_cout<<"Plane 0"<<vcl_endl;
    for (unsigned j=0;j<img_dims_[3];j++)
    {
      for (unsigned i=0;i<img_dims_[2];i++)
        vcl_cout<<image_[(j*img_dims_[2]+i)*4]<<' ';
      vcl_cout<<vcl_endl;
    }
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
  }
}


bool boxm_render_single_block_manager::clean_update()
{
  return true;
}

/*******************************************
 * build_kernel_program - builds kernel program
 * from source (a vcl string)
 *******************************************/

int boxm_render_single_block_manager::build_kernel_program(cl_program & program)
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


bool boxm_render_single_block_manager::set_scene_data()
{
    return set_scene_dims()
        && set_scene_origin()
        && set_block_dims()
        && set_root_level();
}


bool boxm_render_single_block_manager::clean_scene_data()
{
    return clean_scene_dims()
        && clean_scene_origin()
        && clean_block_dims()
        && clean_root_level();
}


bool boxm_render_single_block_manager::set_scene_data_buffers()
{
    return set_scene_dims_buffers()
        && set_scene_origin_buffers()
        && set_block_dims_buffers()
        && set_root_level_buffers();
}


bool boxm_render_single_block_manager::release_scene_data_buffers()
{
    return release_scene_dims_buffers()
        && release_scene_origin_buffers()
        && release_block_dims_buffers()
        && release_root_level_buffers();
}


bool boxm_render_single_block_manager::set_root_level()
{
  return true;
}


bool boxm_render_single_block_manager::clean_root_level()
{
  root_level_=0;
  return true;
}


bool boxm_render_single_block_manager::set_scene_origin()
{
#if 0
  if (scene_==NULL)
  {
    vcl_cout<<"Scene is Missing"<<vcl_endl;
    return false;
  }
  vgl_point_3d<double> orig=scene_->origin();
#endif
  scene_origin_=(cl_float *)boxm_ocl_utils::alloc_aligned(1,sizeof(cl_float4),16);

  scene_origin_[0]=(float)origin_.x();
  scene_origin_[1]=(float)origin_.y();
  scene_origin_[2]=(float)origin_.z();
  scene_origin_[3]=0.0f;

  return true;
}


bool boxm_render_single_block_manager::set_scene_origin_buffers()
{
  cl_int status;
  scene_origin_buf_ = clCreateBuffer(this->context_,
                                     CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                     4*sizeof(cl_float),
                                     scene_origin_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (scene_origin_) failed.")==1;
}


bool boxm_render_single_block_manager::release_scene_origin_buffers()
{
  cl_int status;
  status = clReleaseMemObject(scene_origin_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (scene_origin_buf_).")==1;
}


bool boxm_render_single_block_manager::clean_scene_origin()
{
  if (scene_origin_)
    boxm_ocl_utils::free_aligned(scene_origin_);
  return true;
}


bool boxm_render_single_block_manager::set_scene_dims()
{
  scene_dims_=(cl_int *)boxm_ocl_utils::alloc_aligned(1,sizeof(cl_int4),16);

  scene_dims_[0]=1;
  scene_dims_[1]=1;
  scene_dims_[2]=1;
  scene_dims_[3]=0;

  return true;
}


bool boxm_render_single_block_manager::set_scene_dims_buffers()
{
  cl_int status;
  scene_dims_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                   sizeof(cl_int4),
                                   scene_dims_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (scene_dims_) failed.")==1;
}


bool boxm_render_single_block_manager::release_scene_dims_buffers()
{
  cl_int status;
  status = clReleaseMemObject(scene_dims_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (scene_dims_buf_).")==1;
}


bool boxm_render_single_block_manager::clean_scene_dims()
{
  if (scene_dims_)
    boxm_ocl_utils::free_aligned(scene_dims_);
  return true;
}


bool boxm_render_single_block_manager::set_block_dims()
{
  block_dims_=(cl_float *)boxm_ocl_utils::alloc_aligned(1,sizeof(cl_float4),16);

  block_dims_[0]=(float)block_dim_.x();
  block_dims_[1]=(float)block_dim_.y();
  block_dims_[2]=(float)block_dim_.z();
  block_dims_[3]=0;

  return true;
}


bool boxm_render_single_block_manager::set_block_dims_buffers()
{
  cl_int status;
  block_dims_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                   4*sizeof(cl_float),
                                   block_dims_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (block_dims_) failed.")==1;
}


bool boxm_render_single_block_manager::release_block_dims_buffers()
{
  cl_int status;
  status = clReleaseMemObject(block_dims_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (block_dims_buf_).")==1;
}


bool boxm_render_single_block_manager::clean_block_dims()
{
  if (block_dims_)
    boxm_ocl_utils::free_aligned(block_dims_);
  return true;
}


bool boxm_render_single_block_manager::set_root_level_buffers()
{
  cl_int status;
  root_level_buf_ = clCreateBuffer(this->context_,
                                   CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                   sizeof(cl_uint),
                                   &root_level_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (root level) failed.")==1;
}


bool boxm_render_single_block_manager::release_root_level_buffers()
{
  cl_int status;
  status = clReleaseMemObject(root_level_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (root_level_buf_)."))
    return false;
  return true;
}


bool boxm_render_single_block_manager::set_input_view()
{
  return set_persp_camera()
      && set_input_image();
}


bool boxm_render_single_block_manager::clean_input_view()
{
  return clean_persp_camera()
      && clean_input_image();
}


bool boxm_render_single_block_manager::set_input_view_buffers()
{
  return set_persp_camera_buffers()
      && set_input_image_buffers()
      && set_image_dims_buffers();
}


bool boxm_render_single_block_manager::release_input_view_buffers()
{
  return release_persp_camera_buffers()
      && release_input_image_buffers();
}


bool boxm_render_single_block_manager::set_tree()
{
    cells_ = boxm_ocl_utils::readtree(treefile_.c_str(),cells_size_);
    cell_data_ = boxm_ocl_utils::readtreedata(treedatafile_.c_str(),cell_data_size_);

    if (cells_== NULL||cell_data_ == NULL)
    {
        vcl_cout << "Failed to allocate host memory. (tree input)\n";
        return false;
    }

    tree_bbox_=(cl_float *)boxm_ocl_utils::alloc_aligned(1,sizeof(cl_float4),16);

    tree_bbox_[0] = (cl_float)origin_.x();
    tree_bbox_[1] = (cl_float)origin_.y();
    tree_bbox_[2] = (cl_float)origin_.z();
    //: Assumption: isotropic dimensions.
    tree_bbox_[3] = (cl_float)block_dim_.x();

    return true;

  return true;
}


bool boxm_render_single_block_manager::clean_tree()
{
  if (cells_)
    boxm_ocl_utils::free_aligned(cells_);
  if (cell_data_)
    boxm_ocl_utils::free_aligned(cell_data_);
  if (cell_alpha_)
    boxm_ocl_utils::free_aligned(cell_alpha_);

  return true;
}


bool boxm_render_single_block_manager::set_tree_buffers()
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
  //cell_alpha_buf_ = clCreateBuffer(this->context_,
  //                                 CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
  //                                 cell_data_size_*sizeof(cl_float),
  //                                 cell_alpha_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (cell data) failed.");
}


bool boxm_render_single_block_manager::release_tree_buffers()
{
  cl_int status;
  status = clReleaseMemObject(cells_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (cells_buf_)."))
    return false;
  status = clReleaseMemObject(cell_data_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (cell_data_buf_)."))
    return false;
  //  status = clReleaseMemObject(cell_alpha_buf_);
  //return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (cell_alpha_buf_).");
return true;
}


bool boxm_render_single_block_manager::set_persp_camera(vpgl_perspective_camera<double> * pcam)
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


bool boxm_render_single_block_manager::set_persp_camera()
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


bool boxm_render_single_block_manager::clean_persp_camera()
{
  if (persp_cam_)
    boxm_ocl_utils::free_aligned(persp_cam_);
  return true;
}


bool boxm_render_single_block_manager::set_persp_camera_buffers()
{
  cl_int status;
  persp_cam_buf_ = clCreateBuffer(this->context_,
                                  CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                  3*sizeof(cl_float16),
                                  persp_cam_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (persp_cam_buf_) failed.")==1;
}


bool boxm_render_single_block_manager::write_persp_camera_buffers()
{
  cl_int status;
  status=clEnqueueWriteBuffer(command_queue_,persp_cam_buf_,CL_FALSE, 0,3*sizeof(cl_float16), persp_cam_, 0, 0, 0);
  if (!this->check_val(status,CL_SUCCESS,"clEnqueueWriteBuffer (persp_cam_buf_) failed."))
    return false;
  clFinish(command_queue_);

  return true;
}


bool boxm_render_single_block_manager::release_persp_camera_buffers()
{
  cl_int status;
  status = clReleaseMemObject(persp_cam_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (persp_cam_buf_).")==1;
}


bool boxm_render_single_block_manager::set_input_image()
{
  wni_=(cl_uint)RoundUp(output_img_.ni(),bni_);
  wnj_=(cl_uint)RoundUp(output_img_.nj(),bnj_);

  image_=(cl_uint *)boxm_ocl_utils::alloc_aligned(wni_*wnj_,sizeof(cl_uint),16);
  img_dims_=(cl_uint *)boxm_ocl_utils::alloc_aligned(1,sizeof(cl_uint4),16);

  // pad the image
  for (unsigned i=0;i<output_img_.ni();i++)
  {
    for (unsigned j=0;j<output_img_.nj();j++)
    {
      image_[(j*wni_+i)]=0;
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
  return true;
}

bool boxm_render_single_block_manager::save_output_image(vcl_string filename)
{
  // pad the image
  for (unsigned i=0;i<output_img_.ni();i++)
  {
    for (unsigned j=0;j<output_img_.nj();j++)
    {
      output_img_(i,j)=(image_[(j*wni_+i)]>>24);
    }
  }

  vil_save(output_img_,filename.c_str());

  return true;
}

bool boxm_render_single_block_manager::clean_input_image()
{
  if (image_)
    boxm_ocl_utils::free_aligned(image_);
  if (img_dims_)
    boxm_ocl_utils::free_aligned(img_dims_);
  return true;
}


bool boxm_render_single_block_manager::set_input_image_buffers()
{
  cl_int status;
  image_buf_ = clCreateBuffer(this->context_,
                              CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
                              wni_*wnj_*sizeof(cl_uchar4),
                              image_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (image_buf_) failed.")==1;
}


bool boxm_render_single_block_manager::set_image_dims_buffers()
{
  cl_int status;

  img_dims_buf_ = clCreateBuffer(this->context_,
                                 CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
                                 sizeof(cl_uint4),
                                 img_dims_,&status);
  return this->check_val(status,CL_SUCCESS,"clCreateBuffer (imd_dims_buf_) failed.")==1;
}


bool boxm_render_single_block_manager::release_input_image_buffers()
{
  cl_int status;
  status = clReleaseMemObject(image_buf_);
  if (!this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (image_buf_)."))
    return false;

  status = clReleaseMemObject(img_dims_buf_);
  return this->check_val(status,CL_SUCCESS,"clReleaseMemObject failed (img_dims_buf_).")==1;
}


#endif    //boxm_render_single_block_manager_txx_

