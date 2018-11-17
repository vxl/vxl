// This is brl/bseg/bstm/ocl/pro/processes/bstm_ocl_render_gl_expected_image_process.cxx
//:
// \file


#include <algorithm>
#include <bprb/bprb_func_process.h>
#include <fstream>
#include <iostream>
#include <utility>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bstm/ocl/bstm_opencl_cache.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_data_base.h>
#include <bstm/bstm_util.h>
#include <bstm/ocl/bstm_ocl_util.h>
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <bstm/ocl/algo/bstm_ocl_render_expected_image_function.h>


namespace bstm_ocl_render_gl_expected_image_process_globals
{
  constexpr unsigned n_inputs_ = 10;
  constexpr unsigned n_outputs_ = 1;
  std::size_t lthreads[2]={8,8};

  static std::map<std::string,std::vector<bocl_kernel*> > kernels;

  void compile_kernel(const bocl_device_sptr& device,std::vector<bocl_kernel*> & vec_kernels, std::string opts, bool isViewDep)
  {
    //gather all render sources... seems like a lot for rendering...
    std::vector<std::string> src_paths;
    std::string source_dir = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/bstm/ocl/cl/";
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "pixel_conversion.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "bit/time_tree_library_functions.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "view_dep_app_helper_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/render_bit_scene.cl");
    src_paths.push_back(source_dir + "expected_functor.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //set kernel options
    std::string options = std::move(opts);
    if(!isViewDep)
      options += " -D RENDER_LAMBERT -D RENDER_MOG ";
    else
      options += " -D RENDER_VIEW_DEP ";

    options += " -D STEP_CELL=step_cell_render(aux_args,data_ptr_tt,d*linfo->block_len)";

    //have kernel construct itself using the context and device
    auto * ray_trace_kernel=new bocl_kernel();

    std::cout << "Compiling with options: " << options << std::endl;
    ray_trace_kernel->create_kernel( &device->context(),
                                     device->device_id(),
                                     src_paths,
                                     "render_bit_scene",   //kernel name
                                     options,              //options
                                     "bstm opencl render"); //kernel identifier (for error checking)
    vec_kernels.push_back(ray_trace_kernel);
    //create normalize image kernel
    std::vector<std::string> norm_src_paths;
    norm_src_paths.push_back(source_dir + "pixel_conversion.cl");
    norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
    auto * normalize_render_kernel=new bocl_kernel();

    normalize_render_kernel->create_kernel( &device->context(),
                                            device->device_id(),
                                            norm_src_paths,
                                            "normalize_render_kernel_rgb_gl",   //kernel name
                                            "-D NORMALIZE_RENDER_GL ",       //options
                                            "normalize render kernel rgb gl"); //kernel identifier (for error checking)

    vec_kernels.push_back(normalize_render_kernel);
  }
}

bool bstm_ocl_render_gl_expected_image_process_cons(bprb_func_process& pro)
{
  using namespace bstm_ocl_render_gl_expected_image_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "bstm_scene_sptr";
  input_types_[2] = "bstm_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "unsigned";
  input_types_[5] = "unsigned";
  input_types_[6] = "bocl_mem_sptr"; // exp image buffer;
  input_types_[7] = "bocl_mem_sptr"; // exp image dimensions buffer;
  input_types_[8] = "float"; // time
  input_types_[9] = "bool"; // render label?

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "float";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

}

bool bstm_ocl_render_gl_expected_image_process(bprb_func_process& pro)
{
  using namespace bstm_ocl_render_gl_expected_image_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(i++);
  bstm_scene_sptr scene =pro.get_input<bstm_scene_sptr>(i++);

  bstm_opencl_cache_sptr opencl_cache= pro.get_input<bstm_opencl_cache_sptr>(i++);
  vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(i++);
  auto ni=pro.get_input<unsigned>(i++);
  auto nj=pro.get_input<unsigned>(i++);
  bocl_mem_sptr exp_image =pro.get_input<bocl_mem_sptr>(i++);
  bocl_mem_sptr exp_img_dim =pro.get_input<bocl_mem_sptr>(i++);
  auto time = pro.get_input<float>(i++);
  bool render_label = pro.get_input<bool>(i++);




  //get scene data type and appTypeSize
  std::string data_type,label_data_type;
  int apptypesize,label_apptypesize;
  std::vector<std::string> valid_types;
  valid_types.push_back(bstm_data_traits<BSTM_MOG6_VIEW>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_MOG3_GREY>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_GAUSS_RGB>::prefix());
  valid_types.push_back(bstm_data_traits<BSTM_GAUSS_RGB_VIEW_COMPACT>::prefix());
  if ( !bstm_util::verify_appearance( *scene, valid_types, data_type, apptypesize ) ) {
    std::cout<<"bstm_ocl_render_gl_expected_image_process ERROR: scene doesn't have BSTM_MOG3_GREY or BSTM_GAUSS_RGB data type"<<std::endl;
    return false;
  }

  bool isViewDep = (data_type == bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix()) ||
                    (data_type == bstm_data_traits<BSTM_MOG6_VIEW>::prefix()) ||
                    (data_type == bstm_data_traits<BSTM_GAUSS_RGB_VIEW_COMPACT>::prefix());

  //get initial options (MOG TYPE)
  std::string options = bstm_ocl_util::mog_options(data_type);

  std::cout << data_type << " " << options << " " << apptypesize <<std::endl;
  //get scene label data type and appTypeSize if any.
  std::vector<std::string> valid_label_types;
  valid_label_types.push_back(bstm_data_traits<BSTM_LABEL>::prefix());
  bool foundLabelDataType = bstm_util::verify_appearance( *scene, valid_label_types, label_data_type, label_apptypesize );
#ifdef DEBUG
  if ( !foundLabelDataType )
    std::cout<<"Scene doesn't have BSTM_LABEL label type...rendering without it..."<<std::endl;
  else
    std::cout<<"Scene has " << label_data_type << " type...rendering with it..."<<std::endl;
#endif
  //get options for teh label
  options += bstm_ocl_util::label_options(label_data_type);

  //: create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()), CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0) return false;
  std::string identifier=device->device_identifier()+options;

  // compile the kernel
  if (kernels.find(identifier)==kernels.end())
  {
    std::cout<<"===========Compiling kernels==========="<<std::endl;
    std::vector<bocl_kernel*> ks;
    compile_kernel(device,ks,options, isViewDep);
    kernels[identifier]=ks;
  }

  unsigned cl_ni=RoundUp(ni,lthreads[0]);
  unsigned cl_nj=RoundUp(nj,lthreads[1]);

  auto* buff = new float[4*cl_ni*cl_nj];
  std::fill(buff, buff + 4*cl_ni*cl_nj, 0.0f);
  bocl_mem_sptr exp_color = new bocl_mem(device->context(), buff, 4*cl_ni*cl_nj*sizeof(float), "color im buffer (float4) buffer");
  exp_color->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // visibility image
  auto* vis_buff = new float[cl_ni*cl_nj];
  std::fill(vis_buff, vis_buff + cl_ni*cl_nj, 1.0f);
  bocl_mem_sptr vis_image = new bocl_mem(device->context(), vis_buff, cl_ni*cl_nj*sizeof(float),"vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);


  // run expected image function
  float render_time;

  if(!foundLabelDataType || !render_label)
    render_time = render_expected_image( scene, device, opencl_cache, queue,
                                      cam, exp_color, vis_image, exp_img_dim,
                                      data_type, kernels[identifier][0], lthreads, cl_ni, cl_nj, apptypesize,time);
  else
    render_time = render_expected_image( scene, device, opencl_cache, queue,
                                          cam, exp_color, vis_image, exp_img_dim,
                                          data_type, kernels[identifier][0], lthreads, cl_ni, cl_nj, apptypesize,time, label_data_type, label_apptypesize, render_label);



  // normalize
  {
    std::size_t gThreads[] = {cl_ni,cl_nj};
    bocl_kernel* normalize_kern= kernels[identifier][1];
    normalize_kern->set_arg( exp_color.ptr() );
    normalize_kern->set_arg( vis_image.ptr() );
    normalize_kern->set_arg( exp_img_dim.ptr());
    normalize_kern->set_arg( exp_image.ptr() );
    normalize_kern->execute( queue, 2, lthreads, gThreads);
    clFinish(queue);

    //clear render kernel args so it can reset em on next execution
    normalize_kern->clear_args();
    render_time += normalize_kern->exec_time();
  }



  // read out expected image
  clReleaseCommandQueue(queue);

  delete[] vis_buff;
  delete[] buff;

  //opencl_cache->free_mem(exp_color.ptr());
  //opencl_cache->free_mem(vis_image.ptr());

  //store render time
  int argIdx = 0;
  pro.set_output_val<float>(argIdx, render_time);
  return true;
}
