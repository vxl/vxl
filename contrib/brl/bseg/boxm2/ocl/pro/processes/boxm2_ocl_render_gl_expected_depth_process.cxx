// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_render_gl_expected_depth_process.cxx
//:
// \file
// \brief  A process for rendering a depth map of the scene.
//
// \author Daniel Crispell
// \date 16 Sep 2014

#include <fstream>
#include <iostream>
#include <algorithm>
#include <bprb/bprb_func_process.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_util.h>
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
//#include <boxm2/ocl/algo/boxm2_ocl_render_expected_depth_function.h>


namespace boxm2_ocl_render_gl_expected_depth_process_globals
{
  constexpr unsigned n_inputs_ = 10;
  constexpr unsigned n_outputs_ = 1;
  std::size_t lthreads[2]={8,8};

  static std::map<std::string,std::vector<bocl_kernel*> > kernels;

  void compile_kernel(const bocl_device_sptr& device,std::vector<bocl_kernel*> & vec_kernels, const std::string&  /*opts*/="")
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
    auto * ray_trace_kernel=new bocl_kernel();

    ray_trace_kernel->create_kernel( &device->context(),
                                     device->device_id(),
                                     src_paths,
                                     "render_depth",   //kernel name
                                     options,              //options
                                     "boxm2 opencl render depth image"); //kernel identifier (for error checking)
    vec_kernels.push_back(ray_trace_kernel);

    //create normalize image kernel
    std::vector<std::string> norm_src_paths;
    norm_src_paths.push_back(source_dir + "scene_info.cl");

    norm_src_paths.push_back(source_dir + "pixel_conversion.cl");
    norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
    auto * normalize_render_kernel=new bocl_kernel();

    normalize_render_kernel->create_kernel( &device->context(),
                                            device->device_id(),
                                            norm_src_paths,
                                            "normalize_depth_render_kernel_gl",   //kernel name
                                            "-D NORMALIZE_RENDER_GL",              //options
                                            "normalize depth render kernel"); //kernel identifier (for error checking)


    vec_kernels.push_back(normalize_render_kernel);
  }
}

bool boxm2_ocl_render_gl_expected_depth_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_render_gl_expected_depth_process_globals;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "unsigned";
  input_types_[5] = "unsigned";
  input_types_[6] = "bocl_mem_sptr"; // exp image buffer;
  input_types_[7] = "bocl_mem_sptr"; // exp image dimensions buffer;
  input_types_[8] = "float"; // depth scale
  input_types_[9] = "float"; // depth offset


  // process has 1 output:
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "float";

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  brdb_value_sptr idx = new brdb_value_t<std::string>("");

  return good;
}

bool boxm2_ocl_render_gl_expected_depth_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_render_gl_expected_depth_process_globals;

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
  bocl_mem_sptr exp_image =pro.get_input<bocl_mem_sptr>(i++);
  bocl_mem_sptr exp_img_dim =pro.get_input<bocl_mem_sptr>(i++);

  auto depth_scale = pro.get_input<float>(i++);
  auto depth_offset = pro.get_input<float>(i++);

  //: create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0) return false;
  std::string identifier=device->device_identifier();

  // compile the kernel
  if (kernels.find(identifier)==kernels.end())
  {
    std::cout<<"===========Compiling kernels==========="<<std::endl;
    std::vector<bocl_kernel*> ks;
    compile_kernel(device,ks);
    kernels[identifier]=ks;
  }

  unsigned cl_ni=RoundUp(ni,lthreads[0]);
  unsigned cl_nj=RoundUp(nj,lthreads[1]);

  // visibility image
  auto* vis_buff = new float[cl_ni*cl_nj];
  std::fill(vis_buff, vis_buff + cl_ni*cl_nj, 1.0f);
  bocl_mem_sptr vis_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float),vis_buff,  "vis image (single float) buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    float tnearfar[2] = { 0.0f, 1000000} ;
  bocl_mem_sptr tnearfar_mem_ptr = opencl_cache->alloc_mem(2*sizeof(float), tnearfar, "tnearfar  buffer");
  tnearfar_mem_ptr->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  auto* buff = new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++) buff[i]=0.0f;
  auto* var_buff = new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++) var_buff[i]=0.0f;
  auto* prob_buff = new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++) prob_buff[i]=0.0f;
  auto* t_infinity_buff = new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++) t_infinity_buff[i]=0.0f;

  bocl_mem_sptr var_image=opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float),var_buff,"var image buffer");
  var_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr prob_image=opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float),prob_buff,"vis x omega image buffer");
  prob_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr t_infinity=opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float),t_infinity_buff,"t infinity buffer");
  t_infinity->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //set generic cam
  auto* ray_origins = new cl_float[4*cl_ni*cl_nj];
  auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
  bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj * sizeof(cl_float4), ray_origins, "ray_origins buffer");
  bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj * sizeof(cl_float4), ray_directions, "ray_directions buffer");
  boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

  // Output Array
  float output_arr[100];
  for (float & i : output_arr) i = 0.0f;
  bocl_mem_sptr  cl_output=new bocl_mem(device->context(), output_arr, sizeof(float)*100, "output buffer");
  cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //2. set workgroup size
  std::size_t lThreads[] = {8, 8};
  std::size_t gThreads[] = {cl_ni,cl_nj};
  float subblk_dim = 0.0;
  // set arguments
  std::vector<boxm2_block_id> vis_order;
  if(cam->type_name() == "vpgl_geo_camera" )
      vis_order= scene->get_block_ids(); // order does not matter for a top down orthographic camera  and axis aligned blocks
  else if(cam->type_name() == "vpgl_perspective_camera")
      vis_order= scene->get_vis_blocks_opt((vpgl_perspective_camera<double>*)cam.ptr(),ni,nj);
  else
      vis_order= scene->get_vis_blocks(cam);
  std::vector<boxm2_block_id>::iterator id;
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
    //choose correct render kernel
    boxm2_block_metadata mdata = scene->get_block_metadata(*id);
    bocl_kernel* kern =  kernels[identifier][0];

    // write the image values to the buffer
    bocl_mem* blk = opencl_cache->get_block(scene,*id);
    bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene,*id);
    bocl_mem * blk_info = opencl_cache->loaded_block_info();
    subblk_dim = mdata.sub_block_dim_.x();
    ////3. SET args
    kern->set_arg( blk_info );
    kern->set_arg( blk );
    kern->set_arg( alpha );
    //kern->set_arg( persp_cam.ptr() );
    kern->set_arg( ray_o_buff.ptr() );
    kern->set_arg( ray_d_buff.ptr() );
    kern->set_arg( exp_image.ptr() );
    kern->set_arg( var_image.ptr() );
    kern->set_arg( exp_img_dim.ptr());
    kern->set_arg( cl_output.ptr() );
    kern->set_arg( lookup.ptr() );
    kern->set_arg( vis_image.ptr() );
    kern->set_arg( prob_image.ptr() );
    kern->set_arg( t_infinity.ptr() );

    //local tree , cumsum buffer, imindex buffer
    kern->set_local_arg( lthreads[0]*lthreads[1]*sizeof(cl_uchar16) );
    kern->set_local_arg( lthreads[0]*lthreads[1]*10*sizeof(cl_uchar) );
    kern->set_local_arg( lthreads[0]*lthreads[1]*sizeof(cl_int) );

    //execute kernel
    kern->execute(queue, 2, lThreads, gThreads);
    clFinish(queue);

    cl_output->read_to_buffer(queue);

    // clear render kernel args so it can reset em on next execution
    kern->clear_args();
  }

  bocl_mem_sptr  subblk_dim_mem=new bocl_mem(device->context(), &(subblk_dim), sizeof(float), "sub block dim buffer");
  subblk_dim_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
  // normalize
  {
    cl_float depth_scale_offset_buff[2] = { depth_scale, depth_offset};

    bocl_mem_sptr  depth_scale_offset = new bocl_mem(device->context(), depth_scale_offset_buff, sizeof(float)*2, "depth scale and offset buffer");
    depth_scale_offset->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    bocl_kernel* normalize_kern= kernels[identifier][1];

    normalize_kern->set_arg( exp_image.ptr() );
    normalize_kern->set_arg( vis_image.ptr() );
    normalize_kern->set_arg( depth_scale_offset.ptr() );
    normalize_kern->set_arg( exp_img_dim.ptr());

    normalize_kern->execute( queue, 2, lthreads, gThreads);
    clFinish(queue);

    //clear render kernel args so it can reset em on next execution
    normalize_kern->clear_args();
  }
  exp_image->read_to_buffer(queue);
  var_image->read_to_buffer(queue);
  vis_image->read_to_buffer(queue);


  clReleaseCommandQueue(queue);

#if 0
  vil_image_view<float>* exp_img_out=new vil_image_view<float>(ni,nj);
  vil_image_view<float>* exp_var_out=new vil_image_view<float>(ni,nj);
  vil_image_view<float>* vis_out=new vil_image_view<float>(ni,nj);

  for (unsigned c=0;c<nj;c++)
  {
    for (unsigned r=0;r<ni;r++)
    {
      (*exp_img_out)(r,c)=buff[c*cl_ni+r];
      (*exp_var_out)(r,c)=var_buff[c*cl_ni+r];
      (*vis_out)(r,c)=vis_buff[c*cl_ni+r];
    }
  }
#endif

  delete[] buff;
  delete[] var_buff;
  delete[] vis_buff;
  delete[] prob_buff;
  delete[] t_infinity_buff;
  delete[] ray_origins;
  delete[] ray_directions;

  opencl_cache->unref_mem(exp_image.ptr());
  opencl_cache->unref_mem(var_image.ptr());
  opencl_cache->unref_mem(vis_image.ptr());
  opencl_cache->unref_mem(prob_image.ptr());
  opencl_cache->unref_mem(t_infinity.ptr());
  opencl_cache->unref_mem(ray_o_buff.ptr());
  opencl_cache->unref_mem(ray_d_buff.ptr());
  clReleaseCommandQueue(queue);

  opencl_cache->unref_mem(tnearfar_mem_ptr.ptr());

  //store render time
  int argIdx = 0;
  pro.set_output_val<float>(argIdx, 1.0);
  return true;
}
