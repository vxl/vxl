#include <iostream>
#include <algorithm>
#include "boxm2_ocl_render_expected_image.h"
//
#include <vul/vul_timer.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <boxm2/ocl/algo/boxm2_ocl_render_expected_image_function.h>
#include <vsph/vsph_camera_bounds.h>
#include <vgl/vgl_ray_3d.h>
#include <boct/boct_bit_tree.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <brad/brad_image_metadata.h>
#include <brad/brad_atmospheric_parameters.h>
#include <brad/brad_illum_util.h>

//using variables in global namespace
using namespace boxm2_ocl_render_expected_image_globals;


//--------------------------------------------------
//verifies data type for scene
//--------------------------------------------------
bool boxm2_ocl_render_expected_image_globals::validate_appearances(
    const boxm2_scene_sptr& scene,
    std::string& data_type,
    int& appTypeSize,
    std::string& options)
{
  std::vector<std::string> apps = scene->appearances();
  bool foundDataType = false;
  for (const auto & app : apps) {
    if ( app == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
    {
      data_type = app;
      foundDataType = true;
      options="-D MOG_TYPE_8 ";
      appTypeSize = boxm2_data_traits<BOXM2_MOG3_GREY>::datasize();
    }
    else if ( app == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
    {
      data_type = app;
      foundDataType = true;
      options="-D MOG_TYPE_16 ";
      appTypeSize = boxm2_data_traits<BOXM2_MOG3_GREY_16>::datasize();
    }
    else if ( app == boxm2_data_traits<BOXM2_FLOAT8>::prefix() )
    {
      data_type = app;
      foundDataType = true;
      options="-D FLOAT8 ";
      appTypeSize = boxm2_data_traits<BOXM2_FLOAT8>::datasize();
    }
    else if ( app == boxm2_data_traits<BOXM2_LABEL_SHORT>::prefix() )
    {
      data_type = app;
      foundDataType = true;
      options="-D SHORT ";
      appTypeSize = boxm2_data_traits<BOXM2_LABEL_SHORT>::datasize();
    }
  }
  if (!foundDataType) {
    std::cout<<"BOXM2_OCL_RENDER_EXPECTED_IMAGE ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<std::endl;
    return false;
  }
  return true;
}

//declare static map
std::map<std::string, std::vector<bocl_kernel*> > boxm2_ocl_render_expected_image::kernels_;

bool boxm2_ocl_render_expected_image::render(
    vil_image_view<float>&   exp_img_out,
    vil_image_view<float>&   vis_img_out,
    boxm2_scene_sptr         scene,
    bocl_device_sptr         device,
    boxm2_opencl_cache_sptr  opencl_cache,
    vpgl_camera_double_sptr  cam,
    const std::string&               ident,
    unsigned                 ni,
    unsigned                 nj,
    float                    nearfactor,
    float                    farfactor,
    std::size_t               startI,
    std::size_t               startJ)
{
  vul_timer rtime;
  std::size_t lthreads[2]={8,8};


  if(!exp_img_out) {
    exp_img_out.set_size(ni,nj,1);
  }
  if(!vis_img_out) {
    vis_img_out.set_size(ni,nj,1);
  }
  if ( exp_img_out.ni() != ni || exp_img_out.nj() != nj ) {
    std::cout<<"Expected image must have size (" << ni << "," << nj << ")" <<std::endl;
    return false;
  }
  if ( vis_img_out.ni() != ni || vis_img_out.nj() != nj ) {
    std::cout<<"Visibility image must have size (" << ni << "," << nj << ")" <<std::endl;
    return false;
  }

  std::string data_type, options;
  int appTypeSize = 0;
  if (!validate_appearances(scene, data_type, appTypeSize, options))
    return false;
  if (ident.size() > 0) {
    data_type += "_" + ident;
  }

  //: create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0) return false;

  // grab the kernel
  std::cout<<"===========Compiling kernels==========="<<std::endl;
  std::vector<bocl_kernel*>& kernels = get_kernel(device, options);

  unsigned cl_ni=RoundUp(ni,lthreads[0]);
  unsigned cl_nj=RoundUp(nj,lthreads[1]);

  // expected image
  auto* exp_buff = new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++) exp_buff[i]=0.0f;
  bocl_mem_sptr exp_image=opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), exp_buff,"exp image buffer");
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

  float tnearfar[2] = { 0.0f, 1000000 } ;

  if(cam->type_name() == "vpgl_perspective_camera")
  {
      float f  = ((vpgl_perspective_camera<double> *)cam.ptr())->get_calibration().focal_length()*((vpgl_perspective_camera<double> *)cam.ptr())->get_calibration().x_scale();
      std::cout<<"Focal Length " << f<<std::endl;
      tnearfar[0] = f* scene->finest_resolution()/nearfactor ;
      tnearfar[1] = f* scene->finest_resolution()*farfactor ;

      std::cout<<"Near and Far Clipping planes "<<tnearfar[0]<<" "<<tnearfar[1]<<std::endl;
  }
  bocl_mem_sptr tnearfar_mem_ptr = opencl_cache->alloc_mem(2*sizeof(float), tnearfar, "tnearfar  buffer");
  tnearfar_mem_ptr->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);


  // run expected image function
  render_expected_image(scene, device, opencl_cache, queue, cam, exp_image, vis_image, max_omega_image,exp_img_dim,
                        data_type, kernels[0], lthreads, cl_ni, cl_nj, appTypeSize, tnearfar_mem_ptr, startI, startJ);
  // normalize
  if (kernels.size()>1)
  {
    std::size_t gThreads[] = {cl_ni,cl_nj};
    bocl_kernel* normalize_kern = kernels[1];
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
  for (unsigned c=0;c<nj;c++)
    for (unsigned r=0;r<ni;r++)
      exp_img_out(r,c)=exp_buff[c*cl_ni+r];
#else //option to output a byte image (For easier saving)
  vil_image_view<vxl_byte>* exp_img_out=new vil_image_view<vxl_byte>(ni,nj);
    for (unsigned c=0;c<nj;c++)
      for (unsigned r=0;r<ni;r++)
        exp_img_out(r,c)= (vxl_byte) (exp_buff[c*cl_ni+r] * 255.0f);
#endif
  for (unsigned c=0;c<nj;c++)
    for (unsigned r=0;r<ni;r++)
      vis_img_out(r,c)=vis_buff[c*cl_ni+r];

  std::cout<<"Total Render time: "<<rtime.all()<<" ms"<<std::endl;
  delete [] vis_buff;
  delete [] exp_buff;
  delete [] max_omega_buff;
  opencl_cache->unref_mem(vis_image.ptr());
  opencl_cache->unref_mem(exp_image.ptr());
  opencl_cache->unref_mem(max_omega_image.ptr());
  opencl_cache->unref_mem(tnearfar_mem_ptr.ptr());
  clReleaseCommandQueue(queue);

  return true;
}

std::vector<bocl_kernel*>& boxm2_ocl_render_expected_image::get_kernel(
    const bocl_device_sptr& device, const std::string& opts)
{
  // check to see if this device has compiled kernels already
  std::string identifier = device->device_identifier() + opts;
  if (kernels_.find(identifier) != kernels_.end())
      return kernels_[identifier];

  //if not, compile and cache them
  std::cout<<"===========Compiling multi update kernels===========\n"
          <<"  for device: "<<device->device_identifier()<<std::endl;

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


  std::vector<bocl_kernel*> vec_kernels;
  std::size_t found = opts.find("SHORT");
  if (found!=std::string::npos)
  {
    std::cout<<"COMPILING SHORT"<<std::endl;
    std::string options = opts;
    options += "-D RENDER ";
    options += "-D RENDER_MAX -D STEP_CELL=step_cell_render_max(aux_args.mog,aux_args.alpha,data_ptr,d*linfo->block_len,vis,aux_args.expint,aux_args.maxomega)";


    //have kernel construct itself using the context and device
    auto * ray_trace_kernel=new bocl_kernel();
    ray_trace_kernel->create_kernel( &device->context(),
                                     device->device_id(),
                                     src_paths,
                                     "render_bit_scene",   //kernel name
                                     options,              //options
                                     "boxm2 opencl render_bit_scene"); //kernel identifier (for error checking)
    vec_kernels.push_back(ray_trace_kernel);
  }
  else
  {
    std::string options = opts;
    options += "-D RENDER ";

    options += "-D STEP_CELL=step_cell_render(aux_args.mog,aux_args.alpha,data_ptr,d*linfo->block_len,vis,aux_args.expint)";

    //have kernel construct itself using the context and device
    auto * ray_trace_kernel=new bocl_kernel();
    ray_trace_kernel->create_kernel( &device->context(),
                                     device->device_id(),
                                     src_paths,
                                     "render_bit_scene",   //kernel name
                                     options,              //options
                                     "boxm2 opencl render_bit_scene"); //kernel identifier (for error checking)
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
                                            options,              //options
                                            "normalize render kernel"); //kernel identifier (for error checking)

    vec_kernels.push_back(normalize_render_kernel);
  }
  kernels_[identifier] = vec_kernels;
  return kernels_[identifier];
}
