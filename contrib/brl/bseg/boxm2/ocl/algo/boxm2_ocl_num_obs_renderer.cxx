// This is brl/bseg/boxm2/ocl/algo/boxm2_ocl_num_obs_renderer.cxx
//:
// \file
// \brief  Render and expected image of a boxm2 model
//
// \author Daniel Crispell, adapted from process version
// \date 4 Nov 2014
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include "boxm2_ocl_num_obs_renderer.h"

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
#include <bocl/bocl_cl.h>
#include "boxm2_ocl_render_expected_image_function.h"
#include <vul/vul_timer.h>

boxm2_ocl_num_obs_renderer
::boxm2_ocl_num_obs_renderer(const boxm2_scene_sptr& scene,
                             const boxm2_opencl_cache_sptr& ocl_cache,
                             const std::string& ident) :
  scene_(scene),
  opencl_cache_(ocl_cache),
  buffers_allocated_(false),
  render_success_(false)
{
  device_ = ocl_cache->get_device();
  bool foundDataType = false;
  std::vector<std::string> apps = scene->appearances();

  nobs_type_size_ = 0;
  boxm2_data_type valid_nobs_types[] = {
    BOXM2_NUM_OBS_VIEW, BOXM2_NUM_OBS_SINGLE
  };

  int num_valid_nobs_types = sizeof(valid_nobs_types) / sizeof(valid_nobs_types[0]);

  for (unsigned int i=0; i<apps.size(); ++i) {
    std::cout << "app["<<i<<"] = " << apps[i] << std::endl;
    for (unsigned v = 0; v < num_valid_nobs_types; ++v) {
      boxm2_data_type valid_nobs_type = valid_nobs_types[v];
      std::string valid_nobs_prefix = boxm2_data_info::prefix(valid_nobs_type, ident);
      std::cout << "     valid_nobs_prefix = " << boxm2_data_info::prefix(valid_nobs_type, ident) << std::endl;
      if ( apps[i] == valid_nobs_prefix )
      {
        nobs_type_ = valid_nobs_prefix;
        foundDataType = true;
        nobs_type_size_ = boxm2_data_info::datasize(valid_nobs_type);
        std::cout<<"===========Compiling kernels==========="<<std::endl;
        compile_kernels(device_, kernels_, valid_nobs_type);
        break;
      }
    }
  }

  if (!foundDataType) {
    std::cout<<"BOXM2_OCL_NUM_OBS_RENDERER ERROR: scene doesn't have valid num_obs type"<<std::endl;
    std::cout<<"                                  looking for ident = " << ident << std::endl;
    throw std::runtime_error("scene does not have valid num_obs type");
  }
}

bool
boxm2_ocl_num_obs_renderer
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
  img_buff_ = new float[cl_ni*cl_nj];
  vis_buff_ = new float[cl_ni*cl_nj];
  max_omega_buff_ = new float[cl_ni*cl_nj];

  exp_nobs_image_ = opencl_cache_->alloc_mem(cl_ni*cl_nj*sizeof(cl_float), img_buff_,"exp nobs image buffer");
  exp_nobs_image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR);

  vis_image_ = opencl_cache_->alloc_mem(cl_ni*cl_nj*sizeof(cl_float), vis_buff_,"vis image buffer");
  vis_image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR);

  max_omega_image_ = opencl_cache_->alloc_mem(cl_ni*cl_nj*sizeof(cl_float), max_omega_buff_,"vis image buffer");
  max_omega_image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR);

  img_dim_ = opencl_cache_->alloc_mem(sizeof(cl_int)*4, img_dim_buff_, "image dims");
  img_dim_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR);

  tnearfar_ = opencl_cache_->alloc_mem(2*sizeof(cl_float), tnearfar_buff_, "tnearfar  buffer");
  tnearfar_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR);

  buffers_allocated_ = true;
  prev_ni_ = cl_ni;
  prev_nj_ = cl_nj;
  return true;
}

bool
boxm2_ocl_num_obs_renderer
::cleanup_render_buffers()
{
  if(!buffers_allocated_) {
    return false;
  }

  delete[] img_buff_;
  delete[] vis_buff_;
  delete[] max_omega_buff_;

  opencl_cache_->unref_mem(exp_nobs_image_.ptr());
  exp_nobs_image_ = bocl_mem_sptr(nullptr);
  opencl_cache_->unref_mem(vis_image_.ptr());
  vis_image_ = bocl_mem_sptr(nullptr);
  opencl_cache_->unref_mem(max_omega_image_.ptr());
  max_omega_image_ = bocl_mem_sptr(nullptr);
  opencl_cache_->unref_mem(img_dim_.ptr());
  img_dim_ = bocl_mem_sptr(nullptr);
  opencl_cache_->unref_mem(tnearfar_.ptr());
  tnearfar_ = bocl_mem_sptr(nullptr);

  buffers_allocated_ = false;
  return true;
}

boxm2_ocl_num_obs_renderer
::~boxm2_ocl_num_obs_renderer()
{
  cleanup_render_buffers();
}

bool
boxm2_ocl_num_obs_renderer
::get_last_rendered(vil_image_view<float> &img)
{
  if (render_success_) {
    img.deep_copy( expected_nobs_image_ );
    return true;
  }
  return false;
}

bool
boxm2_ocl_num_obs_renderer
::get_last_vis(vil_image_view<float> &vis_img)
{
  if (render_success_) {
    vis_img.deep_copy( vis_img_ );
    return true;
  }
  return false;
}

bool
boxm2_ocl_num_obs_renderer
::render(vpgl_camera_double_sptr camera,
         unsigned ni,
         unsigned nj,
         float nearfactor,
         float farfactor)
{
  render_success_ = false;

  vul_timer rtime;

  std::size_t lthreads[2]={8,8};

  //: create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device_->context(),*(device_->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);

  unsigned cl_ni=RoundUp(ni,lthreads[0]);
  unsigned cl_nj=RoundUp(nj,lthreads[1]);

  allocate_render_buffers(cl_ni, cl_nj);

  for (unsigned i=0;i<cl_ni*cl_nj;i++){
    img_buff_[i]=0.0f;
  }
  for (unsigned i=0;i<cl_ni*cl_nj;i++) {
    vis_buff_[i] = 1.0f;
    max_omega_buff_[i] = 0.0f;
  }
  exp_nobs_image_->write_to_buffer(queue, true);
  vis_image_->write_to_buffer(queue, true);
  max_omega_image_->write_to_buffer(queue, true);

  img_dim_buff_[0] = 0;   img_dim_buff_[2] = ni;
  img_dim_buff_[1] = 0;   img_dim_buff_[3] = nj;
  img_dim_->write_to_buffer(queue, true);

  tnearfar_buff_[0] = 0.0f;
  tnearfar_buff_[1] = 1000000.0f;

  if(camera->type_name() == "vpgl_perspective_camera")
  {
      float f  = ((vpgl_perspective_camera<double> *)camera.ptr())->get_calibration().focal_length()*((vpgl_perspective_camera<double> *)camera.ptr())->get_calibration().x_scale();
      std::cout<<"Focal Length " << f<<std::endl;
      tnearfar_buff_[0] = f* scene_->finest_resolution()/nearfactor ;
      tnearfar_buff_[1] = f* scene_->finest_resolution()*farfactor ;

      std::cout<<"Near and Far Clipping planes "<<tnearfar_buff_[0]<<" "<<tnearfar_buff_[1]<<std::endl;
  }
  tnearfar_->write_to_buffer(queue, true);

  // run expected image function
  render_expected_image(scene_, device_, opencl_cache_, queue,
                        camera, exp_nobs_image_, vis_image_, max_omega_image_, img_dim_,
                        nobs_type_, kernels_[0], lthreads, cl_ni, cl_nj, nobs_type_size_, tnearfar_);
#if 0
  // normalize
  if (kernels_.size()>1)
  {
    std::size_t gThreads[] = {cl_ni,cl_nj};
    bocl_kernel* normalize_kern = kernels_[1];
    normalize_kern->set_arg( exp_image_.ptr() );
    normalize_kern->set_arg( vis_image_.ptr() );
    normalize_kern->set_arg( img_dim_.ptr());
    normalize_kern->execute( queue, 2, lthreads, gThreads);
    clFinish(queue);

    //clear render kernel args so it can reset em on next execution
    normalize_kern->clear_args();
  }
#endif

  // read out expected image
  exp_nobs_image_->read_to_buffer(queue);
  vis_image_->read_to_buffer(queue);
  clFinish(queue);

  expected_nobs_image_.set_size(ni,nj);
  vis_img_.set_size(ni,nj);
  for (unsigned r=0;r<nj;++r) {
      for (unsigned c=0;c<ni;++c) {
        expected_nobs_image_(c,r)=img_buff_[r*cl_ni + c];
        vis_img_(c,r)=vis_buff_[r*cl_ni+c];
    }
  }

  std::cout<<"Total Render time: "<<rtime.all()<<" ms"<<std::endl;
  clReleaseCommandQueue(queue);

  render_success_ = true;
  return render_success_;
}

bool boxm2_ocl_num_obs_renderer::compile_kernels(const bocl_device_sptr& device, std::vector<bocl_kernel*> & vec_kernels, boxm2_data_type data_type)
{

  if (data_type == BOXM2_NUM_OBS_VIEW)
  {
    std::vector<std::string> src_paths;
    std::string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "view_dep_app_common_functions.cl");
    src_paths.push_back(source_dir + "view_dep_app_helper_functions.cl");
    src_paths.push_back(source_dir + "bit/render_nobs_bit_scene.cl");
    src_paths.push_back(source_dir + "expected_nobs_functor.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    std::string options = "-D RENDER_VIEW_DEP -D STEP_CELL=step_cell_render_nobs(aux_args,data_ptr,d*linfo->block_len)";

    //have kernel construct itself using the context and device
    auto * ray_trace_kernel=new bocl_kernel();
    ray_trace_kernel->create_kernel( &device->context(),
                                     device->device_id(),
                                     src_paths,
                                     "render_nobs_bit_scene",   //kernel name
                                     options,              //options
                                     "boxm2 opencl render_bit_scene"); //kernel identifier (for error checking)
    vec_kernels.push_back(ray_trace_kernel);

#if 0
    //create normalize image kernel
    std::vector<std::string> norm_src_paths;
    norm_src_paths.push_back(source_dir + "pixel_conversion.cl");
    norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
    bocl_kernel * normalize_render_kernel=new bocl_kernel();

    normalize_render_kernel->create_kernel( &device->context(),
                                            device->device_id(),
                                            norm_src_paths,
                                            "normalize_render_kernel",   //kernel name
                                            options,              //options
                                            "normalize render kernel"); //kernel identifier (for error checking)

    vec_kernels.push_back(normalize_render_kernel);
#endif

  }
  else if (data_type == BOXM2_NUM_OBS_SINGLE)
  {
    std::vector<std::string> src_paths;
    std::string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "view_dep_app_common_functions.cl");
    src_paths.push_back(source_dir + "view_dep_app_helper_functions.cl");
    src_paths.push_back(source_dir + "bit/render_nobs_bit_scene.cl");
    src_paths.push_back(source_dir + "expected_nobs_functor.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    std::string options = "-D RENDER -D STEP_CELL=step_cell_render_nobs(aux_args,data_ptr,d*linfo->block_len)";

    //have kernel construct itself using the context and device
    auto * ray_trace_kernel=new bocl_kernel();
    ray_trace_kernel->create_kernel( &device->context(),
                                     device->device_id(),
                                     src_paths,
                                     "render_nobs_bit_scene",   //kernel name
                                     options,              //options
                                     "boxm2 opencl render_bit_scene"); //kernel identifier (for error checking)
    vec_kernels.push_back(ray_trace_kernel);

#if 0
    //create normalize image kernel
    std::vector<std::string> norm_src_paths;
    norm_src_paths.push_back(source_dir + "pixel_conversion.cl");
    norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
    bocl_kernel * normalize_render_kernel=new bocl_kernel();

    normalize_render_kernel->create_kernel( &device->context(),
                                            device->device_id(),
                                            norm_src_paths,
                                            "normalize_render_kernel",   //kernel name
                                            options,              //options
                                            "normalize render kernel"); //kernel identifier (for error checking)

    vec_kernels.push_back(normalize_render_kernel);
#endif

  }
  else {
    std::cerr << "ERROR: boxm2_ocl_num_obs_renderer::compile_kernels(): Unsupported num_obs type " << data_type << std::endl;
    return false;
  }
  return true;
}
