// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_render_expected_height_map_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for rendering height map of a scene.
//
// \author Vishal Jain
// \date Mar 30, 2011

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
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace boxm2_ocl_render_expected_height_map_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 5;
  std::size_t local_threads[2]={8,8};
  static std::map<std::string, std::vector<bocl_kernel*> > kernels_;
  std::vector<bocl_kernel*>& get_kernels(const bocl_device_sptr& device, const std::string& opts)
  {
      std::string identifier = device->device_identifier() + opts;
      if (kernels_.find(identifier) != kernels_.end())
          return kernels_[identifier];
      std::vector<bocl_kernel*> vec_kernels;
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
      std::string options = "-D RENDER_DEPTH -D DETERMINISTIC -D STEP_CELL=step_cell_render_height(tblock,linfo->block_len,aux_args.alpha,data_ptr,d*linfo->block_len,aux_args.vis,aux_args.expdepth,aux_args.expdepthsqr,aux_args.probsum,aux_args.t)";
      //create normalize image kernel
      std::vector<std::string> norm_src_paths;
      norm_src_paths.push_back(source_dir + "scene_info.cl");

      norm_src_paths.push_back(source_dir + "pixel_conversion.cl");
      norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
      auto * normalize_render_kernel = new bocl_kernel();

      normalize_render_kernel->create_kernel(&device->context(),
          device->device_id(),
          norm_src_paths,
          "normalize_render_depth_kernel",   //kernel name
          options,              //options
          "normalize render depth kernel"); //kernel identifier (for error checking)
      //have kernel construct itself using the context and device
      auto * ray_trace_kernel = new bocl_kernel();

      ray_trace_kernel->create_kernel(&device->context(),
                                       device->device_id(),
                                       src_paths,
                                       "render_depth",   //kernel name
                                       options,              //options
                                       "boxm2 opencl render depth image"); //kernel identifier (for error checking)
      vec_kernels.push_back(ray_trace_kernel);




      vec_kernels.push_back(normalize_render_kernel);
      kernels_[identifier] = vec_kernels;

      return kernels_[identifier];
  }

}

bool boxm2_ocl_render_expected_height_map_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_render_expected_height_map_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";

  // process has 1 output:
  // output[0]: scene sptr
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "vil_image_view_base_sptr";
  output_types_[2] = "vil_image_view_base_sptr";
  output_types_[3] = "vil_image_view_base_sptr";
  output_types_[4] = "vil_image_view_base_sptr";
  //output_types_[5] = "vil_image_view_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_render_expected_height_map_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_render_expected_height_map_process_globals;


  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  float transfer_time=0.0f;
  float gpu_time=0.0f;
  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device= pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr opencl_cache= pro.get_input<boxm2_opencl_cache_sptr>(i++);
  vgl_box_3d<double> bbox=scene->bounding_box();


  //get x and y size from scene
  std::vector<boxm2_block_id> vis_order = scene->get_block_ids();
  std::vector<boxm2_block_id>::iterator id;
  float xint=0.0f;
  float yint=0.0f;
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
    boxm2_block_metadata mdata=scene->get_block_metadata(*id);
    float num_octree_cells=std::pow(2.0f,(float)mdata.max_level_-1);
    xint=mdata.sub_block_dim_.x()/num_octree_cells;
    yint=mdata.sub_block_dim_.y()/num_octree_cells;
  }
  auto ni=(unsigned int)std::ceil(bbox.width()/xint);
  auto nj=(unsigned int)std::ceil(bbox.height()/yint);
  std::cout<<"Size of the image "<<ni<<','<<nj<<std::endl;
  float z= bbox.max_z();
  std::string identifier=device->device_identifier();

  //: create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0)return false;

  std::vector<bocl_kernel*>& kernels = get_kernels(device, "");

  float scene_origin[4];
  scene_origin[0]=bbox.min_x();
  scene_origin[1]=bbox.min_y();
  scene_origin[2]=bbox.min_z();
  scene_origin[3]=1.0;
  unsigned cl_ni=RoundUp(ni,local_threads[0]);
  unsigned cl_nj=RoundUp(nj,local_threads[1]);

  auto* ray_origins = new cl_float[4 * cl_ni*cl_nj];
  auto* ray_directions = new cl_float[4 * cl_ni*cl_nj];

  float ray_dx = 0, ray_dy = 0, ray_dz = -1;
  // initialize ray origin buffer, first and last return buffers
  int count = 0;
  for (unsigned int j = 0; j<cl_nj; ++j) {
      for (unsigned int i = 0; i<cl_ni; ++i) {

          int count4 = count * 4;
          ray_origins[count4 + 0] = scene_origin[0] + ((float)i + 0.15f)*(xint);
          ray_origins[count4 + 1] = scene_origin[1] + ((float)j + 0.15f)*(yint);

          ray_origins[count4 + 2] = z + 1.0f;
          ray_origins[count4 + 3] = 0.0;
          ray_directions[count4 + 0] = 0.0;
          ray_directions[count4 + 1] = 0.0;
          ray_directions[count4 + 2] = -1.0;
          ray_directions[count4 + 3] = 0.0;

          ++count;
      }
  }
  bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj * sizeof(cl_float4), ray_origins, "ray_origins buffer");
  bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj * sizeof(cl_float4), ray_directions, "ray_directions buffer");

  ray_o_buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  ray_d_buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  auto* buff = new float[cl_ni*cl_nj];
  for (unsigned i = 0; i<cl_ni*cl_nj; i++) buff[i] = 0.0f;
  auto* var_buff = new float[cl_ni*cl_nj];
  for (unsigned i = 0; i<cl_ni*cl_nj; i++) var_buff[i] = 0.0f;
  auto* vis_buff = new float[cl_ni*cl_nj];
  for (unsigned i = 0; i<cl_ni*cl_nj; i++) vis_buff[i] = 1.0f;
  auto* prob_buff = new float[cl_ni*cl_nj];
  for (unsigned i = 0; i<cl_ni*cl_nj; i++) prob_buff[i] = 0.0f;
  auto* t_infinity_buff = new float[cl_ni*cl_nj];
  for (unsigned i = 0; i<cl_ni*cl_nj; i++) t_infinity_buff[i] = 0.0f;

  bocl_mem_sptr exp_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), buff, "exp image buffer");
  exp_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr var_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), var_buff, "var image buffer");
  var_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr vis_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), vis_buff, "vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr prob_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), prob_buff, "vis x omega image buffer");
  prob_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr t_infinity = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), t_infinity_buff, "t infinity buffer");
  t_infinity->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // Image Dimensions
  int img_dim_buff[4];
  img_dim_buff[0] = 0;
  img_dim_buff[1] = 0;
  img_dim_buff[2] = ni;
  img_dim_buff[3] = nj;
  bocl_mem_sptr exp_img_dim = new bocl_mem(device->context(), img_dim_buff, sizeof(int) * 4, "image dims");
  exp_img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // Output Array
  float output_arr[100];
  for (float & i : output_arr) i = 0.0f;
  bocl_mem_sptr  cl_output = new bocl_mem(device->context(), output_arr, sizeof(float) * 100, "output buffer");
  cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup = new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar) * 256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //2. set workgroup size
  std::size_t lThreads[] = {8, 8};
  std::size_t gThreads[] = {cl_ni,cl_nj};
  float subblk_dim = 0.0;
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
      //choose correct render kernel
      boxm2_block_metadata mdata = scene->get_block_metadata(*id);
      bocl_kernel* kern = kernels[0];

      //write the image values to the buffer
      vul_timer transfer;
      bocl_mem* blk = opencl_cache->get_block(scene, *id);
      bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene, *id);
      bocl_mem * blk_info = opencl_cache->loaded_block_info();
      transfer_time += (float)transfer.all();
      subblk_dim = mdata.sub_block_dim_.x();
      ////3. SET args
      kern->set_arg(blk_info);
      kern->set_arg(blk);
      kern->set_arg(alpha);
      kern->set_arg(ray_o_buff.ptr());
      kern->set_arg(ray_d_buff.ptr());
      kern->set_arg(exp_image.ptr());
      kern->set_arg(var_image.ptr());
      kern->set_arg(exp_img_dim.ptr());
      kern->set_arg(cl_output.ptr());
      kern->set_arg(lookup.ptr());
      kern->set_arg(vis_image.ptr());
      kern->set_arg(prob_image.ptr());
      kern->set_arg(t_infinity.ptr());
      kern->set_local_arg(local_threads[0] * local_threads[1] * sizeof(cl_uchar16));
      kern->set_local_arg(local_threads[0] * local_threads[1] * 10 * sizeof(cl_uchar));
      kern->set_local_arg(local_threads[0] * local_threads[1] * sizeof(cl_int));

      //execute kernel
      kern->execute(queue, 2, lThreads, gThreads);
      clFinish(queue);
      gpu_time += kern->exec_time();
      cl_output->read_to_buffer(queue);

      // clear render kernel args so it can reset em on next execution
      kern->clear_args();
  }

  bocl_mem_sptr  subblk_dim_mem = new bocl_mem(device->context(), &(subblk_dim), sizeof(float), "sub block dim buffer");
  subblk_dim_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  // normalize
  {
      bocl_kernel* normalize_kern = kernels[1];
      normalize_kern->set_arg(exp_image.ptr());
      normalize_kern->set_arg(var_image.ptr());
      normalize_kern->set_arg(vis_image.ptr());
      normalize_kern->set_arg(exp_img_dim.ptr());
      normalize_kern->set_arg(t_infinity.ptr());
      normalize_kern->set_arg(subblk_dim_mem.ptr());
      normalize_kern->execute(queue, 2, local_threads, gThreads);
      clFinish(queue);
      gpu_time += normalize_kern->exec_time();
      //clear render kernel args so it can reset em on next execution
      normalize_kern->clear_args();
      exp_image->read_to_buffer(queue);
      var_image->read_to_buffer(queue);
      vis_image->read_to_buffer(queue);
  }
  clReleaseCommandQueue(queue);

  i=0;
  auto* exp_img_out=new vil_image_view<float>(ni,nj);
  auto* exp_var_out=new vil_image_view<float>(ni,nj);
  auto* xcoord_img=new vil_image_view<float>(ni,nj);
  auto* ycoord_img=new vil_image_view<float>(ni,nj);
  auto* prob_img=new vil_image_view<float>(ni,nj);

  for (unsigned c=0;c<nj;++c)
    for (unsigned r=0;r<ni;++r)
    {
      (*exp_img_out)(r,c) = /*z-*/buff[c*cl_ni+r];
      (*exp_var_out)(r,c) = var_buff[c*cl_ni+r];
      (*xcoord_img)(r, c) = ray_origins[(c*cl_ni + r) * 4 + 0];
      (*ycoord_img)(r, c) = ray_origins[(c*cl_ni + r) * 4 + 1];
      (*prob_img)(r,c) = prob_buff[c*cl_ni+r];
    }
  // store scene smaprt pointer
  pro.set_output_val<vil_image_view_base_sptr>(i++, exp_img_out);
  pro.set_output_val<vil_image_view_base_sptr>(i++, exp_var_out);
  pro.set_output_val<vil_image_view_base_sptr>(i++, xcoord_img);
  pro.set_output_val<vil_image_view_base_sptr>(i++, ycoord_img);
  pro.set_output_val<vil_image_view_base_sptr>(i++, prob_img);
  delete[] buff;
  delete[] var_buff;
  delete[] vis_buff;
  delete[] prob_buff;
  delete[] t_infinity_buff;
  //delete[] app_buff;
  opencl_cache->unref_mem(exp_image.ptr());
  opencl_cache->unref_mem(var_image.ptr());
  opencl_cache->unref_mem(vis_image.ptr());
  opencl_cache->unref_mem(prob_image.ptr());
  opencl_cache->unref_mem(t_infinity.ptr());
  opencl_cache->unref_mem(ray_o_buff.ptr());
  opencl_cache->unref_mem(ray_d_buff.ptr());
  return true;
}
