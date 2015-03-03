// This is brl/bseg/boxm2/ocl/algo/boxm2_ocl_expected_image_renderer.cxx
//:
// \file
// \brief  Render and expected image of a boxm2 model
//
// \author Daniel Crispell, adapted from process version
// \date 4 Nov 2014
#include "boxm2_ocl_expected_image_renderer.h"

#include <vcl_algorithm.h>
#include <vcl_stdexcept.h>
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
#include "boxm2_ocl_render_expected_image_function.h"
#include <vul/vul_timer.h>


boxm2_ocl_expected_image_renderer::boxm2_ocl_expected_image_renderer(boxm2_scene_sptr scene, bocl_device_sptr device, vcl_string ident)
  : scene_(scene), device_(device), render_success_(false)
{
  bool foundDataType = false;
  vcl_vector<vcl_string> apps = scene->appearances();

  apptypesize_ = 0;
  num_planes_ = 0;
  boxm2_data_type valid_appearance_types_grey[] = {
    BOXM2_MOG3_GREY, BOXM2_MOG3_GREY_16, BOXM2_FLOAT8, BOXM2_LABEL_SHORT
  };
  boxm2_data_type valid_appearance_types_rgb[] = {BOXM2_GAUSS_RGB};

  int num_valid_appearances_grey = sizeof(valid_appearance_types_grey) / sizeof(valid_appearance_types_grey[0]);

  for (unsigned int i=0; i<apps.size(); ++i) {
    for (unsigned v = 0; v < num_valid_appearances_grey; ++v) {
      boxm2_data_type valid_apm_type = valid_appearance_types_grey[v];
      vcl_string valid_apm_prefix = boxm2_data_info::prefix(valid_apm_type);
      if ( apps[i] == valid_apm_prefix )
      {
        data_type_ = valid_apm_prefix;
        foundDataType = true;
        apptypesize_ = boxm2_data_info::datasize(valid_apm_type);
        num_planes_ = 1;
        vcl_cout<<"===========Compiling kernels==========="<<vcl_endl;
        compile_kernels(device, kernels_, valid_apm_type);
        break;
      }
    }
  }
  int num_valid_appearances_rgb = sizeof(valid_appearance_types_rgb) / sizeof(valid_appearance_types_rgb[0]);
  for (unsigned int i=0; i<apps.size(); ++i) {
    for (unsigned v = 0; v < num_valid_appearances_rgb; ++v) {
      boxm2_data_type valid_apm_type = valid_appearance_types_rgb[v];
      vcl_string valid_apm_prefix = boxm2_data_info::prefix(valid_apm_type);
      if ( apps[i] == valid_apm_prefix )
      {
        data_type_ = valid_apm_prefix;
        foundDataType = true;
        apptypesize_ = boxm2_data_info::datasize(valid_apm_type);
        num_planes_ = 4;
        vcl_cout<<"===========Compiling kernels==========="<<vcl_endl;
        compile_kernels(device, kernels_, valid_apm_type);
        break;
      }
    }
  }

  if (!foundDataType) {
    vcl_cout<<"BOXM2_OCL_EXPECTED_IMAGE_RENDERER ERROR: scene doesn't have valid appearance type"<<vcl_endl;
    throw vcl_runtime_error("scene does not have valid appearance type");
  }
  if (ident.size() > 0) {
    data_type_ += "_" + ident;
  }
}

bool boxm2_ocl_expected_image_renderer::get_last_rendered(vil_image_view<float> &img)
{
  if (render_success_) {
    img = expected_img_;
    return true;
  }
  return false;
}

bool boxm2_ocl_expected_image_renderer::get_last_vis(vil_image_view<float> &vis_img)
{
  if (render_success_) {
    vis_img = vis_img_;
    return true;
  }
  return false;
}

bool boxm2_ocl_expected_image_renderer::render(vpgl_camera_double_sptr camera, unsigned ni, unsigned nj, boxm2_opencl_cache_sptr opencl_cache, float nearfactor, float farfactor)
{
  render_success_ = false;

  vul_timer rtime;

  vcl_size_t lthreads[2]={8,8};

  //: create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device_->context(),*(device_->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);

  unsigned cl_ni=RoundUp(ni,lthreads[0]);
  unsigned cl_nj=RoundUp(nj,lthreads[1]);
  float* img_buff = new float[cl_ni*cl_nj*num_planes_];
  for (unsigned i=0;i<cl_ni*cl_nj*num_planes_;i++) img_buff[i]=0.0f;

  bocl_mem_sptr exp_image=opencl_cache->alloc_mem(cl_ni*cl_nj*num_planes_*sizeof(cl_float), img_buff,"exp image buffer");
  exp_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  int img_dim_buff[4];
  img_dim_buff[0] = 0;   img_dim_buff[2] = ni;
  img_dim_buff[1] = 0;   img_dim_buff[3] = nj;
  bocl_mem_sptr exp_img_dim=new bocl_mem(device_->context(), img_dim_buff, sizeof(int)*4, "image dims");
  exp_img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // visibility image
  float* vis_buff = new float[cl_ni*cl_nj];
  vcl_fill(vis_buff, vis_buff + cl_ni*cl_nj, 1.0f);
  bocl_mem_sptr vis_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), vis_buff,"vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
   float* max_omega_buff = new float[cl_ni*cl_nj];
  vcl_fill(max_omega_buff, max_omega_buff + cl_ni*cl_nj, 0.0f);
  bocl_mem_sptr max_omega_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), max_omega_buff,"vis image buffer");
  max_omega_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  float tnearfar[2] = { 0.0f, 1000000} ;

  if(camera->type_name() == "vpgl_perspective_camera")
  {
      
      float f  = ((vpgl_perspective_camera<double> *)camera.ptr())->get_calibration().focal_length()*((vpgl_perspective_camera<double> *)camera.ptr())->get_calibration().x_scale();
      vcl_cout<<"Focal Length " << f<<vcl_endl;
      tnearfar[0] = f* scene_->finest_resolution()/nearfactor ;
      tnearfar[1] = f* scene_->finest_resolution()*farfactor ;

      vcl_cout<<"Near and Far Clipping planes "<<tnearfar[0]<<" "<<tnearfar[1]<<vcl_endl;
  }
  bocl_mem_sptr tnearfar_mem_ptr = opencl_cache->alloc_mem(2*sizeof(float), tnearfar, "tnearfar  buffer");
  tnearfar_mem_ptr->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);


  // run expected image function
  render_expected_image(scene_, device_, opencl_cache, queue,
                        camera, exp_image, vis_image, max_omega_image,exp_img_dim,
                        data_type_, kernels_[0], lthreads, cl_ni, cl_nj,apptypesize_,tnearfar_mem_ptr);
  // normalize
  if (kernels_.size()>1)
  {
    vcl_size_t gThreads[] = {cl_ni,cl_nj};
    bocl_kernel* normalize_kern = kernels_[1];
    normalize_kern->set_arg( exp_image.ptr() );
    normalize_kern->set_arg( vis_image.ptr() );
    normalize_kern->set_arg( exp_img_dim.ptr());
    normalize_kern->execute( queue, 2, lthreads, gThreads);
    clFinish(queue);

    //clear render kernel args so it can reset em on next execution
    normalize_kern->clear_args();
  }

  // read out expected image
  exp_image->read_to_buffer(queue);
  vis_image->read_to_buffer(queue);
  clFinish(queue);

  expected_img_.set_size(ni,nj,num_planes_);
  vis_img_.set_size(ni,nj);
  for (unsigned r=0;r<nj;++r) {
      for (unsigned c=0;c<ni;++c) {
        for (unsigned p=0; p<num_planes_; ++p) {
        expected_img_(c,r,p)=img_buff[r*cl_ni*num_planes_ + c*num_planes_ + p];
        vis_img_(c,r)=vis_buff[r*cl_ni+c];
      }
    }
  }

  vcl_cout<<"Total Render time: "<<rtime.all()<<" ms"<<vcl_endl;
  delete [] vis_buff;
  delete [] img_buff;
  delete [] max_omega_buff;
  opencl_cache->unref_mem(vis_image.ptr());
  opencl_cache->unref_mem(exp_image.ptr());
  opencl_cache->unref_mem(max_omega_image.ptr());
  opencl_cache->unref_mem(tnearfar_mem_ptr.ptr());
  clReleaseCommandQueue(queue);

  render_success_ = true;
  return render_success_;
}

bool boxm2_ocl_expected_image_renderer::compile_kernels(bocl_device_sptr device, vcl_vector<bocl_kernel*> & vec_kernels, boxm2_data_type data_type)
{
  vcl_string options_basic = boxm2_ocl_util::mog_options( boxm2_data_info::prefix(data_type) );

  if ( (data_type == BOXM2_MOG3_GREY) ||
       (data_type == BOXM2_MOG3_GREY_16) )
  {
    vcl_vector<vcl_string> src_paths;
    vcl_string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "pixel_conversion.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/render_bit_scene.cl");
    src_paths.push_back(source_dir + "expected_functor.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    vcl_string options = options_basic;
    options += "-D RENDER ";

    options += "-D STEP_CELL=step_cell_render(aux_args.mog,aux_args.alpha,data_ptr,d*linfo->block_len,vis,aux_args.expint)";

    //have kernel construct itself using the context and device
    bocl_kernel * ray_trace_kernel=new bocl_kernel();
    ray_trace_kernel->create_kernel( &device->context(),
                                     device->device_id(),
                                     src_paths,
                                     "render_bit_scene",   //kernel name
                                     options,              //options
                                     "boxm2 opencl render_bit_scene"); //kernel identifier (for error checking)
    vec_kernels.push_back(ray_trace_kernel);

    //create normalize image kernel
    vcl_vector<vcl_string> norm_src_paths;
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

  }
  else if (data_type == BOXM2_LABEL_SHORT) {
    vcl_vector<vcl_string> src_paths;
    vcl_string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "pixel_conversion.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/render_bit_scene.cl");
    src_paths.push_back(source_dir + "expected_functor.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");
    vcl_cout<<"COMPILING SHORT"<<vcl_endl;

    vcl_string options = options_basic;
    options += "-D RENDER ";
    options += "-D RENDER_MAX -D STEP_CELL=step_cell_render_max(aux_args.mog,aux_args.alpha,data_ptr,d*linfo->block_len,vis,aux_args.expint,aux_args.maxomega)";


    //have kernel construct itself using the context and device
    bocl_kernel * ray_trace_kernel=new bocl_kernel();
    ray_trace_kernel->create_kernel( &device->context(),
                                     device->device_id(),
                                     src_paths,
                                     "render_bit_scene",   //kernel name
                                     options,              //options
                                     "boxm2 opencl render_bit_scene"); //kernel identifier (for error checking)
    vec_kernels.push_back(ray_trace_kernel);

  }
  else if (data_type == BOXM2_GAUSS_RGB) {
    vcl_vector<vcl_string> src_paths;
    vcl_string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "pixel_conversion.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/render_rgb.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //set kernel options
    vcl_string options = options_basic + " -D RENDER ";
    options += " -D DETERMINISTIC ";
    options += " -D YUV -D STEP_CELL=step_cell_render(aux_args,data_ptr,llid,d*linfo->block_len)";

    //have kernel construct itself using the context and device
    bocl_kernel * ray_trace_kernel=new bocl_kernel();
    ray_trace_kernel->create_kernel( &device->context(),
                                     device->device_id(),
                                     src_paths,
                                     "render_bit_scene",   //kernel name
                                     options,              //options
                                     "boxm2 opencl render color"); //kernel identifier (for error checking)
    vec_kernels.push_back(ray_trace_kernel);

    //create normalize image kernel
    vcl_vector<vcl_string> norm_src_paths;
    norm_src_paths.push_back(source_dir + "pixel_conversion.cl");
    norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
    bocl_kernel * normalize_render_kernel=new bocl_kernel();

    normalize_render_kernel->create_kernel( &device->context(),
                                            device->device_id(),
                                            norm_src_paths,
                                            "normalize_render_rgb_kernel",   //kernel name
                                            options,              //options
                                            "normalize render color kernel"); //kernel identifier (for error checking)
    vec_kernels.push_back(normalize_render_kernel);
  }
  else {
    vcl_cerr << "ERROR: boxm2_ocl_expected_image_renderer::compile_kernels(): Unsupported Appearance model type " << data_type << vcl_endl;
    return false;
  }
  return true;
}
