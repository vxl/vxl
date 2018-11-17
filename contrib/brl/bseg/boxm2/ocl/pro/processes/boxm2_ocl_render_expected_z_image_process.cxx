// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_render_expected_z_image_process.cxx
//:
// \file
// \brief  A process for rendering the scene.
//
// \author Daniel Crispell
// \date November 11, 2011

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
#include <vul/vul_timer.h>

#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>

namespace boxm2_ocl_render_expected_z_image_process_globals
{
  constexpr unsigned n_inputs_ = 7;
  constexpr unsigned n_outputs_ = 2;
  std::size_t lthreads[2]={8,8};

  static std::map<std::string,std::vector<bocl_kernel*> > kernels;
  void compile_kernel(const bocl_device_sptr& device,std::vector<bocl_kernel*> & vec_kernels)
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
    src_paths.push_back(source_dir + "bit/render_bit_scene.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //set kernel options
    std::string options = " -D RENDER_Z_IMAGE ";
    options += " -D DETERMINISTIC ";
    options += " -D STEP_CELL=step_cell_render_z(cell_minz*linfo->block_len,aux_args.alpha,data_ptr,d*linfo->block_len,vis,aux_args.exp_z,aux_args.exp_z_sqr,aux_args.probsum)";

    //have kernel construct itself using the context and device
    auto * ray_trace_kernel = new bocl_kernel();

    ray_trace_kernel->create_kernel( &device->context(),
                                     device->device_id(),
                                     src_paths,
                                     "render_z_image",   //kernel name
                                     options,              //options
                                     "boxm2 opencl render z image"); //kernel identifier (for error checking)
    vec_kernels.push_back(ray_trace_kernel);
  }
}

bool boxm2_ocl_render_expected_z_image_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_render_expected_z_image_process_globals;

  //process takes 6 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "unsigned";
  input_types_[5] = "unsigned";
  input_types_[6] = "bool"; // if True, scale z values to [0, 255] and save as vil_image_view<vxl_byte>


  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "vil_image_view_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_render_expected_z_image_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_render_expected_z_image_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  float transfer_time=0.0f;
  float gpu_time=0.0f;

  //get the inputs
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(0);
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(1);

  boxm2_opencl_cache_sptr opencl_cache= pro.get_input<boxm2_opencl_cache_sptr>(2);
  vpgl_camera_double_sptr cam= pro.get_input<vpgl_camera_double_sptr>(3);
  auto ni=pro.get_input<unsigned>(4);
  auto nj=pro.get_input<unsigned>(5);
  bool normalize_z_values = pro.get_input<bool>(6);

  std::string identifier=device->device_identifier();

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),
                                                *(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,
                                                &status);
  if (status!=0)
    return false;

  // compile the kernel
  if (kernels.find(identifier)==kernels.end())
  {
    std::cout<<"===========Compiling kernels==========="<<std::endl;
    std::vector<bocl_kernel*> ks;
    compile_kernel(device,ks);
    kernels[identifier]=ks;
  }

#if 0
  // create all buffers
  cl_float cam_buffer[48];
  boxm2_ocl_util::set_persp_camera(cam, cam_buffer);
  bocl_mem_sptr persp_cam=new bocl_mem(device->context(), cam_buffer, 3*sizeof(cl_float16), "persp cam buffer");
  persp_cam->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
#endif

  std::size_t local_threads[2]={8,8};
  unsigned cl_ni=RoundUp(ni,local_threads[0]);
  unsigned cl_nj=RoundUp(nj,local_threads[1]);

  auto* buff = new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++) buff[i]=0.0f;
  auto* var_buff = new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++) var_buff[i]=0.0f;
  auto* vis_buff = new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++) vis_buff[i]=1.0f;
  auto* prob_buff = new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++) prob_buff[i]=0.0f;

  bocl_mem_sptr exp_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), buff, "exp z image buffer");
  exp_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr var_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), var_buff, "z var image buffer");
  var_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr vis_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), vis_buff, "vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr prob_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), prob_buff, "vis x omega image buffer");
  prob_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //set generic cam
  auto* ray_origins = new cl_float[4*cl_ni*cl_nj];
  auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
  bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins, "ray_origins buffer");
  bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions, "ray_directions buffer");
  boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

  // Image Dimensions
  int img_dim_buff[4];
  img_dim_buff[0] = 0;
  img_dim_buff[1] = 0;
  img_dim_buff[2] = ni;
  img_dim_buff[3] = nj;
  bocl_mem_sptr exp_img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(int)*4, "image dims");
  exp_img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

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

  // set arguments
  std::vector<boxm2_block_id> vis_order = scene->get_vis_blocks(cam);
  std::vector<boxm2_block_id>::iterator id;
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
    //choose correct render kernel
    boxm2_block_metadata mdata = scene->get_block_metadata(*id);
    bocl_kernel* kern =  kernels[identifier][0];

    //write the image values to the buffer
    vul_timer transfer;
    bocl_mem* blk = opencl_cache->get_block(scene,*id);
    bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene,*id);
    bocl_mem * blk_info = opencl_cache->loaded_block_info();
    transfer_time          += (float) transfer.all();

    ////3. SET args
    kern->set_arg( blk_info );
    kern->set_arg( blk );
    kern->set_arg( alpha );
    kern->set_arg( ray_o_buff.ptr() );
    kern->set_arg( ray_d_buff.ptr() );
    kern->set_arg( exp_image.ptr() );
    kern->set_arg( var_image.ptr() );
    kern->set_arg( exp_img_dim.ptr());
    kern->set_arg( cl_output.ptr() );
    kern->set_arg( lookup.ptr() );
    kern->set_arg( vis_image.ptr() );
    kern->set_arg( prob_image.ptr() );

    //local tree , cumsum buffer, imindex buffer
    kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );
    kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) );
    kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_int) );

    //execute kernel
    kern->execute(queue, 2, lThreads, gThreads);
    clFinish(queue);
    gpu_time += kern->exec_time();

    cl_output->read_to_buffer(queue);

    // clear render kernel args so it can reset em on next execution
    kern->clear_args();
  }
#if 0
  // normalize
  {
    bocl_kernel* normalize_kern= kernels[identifier][1];
    normalize_kern->set_arg( exp_image.ptr() );
    normalize_kern->set_arg( var_image.ptr() );
    normalize_kern->set_arg( prob_image.ptr() );
    normalize_kern->set_arg( exp_img_dim.ptr());
    normalize_kern->execute( queue, 2, local_threads, gThreads);
    clFinish(queue);
    gpu_time += normalize_kern->exec_time();

    //clear render kernel args so it can reset em on next execution
    normalize_kern->clear_args();
    exp_image->read_to_buffer(queue);
    var_image->read_to_buffer(queue);
    vis_image->read_to_buffer(queue);
  }
#else
  exp_image->read_to_buffer(queue);
  var_image->read_to_buffer(queue);
  vis_image->read_to_buffer(queue);
#endif

  opencl_cache->unref_mem(ray_o_buff.ptr());
  opencl_cache->unref_mem(ray_d_buff.ptr());
  opencl_cache->unref_mem(exp_image.ptr());
  opencl_cache->unref_mem(vis_image.ptr());
  opencl_cache->unref_mem(var_image.ptr());
  opencl_cache->unref_mem(prob_image.ptr());


  clReleaseCommandQueue(queue);


  auto* z_image = new vil_image_view<float>(ni,nj);
  auto* mask = new vil_image_view<vxl_byte>(ni,nj);
  vil_image_view<float> vis_vil_image(ni,nj);

  float max_z = -vnl_numeric_traits<float>::maxval;
  float min_z = vnl_numeric_traits<float>::maxval;

  std::vector<float> zval_vector; // for sorting and normalization

  for (unsigned c=0;c<nj;c++)
    for (unsigned r=0;r<ni;r++)
    {
      float vis = vis_buff[c*cl_ni+r];
      vis_vil_image(r,c) = vis;
      float z = 0.0f;
      bool good_z = false;
      if (vis < 0.95) {
        z = buff[c*cl_ni+r] / (1.0f - vis);
        good_z = true;
        zval_vector.push_back(z);
        max_z = std::max(max_z,z);
        min_z = std::min(min_z,z);
      }
      (*z_image)(r,c) = z;
      (*mask)(r,c) = good_z ? vxl_byte(255) : vxl_byte(0);
    }
    std::cout << " min_z = " << min_z << "  max_z = " << max_z << std::endl;
  if (normalize_z_values) {
    std::sort(zval_vector.begin(),zval_vector.end());
    unsigned int ngood = zval_vector.size();
    float min_z = zval_vector[(ngood-1)*0.01];
    float max_z = zval_vector[(ngood-1)*0.99];
    std::cout << " min_z (pcent) = " << min_z << "  max_z (pcent) = " << max_z << std::endl;
    double scale = 255.0 / (max_z - min_z);
    double offset = -scale * min_z;

    auto* z_image_byte = new vil_image_view<vxl_byte>(ni,nj);
    for (unsigned int c=0; c<nj; ++c) {
      for (unsigned int r=0; r<ni; ++r) {
        if ((*mask)(r,c)) {
          float zval = (*z_image)(r,c);
          float zval_byte = std::max(0.0f,std::min(255.0f,float(zval*scale + offset)));
          (*z_image_byte)(r,c) = vxl_byte(zval_byte);
        }
        else {
          (*z_image_byte)(r,c) = vxl_byte(0);
        }
      }
    }
    pro.set_output_val<vil_image_view_base_sptr>(0,z_image_byte);
  }
  else {
    // store scene smart pointer
    pro.set_output_val<vil_image_view_base_sptr>(0, z_image);
  }

  pro.set_output_val<vil_image_view_base_sptr>(1, mask);
  return true;
}
