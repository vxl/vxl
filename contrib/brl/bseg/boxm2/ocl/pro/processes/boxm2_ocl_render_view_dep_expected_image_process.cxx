// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_render_view_dep_expected_image_process.cxx
#include <fstream>
#include <iostream>
#include <algorithm>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for rendering the scene.
//
// \author Vishal Jain
// \date Mar 10, 2011

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
#include <brdb/brdb_value.h>

#include <boxm2/boxm2_util.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <boxm2/ocl/algo/boxm2_ocl_render_expected_image_function.h>
#include <vul/vul_timer.h>


namespace boxm2_ocl_render_view_dep_expected_image_process_globals
{
  constexpr unsigned n_inputs_ = 7;
  constexpr unsigned n_outputs_ = 4;
  std::size_t lthreads[2]={8,8};

  static std::map<std::string,std::vector<bocl_kernel*> > kernels;
  void compile_kernel(const bocl_device_sptr& device,std::vector<bocl_kernel*> & vec_kernels, const std::string& opts)
  {
    //gather all render sources... seems like a lot for rendering...
    std::vector<std::string> src_paths;
    std::string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "pixel_conversion.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "view_dep_app_common_functions.cl");
    src_paths.push_back(source_dir + "view_dep_app_helper_functions.cl");
    src_paths.push_back(source_dir + "bit/render_bit_scene.cl");
    src_paths.push_back(source_dir + "expected_functor.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    {
      std::string options = opts;
      options += "-D RENDER_VIEW_DEP ";
      options += "-D STEP_CELL=step_cell_render(aux_args.mog,aux_args.alpha,data_ptr,aux_args.app_model_weights,d*linfo->block_len,vis,aux_args.expint)";

      //have kernel construct itself using the context and device
      auto * ray_trace_kernel=new bocl_kernel();
      ray_trace_kernel->create_kernel( &device->context(),
                                       device->device_id(),
                                       src_paths,
                                       "render_bit_scene",   //kernel name
                                       options,              //options
                                       "boxm2 opencl render_view_dep_bit_scene"); //kernel identifier (for error checking)
      vec_kernels.push_back(ray_trace_kernel);

      //create normalize image kernel
      std::vector<std::string> norm_src_paths;
      norm_src_paths.push_back(source_dir + "pixel_conversion.cl");
      norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
      auto * normalize_render_kernel=new bocl_kernel();

      std::string normalize_options = opts;
      normalize_options += "-D RENDER ";
      normalize_render_kernel->create_kernel( &device->context(),
                                              device->device_id(),
                                              norm_src_paths,
                                              "normalize_render_kernel",   //kernel name
                                              normalize_options,              //options
                                              "normalize render kernel"); //kernel identifier (for error checking)

      vec_kernels.push_back(normalize_render_kernel);
    }
  }
}

bool boxm2_ocl_render_view_dep_expected_image_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_render_view_dep_expected_image_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "unsigned";
  input_types_[5] = "unsigned";
  input_types_[6] = "vcl_string";

  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "vil_image_view_base_sptr";
  output_types_[2] = "float";
  output_types_[3] = "float";

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  // in case the 7th input is not set
  brdb_value_sptr idx = new brdb_value_t<std::string>("");
  pro.set_input(6, idx);
  return good;
}

bool boxm2_ocl_render_view_dep_expected_image_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_render_view_dep_expected_image_process_globals;

  vul_timer rtime;
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
  std::string ident = pro.get_input<std::string>(i++);

  std::string data_type;
  int apptypesize;
  std::vector<std::string> valid_types;
  valid_types.push_back(boxm2_data_traits<BOXM2_MOG6_VIEW>::prefix());
  valid_types.push_back(boxm2_data_traits<BOXM2_MOG6_VIEW_COMPACT>::prefix());

  if ( !boxm2_util::verify_appearance( *scene, valid_types, data_type, apptypesize ) ) {
     std::cout<<"boxm2_ocl_render_gl_view_dep_app_expected_image_process ERROR: scene doesn't have BOXM2_MOG6_VIEW data type"<<std::endl;
    return false;
  }

  std::string options = boxm2_ocl_util::mog_options(data_type);

  if (ident.size() > 0) {
    data_type += "_" + ident;
  }

//: create a command queue.
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

  unsigned cl_ni=RoundUp(ni,lthreads[0]);
  unsigned cl_nj=RoundUp(nj,lthreads[1]);
  auto* buff = new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++) buff[i]=0.0f;

  bocl_mem_sptr exp_image=opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), buff,"exp image buffer");
  exp_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  int img_dim_buff[4];
  img_dim_buff[0] = 0;   img_dim_buff[2] = ni;
  img_dim_buff[1] = 0;   img_dim_buff[3] = nj;
  bocl_mem_sptr exp_img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(int)*4, "image dims");
  exp_img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // visibility image
  auto* vis_buff = new float[cl_ni*cl_nj];
  std::fill(vis_buff, vis_buff + cl_ni*cl_nj, 1.0f);
  bocl_mem_sptr vis_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), vis_buff,"vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
   auto* max_omega_buff = new float[cl_ni*cl_nj];
  std::fill(max_omega_buff, max_omega_buff + cl_ni*cl_nj, 0.0f);
  bocl_mem_sptr max_omega_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), max_omega_buff,"vis image buffer");
  max_omega_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  float tnearfar[2] = { 0.0f, 1000000} ;


  bocl_mem_sptr tnearfar_mem_ptr = opencl_cache->alloc_mem(2*sizeof(float), tnearfar, "tnearfar  buffer");
  tnearfar_mem_ptr->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  // run expected image function
  float t = render_expected_image(scene, device, opencl_cache, queue,
                        cam, exp_image, vis_image, max_omega_image,exp_img_dim,
                        data_type, kernels[identifier][0], lthreads, cl_ni, cl_nj,apptypesize,tnearfar_mem_ptr);
  // normalize
  if (kernels[identifier].size()>1)
  {
    std::size_t gThreads[] = {cl_ni,cl_nj};
    bocl_kernel* normalize_kern = kernels[identifier][1];
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

#if 1 //output a float image by default
  auto* exp_img_out=new vil_image_view<float>(ni,nj);
  for (unsigned c=0;c<nj;c++)
    for (unsigned r=0;r<ni;r++)
      (*exp_img_out)(r,c)=buff[c*cl_ni+r];

  auto* vis_img_out=new vil_image_view<float>(ni,nj);
  for (unsigned c=0;c<nj;c++)
    for (unsigned r=0;r<ni;r++)
      (*vis_img_out)(r,c)=vis_buff[c*cl_ni+r];
#else //option to output a byte image (For easier saving)
  std::cout << "Saving byte image..." << std::endl;
  vil_image_view<vxl_byte>* exp_img_out=new vil_image_view<vxl_byte>(ni,nj);
    for (unsigned c=0;c<nj;c++)
      for (unsigned r=0;r<ni;r++)
        (*exp_img_out)(r,c)= (vxl_byte) (buff[c*cl_ni+r] * 255.0f);

  vil_image_view<vxl_byte>* vis_img_out=new vil_image_view<vxl_byte>(ni,nj);
  for (unsigned c=0;c<nj;c++)
    for (unsigned r=0;r<ni;r++)
      (*vis_img_out)(r,c) = (vxl_byte) (vis_buff[c*cl_ni+r] * 255.0f);
#endif

  float all_time = rtime.all();
  std::cout<<"Total Render time: "<< all_time <<" ms"<<std::endl;
  delete [] vis_buff;
  delete [] buff;
  delete [] max_omega_buff;
  opencl_cache->unref_mem(vis_image.ptr());
  opencl_cache->unref_mem(exp_image.ptr());
  opencl_cache->unref_mem(max_omega_image.ptr());
  opencl_cache->unref_mem(tnearfar_mem_ptr.ptr());

  clReleaseCommandQueue(queue);
  i=0;
  // store scene smaprt pointer
  pro.set_output_val<vil_image_view_base_sptr>(i++, exp_img_out);
  pro.set_output_val<vil_image_view_base_sptr>(i++, vis_img_out);
  pro.set_output_val<float>(i++, t);
  pro.set_output_val<float>(i++, all_time );

  return true;
}
