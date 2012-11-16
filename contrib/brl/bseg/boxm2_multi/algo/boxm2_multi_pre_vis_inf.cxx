#include "boxm2_multi_pre_vis_inf.h"
#include <boxm2_multi_util.h>

#include <vcl_algorithm.h>
#include <vcl_sstream.h>
#include <vcl_iomanip.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <bocl/bocl_manager.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
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

vcl_map<vcl_string, vcl_vector<bocl_kernel*> > boxm2_multi_pre_vis_inf::kernels_;


//-------------------------------------------------------------
// pre_vis_inf
//-------------------------------------------------------------
float boxm2_multi_pre_vis_inf::pre_vis_inf( boxm2_multi_cache&              cache,
                                            const vil_image_view<float>&    img,
                                            vpgl_camera_double_sptr         cam,
                                            float*                          norm_img,
                                            boxm2_multi_update_helper&      helper)
{
  vcl_cout<<"  -- boxm2_pre_vis_inf map --"<<vcl_endl;
  //verify appearance model
  vcl_size_t lthreads[2] = {8,8};
  vcl_string data_type, options;
  int apptypesize;
  if ( !boxm2_multi_util::get_scene_appearances(cache.get_scene(), data_type, options, apptypesize) )
    return 0.0f;

  //setup image size
  int ni=img.ni(),
      nj=img.nj();
  unsigned cl_ni=RoundUp(ni,lthreads[0]);
  unsigned cl_nj=RoundUp(nj,lthreads[1]);
  vcl_size_t gThreads[] = {cl_ni,cl_nj};

  //vis inf and pre inf buffers
  float* visImg = new float[ni*nj];
  float* preImg = new float[ni*nj];
  vcl_fill(visImg, visImg+ni*nj, 1.0f);
  vcl_fill(preImg, preImg+ni*nj, 0.0f);

  //-------------------------------------------------------
  //prepare buffers for each device
  //-------------------------------------------------------
  vcl_vector<cl_command_queue>& queues = helper.queues_;
  vcl_vector<bocl_mem_sptr>& out_imgs = helper.outputs_,
                             img_dims = helper.img_dims_,
                             ray_ds = helper.ray_ds_,
                             ray_os = helper.ray_os_,
                             lookups = helper.lookups_;
  vcl_vector<boxm2_opencl_cache*>& ocl_caches = helper.vis_caches_;
  vcl_vector<bocl_mem_sptr> vis_mems, pre_mems, visInfMems, preInfMems;
  for (unsigned int i=0; i<ocl_caches.size(); ++i) {
    //grab sub scene and it's cache
    boxm2_opencl_cache* ocl_cache = ocl_caches[i];

    //pre/vis images
    float* vis_buff = new float[cl_ni*cl_nj];
    vcl_fill(vis_buff, vis_buff+cl_ni*cl_nj, 1.0f);
    bocl_mem_sptr vis_image = ocl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), vis_buff,"vis image buffer");
    vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    vis_mems.push_back(vis_image);

    float* pre_buff = new float[cl_ni*cl_nj];
    vcl_fill(pre_buff, pre_buff+cl_ni*cl_nj, 0.0f);
    bocl_mem_sptr pre_image = ocl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), pre_buff,"pre image buffer");
    pre_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    pre_mems.push_back(pre_image);
  }

  //initialize per group images (vis/pre)
  vcl_vector<boxm2_multi_cache_group*> grp = helper.group_orders_; //cache.get_vis_groups(cam);
  vul_timer t; t.mark();
  float gpu_time = 0.0f, cpu_time = 0.0f;

  //----------------------------------------------------------------
  // Call per block/per scene update (to ensure cpu-> gpu cache works
  //---------------------------------------------------------------
  for (unsigned int grpId=0; grpId<grp.size(); ++grpId)
  {
    boxm2_multi_cache_group& group = *grp[grpId];
    vcl_vector<boxm2_block_id>& ids = group.ids();
    vcl_vector<int> indices = group.order_from_cam(cam);
    for (unsigned int idx=0; idx<indices.size(); ++idx) {
      int i = indices[idx];
      //grab sub scene and it's cache
      boxm2_opencl_cache* ocl_cache = ocl_caches[i];
      boxm2_scene_sptr    sub_scene = ocl_cache->get_scene();
      bocl_device_sptr    device    = ocl_cache->get_device();

      // compile the kernel/retrieve cached kernel for this device
      vcl_vector<bocl_kernel*> kerns = get_kernels(device, options);

      //Run block store aux
      boxm2_block_id id = ids[i]; //vis_order[blk];

      //set visibility to one, set pre to zero
      vis_mems[i]->fill(queues[i], 1.0f, "float");
      pre_mems[i]->zero_gpu_buffer(queues[i]);
      cpu_time += pre_vis_per_block(id, sub_scene, ocl_cache, queues[i], data_type, kerns[0],
                                    vis_mems[i], pre_mems[i], img_dims[i],
                                    ray_os[i], ray_ds[i],
                                    out_imgs[i], lookups[i], lthreads, gThreads);
    }

    //finish queues before moving on
    for (unsigned int idx=0; idx<indices.size(); ++idx) {
      int i = indices[idx];

#if 1
      //find the minU,minV, maxU,maxV points
      vul_timer cpuTimer; cpuTimer.mark();
      double minU=ni, minV=nj,
             maxU=0, maxV=0;
      vgl_box_3d<double>& blkBox = group.bbox(i);
      vcl_vector<vgl_point_3d<double> > verts = blkBox.vertices();
      for (unsigned int vi=0; vi<verts.size(); ++vi) {
        double u, v;
        cam->project(verts[vi].x(), verts[vi].y(), verts[vi].z(), u, v);
        if (u < minU) minU = u;
        if (u > maxU) maxU = u;
        if (v < minV) minV = v;
        if (v > maxV) maxV = v;
      }
      //make sure you clamp the value between
      minU = clamp(minU, 0.0, (double) ni);
      maxU = clamp(maxU, 0.0, (double) ni);
      maxV = clamp(maxV, 0.0, (double) nj);
      minV = clamp(minV, 0.0, (double) nj);

      //first store vis/pre images for first member of group
      vcl_memcpy(group.get_vis(i), visImg, ni*nj*sizeof(float));
      vcl_memcpy(group.get_pre(i), preImg, ni*nj*sizeof(float));

      //next update the vis and pre images
      clFinish(queues[i]);
#if 0 // gpu_time will be overwritten after this for loop
      if (idx+1 == indices.size())
        gpu_time += t.all();
#endif
      vis_mems[i]->read_to_buffer(queues[i]);
      pre_mems[i]->read_to_buffer(queues[i]);
      float* v = (float*) vis_mems[i]->cpu_buffer();
      float* p = (float*) pre_mems[i]->cpu_buffer();
      for (int jj=(int)minV; jj<(int)maxV; ++jj)
        for (int ii=(int)minU; ii<(int)maxU; ++ii) {
          int idx = jj*ni + ii;
          preImg[idx]  = preImg[idx] + p[idx]*visImg[idx];
          visImg[idx] *= v[idx];
        }
      cpu_time += cpuTimer.all();
#else
      //first store vis/pre images for first member of group
      float* p = (float*) preInfMems[i]->enqueue_map(queues[i]);
      float* v = (float*) visInfMems[i]->enqueue_map(queues[i]);
      clFinish(queues[i]);
      vcl_memcpy(group.get_vis(i), v/*visImg*/, ni*nj*sizeof(float));
      vcl_memcpy(group.get_pre(i), p/*preImg*/, ni*nj*sizeof(float));

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
  for (int c=0; c<ni*nj; ++c)
    norm_img[c] = visImg[c] + preImg[c];

  //grab accurate GPU time (includes transfers)
  cpu_time += t.all();
  gpu_time -= cpu_time;

#if 0
  vil_image_view<float> nimg(ni,nj), vimg(ni,nj), pimg(ni,nj);
  int c=0;
  for (int j=0; j<nj; ++j)
    for (int i=0; i<ni; ++i) {
      nimg(i,j) = norm_img[c];
      vimg(i,j) = visImg[c];
      pimg(i,j) = preImg[c];
      c++;
    }
  vil_save(nimg, "norm_image.tiff");
  vil_save(vimg, "vis_image.tiff");
  vil_save(pimg, "pre_image.tiff");
#endif

#if 0
  //------------------
  //read all images in
  //vcl_vector<float*> pre_imgs, vis_imgs;
  //for (unsigned int i=0; i<vis_mems.size(); ++i) {
  //  // read out expected image
  //  pre_mems[i]->read_to_buffer(queues[i]);
  //  vis_mems[i]->read_to_buffer(queues[i]);

  //  //populate vil_image_views for combination
  //  pre_imgs.push_back( (float*) pre_mems[i]->cpu_buffer());
  //  vis_imgs.push_back( (float*) vis_mems[i]->cpu_buffer());
  //}

  //-------------------------------------------------------------------
  // Reduce images into pre/vis image and make sure the interim
  // pre/vis images are correct
  //-------------------------------------------------------------------
  //pre_vis_reduce(cache, pre_imgs, vis_imgs, ocl_caches,
  //               pre_map, vis_map, ni, nj, norm_img);
#endif

  //-------------------------------------
  //clean up
  //-------------------------------------
  delete[] visImg;
  delete[] preImg;
  for (unsigned int i=0; i<queues.size(); ++i) {
    boxm2_opencl_cache* ocl_cache = ocl_caches[i];
    float* v = (float*) vis_mems[i]->cpu_buffer();
    float* p = (float*) pre_mems[i]->cpu_buffer();
    delete[] v;
    delete[] p;

    //free vis mem, pre mem
    ocl_cache->unref_mem(vis_mems[i].ptr());
    ocl_cache->unref_mem(pre_mems[i].ptr());
  }
  return gpu_time;
}


float boxm2_multi_pre_vis_inf::pre_vis_per_block(const boxm2_block_id& id,
                                                 boxm2_scene_sptr      scene,
                                                 boxm2_opencl_cache*   opencl_cache,
                                                 cl_command_queue&     queue,
                                                 vcl_string            data_type,
                                                 bocl_kernel*          kern,
                                                 bocl_mem_sptr&        vis_image,
                                                 bocl_mem_sptr&        pre_image,
                                                 bocl_mem_sptr&        img_dim,
                                                 bocl_mem_sptr&        ray_o_buff,
                                                 bocl_mem_sptr&        ray_d_buff,
                                                 bocl_mem_sptr&        cl_output,
                                                 bocl_mem_sptr&        lookup,
                                                 vcl_size_t*           lthreads,
                                                 vcl_size_t*           gThreads)
{
  vul_timer ttime; ttime.mark();

  //choose correct render kernel
  boxm2_block_metadata mdata = scene->get_block_metadata(id);

  //write the image values to the buffer
  bocl_mem* blk       = opencl_cache->get_block(id);
  bocl_mem* blk_info  = opencl_cache->loaded_block_info();
  bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(id,0,false);

  //calc data buffer length (write it in blk_info)
  vcl_size_t dataLen = (vcl_size_t) (alpha->num_bytes()/boxm2_data_traits<BOXM2_ALPHA>::datasize());
  boxm2_scene_info* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
  info_buffer->data_buffer_length = (int) dataLen;
  blk_info->write_to_buffer(queue);

  //grab MOG
  int apptypesize = (int) boxm2_data_info::datasize(data_type);
  bocl_mem* mog  = opencl_cache->get_data(id,data_type, dataLen*apptypesize,false);

  //grab NumObs
  vcl_string numObsType = boxm2_data_traits<BOXM2_NUM_OBS>::prefix();
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

#if 0
float boxm2_multi_pre_vis_inf::pre_vis_reduce(boxm2_multi_cache&  cache,
                                              vcl_vector<float*>& pre_imgs,
                                              vcl_vector<float*>& vis_imgs,
                                              vcl_vector<boxm2_opencl_cache*>& ocl_caches ,
                                              vcl_map<bocl_device*, float*>& pre_map,
                                              vcl_map<bocl_device*, float*>& vis_map,
                                              int ni,
                                              int nj,
                                              float* norm_img )
{
  //------------------------------------------------------------------------
  // combine images - need to ensure that images along the way are correct
  //------------------------------------------------------------------------
  for (unsigned int idx=0; idx<pre_imgs.size(); ++idx) {
    if (idx > 0) {
      float* prev_vis = vis_imgs[idx-1];
      float* prev_pre = pre_imgs[idx-1];

      float* v = vis_imgs[idx];
      float* p = pre_imgs[idx];

      //make sure v and p are updated
      for (int c=0; c<ni*nj; ++c) {
        v[c] *= prev_vis[c];
        p[c]  = p[c]*prev_vis[c] + prev_pre[c];
      }
    }
  }

  //create a map from device to
  float* vis0 = new float[ni*nj]; vcl_fill(vis0, vis0+ni*nj, 1.0f);
  float* pre0 = new float[ni*nj]; vcl_fill(pre0, pre0+ni*nj, 0.0f);

  //store dev0
  bocl_device_sptr dev0 = ocl_caches[0]->get_device();
  vis_map[dev0.ptr()] = vis0;
  pre_map[dev0.ptr()] = pre0;
  for (unsigned int idx=1; idx<ocl_caches.size(); ++idx) {
    boxm2_opencl_cache*     ocl_cache = ocl_caches[idx];
    boxm2_scene_sptr        sub_scene = ocl_cache->get_scene();
    bocl_device_sptr        device    = ocl_cache->get_device();

    vis_map[ device.ptr() ] = vis_imgs[idx-1];
    pre_map[ device.ptr() ] = pre_imgs[idx-1];
  }

  //--------------------------------------------
  //run proc_norm image to create norm image
  //--------------------------------------------
  if (ocl_caches.size() <= 0)
    return -1.0f;

  //grab sub scene and it's cache
  boxm2_opencl_cache*     ocl_cache = ocl_caches[0];
  boxm2_scene_sptr        sub_scene = ocl_cache->get_scene();
  bocl_device_sptr        device    = ocl_cache->get_device();

  //get scene options
  vcl_string data_type, options;
  int apptypesize;
  if ( !boxm2_multi_util::get_scene_appearances(cache.get_scene(), data_type, options, apptypesize) )
    return 0.0f;
  vcl_vector<bocl_kernel*>& kerns = get_kernels(device, options);
  bocl_kernel* proc_kern = kerns[1];

  //-- create cmd queue
  int status=0;
  cl_command_queue queue = clCreateCommandQueue( device->context(),
                                                 *(device->device_id()),
                                                 CL_QUEUE_PROFILING_ENABLE,
                                                 &status );
  if (status!=0) {
    vcl_cout<<"boxm2_multi_pre_vis_inf::store_aux unable to create command queue"<<vcl_endl;
    return 0.0f;
  }

  //create buffers
  bocl_mem_sptr norm_mem = ocl_cache->alloc_mem(sizeof(float)*ni*nj, norm_img, "norm mem buffer");
  norm_mem->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  float* pre_end = pre_imgs[ pre_imgs.size()-1 ];
  bocl_mem_sptr pre_mem = ocl_cache->alloc_mem(sizeof(float)*ni*nj, pre_end, "pre mem buffer");
  pre_mem->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  float* vis_end = vis_imgs[ pre_imgs.size()-1 ];
  bocl_mem_sptr vis_mem = ocl_cache->alloc_mem(sizeof(float)*ni*nj, vis_end, "vis mem buffer");
  vis_mem->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // Image Dimensions
  int img_dim_buff[4] = {0, 0, ni, nj};
  bocl_mem_sptr img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(int)*4, "image dims");
  img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //set args and execute
  proc_kern->set_arg( norm_mem.ptr() );
  proc_kern->set_arg( vis_mem.ptr() );
  proc_kern->set_arg( pre_mem.ptr() );
  proc_kern->set_arg( img_dim.ptr() );

  //execute kernel
  vcl_size_t lthreads[2] = {8,8};
  vcl_size_t gThreads[] = { RoundUp(ni,lthreads[0]), RoundUp(nj,lthreads[1]) };
  proc_kern->execute( queue, 2, lthreads, gThreads);
  status = clFinish(queue);
  check_val(status, MEM_FAILURE, "PROC NORM KERNEL EXECUTE FAILED: " + error_to_string(status));
  proc_kern->clear_args();
  norm_mem->read_to_buffer(queue);

  //clean up buffers
  //last image is not used, clean up
  delete[] vis_imgs[ocl_caches.size()-1];
  delete[] pre_imgs[ocl_caches.size()-1];
  ocl_cache->unref_mem(norm_mem.ptr());
  ocl_cache->unref_mem(pre_mem.ptr());
  ocl_cache->unref_mem(vis_mem.ptr());
}
#endif


//-----------------------------------------------------------------
// returns vector of bocl_kernels for this specific device
//-----------------------------------------------------------------
vcl_vector<bocl_kernel*>& boxm2_multi_pre_vis_inf::get_kernels(bocl_device_sptr device, vcl_string opts)
{
  // check to see if this device has compiled kernels already
  vcl_string identifier = device->device_identifier()+opts;
  if (kernels_.find(identifier) != kernels_.end())
    return kernels_[identifier];

  //if not, compile and cache them
  vcl_cout<<"===========Compiling multi update kernels===========\n"
          <<"  for device: "<<device->device_identifier()<<vcl_endl;

  //gather all render sources... seems like a lot for rendering...
  vcl_vector<vcl_string> src_paths;
  vcl_string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "cell_utils.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "backproject.cl");
  src_paths.push_back(source_dir + "statistics_library_functions.cl");
  src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
  src_paths.push_back(source_dir + "bit/update_kernels.cl");

  vcl_vector<vcl_string> non_ray_src = vcl_vector<vcl_string>(src_paths);
  src_paths.push_back(source_dir + "update_functors.cl");
  src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

  //compilation options
  vcl_string options = opts+" -D INTENSITY  ";
  options += " -D DETERMINISTIC ";

  //create all passes
  bocl_kernel* pre_inf = new bocl_kernel();
  vcl_string pre_opts = options + " -D PREINF -D STEP_CELL=step_cell_preinf(aux_args,data_ptr,llid,d) ";
  pre_inf->create_kernel(&device->context(),device->device_id(), src_paths, "pre_inf_main", pre_opts, "update::pre_inf");

  //may need DIFF LIST OF SOURCES FOR THIS GUY
  bocl_kernel* proc_img = new bocl_kernel();
  vcl_string proc_opts = options + " -D PROC_NORM ";
  proc_img->create_kernel(&device->context(),device->device_id(), non_ray_src, "proc_norm_image", proc_opts, "update::proc_norm_image");

  bocl_kernel* combine_pre_vis = new bocl_kernel();
  vcl_string comb_opts = options + " -D COMBINE_PRE_VIS ";
  combine_pre_vis->create_kernel(&device->context(), device->device_id(), non_ray_src, "combine_pre_vis", comb_opts, "update::combine_pre_vis");

  //vector of kernels:
  vcl_vector<bocl_kernel*> kerns(3);
  kerns[0] = pre_inf;
  kerns[1] = proc_img;
  kerns[2] = combine_pre_vis;

  //cache in map
  kernels_[identifier] = kerns;
  return kernels_[identifier];
}


void boxm2_multi_pre_vis_inf::write_imgs_out(vcl_map<bocl_device*, float*>& img_map, int ni, int nj, vcl_string name)
{
  vcl_map<bocl_device*, float*>::iterator iter;
  for (iter=img_map.begin(); iter!=img_map.end(); ++iter) {
    float* img = iter->second;
    int count=0;
    vil_image_view<float> outImg(ni,nj);
    for (int j=0; j<nj; ++j)
      for (int i=0; i<ni; ++i)
        outImg(i,j) = img[count++];

    vcl_string outName = name + iter->first->device_identifier() + ".tiff";
    vil_save(outImg, outName.c_str());
  }
}
