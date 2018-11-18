// This is brl/bseg/boxm2/ocl/algo/boxm2_ocl_depth_renderer.cxx
//:
// \file
// \brief  Render and expected image of a boxm2 model
//
// \author Daniel Crispell, adapted from process version
// \date 4 Nov 2014
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include "boxm2_ocl_depth_renderer.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <vil/vil_image_view.h>
//brdb stuff

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include "boxm2_ocl_camera_converter.h"
#include "boxm2_ocl_render_expected_image_function.h"
#include <vul/vul_timer.h>

boxm2_ocl_depth_renderer
::boxm2_ocl_depth_renderer(const boxm2_scene_sptr& scene,
                           const boxm2_opencl_cache_sptr& ocl_cache,
                           const std::string&  /*ident*/) :
  scene_(scene),
  opencl_cache_(ocl_cache),
  buffers_allocated_(false),
  render_success_(false)
{
  device_ = ocl_cache->get_device();
  compile_kernels(device_);
}

bool
boxm2_ocl_depth_renderer
::allocate_render_buffers(int cl_ni, int cl_nj)
{
  if ( buffers_allocated_ && (prev_ni_ == cl_ni) && (prev_nj_ == cl_nj) ) {
    // can reuse old buffers
    return true;
  }
  // else we need to allocate new buffers
  if (buffers_allocated_) {
    cleanup_render_buffers();
  }
  depth_buff_ = new float[cl_ni*cl_nj];
  vis_buff_ = new float[cl_ni*cl_nj];
  prob_buff_ = new float[cl_ni*cl_nj];
  var_buff_ = new float[cl_ni*cl_nj];
  t_infinity_buff_ = new float[cl_ni*cl_nj];

  ray_origins_buff_ = new cl_float[4*cl_ni*cl_nj];
  ray_directions_buff_ = new cl_float[4*cl_ni*cl_nj];

  depth_image_ = opencl_cache_->alloc_mem(cl_ni*cl_nj*sizeof(cl_float), depth_buff_,"depth image buffer");
  depth_image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR);

  vis_image_ = opencl_cache_->alloc_mem(cl_ni*cl_nj*sizeof(cl_float), vis_buff_,"vis image buffer");
  vis_image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR);

  prob_image_ = opencl_cache_->alloc_mem(cl_ni*cl_nj*sizeof(cl_float), prob_buff_,"vis image buffer");
  prob_image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR);

  var_image_ = opencl_cache_->alloc_mem(cl_ni*cl_nj*sizeof(cl_float), var_buff_, "var image buffer");
  var_image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR);

  t_infinity_image_ = opencl_cache_->alloc_mem(cl_ni*cl_nj*sizeof(cl_float), t_infinity_buff_, "t_inf image buffer");
  t_infinity_image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR);

  ray_origins_image_ = opencl_cache_->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins_buff_, "ray_origins buffer");
  ray_origins_image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR);

  ray_directions_image_= opencl_cache_->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions_buff_, "ray_directions buffer");
  ray_directions_image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR);

  img_dim_ = opencl_cache_->alloc_mem(sizeof(cl_int)*4, img_dim_buff_, "image dims");
  img_dim_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR);

  tnearfar_ = opencl_cache_->alloc_mem(2*sizeof(cl_float), tnearfar_buff_, "tnearfar  buffer");
  tnearfar_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR);

  cl_output_ = opencl_cache_->alloc_mem(100*sizeof(cl_float), output_buff_, "output buffer");
  cl_output_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR);

  boxm2_ocl_util::set_bit_lookup( lookup_buff_ );
  lookup_ = opencl_cache_->alloc_mem(256*sizeof(cl_uchar), lookup_buff_, "lookup buffer");
  lookup_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  cl_subblk_dim_ = opencl_cache_->alloc_mem(sizeof(cl_float), &subblk_dim_, "subblock dim");
  cl_subblk_dim_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR);

  buffers_allocated_ = true;
  prev_ni_ = cl_ni;
  prev_nj_ = cl_nj;
  return true;
}

bool
boxm2_ocl_depth_renderer
::cleanup_render_buffers()
{
  if(!buffers_allocated_) {
    return false;
  }

  delete[] depth_buff_;
  delete[] vis_buff_;
  delete[] prob_buff_;
  delete[] var_buff_;
  delete[] t_infinity_buff_;
  delete[] ray_origins_buff_;
  delete[] ray_directions_buff_;

  opencl_cache_->unref_mem(depth_image_.ptr());
  depth_image_ = bocl_mem_sptr(nullptr);
  opencl_cache_->unref_mem(vis_image_.ptr());
  vis_image_ = bocl_mem_sptr(nullptr);
  opencl_cache_->unref_mem(prob_image_.ptr());
  prob_image_ = bocl_mem_sptr(nullptr);
  opencl_cache_->unref_mem(var_image_.ptr());
  var_image_ = bocl_mem_sptr(nullptr);
  opencl_cache_->unref_mem(t_infinity_image_.ptr());
  t_infinity_image_ = bocl_mem_sptr(nullptr);
  opencl_cache_->unref_mem(ray_origins_image_.ptr());
  ray_origins_image_ = bocl_mem_sptr(nullptr);
  opencl_cache_->unref_mem(ray_directions_image_.ptr());
  ray_directions_image_ = bocl_mem_sptr(nullptr);

  opencl_cache_->unref_mem(tnearfar_.ptr());
  tnearfar_ = bocl_mem_sptr(nullptr);
  opencl_cache_->unref_mem(img_dim_.ptr());
  img_dim_ = bocl_mem_sptr(nullptr);
  opencl_cache_->unref_mem(cl_output_.ptr());
  cl_output_ = bocl_mem_sptr(nullptr);
  opencl_cache_->unref_mem(cl_subblk_dim_.ptr());
  cl_subblk_dim_ = bocl_mem_sptr(nullptr);
  opencl_cache_->unref_mem(lookup_.ptr());
  lookup_ = bocl_mem_sptr(nullptr);

  buffers_allocated_ = false;
  return true;
}

boxm2_ocl_depth_renderer
::~boxm2_ocl_depth_renderer()
{
  cleanup_render_buffers();
}

bool
boxm2_ocl_depth_renderer
::get_last_rendered(vil_image_view<float> &img)
{
  if (render_success_) {
    img.deep_copy(depth_img_);
    return true;
  }
  return false;
}

bool
boxm2_ocl_depth_renderer
::get_last_vis(vil_image_view<float> &vis_img)
{
  if (render_success_) {
    vis_img.deep_copy( vis_img_ );
    return true;
  }
  return false;
}

bool
boxm2_ocl_depth_renderer
::render(vpgl_camera_double_sptr camera, unsigned ni, unsigned nj, float  /*nearfactor*/, float  /*farfactor*/)
{
  render_success_ = false;

  vul_timer rtime;

  std::size_t local_threads[2]={8,8};

  //: create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device_->context(),*(device_->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);

  bool good_queue = check_val(status, CL_SUCCESS, "ERROR: boxm2_ocl_depth_renderer: Failed to create command queue: " + error_to_string(status));
  if(!good_queue) {
    return false;
  }

  unsigned cl_ni=RoundUp(ni,local_threads[0]);
  unsigned cl_nj=RoundUp(nj,local_threads[1]);
  std::size_t global_threads[] = {cl_ni,cl_nj};

  allocate_render_buffers(cl_ni, cl_nj);

  // intialize the render image planes
  std::fill(depth_buff_, depth_buff_ + cl_ni*cl_nj, 0.0f);
  std::fill(vis_buff_, vis_buff_ + cl_ni*cl_nj, 1.0f);
  std::fill(var_buff_, var_buff_ + cl_ni*cl_nj, 0.0f);
  std::fill(prob_buff_, prob_buff_ + cl_ni*cl_nj, 0.0f);
  std::fill(t_infinity_buff_, t_infinity_buff_ + cl_ni*cl_nj, 0.0f);

  img_dim_buff_[0] = 0;
  img_dim_buff_[1] = 0;
  img_dim_buff_[2] = ni;
  img_dim_buff_[3] = nj;

  depth_image_->write_to_buffer(queue);
  vis_image_->write_to_buffer(queue);
  var_image_->write_to_buffer(queue);
  prob_image_->write_to_buffer(queue);
  t_infinity_image_->write_to_buffer(queue);
  img_dim_->write_to_buffer(queue);

  // assumes that the camera may be changing between calls
  boxm2_ocl_camera_converter::compute_ray_image( device_, queue, camera, cl_ni, cl_nj, ray_origins_image_, ray_directions_image_, 0, 0, false);

  int statusw = clFinish(queue);
  bool good_write = check_val(statusw, CL_SUCCESS, "ERROR: boxm2_ocl_depth_renderer: Initial write to GPU failed: " + error_to_string(statusw));
  if(!good_write) {
    return false;
  }

  std::vector<boxm2_block_id> vis_order;
  if(camera->type_name() == "vpgl_perspective_camera") {
      vis_order = scene_->get_vis_blocks_opt(dynamic_cast<vpgl_perspective_camera<double>*>(camera.ptr()),ni,nj);
  }
  else {
      vis_order = scene_->get_vis_blocks(camera);
  }

  subblk_dim_ = 0.0f; // in case there are no visible blocks;

  for (auto & id : vis_order) {

    boxm2_block_metadata mdata = scene_->get_block_metadata(id);

    bocl_mem* blk       = opencl_cache_->get_block(scene_,id);
    bocl_mem* alpha     = opencl_cache_->get_data<BOXM2_ALPHA>(scene_,id);
    bocl_mem* blk_info  = opencl_cache_->loaded_block_info();
    subblk_dim_         = mdata.sub_block_dim_.x(); // assume this is not changing per block

    depth_kern_.set_arg( blk_info );
    depth_kern_.set_arg( blk );
    depth_kern_.set_arg( alpha );
    depth_kern_.set_arg( ray_origins_image_.ptr() );
    depth_kern_.set_arg( ray_directions_image_.ptr() );
    depth_kern_.set_arg( depth_image_.ptr() );
    depth_kern_.set_arg( var_image_.ptr() );
    depth_kern_.set_arg( img_dim_.ptr() );
    depth_kern_.set_arg( cl_output_.ptr() );
    depth_kern_.set_arg( lookup_.ptr() );
    depth_kern_.set_arg( vis_image_.ptr() );
    depth_kern_.set_arg( prob_image_.ptr() );
    depth_kern_.set_arg( t_infinity_image_.ptr() );

    //local tree , cumsum buffer, imindex buffer
    depth_kern_.set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );
    depth_kern_.set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) );
    depth_kern_.set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_int) );

    //execute kernel
    depth_kern_.execute(queue, 2, local_threads, global_threads);
    statusw = clFinish(queue);
    bool good_run = check_val(statusw, CL_SUCCESS, "ERROR: boxm2_ocl_depth_renderer: Execution of depth kernel failed: " + error_to_string(statusw));
    if (!good_run) {
      return false;
    }
    depth_kern_.clear_args();
  }

  cl_subblk_dim_->write_to_buffer(queue, true);

  depth_norm_kern_.set_arg( depth_image_.ptr() );
  depth_norm_kern_.set_arg( var_image_.ptr() );
  depth_norm_kern_.set_arg( vis_image_.ptr() );
  depth_norm_kern_.set_arg( img_dim_.ptr());
  depth_norm_kern_.set_arg( t_infinity_image_.ptr());
  depth_norm_kern_.set_arg( cl_subblk_dim_.ptr() );
  depth_norm_kern_.execute( queue, 2, local_threads, global_threads);
  statusw = clFinish(queue);

  bool good_norm = check_val(statusw, CL_SUCCESS, "ERROR: boxm2_ocl_depth_renderer: Execution of depth norm kernel failed: " + error_to_string(statusw));
  if (!good_norm) {
    return false;
  }

  depth_image_->read_to_buffer(queue);
  var_image_->read_to_buffer(queue);
  vis_image_->read_to_buffer(queue);
  statusw = clFinish(queue);
  bool good_read = check_val(statusw, CL_SUCCESS, "ERROR: boxm2_ocl_depth_renderer: Read of depth buffers failed: " + error_to_string(statusw));
  if (!good_read) {
    return false;
  }
  depth_norm_kern_.clear_args();

  depth_img_.set_size(ni, nj);
  vis_img_.set_size(ni, nj);

  for (unsigned r=0;r<nj;r++) {
    for (unsigned c=0;c<ni;c++) {
      const unsigned offset = r*cl_ni + c;
      depth_img_(c,r)=depth_buff_[offset];
      vis_img_(c,r)=vis_buff_[offset];
    }
  }

  std::cout<<"Total Render time: "<<rtime.all()<<" ms"<<std::endl;
  clReleaseCommandQueue(queue);

  render_success_ = true;
  return true;
}

bool
boxm2_ocl_depth_renderer
::compile_kernels(const bocl_device_sptr&  /*device*/)
{
  {
    std::vector<std::string> src_paths;
    std::string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "pixel_conversion.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "expected_functor.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/render_bit_scene.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //set kernel options
    std::string options = " -D RENDER_DEPTH ";
    options +=  "-D DETERMINISTIC";
    options += " -D STEP_CELL=step_cell_render_depth2(tblock,linfo->block_len,aux_args.alpha,data_ptr,d*linfo->block_len,aux_args.vis,aux_args.expdepth,aux_args.expdepthsqr,aux_args.probsum,aux_args.t)";

    //have kernel construct itself using the context and device
    bool good = depth_kern_.create_kernel( &device_->context(),
                                           device_->device_id(),
                                           src_paths,
                                           "render_depth",   //kernel name
                                           options,              //options
                                           "boxm2 opencl render depth image"); //kernel identifier (for error checking)
    if (!good) {
      std::cerr << "ERROR: boxm2_ocl_depth_renderer: error compiling depth kernel" << std::endl;
      return false;
    }
  }
  {
    std::vector<std::string> norm_src_paths;
    std::string source_dir = boxm2_ocl_util::ocl_src_root();
    norm_src_paths.push_back(source_dir + "scene_info.cl");

    norm_src_paths.push_back(source_dir + "pixel_conversion.cl");
    norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");

    std::string options = " -D RENDER_DEPTH ";

    bool good = depth_norm_kern_.create_kernel( &device_->context(),
                                                device_->device_id(),
                                                norm_src_paths,
                                                "normalize_render_depth_kernel",   //kernel name
                                                options,              //options
                                                "normalize render depth kernel"); //kernel identifier (for error checking)

    if (!good) {
      std::cerr << "ERROR: boxm2_ocl_depth_renderer: error compiling depth normalization kernel" << std::endl;
      return false;
    }
  }
  return true;
}
