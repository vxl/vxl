// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_render_expected_color_process.cxx
//:
// \file
// \brief  A process for rendering the scene.
//
// \author Vishal Jain
// \date Mar 10, 2011

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
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <boxm2/ocl/algo/boxm2_ocl_render_expected_image_function.h>


namespace boxm2_ocl_render_expected_color_process_globals
{
  constexpr unsigned n_inputs_ = 8;
  constexpr unsigned n_outputs_ = 2;
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
    src_paths.push_back(source_dir + "bit/render_rgb.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //set kernel options
    //#define STEP_CELL step_cell_render(mixture_array, alpha_array, data_ptr, d, &vis, &expected_int);
    std::string options = opts + " -D RENDER ";
    options += " -D DETERMINISTIC ";
    options += " -D YUV -D STEP_CELL=step_cell_render(aux_args,data_ptr,llid,d*linfo->block_len)";

    //have kernel construct itself using the context and device
    auto * ray_trace_kernel=new bocl_kernel();
    ray_trace_kernel->create_kernel( &device->context(),
                                     device->device_id(),
                                     src_paths,
                                     "render_bit_scene",   //kernel name
                                     options,              //options
                                     "boxm2 opencl render process color"); //kernel identifier (for error checking)
    vec_kernels.push_back(ray_trace_kernel);

    //create normalize image kernel
    std::vector<std::string> norm_src_paths;
    norm_src_paths.push_back(source_dir + "pixel_conversion.cl");
    norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
    auto * normalize_render_kernel=new bocl_kernel();

    normalize_render_kernel->create_kernel( &device->context(),
                                            device->device_id(),
                                            norm_src_paths,
                                            "normalize_render_rgb_kernel",   //kernel name
                                            options,              //options
                                            "normalize render color kernel"); //kernel identifier (for error checking)
    vec_kernels.push_back(normalize_render_kernel);
  }
}

bool boxm2_ocl_render_expected_color_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_render_expected_color_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "unsigned";
  input_types_[5] = "unsigned";
  input_types_[6] = "float";                        // near factor ( maximum # of pixels should map to the finest voxel )
  input_types_[7] = "float";                        // far factor ( minimum # of pixels should map to the finest voxel )

  brdb_value_sptr tnearfactor = new brdb_value_t<float>(100000.0f);  //by default update alpha
  brdb_value_sptr tfarfactor = new brdb_value_t<float>(100000.0f);  //by default update alpha

  pro.set_input(6, tnearfactor);
  pro.set_input(7, tfarfactor);
  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "vil_image_view_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_render_expected_color_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_render_expected_color_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned argIdx = 0;
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(argIdx++);
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(argIdx++);
  boxm2_opencl_cache_sptr opencl_cache= pro.get_input<boxm2_opencl_cache_sptr>(argIdx++);
  vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(argIdx++);
  auto ni=pro.get_input<unsigned>(argIdx++);
  auto nj=pro.get_input<unsigned>(argIdx++);
  auto   nearfactor = pro.get_input<float>(argIdx++);
  auto   farfactor = pro.get_input<float>(argIdx++);
  //make sure the data types match the scene
  bool foundDataType = false;
  std::string data_type,options;
  std::vector<std::string> apps = scene->appearances();
  int apptypesize = 0;
  for (const auto & app : apps) {
    if ( app == boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix() )
    {
      data_type = app;
      foundDataType = true;
      options=" -D MOG_TYPE_8 ";
      apptypesize= boxm2_data_traits<BOXM2_GAUSS_RGB>::datasize();
    }
  }
  if (!foundDataType) {
    std::cout<<"BOXM2_OCL_RENDER_COLOR_PROCESS ERROR: scene doesn't have BOXM2_GAUSS_RGB data type"<<std::endl;
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
    std::cout<<"====boxm2_ocl_render_color_process::Compiling kernels on "<<identifier<<std::endl;
    std::vector<bocl_kernel*> ks;
    compile_kernel(device,ks,options);
    kernels[identifier]=ks;
  }
  unsigned cl_ni=RoundUp(ni,lthreads[0]);
  unsigned cl_nj=RoundUp(nj,lthreads[1]);

  //create color buffer
  auto* buff = new float[4*cl_ni*cl_nj];
  std::fill(buff, buff + 4*cl_ni*cl_nj, 0.0f);
  bocl_mem_sptr exp_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4),buff,  "exp color image (float4) buffer");
  exp_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // visibility image
  auto* vis_buff = new float[cl_ni*cl_nj];
  std::fill(vis_buff, vis_buff + cl_ni*cl_nj, 1.0f);
  bocl_mem_sptr vis_image = opencl_cache->alloc_mem( cl_ni*cl_nj*sizeof(cl_float), vis_buff, "vis image (single float) buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);


  //image dimensions
  int img_dim_buff[4];
  img_dim_buff[0] = 0;   img_dim_buff[2] = ni;
  img_dim_buff[1] = 0;   img_dim_buff[3] = nj;
  bocl_mem_sptr exp_img_dim=opencl_cache->alloc_mem(sizeof(int)*4,img_dim_buff,  "image dims");
  exp_img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  float tnearfar[2] = { 0.0f, 1000000} ;
  if(cam->type_name() == "vpgl_perspective_camera")
  {

      float f = ((vpgl_perspective_camera<double> *)cam.ptr())->get_calibration().focal_length()*((vpgl_perspective_camera<double> *)cam.ptr())->get_calibration().x_scale();
      std::cout<<"Focal Length " << f<<std::endl;
      tnearfar[0] = f* scene->finest_resolution()/nearfactor ;
      tnearfar[1] = f* scene->finest_resolution()*farfactor ;

      std::cout<<"Near and Far Clipping planes "<<tnearfar[0]<<" "<<tnearfar[1]<<std::endl;
  }
  bocl_mem_sptr tnearfar_mem_ptr = opencl_cache->alloc_mem(2*sizeof(float), tnearfar, "tnearfar  buffer");
  tnearfar_mem_ptr->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  auto* max_omega_buff = new float[cl_ni*cl_nj];
  std::fill(max_omega_buff, max_omega_buff + cl_ni*cl_nj, 0.0f);
  bocl_mem_sptr max_omega_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), max_omega_buff,"max omega image buffer");
  max_omega_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // run expected image function
  render_expected_image(scene, device, opencl_cache, queue,
                        cam, exp_image, vis_image, max_omega_image, exp_img_dim,
                        data_type, kernels[identifier][0], lthreads, cl_ni, cl_nj,apptypesize,tnearfar_mem_ptr);

  // normalize
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
  clFinish(queue);
  auto* exp_img_out = new vil_image_view<vil_rgba<vxl_byte> >(ni,nj);
  int numFloats = 4;
  int count = 0;
  for (unsigned c=0;c<nj;++c) {
    for (unsigned r=0;r<ni;++r,count+=numFloats) {
      buff[count] = buff[count] > 1.0 ? 1.0 : buff[count] ;
      buff[count+1] = buff[count+1] > 1.0 ? 1.0 : buff[count+1] ;
      buff[count+2] = buff[count+2] > 1.0 ? 1.0 : buff[count+2] ;

      (*exp_img_out)(r,c) =
      vil_rgba<vxl_byte> ( (vxl_byte) (buff[count]*255.0f),
                           (vxl_byte) (buff[count+1]*255.0f),
                           (vxl_byte) (buff[count+2]*255.0f),
                           (vxl_byte) 255 );
    }
  }
  auto* vis_img_out=new vil_image_view<float>(ni,nj);
  for (unsigned c=0;c<nj;c++)
    for (unsigned r=0;r<ni;r++)
      (*vis_img_out)(r,c)=vis_buff[c*cl_ni+r];

  opencl_cache->unref_mem(exp_image.ptr());
  opencl_cache->unref_mem(vis_image.ptr());
  opencl_cache->unref_mem(exp_img_dim.ptr());
  opencl_cache->unref_mem(max_omega_image.ptr());
  opencl_cache->unref_mem(tnearfar_mem_ptr.ptr());
  delete [] buff;
  delete [] vis_buff;
  delete [] max_omega_buff;
  clReleaseCommandQueue(queue);


  argIdx=0;
  // store scene smaprt pointer
  pro.set_output_val<vil_image_view_base_sptr>(argIdx++, exp_img_out);
  pro.set_output_val<vil_image_view_base_sptr>(argIdx++, vis_img_out);
  return true;
}
