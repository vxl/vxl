// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_update_sun_visibilities_process.cxx
//:
// \file
// \brief  A process for updating a color model
//
// \author Vishal Jain
// \date Mar 25, 2011

#include <bprb/bprb_func_process.h>

#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace boxm2_ocl_update_sun_visibilities_process_globals
{
  const unsigned n_inputs_  = 8;
  const unsigned n_outputs_ = 0;
  vcl_size_t local_threads[2]={8,8};
  vcl_size_t global_threads[2]={8,8};


  void compile_kernel(bocl_device_sptr device,vcl_vector<bocl_kernel*> & vec_kernels,vcl_string opts)
  {
    vcl_vector<vcl_string> src_paths;
    vcl_string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "pixel_conversion.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/sun_visibilities_kernel.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    vcl_vector<vcl_string> src_paths2;
    src_paths2.push_back(source_dir + "scene_info.cl");
    src_paths2.push_back(source_dir + "bit/sun_visibilities_kernel.cl");

    //compilation options
    vcl_string options ="" + opts;
    //seg len pass
    bocl_kernel* seg_len = new bocl_kernel();
    vcl_string seg_opts = options + " -D SEGLENVIS -D STEP_CELL=step_cell_seglen_vis(aux_args,data_ptr,llid,d*linfo->block_len) ";
    seg_len->create_kernel(&device->context(), device->device_id(), src_paths, "seg_len_and_vis_main", seg_opts, "update::seg_len_vis");
    vec_kernels.push_back(seg_len);

    //may need DIFF LIST OF SOURCES FOR THSI GUY TOO
    bocl_kernel* update = new bocl_kernel();
    update->create_kernel(&device->context(), device->device_id(), src_paths2, "update_visibilities_main", " -D UPDATE_SUN_VIS", "update::update_visibilities_main");
    vec_kernels.push_back(update);

    return ;
  }

  //: Map of kernels should persist between process executions
  static vcl_map<vcl_string,vcl_vector<bocl_kernel*> > kernels;
}

bool boxm2_ocl_update_sun_visibilities_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_sun_visibilities_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "boxm2_cache_sptr";
  input_types_[4] = "vpgl_camera_double_sptr";
  input_types_[5] = "unsigned";
  input_types_[6] = "unsigned";
  input_types_[7] = "vcl_string";

  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_update_sun_visibilities_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_sun_visibilities_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  float transfer_time=0.0f;
  float gpu_time=0.0f;

  //get the inputs: device, scene, opencl_cache, cam, img
  unsigned argIdx = 0;
  bocl_device_sptr          device       = pro.get_input<bocl_device_sptr>(argIdx++);
  boxm2_scene_sptr          scene        = pro.get_input<boxm2_scene_sptr>(argIdx++);
  boxm2_opencl_cache_sptr   opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(argIdx++);
  boxm2_cache_sptr          cache        = pro.get_input<boxm2_cache_sptr>(argIdx++);
  vpgl_camera_double_sptr   cam          = pro.get_input<vpgl_camera_double_sptr>(argIdx++);
  unsigned                  ni           = pro.get_input<unsigned>(argIdx++);
  unsigned                  nj           = pro.get_input<unsigned>(argIdx++);
  vcl_string                prefix_name  = pro.get_input<vcl_string>(argIdx++);

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)) )
    return false;

  // compile kernels if not already compiled
  vcl_string identifier = device->device_identifier();
  if (kernels.find(identifier)==kernels.end())
  {
    vcl_cout<<"boxm2_ocl_update_sun_visibilities_process::compiling kernels on device"<<identifier<<"==="<<vcl_endl;
    vcl_vector<bocl_kernel*> ks;
    compile_kernel(device,ks,"");
    kernels[identifier]=ks;
  }

  //grab input image dimensions - round up to the nearest 8 for OPENCL
  unsigned cl_ni=RoundUp(ni,nj);
  unsigned cl_nj=RoundUp(ni,nj);
  global_threads[0]=cl_ni;
  global_threads[1]=cl_nj;

  // create all buffers
  cl_float* ray_origins    = new cl_float[4*cl_ni*cl_nj];
  cl_float* ray_directions = new cl_float[4*cl_ni*cl_nj];
  bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins,"ray_origins buffer");
  bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions,"ray_directions buffer");
  boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

  //create vis, pre, norm and input image buffers
  float* vis_buff  = new float[cl_ni*cl_nj];
  vcl_fill(vis_buff, vis_buff+cl_ni*cl_nj, 1.0f);

  bocl_mem_sptr vis_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), vis_buff,"vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  float* last_vis_buff  = new float[cl_ni*cl_nj];
  vcl_fill(last_vis_buff, last_vis_buff+cl_ni*cl_nj, 1.0f);

  bocl_mem_sptr last_vis_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), last_vis_buff, "last vis image buffer");
  last_vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // Image Dimensions
  int img_dim_buff[4];
  img_dim_buff[0] = 0;
  img_dim_buff[1] = 0;
  img_dim_buff[2] = ni;
  img_dim_buff[3] = nj;
  bocl_mem_sptr img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(int)*4, "image dims");
  img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // Output Array
  float output_arr[100];
  for (int i=0; i<100; ++i) output_arr[i] = 0.0f;
  bocl_mem_sptr  cl_output=new bocl_mem(device->context(), output_arr, sizeof(float)*100, "output buffer");
  cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  // set arguments
  vcl_vector<boxm2_block_id> vis_order = scene->get_vis_blocks((vpgl_generic_camera<double>*)cam.ptr());
  vcl_vector<boxm2_block_id>::iterator id;

  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
      vcl_cout << "update_sun_vis0: id = " << id->to_string() << vcl_endl;
      //choose correct render kernel
      boxm2_block_metadata mdata = scene->get_block_metadata(*id);
      bocl_kernel* kern =  kernels[identifier][0];

      //write the image values to the buffer
      vul_timer transfer;
      bocl_mem* blk       = opencl_cache->get_block(*id);
      bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(*id);
      bocl_mem * blk_info  = opencl_cache->loaded_block_info();

      //make sure the scene info data size reflects the real data size
      boxm2_scene_info* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
      int alphaTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
      info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
      blk_info->write_to_buffer((queue));

      //grab an appropriately sized AUX data buffer
      int auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
      bocl_mem *aux0   = opencl_cache->get_data<BOXM2_AUX0>(*id, info_buffer->data_buffer_length*auxTypeSize,false);
      auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
      bocl_mem *aux1   = opencl_cache->get_data<BOXM2_AUX1>(*id, info_buffer->data_buffer_length*auxTypeSize,false);
      transfer_time += (float) transfer.all();
      local_threads[0] = 8; local_threads[1] = 8;
      global_threads[0]=cl_ni; global_threads[1]=cl_nj;
      kern->set_arg( blk_info );
      kern->set_arg( blk );
      kern->set_arg( alpha );
      kern->set_arg( aux0 );
      kern->set_arg( aux1 );
      kern->set_arg( lookup.ptr() );
      kern->set_arg( ray_o_buff.ptr() );
      kern->set_arg( ray_d_buff.ptr() );
      kern->set_arg( img_dim.ptr() );
      kern->set_arg( vis_image.ptr() );
      kern->set_arg( last_vis_image.ptr() );
      kern->set_arg( cl_output.ptr() );
      kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
      kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_short2) ); //ray bundle,
      kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_int) );    //cell pointers,
      kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_float) ); //cached aux,
      kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer

      //execute kernel
      kern->execute(queue, 2, local_threads, global_threads);
      int status = clFinish(queue);
      if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)) )
          return false;
      gpu_time += kern->exec_time();

      //clear render kernel args so it can reset em on next execution
      kern->clear_args();
      //read info back to host memory
      blk->read_to_buffer(queue);
      aux0->read_to_buffer(queue);
      aux1->read_to_buffer(queue);
      vis_image->read_to_buffer(queue);
      last_vis_image->read_to_buffer(queue);
      cl_output->read_to_buffer(queue);
      clFinish(queue);
  }

  opencl_cache->unref_mem(ray_o_buff.ptr());
  opencl_cache->unref_mem(ray_d_buff.ptr());
  opencl_cache->unref_mem(vis_image.ptr());
  opencl_cache->unref_mem(last_vis_image.ptr());

  delete [] vis_buff;
  delete [] last_vis_buff;
  delete [] ray_origins;
  delete [] ray_directions;

  //reset local threads to 8/8 (default);
  local_threads[0] = 64;
  local_threads[1] = 1;
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
      vcl_cout << "update_sun_vis2 : id = " << id->to_string() << vcl_endl;
      //choose correct render kernel
      boxm2_block_metadata mdata = scene->get_block_metadata(*id);
      //vcl_cout << "got metadata " << vcl_endl;
      bocl_kernel* kern =  kernels[identifier][1];

      //write the image values to the buffer
      vul_timer transfer;
      bocl_mem* blk       = opencl_cache->get_block(*id);
      bocl_mem * blk_info  = opencl_cache->loaded_block_info();

      bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(*id);
      boxm2_scene_info* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
      int alphaTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
      info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
      blk_info->write_to_buffer((queue));

      //vcl_cout << "getting aux0 and aux1 data " << vcl_endl;
      //grab an appropriately sized AUX data buffer
      int auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
      bocl_mem *aux0   = opencl_cache->get_data<BOXM2_AUX0>(*id, info_buffer->data_buffer_length*auxTypeSize,true);
      auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
      bocl_mem *aux1   = opencl_cache->get_data<BOXM2_AUX1>(*id, info_buffer->data_buffer_length*auxTypeSize,true);

      //vcl_cout << "getting output data: prefix_name = " << prefix_name << vcl_endl;
      bocl_mem *aux_out  = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_AUX0>::prefix(prefix_name),
                                                  info_buffer->data_buffer_length*auxTypeSize,false);

      //vcl_cout << "got data. " << vcl_endl;
      transfer_time += (float) transfer.all();
      kern->set_arg( blk_info );
      kern->set_arg( aux0 );
      kern->set_arg( aux1 );
      kern->set_arg( aux_out );
      global_threads[0]=RoundUp(info_buffer->data_buffer_length,local_threads[0]);
      global_threads[1]=1;
      //vcl_cout << "executing kernel " << vcl_endl;

      //execute kernel
      kern->execute(queue, 2, local_threads, global_threads);
      int status = clFinish(queue);
      if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)) )
          return false;
      gpu_time += kern->exec_time();
      clFinish(queue);
      //vcl_cout << "kernel completed" << vcl_endl;

      //clear render kernel args so it can reset em on next execution
      kern->clear_args();
      //vcl_cout << "1" << vcl_endl;
      //read info back to host memory
      blk->read_to_buffer(queue);
      //vcl_cout << "2" << vcl_endl;
      //vcl_cout << "aux_out cpu_buffer= " << aux_out->cpu_buffer() << vcl_endl;
      //vcl_cout << "aux_out nbytes = " << aux_out->num_bytes() << vcl_endl;
      aux_out->read_to_buffer(queue);
      //vcl_cout << "3" << vcl_endl;
      clFinish(queue);
      //vcl_cout << "4" << vcl_endl;
      cache->remove_data_base(*id,boxm2_data_traits<BOXM2_AUX0>::prefix(prefix_name));
      //vcl_cout << "5" << vcl_endl;
  }
  vcl_cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<vcl_endl;
  clReleaseCommandQueue(queue);
  return true;
}
