#include <iostream>
#include <algorithm>
#include "boxm2_multi_store_aux.h"
//:
// \file

#include <boxm2_multi_util.h>

//ocl includes
#include <bocl/bocl_manager.h>
#include <bocl/bocl_mem.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/ocl/boxm2_opencl_cache1.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <vpgl/vpgl_camera.h>

#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vul/vul_timer.h>

std::map<std::string, bocl_kernel*> boxm2_multi_store_aux::kernels_;

//-------------------------------------------------------------
// Stores seg len and observation in cell-level aux data
//-------------------------------------------------------------
float boxm2_multi_store_aux::store_aux(boxm2_multi_cache&       cache,
                                       vil_image_view<float>&   img,
                                       const vpgl_camera_double_sptr&  cam,
                                       boxm2_multi_update_helper& helper)
{
  std::cout<<"  -- boxm2_multi_store_aux store aux --"<<std::endl;
  //verify appearance model
  std::size_t lthreads[2] = {8,8};
  std::string data_type, options;
  int apptypesize;
  if ( !boxm2_multi_util::get_scene_appearances(cache.get_scene(), data_type, options, apptypesize) )
    return 0.0f;

  //setup image size
  int ni=img.ni(),
      nj=img.nj();
  unsigned cl_ni=RoundUp(ni,lthreads[0]);
  unsigned cl_nj=RoundUp(nj,lthreads[1]);
  std::size_t gThreads[] = {cl_ni,cl_nj};

  //input image buffer
  float* inImg;
  if (img.size() == cl_ni*cl_nj) {
    inImg = img.top_left_ptr();
  }
  else {
    inImg = new float[cl_ni*cl_nj];
    int c = 0;
    for (int j=0; j<cl_nj; ++j)
      for (int i=0; i<cl_ni; ++i)
    if( i <ni && j <nj )
      inImg[c++] = img(i,j);
    else
      c++;
  }
  //-------------------------------------------------------
  //prepare buffers for each device
  //-------------------------------------------------------
  std::vector<bocl_mem_sptr> in_imgs;
  std::vector<cl_command_queue>& queues = helper.queues_;
  std::vector<bocl_mem_sptr>& out_imgs = helper.outputs_,
                             img_dims = helper.img_dims_,
                             ray_ds = helper.ray_ds_,
                             ray_os = helper.ray_os_,
                             lookups = helper.lookups_,
                             tnearfarptrs = helper.tnearfarptrs_;

  std::vector<boxm2_opencl_cache1*>& ocl_caches = helper.vis_caches_;
  for (auto ocl_cache : ocl_caches) {
    //grab sub scene and its cache
    boxm2_scene_sptr    sub_scene = ocl_cache->get_scene();
    bocl_device_sptr    device    = ocl_cache->get_device();

    //create image var
    bocl_mem_sptr in_mem = ocl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float),inImg,"exp image buffer");
    in_mem->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    in_imgs.push_back(in_mem);
  }

  //----------------------------------------------------------------
  // Call per block/per scene update (to ensure cpu-> gpu cache works
  //---------------------------------------------------------------
  //visibility order
  std::vector<boxm2_multi_cache_group*> grp = helper.group_orders_; //cache.get_vis_groups(cam);
  std::cout<<"Group list size; "<<grp.size()<<std::endl;
  vul_timer t; t.mark();
  float transfer_time = 0.0f;
  for (auto & grpId : grp) {
    boxm2_multi_cache_group& group = *grpId;
    std::vector<boxm2_block_id>& ids = group.ids();
    std::vector<int> indices = group.order_from_cam(cam);
    for (int i : indices) {
      //grab sub scene and its cache
      boxm2_opencl_cache1* ocl_cache = ocl_caches[i];
      boxm2_scene_sptr    sub_scene = ocl_cache->get_scene();
      bocl_device_sptr    device    = ocl_cache->get_device();

      // compile the kernel/retrieve cached kernel for this device
      bocl_kernel* kern = get_kernels(device, options);

      //Run block store aux
      boxm2_block_id id = ids[i];
      store_aux_per_block(id, sub_scene, ocl_cache, queues[i], kern,
                          in_imgs[i], img_dims[i], ray_os[i], ray_ds[i],tnearfarptrs[i],
                          out_imgs[i], lookups[i], lthreads, gThreads);
    }

    //finish
    for (int i : indices) {
      clFinish(queues[i]);
    }
  }
  float gpu_time = t.all() - transfer_time;

  //unref mems
  for (unsigned int i=0; i<queues.size(); ++i) {
    ocl_caches[i]->unref_mem(in_imgs[i].ptr());
  }

  //cleanup input image buffer
  if (inImg != img.top_left_ptr())
    delete[] inImg;

  return gpu_time;
}

//: Reads aux memory from GPU to CPU ram
void boxm2_multi_store_aux::read_aux(boxm2_block_id const& id,
                                     boxm2_opencl_cache1*   opencl_cache,
                                     cl_command_queue&     queue)
{
  //calc data buffer length
  bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(id,0,false);
  std::size_t alphaTypeSize = boxm2_data_traits<BOXM2_ALPHA>::datasize();
  auto dataLen = (std::size_t) (alpha->num_bytes() / alphaTypeSize);

  //grab an appropriately sized AUX data buffer
  bocl_mem *aux0 = opencl_cache->get_data<BOXM2_AUX0>(id, dataLen*boxm2_data_traits<BOXM2_AUX0>::datasize());
  bocl_mem *aux1 = opencl_cache->get_data<BOXM2_AUX1>(id, dataLen*boxm2_data_traits<BOXM2_AUX1>::datasize());
  aux0->read_to_buffer(queue);
  aux1->read_to_buffer(queue);
}

//: helper to call ocl kernel - stores aux per block
void boxm2_multi_store_aux::store_aux_per_block(boxm2_block_id const& id,
                                                const boxm2_scene_sptr&      scene,
                                                boxm2_opencl_cache1*   opencl_cache,
                                                cl_command_queue&     queue,
                                                bocl_kernel*          kernel,
                                                bocl_mem_sptr&        in_image,
                                                bocl_mem_sptr&        img_dim,
                                                bocl_mem_sptr&        ray_o_buff,
                                                bocl_mem_sptr&        ray_d_buff,
                                                bocl_mem_sptr&        tnearfarptr,
                                                bocl_mem_sptr&        cl_output,
                                                bocl_mem_sptr&        lookup,
                                                std::size_t*           lthreads,
                                                std::size_t*           gThreads,
                                                bool                  store_rgb)
{
  //std::cout<<(*id);
  //choose correct render kernel
  boxm2_block_metadata mdata = scene->get_block_metadata(id);
  bocl_kernel* kern =  kernel;

  //write the image values to the buffer
  bocl_mem* blk       = opencl_cache->get_block(id);
  bocl_mem* blk_info  = opencl_cache->loaded_block_info();
  bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(id,0,false);

  //calc data buffer length
  std::size_t alphaTypeSize = boxm2_data_traits<BOXM2_ALPHA>::datasize();
  auto dataLen = (std::size_t) (alpha->num_bytes() / alphaTypeSize);

  //store len in info buffer
  auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
  info_buffer->data_buffer_length = dataLen;
  blk_info->write_to_buffer(queue);

  //grab an appropriately sized AUX data buffer
  bocl_mem *aux0   = opencl_cache->get_data<BOXM2_AUX0>(id, dataLen*boxm2_data_traits<BOXM2_AUX0>::datasize());
  bocl_mem *aux1   = opencl_cache->get_data<BOXM2_AUX1>(id, dataLen*boxm2_data_traits<BOXM2_AUX1>::datasize());
  aux0->zero_gpu_buffer(queue);
  aux1->zero_gpu_buffer(queue);

  //set args
  kern->set_arg( blk_info );
  kern->set_arg( blk );
  kern->set_arg( alpha );
  kern->set_arg( aux0 );
  kern->set_arg( aux1 );
  if (store_rgb) {
    bocl_mem *aux2  = opencl_cache->get_data<BOXM2_AUX2>(id, dataLen*boxm2_data_traits<BOXM2_AUX2>::datasize());
    bocl_mem *aux3  = opencl_cache->get_data<BOXM2_AUX3>(id, dataLen*boxm2_data_traits<BOXM2_AUX3>::datasize());
    aux2->zero_gpu_buffer(queue);
    aux3->zero_gpu_buffer(queue);
    kern->set_arg( aux2 );
    kern->set_arg( aux3 );
  }
  kern->set_arg( lookup.ptr() );
  kern->set_arg( ray_o_buff.ptr() );
  kern->set_arg( ray_d_buff.ptr() );
  kern->set_arg( tnearfarptr.ptr() );
  kern->set_arg( img_dim.ptr() );
  kern->set_arg( in_image.ptr() );
  kern->set_arg( cl_output.ptr() );
  kern->set_local_arg( lthreads[0]*lthreads[1]*sizeof(cl_uchar16) );//local tree,
  kern->set_local_arg( lthreads[0]*lthreads[1]*sizeof(cl_uchar4) ); //ray bundle,
  kern->set_local_arg( lthreads[0]*lthreads[1]*sizeof(cl_int) );    //cell pointers,
  kern->set_local_arg( lthreads[0]*lthreads[1]*sizeof(cl_float4) ); //cached aux,
  kern->set_local_arg( lthreads[0]*lthreads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer

  //execute kernel
  kern->execute(queue, 2, lthreads, gThreads);

  //clear render kernel args so it can reset em on next execution
  kern->clear_args();

}

//-----------------------------------------------------------------
// returns vector of bocl_kernels for this specific device
//-----------------------------------------------------------------
bocl_kernel* boxm2_multi_store_aux::get_kernels(const bocl_device_sptr& device, const std::string& opts)
{
  // check to see if this device has compiled kernels already
  std::string identifier = device->device_identifier()+opts;
  if (kernels_.find(identifier) != kernels_.end())
    return kernels_[identifier];

  //if not, compile and cache them
  std::cout<<"===========Compiling multi update kernels===========\n"
          <<"  for device: "<<device->device_identifier()<<std::endl;

  //gather all render sources... seems like a lot for rendering...
  std::vector<std::string> src_paths;
  std::string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "cell_utils.cl");
  src_paths.push_back(source_dir + "atomics_util.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "backproject.cl");
  src_paths.push_back(source_dir + "statistics_library_functions.cl");
  src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
  src_paths.push_back(source_dir + "bit/update_kernels.cl");
  src_paths.push_back(source_dir + "update_functors.cl");
  src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

  //compilation options
  std::string options = opts + "";

  //create all passes
  auto* seg_len = new bocl_kernel();
  std::string seg_opts = options + "-D SEGLEN -D STEP_CELL=step_cell_seglen(aux_args,data_ptr,llid,d) ";
  seg_len->create_kernel(&device->context(),device->device_id(), src_paths, "seg_len_main", seg_opts, "update::seg_len");

  //cache in map
  kernels_[identifier] = seg_len;
  return kernels_[identifier];
}


bocl_kernel* boxm2_multi_store_aux::get_kernels_color(const bocl_device_sptr& device, const std::string& opts)
{
  // compile kernels if not already compiled
  std::string identifier = device->device_identifier() + opts + "_color";
  if (kernels_.find(identifier) != kernels_.end())
    return kernels_[identifier];

  //otherwise compile the kernels
  std::cout<<"=== boxm2_multi_store_aux::compiling kernels===\n"
          <<"    for device "<<identifier<<std::endl;

  std::vector<std::string> src_paths;
  std::string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "pixel_conversion.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "backproject.cl");
  src_paths.push_back(source_dir + "statistics_library_functions.cl");
  src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
  src_paths.push_back(source_dir + "bit/update_rgb_kernels.cl");
  std::vector<std::string> non_ray_src = std::vector<std::string>(src_paths);
  src_paths.push_back(source_dir + "update_rgb_functors.cl");
  src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

  //compilation options
  std::string options = " -D INTENSITY ";
  options += " -D YUV -D DETERMINISTIC -D MOG_TYPE_8 ";
  options += opts;

  //seg len pass
  auto* seg_len = new bocl_kernel();
  std::string seg_opts = options + " -D SEGLEN -D STEP_CELL=step_cell_seglen(aux_args,data_ptr,llid,d) ";
  seg_len->create_kernel(&device->context(), device->device_id(), src_paths, "seg_len_main", seg_opts, "update_color::seg_len");

  //create  compress rgb pass
  auto* comp = new bocl_kernel();
  std::string comp_opts = options + " -D COMPRESS_RGB ";
  comp->create_kernel(&device->context(), device->device_id(), non_ray_src, "compress_rgb", comp_opts, "update_color::compress_rgb");

  //store and return
  kernels_[identifier] = seg_len;
  return kernels_[identifier];
}
