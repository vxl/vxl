#include "boxm2_multi_pre_vis_inf.h"
#include <boxm2_multi_util.h>

#include <vcl_algorithm.h>
#include <vcl_sstream.h>
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
#include <vil/vil_image_view_base.h>
#include <vil/vil_save.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vnl/vnl_random.h>
#include <vul/vul_timer.h>

vcl_map<vcl_string, vcl_vector<bocl_kernel*> > boxm2_multi_pre_vis_inf::kernels_;


//-------------------------------------------------------------
// Stores seg len and observation in cell-level aux data
//-------------------------------------------------------------
float boxm2_multi_pre_vis_inf::pre_vis_inf(       boxm2_multi_cache&         cache, 
                                          const vil_image_view<float>&   img, 
                                                vpgl_camera_double_sptr  cam,
                                                vcl_map<bocl_device*, float*>& vis_map, 
                                                vcl_map<bocl_device*, float*>& pre_map, 
                                                float*                         norm_img )
{
  vcl_cout<<"------------ boxm2_multi_pre_vis_inf pre_vis_inf--------------"<<vcl_endl;
  //verify appearance model
  vcl_size_t lthreads[2] = {8,8};
  vcl_string data_type, options;
  int apptypesize; 
  if( !boxm2_multi_util::get_scene_appearances(cache.get_scene(), data_type, options, apptypesize) ) 
    return 0.0f; 
   
  //setup image size
  int ni=img.ni(), 
      nj=img.nj();

  //set up image lists
  vcl_vector<cl_command_queue> queues; 
  vcl_vector<bocl_mem_sptr> vis_mems, pre_mems; 
  
  //for each device/cache, run an update
  vcl_vector<boxm2_opencl_cache*>  ocl_caches = cache.get_vis_sub_scenes( (vpgl_perspective_camera<double>*) cam.ptr());
  for(int i=0; i<ocl_caches.size(); ++i) {
    
    //grab sub scene and it's cache
    boxm2_opencl_cache*     ocl_cache = ocl_caches[i]; 
    boxm2_scene_sptr        sub_scene = ocl_cache->get_scene(); 
    bocl_device_sptr        device    = ocl_cache->get_device(); 
    
    // compile the kernel/retrieve cached kernel for this device
    vcl_vector<bocl_kernel*>& kerns = get_kernels(device, options);

    // create a command queue.
    int status=0;
    cl_command_queue queue = clCreateCommandQueue( device->context(),
                                                   *(device->device_id()),
                                                   CL_QUEUE_PROFILING_ENABLE,
                                                   &status );
    queues.push_back(queue); 
    if (status!=0) {
      vcl_cout<<"boxm2_multi_pre_vis_inf::store_aux unable to create command queue"<<vcl_endl;
      return 0.0f;
    }

    //prepare kernel variables
    //create image and workspace size
    unsigned cl_ni=RoundUp(ni,lthreads[0]);
    unsigned cl_nj=RoundUp(nj,lthreads[1]);
    float* vis_buff = new float[cl_ni*cl_nj];
    vcl_fill(vis_buff, vis_buff+cl_ni*cl_nj, 1.0f); 
    bocl_mem_sptr vis_image = new bocl_mem(device->context(),vis_buff, cl_ni*cl_nj*sizeof(float), "vis image buffer");
    vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    vis_mems.push_back(vis_image);
    
    float* pre_buff = new float[cl_ni*cl_nj];
    vcl_fill(pre_buff, pre_buff+cl_ni*cl_nj, 0.0f); 
    bocl_mem_sptr pre_image = new bocl_mem(device->context(),pre_buff, cl_ni*cl_nj*sizeof(float), "pre image buffer");
    pre_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    pre_mems.push_back(pre_image);

    //create image dim buff
    int img_dim_buff[4] = {0, 0, ni, nj}; 
    bocl_mem_sptr img_dim = new bocl_mem(device->context(), img_dim_buff, sizeof(int)*4, "image dims");
    img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    
    //store aux data
    pre_vis_scene( sub_scene, device, ocl_cache, queue, 
                   cam, vis_image, pre_image, img_dim, data_type, 
                   kerns[0], lthreads, cl_ni, cl_nj, apptypesize); 
  }

  //-------------------------------------------------------------------
  //finish execution along each queue (block c++ until all GPUS are done
  //-------------------------------------------------------------------
  for(int i=0; i<queues.size(); ++i)
    clFinish(queues[i]); 
  
  //read all images in
  vcl_vector<float*> pre_imgs, vis_imgs; 
  for(int i=0; i<vis_mems.size(); ++i) {
    // read out expected image
    pre_mems[i]->read_to_buffer(queues[i]);
    vis_mems[i]->read_to_buffer(queues[i]);
    
    //populate vil_image_views for combination
    pre_imgs.push_back( (float*) pre_mems[i]->cpu_buffer()); 
    vis_imgs.push_back( (float*) vis_mems[i]->cpu_buffer()); 
  }
  
  //release these queues
  for(int i=0; i<queues.size(); ++i)
    clReleaseCommandQueue(queues[i]); 

  //-------------------------------------------------------------------
  // Reduce images into pre/vis image and make sure the interim 
  // pre/vis images are correct
  //-------------------------------------------------------------------
  pre_vis_reduce(cache, pre_imgs, vis_imgs, ocl_caches, pre_map, vis_map, ni, nj, norm_img); 
  
  return 0.0f; 
}


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
  for(int idx=0; idx<pre_imgs.size(); ++idx) {
    if(idx > 0) {
      float* prev_vis = vis_imgs[idx-1]; 
      float* prev_pre = pre_imgs[idx-1]; 
      
      float* v = vis_imgs[idx]; 
      float* p = pre_imgs[idx];
      
      //make sure v and p are updated
      for(int c=0; c<ni*nj; ++c) {
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
  for(int idx=1; idx<ocl_caches.size(); ++idx) {
    vcl_cout<<"adding image for cache "<<idx<<vcl_endl;
    boxm2_opencl_cache*     ocl_cache = ocl_caches[idx]; 
    boxm2_scene_sptr        sub_scene = ocl_cache->get_scene(); 
    bocl_device_sptr        device    = ocl_cache->get_device(); 
      
    vis_map[ device.ptr() ] = vis_imgs[idx-1]; 
    pre_map[ device.ptr() ] = pre_imgs[idx-1]; 
  }

#if 0
  write_imgs_out(vis_map, ni, nj, "vis");
  write_imgs_out(pre_map, ni, nj, "pre");
#endif

  //--------------------------------------------  
  //run proc_norm image to create norm image
  //--------------------------------------------
  if(ocl_caches.size() <= 0)
    return -1.0f; 
  
  //grab sub scene and it's cache
  boxm2_opencl_cache*     ocl_cache = ocl_caches[0]; 
  boxm2_scene_sptr        sub_scene = ocl_cache->get_scene(); 
  bocl_device_sptr        device    = ocl_cache->get_device(); 
  
  //get scene options
  vcl_string data_type, options;
  int apptypesize; 
  if( !boxm2_multi_util::get_scene_appearances(cache.get_scene(), data_type, options, apptypesize) ) 
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
  bocl_mem_sptr norm_mem = new bocl_mem(device->context(), norm_img, sizeof(float)*ni*nj, "norm mem buffer");
  norm_mem->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  float* pre_end = pre_imgs[ pre_imgs.size()-1 ]; 
  bocl_mem_sptr pre_mem = new bocl_mem(device->context(), pre_end, sizeof(float)*ni*nj, "pre mem buffer");
  pre_mem->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  float* vis_end = vis_imgs[ pre_imgs.size()-1 ];  
  bocl_mem_sptr vis_mem = new bocl_mem(device->context(), vis_end, sizeof(float)*ni*nj, "vis mem buffer");
  vis_mem->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // Image Dimensions
  int img_dim_buff[4] = {0, 0, ni, nj}; 
  bocl_mem_sptr img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(int)*4, "image dims");
  img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //set args and execute
  proc_kern->set_arg( norm_mem.ptr() );
  proc_kern->set_arg( vis_mem.ptr() );
  proc_kern->set_arg( pre_mem.ptr());
  proc_kern->set_arg( img_dim.ptr() );

  //execute kernel
  vcl_size_t lthreads[2] = {8,8};
  vcl_size_t gThreads[] = { RoundUp(ni,lthreads[0]), RoundUp(nj,lthreads[1]) }; 
  proc_kern->execute( queue, 2, lthreads, gThreads);
  status = clFinish(queue);
  check_val(status, MEM_FAILURE, "PROC NORM KERNEL EXECUTE FAILED: " + error_to_string(status));
  proc_kern->clear_args();
  norm_mem->read_to_buffer(queue);
  
#if 0
  int count=0;
  vil_image_view<float> visInf(ni,nj), preInf(ni,nj), norm(ni,nj);
  for(int j=0; j<nj; ++j)
    for(int i=0; i<ni; ++i) {
      visInf(i,j) = vis_end[count]; 
      preInf(i,j) = pre_end[count]; 
      norm(i,j)   = norm_img[count];
      count++;
    }
  vil_save(visInf, "visInf.tiff");
  vil_save(preInf, "preInf.tiff");
  vil_save(norm, "norm.tiff");
#endif
  
}

//MAP function for store aux scene step
float boxm2_multi_pre_vis_inf::pre_vis_scene(boxm2_scene_sptr           scene,
                                            bocl_device_sptr          device,
                                            boxm2_opencl_cache*       opencl_cache,
                                            cl_command_queue &        queue,
                                            vpgl_camera_double_sptr & cam,
                                            bocl_mem_sptr &           vis_image,
                                            bocl_mem_sptr &           pre_image,
                                            bocl_mem_sptr &           img_dim,
                                            vcl_string                data_type,
                                            bocl_kernel*              kern,
                                            vcl_size_t *              lThreads,
                                            unsigned                  cl_ni,
                                            unsigned                  cl_nj,
                                            int                       apptypesize )
{
  //camera check
  if (cam->type_name()!= "vpgl_perspective_camera" &&
      cam->type_name()!= "vpgl_generic_camera" ) {
    vcl_cout<<"Cannot render with camera of type "<<cam->type_name()<<vcl_endl;
    return 0.0f;
  }

  //set generic cam and get visible block order
  cl_float* ray_origins    = new cl_float[4*cl_ni*cl_nj];
  cl_float* ray_directions = new cl_float[4*cl_ni*cl_nj];
  bocl_mem_sptr ray_o_buff = new bocl_mem(device->context(), ray_origins   ,  cl_ni*cl_nj * sizeof(cl_float4), "ray_origins buffer");
  bocl_mem_sptr ray_d_buff = new bocl_mem(device->context(), ray_directions,  cl_ni*cl_nj * sizeof(cl_float4), "ray_directions buffer");
  boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

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

  //2. set global thread size
  vcl_size_t gThreads[] = {cl_ni,cl_nj};

  // set arguments
  vcl_vector<boxm2_block_id> vis_order = scene->get_vis_blocks(cam);
  vcl_vector<boxm2_block_id>::iterator id;
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
    //vcl_cout<<(*id);
    
    //choose correct render kernel
    boxm2_block_metadata mdata = scene->get_block_metadata(*id);

    //write the image values to the buffer
    bocl_mem* blk       = opencl_cache->get_block(*id);
    bocl_mem* blk_info  = opencl_cache->loaded_block_info();
    bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(*id,0,false);
    
    //calc data buffer length
    boxm2_scene_info* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
    int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
    info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
    blk_info->write_to_buffer((queue));
    bocl_mem* mog       = opencl_cache->get_data(*id,data_type,alpha->num_bytes()/alphaTypeSize*apptypesize,false);    
    
    vcl_string numObsType = boxm2_data_traits<BOXM2_NUM_OBS>::prefix(); 
    int nobsTypeSize = (int)boxm2_data_info::datasize(numObsType);
    bocl_mem* num_obs   = opencl_cache->get_data(*id,numObsType,alpha->num_bytes()/alphaTypeSize*nobsTypeSize,false);
    
    //grab an appropriately sized AUX data buffer
    int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
    bocl_mem *aux0   = opencl_cache->get_data<BOXM2_AUX0>(*id, info_buffer->data_buffer_length*auxTypeSize);
    auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
    bocl_mem *aux1   = opencl_cache->get_data<BOXM2_AUX1>(*id, info_buffer->data_buffer_length*auxTypeSize);
    
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
    kern->set_local_arg( lThreads[0]*lThreads[1]*sizeof(cl_uchar16) );//local tree,
    kern->set_local_arg( lThreads[0]*lThreads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer
    //execute kernel
    kern->execute(queue, 2, lThreads, gThreads);
    
    //int status = clFinish(queue);
    //check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
    //gpu_time += kern->exec_time();

    //clear render kernel args so it can reset em on next execution
    kern->clear_args();
  }

  //clean up cam
  delete[] ray_origins;
  delete[] ray_directions;
  return 0.0f;
}



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
  proc_img->create_kernel(&device->context(),device->device_id(), non_ray_src, "proc_norm_image", options, "update::proc_norm_image");

  //vector of kernels:
  vcl_vector<bocl_kernel*> kerns(2); 
  kerns[0] = pre_inf; 
  kerns[1] = proc_img; 

  //cache in map
  kernels_[identifier] = kerns; 
  return kernels_[identifier]; 
}


void boxm2_multi_pre_vis_inf::write_imgs_out(vcl_map<bocl_device*, float*>& img_map, int ni, int nj, vcl_string name)
{
  vcl_map<bocl_device*, float*>::iterator iter; 
  for(iter=img_map.begin(); iter!=img_map.end(); ++iter) {
    float* img = iter->second; 
    int count=0; 
    vil_image_view<float> outImg(ni,nj); 
    for(int j=0; j<nj; ++j)
      for(int i=0; i<ni; ++i)
        outImg(i,j) = img[count++]; 
        
    vcl_string outName = name + iter->first->device_identifier() + ".tiff"; 
    vil_save(outImg, outName.c_str());
  }
  
}
