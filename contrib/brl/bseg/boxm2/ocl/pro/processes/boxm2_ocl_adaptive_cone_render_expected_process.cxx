// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_adaptive_cone_render_expected_process.cxx
//:
// \file
// \brief  A process for rendering the scene.
//
// \author Vishal Jain
// \date Mar 10, 2011
//
#include <fstream>
#include <iostream>
#include <algorithm>
#include <bprb/bprb_func_process.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <boxm2/ocl/algo/boxm2_ocl_render_expected_image_function.h>


namespace boxm2_ocl_adaptive_cone_render_expected_process_globals
{
  constexpr unsigned n_inputs_ = 6;
  constexpr unsigned n_outputs_ = 2;
  std::size_t lthreads[2]={8,8};

  static std::map<std::string,std::vector<bocl_kernel*> > kernels;

  void compile_kernel(const bocl_device_sptr& device,std::vector<bocl_kernel*> & vec_kernels, std::string opts)
  {
    //gather all render sources... seems like a lot for rendering...
    std::vector<std::string> src_paths;
    std::string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "basic/linked_list.cl");
    src_paths.push_back(source_dir + "basic/ray_pyramid.cl");
    src_paths.push_back(source_dir + "basic/image_pyramid.cl");
    src_paths.push_back(source_dir + "ogl/intersect.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "cone/render_adaptive_cone_kernels.cl");
    src_paths.push_back(source_dir + "cone/cone_util.cl");
    src_paths.push_back(source_dir + "cone/cast_adaptive_cone_ray.cl");

    //set kernel options
    opts += " -D STEP_CELL=step_cell_cone(aux_args,data_ptr,intersect_volume) ";
    opts += " -D COMPUTE_BALL_PROPERTIES=compute_ball_properties(aux_args) ";
    opts += " -D RENDER ";
    opts += " -D IMG_TYPE=float ";

    //have kernel construct itself using the context and device
    auto * ray_trace_kernel=new bocl_kernel();

    ray_trace_kernel->create_kernel( &device->context(),
                                     device->device_id(),
                                     src_paths,
                                     "render_adaptive_cone",   //kernel name
                                     opts,              //options
                                     "boxm2 opencl render expected adaptive cone"); //kernel identifier (for error checking)
    vec_kernels.push_back(ray_trace_kernel);

    //create normalize image kernel
    std::vector<std::string> norm_src_paths;
    norm_src_paths.push_back(source_dir + "pixel_conversion.cl");
    norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
    auto * normalize_render_kernel=new bocl_kernel();
    normalize_render_kernel->create_kernel( &device->context(),
                                            device->device_id(),
                                            norm_src_paths,
                                            "normalize_render_kernel",   //kernel name
                                            " -D RENDER ",              //options
                                            "normalize render kernel"); //kernel identifier (for error checking)
    vec_kernels.push_back(normalize_render_kernel);
  }
}

bool boxm2_ocl_adaptive_cone_render_expected_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_adaptive_cone_render_expected_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "unsigned";
  input_types_[5] = "unsigned";

  // process has 1 output:
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "vil_image_view_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_adaptive_cone_render_expected_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_adaptive_cone_render_expected_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);

  boxm2_opencl_cache_sptr opencl_cache= pro.get_input<boxm2_opencl_cache_sptr>(i++);
  vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(i++);
  auto ni=pro.get_input<unsigned>(i++);
  auto nj=pro.get_input<unsigned>(i++);

  bool foundDataType = false;
  std::string data_type,options;
  std::vector<std::string> apps = scene->appearances();
  for (const auto & app : apps) {
    if ( app == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
    {
      data_type = app;
      foundDataType = true;
      options=" -D MOG_TYPE_8 ";
    }
    else if ( app == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
    {
      data_type = app;
      foundDataType = true;
      options=" -D MOG_TYPE_16 ";
    }
  }
  if (!foundDataType) {
    std::cout<<"BOXM2_OCL_ADAPTIVE_CONE_RENDER_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<std::endl;
    return false;
  }

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0) return false;
  std::string identifier=device->device_identifier()+options;

  // compile the kernel
  if (kernels.find(identifier)==kernels.end())
  {
    std::cout<<"===========Compiling kernels==========="<<std::endl;
    std::vector<bocl_kernel*> ks;
    compile_kernel(device,ks,options);
    kernels[identifier]=ks;
  }

  //make sure the global size is divisible by the local size
  unsigned cl_ni=RoundUp(ni,lthreads[0]);
  unsigned cl_nj=RoundUp(nj,lthreads[1]);
  auto* buff = new float[cl_ni*cl_nj];
  std::fill(buff, buff+cl_ni*cl_nj, 0.0f);
  auto* ray_level_buff = new unsigned char[cl_ni*cl_nj];
  std::fill(ray_level_buff, ray_level_buff+cl_ni*cl_nj, 0);

  bocl_mem_sptr exp_image=new bocl_mem(device->context(),buff,cl_ni*cl_nj*sizeof(float),"exp cone image buffer");
  exp_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr ray_level_image=new bocl_mem(device->context(),ray_level_buff,cl_ni*cl_nj*sizeof(unsigned char),"exp cone image buffer");
  ray_level_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //write image dims (real img dims, not ni, nj)
  unsigned int img_dim_buff[] = {0, 0, ni, nj};
  bocl_mem_sptr exp_img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(unsigned int)*4, "image dims");
  exp_img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // visibility image
  auto* vis_buff = new float[cl_ni*cl_nj];
  std::fill(vis_buff, vis_buff + cl_ni*cl_nj, 1.0f);
  bocl_mem_sptr vis_image = new bocl_mem(device->context(), vis_buff, cl_ni*cl_nj*sizeof(float), "vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  ////: run expected image function
  render_cone_expected_image(scene, device, opencl_cache, queue,
                             cam, exp_image, vis_image,ray_level_image, exp_img_dim,
                             data_type, kernels[identifier][0], lthreads, cl_ni, cl_nj);

  exp_image->read_to_buffer(queue);
  auto* exp_img_out=new vil_image_view<float>(ni,nj);
  for (unsigned c=0;c<nj;c++)
    for (unsigned r=0;r<ni;r++)
      (*exp_img_out)(r,c)=buff[c*cl_ni+r];

  ///debugging save vis, pre, norm images
#if 1
  vis_image->read_to_buffer(queue);
  ray_level_image->read_to_buffer(queue);
  clFinish(queue);
  int idx = 0;
  auto * vis_view=new vil_image_view<float>(cl_ni,cl_nj);

  for (unsigned c=0;c<cl_nj;++c) {
    for (unsigned r=0;r<cl_ni;++r) {
      float vis = vis_buff[idx];
      if (ray_level_buff[idx] < 4)
      {
        float pow_factor =std::pow(0.25f,(float) (4-ray_level_buff[idx]));
        vis =std::pow(vis,pow_factor);
      }
      (*vis_view)(r,c) = vis;
      idx++;
    }
  }
  //vil_save( vis_view, "vis_render_debug.tiff");
  //vil_save( *exp_img_out, "exp_render_debug.tiff");
#endif

  delete [] buff;
  delete [] vis_buff;
  clReleaseCommandQueue(queue);

  // store scene smaprt pointer
  i=0;
  pro.set_output_val<vil_image_view_base_sptr>(i++, exp_img_out);
  pro.set_output_val<vil_image_view_base_sptr>(i++, vis_view);
  return true;
}
