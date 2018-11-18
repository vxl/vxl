#include "boxm2_ocl_render_expected_shadow_map.h"
#include <algorithm>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <iostream>
#include <utility>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <boxm2/ocl/algo/boxm2_ocl_render_expected_image_function.h>

std::map<std::string,std::vector<bocl_kernel*> > boxm2_ocl_render_expected_shadow_map::kernels_;

bool boxm2_ocl_render_expected_shadow_map::render(bocl_device_sptr device,
                                                  boxm2_scene_sptr scene,
                                                  boxm2_opencl_cache_sptr opencl_cache,
                                                  vpgl_camera_double_sptr cam,
                                                  unsigned ni, unsigned nj,
                                                  std::string ident,
                                                  vil_image_view<float> &rendered_img)
{
  std::size_t lthreads[2]={8,8};

  //: create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0) return false;
  std::string identifier=device->device_identifier();

  // compile the kernel
  if (kernels_.find(identifier)==kernels_.end())
  {
    std::cout<<"===========Compiling kernels==========="<<std::endl;
    std::vector<bocl_kernel*> ks;
    compile_kernel(device,ks,"");
    kernels_[identifier]=ks;
  }

  unsigned cl_ni=RoundUp(ni,lthreads[0]);
  unsigned cl_nj=RoundUp(nj,lthreads[1]);
  auto* buff = new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++) buff[i]=0.0f;

  bocl_mem_sptr exp_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), buff,"exp image buffer");
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

  // run expected image function
  render_expected_shadow_map(scene, device, opencl_cache, queue,
                             cam, exp_image, vis_image, exp_img_dim,
                             std::move(ident), kernels_[identifier][0], lthreads, cl_ni, cl_nj);

  // normalize
  //{
  //  std::size_t gThreads[] = {cl_ni,cl_nj};
  //  bocl_kernel* normalize_kern = kernels[identifier][1];
  //  normalize_kern->set_arg( exp_image.ptr() );
  //  normalize_kern->set_arg( vis_image.ptr() );
  //  normalize_kern->set_arg( exp_img_dim.ptr());
  //  normalize_kern->execute( queue, 2, lthreads, gThreads);
  //  clFinish(queue);

  //  //clear render kernel args so it can reset em on next execution
  //  normalize_kern->clear_args();
  //}

  // read out expected image
  exp_image->read_to_buffer(queue);
  vis_image->read_to_buffer(queue);
  rendered_img.set_size(ni, nj);
  for (unsigned c=0;c<nj;c++)
    for (unsigned r=0;r<ni;r++) {
      rendered_img(r,c)=buff[c*cl_ni+r];
  }

  opencl_cache->unref_mem(exp_image.ptr());
  opencl_cache->unref_mem(vis_image.ptr());

  delete [] buff;
  delete [] vis_buff;

  clReleaseCommandQueue(queue);

  return true;
}


void boxm2_ocl_render_expected_shadow_map::compile_kernel(const bocl_device_sptr& device,std::vector<bocl_kernel*> & vec_kernels, const std::string& opts)
{
  //gather all render sources... seems like a lot for rendering...
  std::vector<std::string> src_paths;
  std::string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "pixel_conversion.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "backproject.cl");
  src_paths.push_back(source_dir + "statistics_library_functions.cl");
  src_paths.push_back(source_dir + "expected_functor.cl");
  src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
  src_paths.push_back(source_dir + "bit/render_sun_visibilities.cl");
  src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

  //set kernel options
  //#define STEP_CELL step_cell_render(mixture_array, alpha_array, data_ptr, d, &vis, &expected_int);
  std::string options = opts + " -D RENDER_SUN_VIS";
  options += " -D STEP_CELL=step_cell_render_sun_vis(aux_args.auxsun,aux_args.alpha,data_ptr,d*linfo->block_len,vis,aux_args.expint)";

  //have kernel construct itself using the context and device
  auto * ray_trace_kernel=new bocl_kernel();

  ray_trace_kernel->create_kernel( &device->context(),
                                   device->device_id(),
                                   src_paths,
                                   "render_sun_vis_scene",   //kernel name
                                   options,              //options
                                   "boxm2 opencl render random blocks"); //kernel identifier (for error checking)
  vec_kernels.push_back(ray_trace_kernel);
}
