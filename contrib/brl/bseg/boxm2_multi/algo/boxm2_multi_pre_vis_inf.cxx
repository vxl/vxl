#include <iostream>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include "boxm2_multi_pre_vis_inf.h"
#include <boxm2_multi_util.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <bocl/bocl_manager.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/ocl/boxm2_opencl_cache1.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>

#include <bocl/bocl_mem.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>
#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vpgl/vpgl_camera_double_sptr.h>
#include <vul/vul_timer.h>

std::map<std::string, std::vector<bocl_kernel*> > boxm2_multi_pre_vis_inf::kernels_;


//-------------------------------------------------------------
// pre_vis_inf
//-------------------------------------------------------------
float boxm2_multi_pre_vis_inf::pre_vis_inf( boxm2_multi_cache&              cache,
                                            const vil_image_view<float>&    img,
                                            const vpgl_camera_double_sptr&         cam,
                                            float*                          norm_img,
                                            boxm2_multi_update_helper&      helper)
{
  std::cout<<"  -- boxm2_pre_vis_inf map --"<<std::endl;
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

  //vis inf and pre inf buffers
  auto* visImg = new float[cl_ni*cl_nj];
  auto* preImg = new float[cl_ni*cl_nj];
  std::fill(visImg, visImg+cl_ni*cl_nj, 1.0f);
  std::fill(preImg, preImg+cl_ni*cl_nj, 0.0f);

  //-------------------------------------------------------
  //prepare buffers for each device
  //-------------------------------------------------------
  std::vector<cl_command_queue>& queues = helper.queues_;
  std::vector<bocl_mem_sptr>& out_imgs = helper.outputs_,
                             img_dims = helper.img_dims_,
                             ray_ds = helper.ray_ds_,
                             ray_os = helper.ray_os_,
                             lookups = helper.lookups_,
                             tnearfarptrs=  helper.tnearfarptrs_;
  std::vector<boxm2_opencl_cache1*>& ocl_caches = helper.vis_caches_;
  std::vector<bocl_mem_sptr> vis_mems, pre_mems, visInfMems, preInfMems;
  for (auto ocl_cache : ocl_caches) {
    //grab sub scene and it's cache
    //pre/vis images
    auto* vis_buff = new float[cl_ni*cl_nj];
    std::fill(vis_buff, vis_buff+cl_ni*cl_nj, 1.0f);
    bocl_mem_sptr vis_image = ocl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), vis_buff,"vis image buffer");
    vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    vis_mems.push_back(vis_image);

    auto* pre_buff = new float[cl_ni*cl_nj];
    std::fill(pre_buff, pre_buff+cl_ni*cl_nj, 0.0f);
    bocl_mem_sptr pre_image = ocl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), pre_buff,"pre image buffer");
    pre_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    pre_mems.push_back(pre_image);
  }

  //initialize per group images (vis/pre)
  std::vector<boxm2_multi_cache_group*> grp = helper.group_orders_; //cache.get_vis_groups(cam);
  vul_timer t; t.mark();
  float gpu_time = 0.0f, cpu_time = 0.0f;

  //----------------------------------------------------------------
  // Call per block/per scene update (to ensure cpu-> gpu cache works
  //---------------------------------------------------------------
  for (auto & grpId : grp)
  {
    boxm2_multi_cache_group& group = *grpId;
    std::vector<boxm2_block_id>& ids = group.ids();
    std::vector<int> indices = group.order_from_cam(cam);
    for (int i : indices) {
      //grab sub scene and it's cache
      boxm2_opencl_cache1* ocl_cache = ocl_caches[i];
      boxm2_scene_sptr    sub_scene = ocl_cache->get_scene();
      bocl_device_sptr    device    = ocl_cache->get_device();

      // compile the kernel/retrieve cached kernel for this device
      std::vector<bocl_kernel*> kerns = get_kernels(device, options);

      //Run block store aux
      boxm2_block_id id = ids[i]; //vis_order[blk];

      //set visibility to one, set pre to zero
      vis_mems[i]->fill(queues[i], 1.0f, "float");
      pre_mems[i]->zero_gpu_buffer(queues[i]);
      cpu_time += pre_vis_per_block(id, sub_scene, ocl_cache, queues[i], data_type, kerns[0],
                                    vis_mems[i], pre_mems[i], img_dims[i],
                                    ray_os[i], ray_ds[i],tnearfarptrs[i],
                                    out_imgs[i], lookups[i], lthreads, gThreads);
    }

    //finish queues before moving on
    for (int i : indices) {

#if 1

      vul_timer cpuTimer; cpuTimer.mark();
      //first store vis/pre images for first member of group
      std::memcpy(group.get_vis(i), visImg, cl_ni*cl_nj*sizeof(float));
      std::memcpy(group.get_pre(i), preImg, cl_ni*cl_nj*sizeof(float));
      //next update the vis and pre images
      clFinish(queues[i]);

      vis_mems[i]->read_to_buffer(queues[i]);
      pre_mems[i]->read_to_buffer(queues[i]);
      auto* v = (float*) vis_mems[i]->cpu_buffer();
      auto* p = (float*) pre_mems[i]->cpu_buffer();
      for (int jj=(int)0; jj<(int)cl_nj; ++jj)
        for (int ii=(int)0; ii<(int)cl_ni; ++ii) {
          int index = jj*cl_ni + ii;
      preImg[index]  = preImg[index] + p[index]*visImg[index];
          visImg[index] *= v[index];
        }
      cpu_time += cpuTimer.all();
#else

      //first store vis/pre images for first member of group
      float* p = (float*) preInfMems[i]->enqueue_map(queues[i]);
      float* v = (float*) visInfMems[i]->enqueue_map(queues[i]);
      clFinish(queues[i]);
      std::memcpy(group.get_vis(i), v, cl_ni*cl_nj*sizeof(float));
      std::memcpy(group.get_pre(i), p/*preImg*/, cl_ni*cl_nj*sizeof(float));

      bocl_device_sptr device = ocl_caches[i]->get_device();
      bocl_kernel*     kern   = get_kernels(device, options)[2];

      preInfMems[i]->enqueue_unmap(queues[i], p);
      visInfMems[i]->enqueue_unmap(queues[i], v);
      kern->set_arg( preInfMems[i].ptr() );
      kern->set_arg( visInfMems[i].ptr() );
      kern->set_arg( pre_mems[i].ptr() );
      kern->set_arg( vis_mems[i].ptr() );
      kern->set_arg( img_dims[i].ptr() );
      kern->execute(queues[i], 2, lthreads, gThreads);
      clFinish(queues[i]);
      kern->clear_args();
#endif
    }



  }
  gpu_time = t.all();

  t.mark();
  //---- This instead of the reduce step ----
  //Norm image create on CPU
  for (unsigned int c=0; c<cl_ni*cl_nj; ++c)
    norm_img[c] = visImg[c] + preImg[c];

  //grab accurate GPU time (includes transfers)
  cpu_time += t.all();
  gpu_time -= cpu_time;

#if 1
  vil_image_view<float> nimg(ni,nj), vimg(ni,nj), pimg(ni,nj);
  int c=0;

  for (unsigned int j=0; j<cl_nj; ++j)
    for (unsigned int i=0; i<cl_ni; ++i)
      {
      if ( i < ni && j < nj )
        {
        nimg(i,j) = norm_img[c];
        vimg(i,j) = visImg[c];
        pimg(i,j) = preImg[c];
        }
      c++;
    }
  vil_save(nimg, "e:/norm_image.tiff");
  vil_save(vimg, "e:/vis_image.tiff");
  vil_save(pimg, "e:/pre_image.tiff");
#endif

  //-------------------------------------
  //clean up
  //-------------------------------------
  delete[] visImg;
  delete[] preImg;

  for (unsigned int i=0; i<queues.size(); ++i) {
    boxm2_opencl_cache1* ocl_cache = ocl_caches[i];
    auto* v = (float*) vis_mems[i]->cpu_buffer();
    auto* p = (float*) pre_mems[i]->cpu_buffer();
    delete[] v;
    delete[] p;

    //free vis mem, pre mem
    ocl_cache->unref_mem(vis_mems[i].ptr());
    ocl_cache->unref_mem(pre_mems[i].ptr());
  }
  return gpu_time;
}


float boxm2_multi_pre_vis_inf::pre_vis_per_block(const boxm2_block_id& id,
                                                 const boxm2_scene_sptr&      scene,
                                                 boxm2_opencl_cache1*   opencl_cache,
                                                 cl_command_queue&     queue,
                                                 const std::string&            data_type,
                                                 bocl_kernel*          kern,
                                                 bocl_mem_sptr&        vis_image,
                                                 bocl_mem_sptr&        pre_image,
                                                 bocl_mem_sptr&        img_dim,
                                                 bocl_mem_sptr&        ray_o_buff,
                                                 bocl_mem_sptr&        ray_d_buff,
                                                 bocl_mem_sptr&        tnearfarptr,
                                                 bocl_mem_sptr&        cl_output,
                                                 bocl_mem_sptr&        lookup,
                                                 std::size_t*           lthreads,
                                                 std::size_t*           gThreads)
{
  vul_timer ttime; ttime.mark();

  //choose correct render kernel
  boxm2_block_metadata mdata = scene->get_block_metadata(id);

  //write the image values to the buffer
  bocl_mem* blk       = opencl_cache->get_block(id);
  bocl_mem* blk_info  = opencl_cache->loaded_block_info();
  bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(id,0,false);

  //calc data buffer length (write it in blk_info)
  auto dataLen = (std::size_t) (alpha->num_bytes()/boxm2_data_traits<BOXM2_ALPHA>::datasize());
  auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
  info_buffer->data_buffer_length = (int) dataLen;
  blk_info->write_to_buffer(queue);

  //grab MOG
  int apptypesize = (int) boxm2_data_info::datasize(data_type);
  bocl_mem* mog  = opencl_cache->get_data(id,data_type, dataLen*apptypesize,false);

  //grab NumObs
  std::string numObsType = boxm2_data_traits<BOXM2_NUM_OBS>::prefix();
  int nobsTypeSize = (int) boxm2_data_info::datasize(numObsType);
  bocl_mem* num_obs = opencl_cache->get_data(id,numObsType, dataLen*nobsTypeSize,false);

  //grab an appropriately sized AUX data buffer
  bocl_mem *aux0 = opencl_cache->get_data<BOXM2_AUX0>(id, dataLen*boxm2_data_traits<BOXM2_AUX0>::datasize());
  bocl_mem *aux1 = opencl_cache->get_data<BOXM2_AUX1>(id, dataLen*boxm2_data_traits<BOXM2_AUX1>::datasize());

  //--------- set args and execute ------------
  kern->set_arg( blk_info );
  kern->set_arg( blk );
  kern->set_arg( alpha );
  kern->set_arg( mog );
  kern->set_arg( num_obs );
  kern->set_arg( aux0 );
  kern->set_arg( aux1 );
  kern->set_arg( lookup.ptr() );
  kern->set_arg( ray_o_buff.ptr() );
  kern->set_arg( ray_d_buff.ptr() );
  kern->set_arg( tnearfarptr.ptr() );
  kern->set_arg( img_dim.ptr() );
  kern->set_arg( vis_image.ptr() );
  kern->set_arg( pre_image.ptr() );
  kern->set_arg( cl_output.ptr() );
  kern->set_local_arg( lthreads[0]*lthreads[1]*sizeof(cl_uchar16) );//local tree,
  kern->set_local_arg( lthreads[0]*lthreads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer
  float transfer_time = ttime.all();


  //execute kernel
  kern->execute(queue, 2, lthreads, gThreads);
  //clear render kernel args so it can reset em on next execution
  kern->clear_args();
  return transfer_time;
}



//-----------------------------------------------------------------
// returns vector of bocl_kernels for this specific device
//-----------------------------------------------------------------
std::vector<bocl_kernel*>& boxm2_multi_pre_vis_inf::get_kernels(const bocl_device_sptr& device, const std::string& opts)
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
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "backproject.cl");
  src_paths.push_back(source_dir + "statistics_library_functions.cl");
  src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
  src_paths.push_back(source_dir + "bit/update_kernels.cl");

  std::vector<std::string> non_ray_src = std::vector<std::string>(src_paths);
  src_paths.push_back(source_dir + "update_functors.cl");
  src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

  //compilation options
  std::string options = opts+"";
  //create all passes
  auto* pre_inf = new bocl_kernel();
  std::string pre_opts = options + " -D PREINF -D STEP_CELL=step_cell_preinf(aux_args,data_ptr,llid,d) ";
  pre_inf->create_kernel(&device->context(),device->device_id(), src_paths, "pre_inf_main", pre_opts, "update::pre_inf");

  //may need DIFF LIST OF SOURCES FOR THIS GUY
  auto* proc_img = new bocl_kernel();
  std::string proc_opts = options + " -D PROC_NORM ";
  proc_img->create_kernel(&device->context(),device->device_id(), non_ray_src, "proc_norm_image", proc_opts, "update::proc_norm_image");

  auto* combine_pre_vis = new bocl_kernel();
  std::string comb_opts = options + " -D COMBINE_PRE_VIS ";
  combine_pre_vis->create_kernel(&device->context(), device->device_id(), non_ray_src, "combine_pre_vis", comb_opts, "update::combine_pre_vis");

  //vector of kernels:
  std::vector<bocl_kernel*> kerns(3);
  kerns[0] = pre_inf;
  kerns[1] = proc_img;
  kerns[2] = combine_pre_vis;

  //cache in map
  kernels_[identifier] = kerns;
  return kernels_[identifier];
}


void boxm2_multi_pre_vis_inf::write_imgs_out(std::map<bocl_device*, float*>& img_map, int ni, int nj, const std::string& name)
{
  std::map<bocl_device*, float*>::iterator iter;
  for (iter=img_map.begin(); iter!=img_map.end(); ++iter) {
    float* img = iter->second;
    int count=0;
    vil_image_view<float> outImg(ni,nj);
    for (int j=0; j<nj; ++j)
      for (int i=0; i<ni; ++i)
        outImg(i,j) = img[count++];

    std::string outName = name + iter->first->device_identifier() + ".tiff";
    vil_save(outImg, outName.c_str());
  }
}
