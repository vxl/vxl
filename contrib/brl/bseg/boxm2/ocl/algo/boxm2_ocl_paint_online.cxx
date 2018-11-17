#include <fstream>
#include <iostream>
#include <algorithm>
#include "boxm2_ocl_paint_online.h"
//:
// \file
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/cpp/algo/boxm2_mog3_grey_processor.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <vul/vul_timer.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>

//: Declare kernels
std::map<std::string, std::vector<bocl_kernel*> > boxm2_ocl_paint_online::kernels_;

//paint block
bool boxm2_ocl_paint_online::paint_scene(const boxm2_scene_sptr&          scene,
                                         const bocl_device_sptr&          device,
                                         const boxm2_opencl_cache_sptr&    opencl_cache,
                                         const vil_image_view_base_sptr&   img,
                                         const vpgl_camera_double_sptr&    cam,
                                         std::string const& apm_id)
{
  vil_image_view<float> weights(img->ni(), img->nj());
  weights.fill(1.0f);
  return paint_scene_with_weights(scene, device, opencl_cache, img, weights, cam, apm_id);
}

bool boxm2_ocl_paint_online::paint_scene_with_weights(const boxm2_scene_sptr&          scene,
                                                      bocl_device_sptr          device,
                                                      const boxm2_opencl_cache_sptr&    opencl_cache,
                                                      const vil_image_view_base_sptr&   img,
                                                      vil_image_view<float> const& weights,
                                                      vpgl_camera_double_sptr    cam,
                                                      std::string const& apm_id)
{
  typedef boxm2_data_traits<BOXM2_AUX0>::datatype aux0_datatype;
  typedef boxm2_data_traits<BOXM2_AUX2>::datatype aux2_datatype;

  float transfer_time=0.0f;
  float gpu_time=0.0f;

  //cache size sanity check
  long binCache = opencl_cache.ptr()->bytes_in_cache();
  std::cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;

  //make correct data types are here
  bool foundDataType = false, foundNumObsType = false;
  std::string data_type, num_obs_type;
  std::vector<std::string> apps = scene->appearances();
  for (const auto & app : apps) {
    if ( app == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix(apm_id) )
    {
      data_type = app;
      foundDataType = true;
      // boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
    }
    else if ( app == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix(apm_id) )
    {
      data_type = app;
      foundDataType = true;

      // boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix());
    }
    else if ( app == boxm2_data_traits<BOXM2_NUM_OBS>::prefix(apm_id) )
    {
      num_obs_type = app;
      foundNumObsType = true;
    }
  }
  if (!foundDataType) {
    std::cout<<"BOXM2_OPENCL_UPDATE_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<std::endl;
    return false;
  }
  if (!foundNumObsType) {
    std::cout<<"BOXM2_OPENCL_UPDATE_PROCESS ERROR: scene doesn't have BOXM2_NUM_OBS type"<<std::endl;
    return false;
  }

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue( device->context(),
                                                 *(device->device_id()),
                                                 CL_QUEUE_PROFILING_ENABLE,
                                                 &status);
  if (status!=0)
    return false;

  //grab input image, establish cl_ni, cl_nj (so global size is divisible by local size)
  vil_image_view_base_sptr float_img = boxm2_util::prepare_input_image(img, true);
  auto* img_view = static_cast<vil_image_view<float>* >(float_img.ptr());


  std::size_t local_threads[2]={8,8};
  std::size_t global_threads[2]={8,8};
  auto cl_ni=(unsigned)RoundUp(img_view->ni(),(int)local_threads[0]);
  auto cl_nj=(unsigned)RoundUp(img_view->nj(),(int)local_threads[1]);

  //set generic cam
  auto* ray_origins    = new cl_float[4*cl_ni*cl_nj];
  auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
  bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem( cl_ni*cl_nj * sizeof(cl_float4), ray_origins,    "ray_origins buffer");
  bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem( cl_ni*cl_nj * sizeof(cl_float4), ray_directions, "ray_directions buffer");
  boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

  //Visibility, Preinf, Norm, and input image buffers
  auto* vis_buff = new float[cl_ni*cl_nj];
  auto* input_buff=new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++)
    vis_buff[i]=1.0f;

  //copy input vals into image
  int count=0;
  for (unsigned int j=0;j<cl_nj;++j) {
    for (unsigned int i=0;i<cl_ni;++i) {
      if ( i<img_view->ni() && j< img_view->nj() ) {
        input_buff[count] = (*img_view)(i,j);
        vis_buff[count] = weights(i,j);
      }
      else {
        input_buff[count] = 0.0f;
        vis_buff[count] = 1.0f;
      }
      ++count;
    }
  }

  bocl_mem_sptr in_image=opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float),input_buff,"input image buffer");
  in_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr vis_image=opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float),vis_buff,"vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // Image Dimensions
  unsigned int img_dim_buff[4]={0,0,img_view->ni(),img_view->nj()};

  bocl_mem_sptr img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(unsigned int)*4, "image dims");
  img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

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

  // set arguments
  std::vector<boxm2_block_id> vis_order = scene->get_vis_blocks(cam);
  std::vector<boxm2_block_id>::iterator id;
  std::string opts = boxm2_ocl_util::mog_options( data_type );
  std::vector<bocl_kernel* > kerns = compile_kernels(device, opts);

  //set masked values
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
    std::cout<<*id;
    //choose correct render kernel
    boxm2_block_metadata mdata = scene->get_block_metadata(*id);
    bocl_kernel* kern =  kerns[0];
    global_threads[0]=cl_ni;
    global_threads[1]=cl_nj;
    local_threads[0]=8;
    local_threads[1]=8;

    //write the image values to the buffer
    vul_timer transfer;
    bocl_mem* blk       = opencl_cache->get_block(scene, *id);
    bocl_mem* blk_info  = opencl_cache->loaded_block_info();
    bocl_mem* alpha     = opencl_cache->get_data(scene, *id,boxm2_data_traits<BOXM2_ALPHA>::prefix());
    auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
    int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
    info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
    blk_info->write_to_buffer((queue));

    //grab an appropriately sized AUX data buffer
    int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
    bocl_mem *aux0   = opencl_cache->get_data<BOXM2_AUX0>(scene, *id, info_buffer->data_buffer_length*auxTypeSize);
    auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
    bocl_mem *aux1   = opencl_cache->get_data<BOXM2_AUX1>(scene, *id, info_buffer->data_buffer_length*auxTypeSize);
    auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
    bocl_mem *aux2   = opencl_cache->get_data<BOXM2_AUX2>(scene, *id, info_buffer->data_buffer_length*auxTypeSize);
    auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
    bocl_mem *aux3   = opencl_cache->get_data<BOXM2_AUX3>(scene, *id, info_buffer->data_buffer_length*auxTypeSize);

    transfer_time += (float) transfer.all();
    aux0->zero_gpu_buffer(queue);
    aux1->zero_gpu_buffer(queue);
    aux2->zero_gpu_buffer(queue);

    kern->set_arg( blk_info );
    kern->set_arg( blk );
    kern->set_arg( alpha );
    kern->set_arg( aux0 );
    kern->set_arg( aux1 );
    kern->set_arg( aux2 );
    kern->set_arg( aux3 );
    kern->set_arg( lookup.ptr() );

    // kern->set_arg( persp_cam.ptr() );
    kern->set_arg( ray_o_buff.ptr() );
    kern->set_arg( ray_d_buff.ptr() );

    kern->set_arg( img_dim.ptr() );
    kern->set_arg( vis_image.ptr() );
    kern->set_arg( in_image.ptr() );
    kern->set_arg( cl_output.ptr() );
    kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
    kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar4) ); //ray bundle,
    kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_int) );    //cell pointers,
    kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_float4) ); //cached aux,
    kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer

    //execute kernel
    kern->execute(queue, 2, local_threads, global_threads);
    int status = clFinish(queue);
    check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
    gpu_time += kern->exec_time();

    //clear render kernel args so it can reset em on next execution
    kern->clear_args();

    // update_alpha boolean buffer
    local_threads[0] = 64;
    local_threads[1] = 1 ;
    global_threads[0]=RoundUp(info_buffer->data_buffer_length,local_threads[0]);
    global_threads[1]=1;
    aux2->read_to_buffer(queue);
    bocl_kernel* kern1 =  kerns[1];
    int mogTypeSize    = (int) boxm2_data_info::datasize(data_type);
    bocl_mem* mog      = opencl_cache->get_data(scene, *id, data_type, info_buffer->data_buffer_length*mogTypeSize, false);
    int nobs_size      = (int) boxm2_data_info::datasize(num_obs_type);
    bocl_mem* num_obs  = opencl_cache->get_data(scene, *id, num_obs_type, info_buffer->data_buffer_length*nobs_size, false);

    kern1->set_arg( blk_info );
    kern1->set_arg( mog );
    kern1->set_arg( num_obs );
    kern1->set_arg( aux0 );
    kern1->set_arg( aux1 );
    kern1->set_arg( aux2 );
    kern1->set_arg( cl_output.ptr() );

    //execute kernel
    kern1->execute(queue, 2, local_threads, global_threads);
    status = clFinish(queue);

    check_val(status, MEM_FAILURE, "UPDATE App FAILED: " + error_to_string(status));
    gpu_time += kern1->exec_time();
    //clear render kernel args so it can reset em on next execution
    kern1->clear_args();
    //write info to disk
    mog->read_to_buffer(queue);
    num_obs->read_to_buffer(queue);
    cl_output->read_to_buffer(queue);
    vis_image->read_to_buffer(queue);

    //read image out to buffer (from gpu)
    clFinish(queue);
  }
  ///debugging save vis, pre, norm images
  opencl_cache->unref_mem(ray_d_buff.ptr());
  opencl_cache->unref_mem(ray_o_buff.ptr());
  opencl_cache->unref_mem(vis_image.ptr());
  opencl_cache->unref_mem(in_image.ptr());
  delete [] vis_buff;
  delete [] input_buff;
  delete [] ray_origins;
  delete [] ray_directions;

  std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
  clReleaseCommandQueue(queue);
  return true;
}


//: Keeps track of already compiled kernels, and returns matching ones
std::vector< bocl_kernel* > boxm2_ocl_paint_online::compile_kernels( const bocl_device_sptr& device,
                                                                    const std::string& opts )
{
  //make id out of device
  std::string identifier = device->device_identifier();
  if ( kernels_.find(identifier) != kernels_.end() )
    return kernels_[identifier];

  std::vector<bocl_kernel * > kernels;

  std::cout<<"===========Compiling kernels==========="<<std::endl;
  //gather all render sources... seems like a lot for rendering...
  std::vector<std::string> src_paths;
  std::string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "cell_utils.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "backproject.cl");
  src_paths.push_back(source_dir + "statistics_library_functions.cl");
  src_paths.push_back(source_dir + "bit/batch_update_kernels.cl");
  src_paths.push_back(source_dir + "batch_update_functors.cl");
  src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

  //compilation options
  auto* kernel = new bocl_kernel();

  std::string updt_opts = opts +  " -D AUX_LEN_INT_VIS -D STEP_CELL=step_cell_aux_len_int_vis(aux_args,data_ptr,llid,d) ";
  kernel->create_kernel(&device->context(),device->device_id(), src_paths, "aux_len_int_vis_main", updt_opts, "online_paint::aux_len_int_vis_main");
  kernels.push_back(kernel);

  std::vector<std::string> second_src_paths;
  second_src_paths.push_back(source_dir + "scene_info.cl");
  second_src_paths.push_back(source_dir + "cell_utils.cl");
  second_src_paths.push_back(source_dir + "statistics_library_functions.cl");
  second_src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
  second_src_paths.push_back(source_dir + "bit/update_kernels.cl");

  auto* kernel1 = new bocl_kernel();
  std::string app_opts = opts +  " -D UPDATE_APP_GREY";
  kernel1->create_kernel(&device->context(),device->device_id(), second_src_paths, "update_mog3_main", app_opts, "online_paint::update_mog3_main");
  kernels.push_back(kernel1);

  //store in map
  kernels_[identifier] = kernels;
  return kernels;
}
