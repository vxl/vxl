// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_render_expected_image_naa_process.cxx
//:
// \file
// \brief  A process for rendering the scene using the normalized_albedo_array appearance model.
//
// \author Daniel Crispell
// \date Dec 19, 2011

#include <fstream>
#include <iostream>
#include <algorithm>
#include <vector>
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
#include <vgl/vgl_vector_3d.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <boxm2/ocl/algo/boxm2_ocl_render_expected_image_function.h>
#include <vul/vul_timer.h>

#include <brad/brad_image_metadata.h>
#include <brad/brad_atmospheric_parameters.h>
#include <brad/brad_illum_util.h>

namespace boxm2_ocl_render_expected_image_naa_process_globals
{
  constexpr unsigned n_inputs_ = 8;
  constexpr unsigned n_outputs_ = 2;
  std::size_t lthreads[2]={8,8};

  static std::map<std::string,std::vector<bocl_kernel*> > kernels;
  bool compile_kernel(const bocl_device_sptr& device,std::vector<bocl_kernel*> & vec_kernels, const std::string& opts)
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
    src_paths.push_back(source_dir + "bit/render_bit_scene.cl");
    src_paths.push_back(source_dir + "expected_functor.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //set kernel options
    //#define STEP_CELL step_cell_render(mixture_array, alpha_array, data_ptr, d, &vis, &expected_int);
    std::string options = opts + " -D RENDER_NAA ";
    options += " -D DETERMINISTIC ";
    options += " -D STEP_CELL=step_cell_render_naa(aux_args,data_ptr,d*linfo->block_len,vis,aux_args.expint)";

    //have kernel construct itself using the context and device
    auto * ray_trace_kernel=new bocl_kernel();

    if (!ray_trace_kernel->create_kernel( &device->context(),
                                          device->device_id(),
                                          src_paths,
                                          "render_bit_scene",   //kernel name
                                          options,              //options
                                          "boxm2 opencl render_bit_scene")) { //kernel identifier (for error checking)
       std::cerr << "create_kernel (render kernel) returned error.\n";
       return false;
    }
    vec_kernels.push_back(ray_trace_kernel);

    //create normalize image kernel
    std::vector<std::string> norm_src_paths;
    norm_src_paths.push_back(source_dir + "pixel_conversion.cl");
    norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
    auto * normalize_render_kernel=new bocl_kernel();

    options = opts + " -D RENDER_NAA ";
    if (!normalize_render_kernel->create_kernel( &device->context(),
                                                 device->device_id(),
                                                 norm_src_paths,
                                                 "normalize_render_kernel",   //kernel name
                                                 options,              //options
                                                 "normalize render kernel")){ //kernel identifier (for error checking)
      std::cerr << "create_kernel (normalize kernel) returned error.\n";
      return false;
    }
    vec_kernels.push_back(normalize_render_kernel);
    return true;
  }
}

bool boxm2_ocl_render_expected_image_naa_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_render_expected_image_naa_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "unsigned";
  input_types_[5] = "unsigned";
  input_types_[6] = "brad_image_metadata_sptr";
  input_types_[7] = "brad_atmospheric_parameters_sptr";

  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "vil_image_view_base_sptr";

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  return good;
}

bool boxm2_ocl_render_expected_image_naa_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_render_expected_image_naa_process_globals;

  vul_timer rtime;
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(0);
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(1);

  boxm2_opencl_cache_sptr opencl_cache= pro.get_input<boxm2_opencl_cache_sptr>(2);
  vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(3);
  auto ni=pro.get_input<unsigned>(4);
  auto nj=pro.get_input<unsigned>(5);

  brad_image_metadata_sptr metadata = pro.get_input<brad_image_metadata_sptr>(6);
  brad_atmospheric_parameters_sptr atm_params = pro.get_input<brad_atmospheric_parameters_sptr>(7);

  bool found_appearance = false;
  std::string data_type,options;
  std::vector<std::string> apps = scene->appearances();

  for (const auto & app : apps) {
    if ( app == boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::prefix() )
    {
      data_type = app;
      found_appearance = true;
      // boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::datasize();
    }
  }
  if (!found_appearance) {
    std::cout<<"BOXM2_OCL_RENDER_IMAGE_NAA_PROCESS ERROR: scene doesn't have BOXM2_NORMAL_ALBEDO_ARRAY data type" << std::endl;
    return false;
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
    if (!compile_kernel(device,ks,options)){
      std::cerr << "ERROR: compile_kernel returned false.\n";
      return false;
    }
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

  float background_rad_buff[1];
  // assume expected value of albedo is around 0.2
  constexpr double expected_albedo = 0.2;
  double expected_radiance = brad_expected_radiance_chavez(expected_albedo,vgl_vector_3d<double>(0,0,1),*metadata,*atm_params);
  background_rad_buff[0] = expected_radiance;
  bocl_mem_sptr background_rad = new bocl_mem(device->context(), background_rad_buff, sizeof(float), "background radiance value");
  background_rad->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // visibility image
  auto* vis_buff = new float[cl_ni*cl_nj];
  std::fill(vis_buff, vis_buff + cl_ni*cl_nj, 1.0f);
  bocl_mem_sptr vis_image=opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float),vis_buff,"vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);


  // run expected image function
  render_expected_image_naa(scene, device, opencl_cache, queue,
                            cam, exp_image, vis_image, exp_img_dim,
                            kernels[identifier][0], lthreads, cl_ni, cl_nj, metadata, atm_params);

  std::cout << "Normalizing" << std::endl;

  // normalize
  {
    std::size_t gThreads[] = {cl_ni,cl_nj};
    bocl_kernel* normalize_kern = kernels[identifier][1];
    normalize_kern->set_arg( exp_image.ptr() );
    normalize_kern->set_arg( vis_image.ptr() );
    normalize_kern->set_arg( exp_img_dim.ptr());
    normalize_kern->set_arg( background_rad.ptr() );
    normalize_kern->execute( queue, 2, lthreads, gThreads);
    clFinish(queue);

    //clear render kernel args so it can reset em on next execution
    normalize_kern->clear_args();
  }

  std::cout << "done normalizing" << std::endl;

  // read out expected image
  exp_image->read_to_buffer(queue);
  vis_image->read_to_buffer(queue);

  std::cout << "done reading images" << std::endl;

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
  vil_image_view<vxl_byte>* exp_img_out=new vil_image_view<vxl_byte>(ni,nj);
    for (unsigned c=0;c<nj;c++)
      for (unsigned r=0;r<ni;r++)
        (*exp_img_out)(r,c)= (vxl_byte) (buff[c*cl_ni+r] * 255.0f);
#endif

  std::cout<<"Total Render time: "<<rtime.all()<<" ms"<<std::endl;
  opencl_cache->unref_mem(exp_image.ptr());
  opencl_cache->unref_mem(vis_image.ptr());

  delete [] vis_buff;
  delete [] buff;


  clReleaseCommandQueue(queue);

  // store scene smaprt pointer
  pro.set_output_val<vil_image_view_base_sptr>(0, exp_img_out);
  pro.set_output_val<vil_image_view_base_sptr>(1, vis_img_out);
  return true;
}
