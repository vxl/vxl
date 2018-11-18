#include <iostream>
#include <algorithm>
#include "boxm2_multi_update_cell.h"
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
#include <vpgl/vpgl_camera.h>

#include <bocl/bocl_mem.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <vil/vil_image_view.h>
#include <vul/vul_timer.h>


std::map<std::string, std::vector<bocl_kernel*> > boxm2_multi_update_cell::kernels_;


//-------------------------------------------------------------
// Stores seg len and observation in cell-level aux data
//-------------------------------------------------------------
float boxm2_multi_update_cell::update_cells(boxm2_multi_cache&           cache,
                                            const vil_image_view<float>& img,
                                            const vpgl_camera_double_sptr&      cam,
                                            float*                       norm_image,
                                            boxm2_multi_update_helper&   helper)
{
  std::cout<<"  -- boxm2_multi_update_cell update cells --"<<std::endl;
  float transfer_time = 0.0f;

  //verify appearance model
  std::size_t lthreads[2] = {8,8};
  std::string data_type, options;
  int apptypesize;
  if ( !boxm2_multi_util::get_scene_appearances(cache.get_scene(), data_type, options, apptypesize) )
    return 0.0f;

  //setup image size
  int ni=img.ni(),
      nj=img.nj();
  //workspace size
  unsigned cl_ni = RoundUp(ni,lthreads[0]);
  unsigned cl_nj = RoundUp(nj,lthreads[1]);

  //2. set global thread size
  std::size_t gThreads[] = {cl_ni,cl_nj};

  //-------------------------------------------------------
  //prepare buffers for each device
  //-------------------------------------------------------
  std::vector<cl_command_queue>& queues = helper.queues_;
  std::vector<bocl_mem_sptr>& out_imgs = helper.outputs_,
                             img_dims = helper.img_dims_,
                             ray_ds = helper.ray_ds_,
                             ray_os = helper.ray_os_,
                             lookups = helper.lookups_,
                             tnearfarptrs = helper.tnearfarptrs_;
  std::vector<boxm2_opencl_cache1*>& ocl_caches = helper.vis_caches_;
  std::vector<bocl_mem_sptr> vis_mems, pre_mems, norm_mems;

  for (auto ocl_cache : ocl_caches) {
    //grab sub scene and it's cache
    boxm2_scene_sptr    sub_scene = ocl_cache->get_scene();
    bocl_device_sptr    device    = ocl_cache->get_device();

    //grab vis and pre images that correspond

    bocl_mem_sptr vis_mem  = ocl_cache->alloc_mem(sizeof(float)*cl_ni*cl_nj, nullptr /*vis_img*/, "vis image buff");
    bocl_mem_sptr pre_mem  = ocl_cache->alloc_mem(sizeof(float)*cl_ni*cl_nj, nullptr /*pre_img*/, "pre image buff");
    bocl_mem_sptr norm_mem = ocl_cache->alloc_mem(sizeof(float)*cl_ni*cl_nj, norm_image, "norm image buff");
    vis_mem->create_buffer(CL_MEM_READ_WRITE); // | CL_MEM_COPY_HOST_PTR);
    pre_mem->create_buffer(CL_MEM_READ_WRITE); // | CL_MEM_COPY_HOST_PTR);
    norm_mem->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    //pre/vis images
    vis_mems.push_back(vis_mem);
    pre_mems.push_back(pre_mem);
    norm_mems.push_back(norm_mem);
  }

  vul_timer t; t.mark();
  float gpu_time = 0.0f;

  //----------------------------------------------------------------
  // Call per block/per scene update (to ensure cpu-> gpu cache works
  //---------------------------------------------------------------
  std::vector<boxm2_multi_cache_group*> grp = helper.group_orders_; //cache.get_vis_groups(cam);
  for (auto & grpId : grp) {
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
      boxm2_block_id id = ids[i];

      //make sure vis/pre are correct
      vis_mems[i]->write_to_gpu_mem(queues[i], group.get_vis(i), cl_ni*cl_nj*sizeof(float));
      pre_mems[i]->write_to_gpu_mem(queues[i], group.get_pre(i), cl_ni*cl_nj*sizeof(float));
      transfer_time += calc_beta_per_block(id, sub_scene, ocl_cache, queues[i], data_type, kerns[0],
                                           vis_mems[i], pre_mems[i], norm_mems[i], img_dims[i],ray_os[i], ray_ds[i],tnearfarptrs[i], out_imgs[i], lookups[i],lthreads, gThreads);

    }

    //finish queues before moving on (Maybe read in AUX 2 and 3 here)
    for (int i : indices) {
      clFinish(queues[i]);

     // vul_timer ttime; ttime.mark();
     //boxm2_opencl_cache1* opencl_cache = ocl_caches[i];
     //boxm2_block_id id = ids[i];

     // //calc data buffer length
     //bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(id,0,false);
     //std::size_t alphaTypeSize = boxm2_data_traits<BOXM2_ALPHA>::datasize();
     //std::size_t dataLen = (std::size_t) (alpha->num_bytes() / alphaTypeSize);

     // //grab an appropriately sized AUX data buffer
     //bocl_mem *aux2 = opencl_cache->get_data<BOXM2_AUX2>(id, dataLen*boxm2_data_traits<BOXM2_AUX2>::datasize());
     //bocl_mem *aux3 = opencl_cache->get_data<BOXM2_AUX3>(id, dataLen*boxm2_data_traits<BOXM2_AUX3>::datasize());
     //aux2->read_to_buffer(queues[i]);
     //aux3->read_to_buffer(queues[i]);
     // transfer_time += ttime.all();
    }
  }
  gpu_time += t.all(); t.mark();

  //-------------------------------------------------------------------
  // Reduce images into pre/vis image and make sure the interim
  // pre/vis images are correct
  //-------------------------------------------------------------------
  transfer_time += calc_beta_reduce(cache, cam, helper);

  gpu_time += t.all();

  //--------------------------------------
  //Clean up vis, pre, norm images buffers
  //--------------------------------------
  for (unsigned int i=0; i<ocl_caches.size(); ++i) {
    //grab sub scene and it's cache
    boxm2_opencl_cache1* ocl_cache = ocl_caches[i];
    ocl_cache->unref_mem(vis_mems[i].ptr());
    ocl_cache->unref_mem(pre_mems[i].ptr());
    ocl_cache->unref_mem(norm_mems[i].ptr());
  }
  return gpu_time-transfer_time;
}


//runs pre/vis on single block
float boxm2_multi_update_cell::calc_beta_per_block(const boxm2_block_id&     id,
                                                   const boxm2_scene_sptr&     /*scene*/,
                                                   boxm2_opencl_cache1* opencl_cache,
                                                   cl_command_queue&   queue,
                                                   const std::string&          data_type,
                                                   bocl_kernel*        kern,
                                                   bocl_mem_sptr&      vis_image,
                                                   bocl_mem_sptr&      pre_image,
                                                   bocl_mem_sptr&      norm_image,
                                                   bocl_mem_sptr&      img_dim,
                                                   bocl_mem_sptr&      ray_o_buff,
                                                   bocl_mem_sptr&      ray_d_buff,
                                                   bocl_mem_sptr&      tnearfarptr,
                                                   bocl_mem_sptr&      cl_output,
                                                   bocl_mem_sptr&      lookup,
                                                   std::size_t*         lThreads,
                                                   std::size_t*         gThreads)
{
  float transfer_time = 0.0f;
  vul_timer ttime; ttime.mark();

  //write the image values to the buffer
  bocl_mem* blk       = opencl_cache->get_block(id);
  bocl_mem* blk_info  = opencl_cache->loaded_block_info();
  bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(id,0,false);

  //num data in this block
  auto dataLen = (std::size_t) (alpha->num_bytes()/boxm2_data_traits<BOXM2_ALPHA>::datasize());

  //calc data buffer length
  auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
  info_buffer->data_buffer_length = (int) dataLen;
  blk_info->write_to_buffer(queue);

  //grab mog, numobs and aux
  int apptypesize = (int) boxm2_data_info::datasize(data_type);
  bocl_mem* mog  = opencl_cache->get_data(id,data_type,dataLen*apptypesize,false);
  std::string numObsType = boxm2_data_traits<BOXM2_NUM_OBS>::prefix();
  int nobsTypeSize = (int)boxm2_data_info::datasize(numObsType);
  bocl_mem* num_obs   = opencl_cache->get_data(id,numObsType,dataLen*nobsTypeSize,false);

  //get two aux - cell_vis and cell_beta
  int auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
  bocl_mem *aux0  = opencl_cache->get_data<BOXM2_AUX0>(id, info_buffer->data_buffer_length*auxTypeSize);
  auxTypeSize     = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
  bocl_mem *aux1  = opencl_cache->get_data<BOXM2_AUX1>(id, info_buffer->data_buffer_length*auxTypeSize);
  auxTypeSize     = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
  bocl_mem *aux2  = opencl_cache->get_data<BOXM2_AUX2>(id, info_buffer->data_buffer_length*auxTypeSize);
  auxTypeSize     = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
  bocl_mem *aux3  = opencl_cache->get_data<BOXM2_AUX3>(id, info_buffer->data_buffer_length*auxTypeSize);

  //prep aux2 and 3
  aux2->zero_gpu_buffer(queue);
  aux3->zero_gpu_buffer(queue);

  transfer_time += ttime.all();

  kern->set_arg( blk_info );
  kern->set_arg( blk );
  kern->set_arg( alpha );
  kern->set_arg( mog );
  kern->set_arg( num_obs );
  kern->set_arg( aux0 );
  kern->set_arg( aux1 );
  kern->set_arg( aux2 );
  kern->set_arg( aux3 );
  kern->set_arg( lookup.ptr() );
  kern->set_arg( ray_o_buff.ptr() );
  kern->set_arg( ray_d_buff.ptr() );
  kern->set_arg( tnearfarptr.ptr() );
  kern->set_arg( img_dim.ptr() );
  kern->set_arg( vis_image.ptr() );
  kern->set_arg( pre_image.ptr() );
  kern->set_arg( norm_image.ptr() );
  kern->set_arg( cl_output.ptr() );
  kern->set_local_arg( lThreads[0]*lThreads[1]*sizeof(cl_uchar16) );//local tree,
  kern->set_local_arg( lThreads[0]*lThreads[1]*sizeof(cl_short2) ); //ray bundle,
  kern->set_local_arg( lThreads[0]*lThreads[1]*sizeof(cl_int) );    //cell pointers,
  kern->set_local_arg( lThreads[0]*lThreads[1]*sizeof(cl_float) ); //cached aux,
  kern->set_local_arg( lThreads[0]*lThreads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer
  //execute kernel
  kern->execute(queue, 2, lThreads, gThreads);
  kern->clear_args();

  //async reads for aux2 and aux3
  //aux2->read_to_buffer(queue);
  //aux3->read_to_buffer(queue);
  return transfer_time;
}


float boxm2_multi_update_cell::calc_beta_reduce( boxm2_multi_cache& mcache,
                                                 const vpgl_camera_double_sptr& cam,
                                                 boxm2_multi_update_helper& helper )
{
  vul_timer ttime; ttime.mark();
  float transfer_time = 0.0f;

  //get total scene info first
  std::string data_type, options;
  int apptypesize;
  if ( !boxm2_multi_util::get_scene_appearances(mcache.get_scene(), data_type, options, apptypesize) )
    return 0.0f;

  //use existing queues
  std::vector<cl_command_queue>& queues = helper.queues_;


  std::vector<bocl_mem_sptr>& out_imgs = helper.outputs_,
                             lookups = helper.lookups_;
  std::vector<boxm2_opencl_cache1*>& ocl_caches = helper.vis_caches_;

  //Only bother updating the visible groups
  std::vector<boxm2_multi_cache_group*> grp = mcache.get_vis_groups(cam);
  for (auto & grpId : grp) {
    boxm2_multi_cache_group& group = *grpId;
    std::vector<boxm2_block_id>& ids = group.ids();
    for (unsigned int i=0; i<ids.size(); ++i) {
      ttime.mark();
      //grab sub scene and it's cache
      boxm2_opencl_cache1* ocl_cache = ocl_caches[i];
      boxm2_scene_sptr    sub_scene = ocl_cache->get_scene();
      bocl_device_sptr    device    = ocl_cache->get_device();
      boxm2_block_id       id    = ids[i];
      //boxm2_block_metadata mdata = sub_scene->get_block_metadata(id);
      // compile the kernel/retrieve cached kernel for this device
      std::vector<bocl_kernel*>& kerns = get_kernels(device, options);
      bocl_kernel* kern = kerns[1];
      //write the image values to the buffer
      /*bocl_mem* blk= */  ocl_cache->get_block(id);
      bocl_mem* blk_info  = ocl_cache->loaded_block_info();
      bocl_mem* alpha     = ocl_cache->get_data<BOXM2_ALPHA>(id,0,false);
      auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();

      int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
      // check for invalid parameters
      if( alphaTypeSize == 0 ) //This should never happen, it will result in division by zero later
      {
        std::cerr << "ERROR: Division by 0 in " << __FILE__ << __LINE__ << std::endl;
        throw 0;
      }

      info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
      blk_info->write_to_buffer(queues[i]);
      //grab mog
      bocl_mem* mog       = ocl_cache->get_data(id,data_type,alpha->num_bytes()/alphaTypeSize*apptypesize,false);

      //numobs
      std::string num_obs_type = boxm2_data_traits<BOXM2_NUM_OBS>::prefix();
      int nobsTypeSize        = (int)boxm2_data_info::datasize(num_obs_type);
      bocl_mem* num_obs       = ocl_cache->get_data(id,num_obs_type,alpha->num_bytes()/alphaTypeSize*nobsTypeSize,false);

      //grab an appropriately sized AUX data buffer
      int auxTypeSize  = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
      bocl_mem *aux0   = ocl_cache->get_data<BOXM2_AUX0>(id, info_buffer->data_buffer_length*auxTypeSize);
      auxTypeSize      = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
      bocl_mem *aux1   = ocl_cache->get_data<BOXM2_AUX1>(id, info_buffer->data_buffer_length*auxTypeSize);
      auxTypeSize      = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
      bocl_mem *aux2   = ocl_cache->get_data<BOXM2_AUX2>(id, info_buffer->data_buffer_length*auxTypeSize);
      auxTypeSize      = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
      bocl_mem *aux3   = ocl_cache->get_data<BOXM2_AUX3>(id, info_buffer->data_buffer_length*auxTypeSize);

      // update_alpha boolean buffer
      bool update_alpha = true;
      cl_int up_alpha[1];
      up_alpha[0] = update_alpha ? 1 : 0;
      bocl_mem_sptr up_alpha_mem = new bocl_mem(device->context(), up_alpha, sizeof(up_alpha), "update alpha bool buffer");
      up_alpha_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

      //fixed variance float
      float mog_var = -1.0f;
      bocl_mem_sptr mog_var_mem = new bocl_mem(device->context(), &mog_var, sizeof(mog_var), "update gauss variance");
      mog_var_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

      bool update_app = true;
      cl_int up_app[1];
      up_app[0] = update_app ? 1 : 0;
      bocl_mem_sptr up_app_mem = new bocl_mem(device->context(), up_app, sizeof(up_app), "update app bool buffer");
      up_app_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
      //workspace size
      std::size_t local_threads[2]  = {64,1};
      std::size_t global_threads[2] = {RoundUp(info_buffer->data_buffer_length,local_threads[0]), 1};

      //set args and execute
      kern->set_arg( blk_info );
      kern->set_arg( alpha );
      kern->set_arg( mog );
      kern->set_arg( num_obs );
      kern->set_arg( aux0 );
      kern->set_arg( aux1 );
      kern->set_arg( aux2 );
      kern->set_arg( aux3 );
      kern->set_arg( up_alpha_mem.ptr() );
      kern->set_arg( mog_var_mem.ptr() );
      kern->set_arg( up_app_mem.ptr() );
      kern->set_arg( out_imgs[i].ptr() );
      transfer_time += ttime.all();
      //execute kernel
      kern->execute(queues[i], 2, local_threads, global_threads);
      //int status = clFinish(queues[i]);
      kern->clear_args();

      //async reads
      //alpha->read_to_buffer(queues[i]);
      //mog->read_to_buffer(queues[i]);
      //num_obs->read_to_buffer(queues[i]);
    }

    //-------------------------------------------------
    //Finish this group before moving onto the next one
    //-------------------------------------------------
    for (unsigned int i=0; i<ids.size(); ++i) {
      clFinish(queues[i]);

      ttime.mark();
      boxm2_opencl_cache1* ocl_cache = ocl_caches[i];
      boxm2_block_id id = ids[i];

      //write alpha, mog and num obs to disk
      bocl_mem* alpha     = ocl_cache->get_data<BOXM2_ALPHA>(id,0,false);
      int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
      // check for invalid parameters
      if( alphaTypeSize == 0 ) //This should never happen, it will result in division by zero later
      {
        std::cerr << "ERROR: Division by 0 in " << __FILE__ << __LINE__ << std::endl;
        throw 0;
      }

      auto dataLen = (std::size_t) (alpha->num_bytes()/alphaTypeSize);
      bocl_mem* mog       = ocl_cache->get_data(id,data_type,dataLen*apptypesize,false);
      //numobs
      std::string num_obs_type = boxm2_data_traits<BOXM2_NUM_OBS>::prefix();
      int nobsTypeSize        = (int)boxm2_data_info::datasize(num_obs_type);
      bocl_mem* num_obs       = ocl_cache->get_data(id,num_obs_type,alpha->num_bytes()/alphaTypeSize*nobsTypeSize,false);

      //write info to disk
      alpha->read_to_buffer(queues[i]);
      mog->read_to_buffer(queues[i]);
      num_obs->read_to_buffer(queues[i]);
      transfer_time += ttime.all();
    }
  }

  //TODO keep track of execution time
  return transfer_time;
}


//-----------------------------------------------------------------
// returns vector of bocl_kernels for this specific device
//-----------------------------------------------------------------
std::vector<bocl_kernel*>& boxm2_multi_update_cell::get_kernels(const bocl_device_sptr& device, const std::string& opts)
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
  std::string options = opts + "";

  //push back cast_ray_bit
  auto* bayes_main = new bocl_kernel();
  std::string bayes_opt = options + " -D BAYES -D STEP_CELL=step_cell_bayes(aux_args,data_ptr,llid,d) ";
  bayes_main->create_kernel(&device->context(),device->device_id(), src_paths, "bayes_main", bayes_opt, "update::bayes_main");

  //may need DIFF LIST OF SOURCES FOR THSI GUY TOO
  auto* update = new bocl_kernel();
  std::string main_opts = options + " -D UPDATE_BIT_SCENE_MAIN ";
  update->create_kernel(&device->context(),device->device_id(), non_ray_src, "update_bit_scene_main", main_opts, "update::update_main");

  //vector of kernels:
  std::vector<bocl_kernel*> kerns(2);
  kerns[0] = bayes_main;
  kerns[1] = update;

  //cache in map
  kernels_[identifier] = kerns;
  return kernels_[identifier];
}
