// This is brl/bseg/bstm/ocl/pro/processes/bstm_ocl_get_surface_pt_process.cxx
//:
// \file
// \brief  A process to help localize surface points in the 4d world. Given a camera and pixel location, the process shoots a ray
//         and traverses the volume until it hits a voxel with a prob > prob_t. It returns the location of this voxel so it can be
//         queried later on.
//
// \author Ali Osman Ulusoy
// \date Jan 30, 2013

#include <fstream>
#include <iostream>
#include <algorithm>
#include <bprb/bprb_func_process.h>

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
#include <vul/vul_timer.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <vsph/vsph_camera_bounds.h>
#include <vgl/vgl_ray_3d.h>
#include <boct/boct_bit_tree.h>


namespace bstm_ocl_get_surface_pt_process_globals
{
  constexpr unsigned n_inputs_ = 10;
  constexpr unsigned n_outputs_ = 3;
  std::size_t lthreads[2]={8,8};

  static std::map<std::string,std::vector<bocl_kernel*> > kernels;

  void compile_kernel(const bocl_device_sptr& device,std::vector<bocl_kernel*> & vec_kernels, const std::string& opts)
  {
    //gather all render sources... seems like a lot for rendering...
    std::vector<std::string> src_paths;
    std::string source_dir = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/bstm/ocl/cl/";
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "pixel_conversion.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "bit/time_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/compute_surface_pt.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //set kernel options
    std::string options = opts + "-D SURFACE_PT ";
    options += " -D STEP_CELL=step_cell_surface_pt(aux_args,data_ptr_tt,d*linfo->block_len,posx*linfo->block_len+linfo->origin.x,posy*linfo->block_len+linfo->origin.y,posz*linfo->block_len+linfo->origin.z)";

    //have kernel construct itself using the context and device
    auto * ray_trace_kernel=new bocl_kernel();

    std::cout << "Compiling with options: " << options << std::endl;
    ray_trace_kernel->create_kernel( &device->context(),
                                     device->device_id(),
                                     src_paths,
                                     "compute_surface_pt",   //kernel name
                                     options,              //options
                                     "bstm compute_surface_pt"); //kernel identifier (for error checking)
    vec_kernels.push_back(ray_trace_kernel);


  }
}

bool bstm_ocl_get_surface_pt_process_cons(bprb_func_process& pro)
{
  using namespace bstm_ocl_get_surface_pt_process_globals;

  //process takes 1 input
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "bstm_scene_sptr";
  input_types_[2] = "bstm_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "unsigned"; //ni
  input_types_[5] = "unsigned"; //nj
  input_types_[6] = "unsigned"; //pixel_x
  input_types_[7] = "unsigned"; //pixel_y
  input_types_[8] = "float"; // time
  input_types_[9] = "float"; // prob threshold

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "float";
  output_types_[1] = "float";
  output_types_[2] = "float";
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

}

bool bstm_ocl_get_surface_pt_process(bprb_func_process& pro)
{
  using namespace bstm_ocl_get_surface_pt_process_globals;

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
  auto pixel_x=pro.get_input<unsigned>(i++);
  auto pixel_y=pro.get_input<unsigned>(i++);
  auto time = pro.get_input<float>(i++);
  auto prob_t = pro.get_input<float>(i++);


  //: create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()), CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0) return false;
  std::string options = "";
  std::string identifier=device->device_identifier()+options;
  if (kernels.find(identifier)==kernels.end())  // compile the kernel
  {
    std::cout<<"===========Compiling kernels==========="<<std::endl;
    std::vector<bocl_kernel*> ks;
    compile_kernel(device,ks,options);
    kernels[identifier]=ks;
  }



  //start ray tracing
  float transfer_time=0.0f;
  float gpu_time=0.0f;

  //camera check
  if (cam->type_name()!= "vpgl_perspective_camera" && cam->type_name()!= "vpgl_generic_camera" ) {
    std::cout<<"Cannot render with camera of type "<<cam->type_name()<<std::endl;
    return 0.0f;
  }

  // create all buffers
  unsigned cl_ni=RoundUp(ni,8);
  unsigned cl_nj=RoundUp(nj,8);
  auto* ray_origins = new cl_float[4*cl_ni*cl_nj];
  auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
  bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins, "ray_origins buffer");
  bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions, "ray_directions buffer");
  boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

  // Output Array
  float output_arr[100];
  for (float & i : output_arr) i = -1.0f;
  bocl_mem_sptr  cl_output=new bocl_mem(device->context(), output_arr, sizeof(float)*100, "output buffer");
  cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // bit lookup buffer
  cl_uchar lookup_arr[256];
  bstm_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  int img_dim_buff[4];
  img_dim_buff[0] = pixel_x;   img_dim_buff[2] = ni;
  img_dim_buff[1] = pixel_y;   img_dim_buff[3] = nj;
  bocl_mem_sptr exp_img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(int)*4, "image dims");
  exp_img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  auto cl_prob_t = (cl_float)prob_t;
  bocl_mem_sptr prob_t_mem =new bocl_mem(device->context(), &cl_prob_t, sizeof(cl_float), "prob t buffer");
  prob_t_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //2. set global thread size
  std::size_t gThreads[] = {cl_ni,cl_nj};

  //3. set arguments
  std::vector<bstm_block_id> vis_order = scene->get_vis_blocks(cam);
  std::vector<bstm_block_id>::iterator id;
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
      //choose correct render kernel
      bstm_block_metadata mdata = scene->get_block_metadata(*id);

      //if the current blk does not contain the queried time, no need to ray cast
      double local_time;
      if(!mdata.contains_t(time,local_time))
        continue;

      auto cl_time = (cl_float)local_time;
      bocl_mem_sptr time_mem =new bocl_mem(device->context(), &cl_time, sizeof(cl_float), "time instance buffer");
      time_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

      bocl_kernel* kern =  kernels[identifier][0];

      //write the image values to the buffer
      vul_timer transfer;
      bocl_mem* blk = opencl_cache->get_block(*id);
      bocl_mem* blk_info = opencl_cache->loaded_block_info();
      bocl_mem* blk_t = opencl_cache->get_time_block(*id);
      bocl_mem* alpha = opencl_cache->get_data<BSTM_ALPHA>(*id);
      transfer_time += (float) transfer.all();

      ////3. SET args
      kern->set_arg( blk_info );
      kern->set_arg( blk );
      kern->set_arg( blk_t );
      kern->set_arg( alpha );
      kern->set_arg( ray_o_buff.ptr() );
      kern->set_arg( ray_d_buff.ptr() );
      kern->set_arg(exp_img_dim.ptr());
      kern->set_arg( cl_output.ptr() );
      kern->set_arg( lookup.ptr() );
      kern->set_arg( time_mem.ptr() );
      kern->set_arg( prob_t_mem.ptr() );

      //local tree , cumsum buffer, imindex buffer
      kern->set_local_arg( lthreads[0]*lthreads[1]*sizeof(cl_uchar16) );
      kern->set_local_arg( lthreads[0]*lthreads[1]*sizeof(cl_uchar8) );
      kern->set_local_arg( lthreads[0]*lthreads[1]*10*sizeof(cl_uchar) );
      kern->set_local_arg( lthreads[0]*lthreads[1]*sizeof(cl_int) );

      //execute kernel
      kern->execute(queue, 2, lthreads, gThreads);
      clFinish(queue);
      gpu_time += kern->exec_time();

      //clear render kernel args so it can reset em on next execution
      kern->clear_args();
      kern->release_current_event();
  }

  //read outout
  cl_output->read_to_buffer(queue);
  std::cout << "prob: " << output_arr[0] << " pt: (" <<  output_arr[1] << "," << output_arr[2] << "," << output_arr[3] << ")" << std::endl;


  //clean up cam
  delete[] ray_origins;
  delete[] ray_directions;
  opencl_cache->unref_mem(ray_o_buff.ptr());
  opencl_cache->unref_mem(ray_d_buff.ptr());

  clReleaseCommandQueue(queue);

  i=0;
  pro.set_output_val<float>(i++, output_arr[1]);
  pro.set_output_val<float>(i++, output_arr[2]);
  pro.set_output_val<float>(i++, output_arr[3]);
  return true;
}
