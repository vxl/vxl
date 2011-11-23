#include "boxm2_multi_update_cell.h"
#include <boxm2_multi_util.h>

#include <vcl_algorithm.h>
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


vcl_map<vcl_string, vcl_vector<bocl_kernel*> > boxm2_multi_update_cell::kernels_;


//-------------------------------------------------------------
// Stores seg len and observation in cell-level aux data
//-------------------------------------------------------------
float boxm2_multi_update_cell::update_cells(       boxm2_multi_cache&         cache, 
                                           const vil_image_view<float>&   img, 
                                                 vpgl_camera_double_sptr  cam,
                                                 vcl_map<bocl_device*, float*>& vis_map, 
                                                 vcl_map<bocl_device*, float*>& pre_map, 
                                                 float*                         norm_image )
{
  vcl_cout<<"------------ boxm2_multi_update_cell store aux--------------"<<vcl_endl;
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
  
  //----------------------------------------------------------------------
  // Map work onto each device , 
  // run beta calculating ray trace pass
  //----------------------------------------------------------------------
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
      vcl_cout<<"boxm2_multi_update_cell::store_aux unable to create command queue"<<vcl_endl;
      return 0.0f;
    }
    
    //workspace size
    unsigned cl_ni = RoundUp(ni,lthreads[0]);
    unsigned cl_nj = RoundUp(nj,lthreads[1]);

    //grab vis and pre images that correspond
    float* vis_img = vis_map[ device.ptr() ];
    float* pre_img = pre_map[ device.ptr() ];
    
    bocl_mem_sptr vis_mem  = new bocl_mem(device->context(), vis_img, sizeof(float)*ni*nj, "vis image buff");
    bocl_mem_sptr pre_mem  = new bocl_mem(device->context(), pre_img, sizeof(float)*ni*nj, "pre image buff");
    bocl_mem_sptr norm_mem = new bocl_mem(device->context(), norm_image ,sizeof(float)*ni*nj, "norm image buff");
    vis_mem->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    pre_mem->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    norm_mem->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  
    //create image dim buff
    int img_dim_buff[] = {0, 0, ni, nj}; 
    bocl_mem_sptr img_dim = new bocl_mem(device->context(), img_dim_buff, sizeof(int)*4, "image dims");
    img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    
    //store aux data
    calc_beta_scene(sub_scene, device, ocl_cache, queue, 
                    cam, vis_mem, pre_mem, norm_mem, img_dim, data_type, 
                    kerns[0], lthreads, cl_ni, cl_nj, apptypesize); 
  }

  //-------------------------------------------------------------------
  //finish execution along each queue (block c++ until all GPUS are done
  //-------------------------------------------------------------------
  for(int i=0; i<queues.size(); ++i) {
    clFinish(queues[i]); 
    clReleaseCommandQueue(queues[i]); 
  }
  
  //read all buffers in

  //-------------------------------------------------------------------
  // Reduce images into pre/vis image and make sure the interim 
  // pre/vis images are correct
  //-------------------------------------------------------------------
  calc_beta_reduce(cache, ocl_caches); 
  
  return 0.0f; 
}


float boxm2_multi_update_cell::calc_beta_reduce( boxm2_multi_cache& mcache, 
                                               vcl_vector<boxm2_opencl_cache*>& ocl_caches )
{
  //get total scene info first    
  vcl_string data_type, options;
  int apptypesize; 
  if( !boxm2_multi_util::get_scene_appearances(mcache.get_scene(), data_type, options, apptypesize) ) 
    return 0.0f; 
  
  vcl_vector<cl_command_queue> queues; 
  for(int i=0; i<ocl_caches.size(); ++i) {
    
    //grab sub scene and it's cache
    boxm2_opencl_cache*     opencl_cache = ocl_caches[i]; 
    boxm2_scene_sptr        sub_scene    = opencl_cache->get_scene(); 
    bocl_device_sptr        device       = opencl_cache->get_device(); 
    
    // compile the kernel/retrieve cached kernel for this device
    vcl_vector<bocl_kernel*>& kerns = get_kernels(device, options);
    bocl_kernel* kern = kerns[1]; 

    // create a command queue.
    int status=0;
    cl_command_queue queue = clCreateCommandQueue( device->context(),
                                                   *(device->device_id()),
                                                   CL_QUEUE_PROFILING_ENABLE,
                                                   &status );
    queues.push_back(queue); 
    if (status!=0) {
      vcl_cout<<"boxm2_multi_update_cell::store_aux unable to create command queue"<<vcl_endl;
      return 0.0f;
    }
    
    vcl_map<boxm2_block_id, boxm2_block_metadata>& blks = sub_scene->blocks();
    vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator iter; 
    for(iter = blks.begin(); iter != blks.end(); ++iter) 
    {
      boxm2_block_metadata mdata = iter->second; 
      boxm2_block_id       id    = iter->first; 

      //write the image values to the buffer
      vul_timer transfer;
      bocl_mem* blk       = opencl_cache->get_block(id);
      bocl_mem* blk_info  = opencl_cache->loaded_block_info();
      bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(id,0,false);
      boxm2_scene_info* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
      int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
      info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
      blk_info->write_to_buffer((queue));

      //grab mog
      bocl_mem* mog       = opencl_cache->get_data(id,data_type,alpha->num_bytes()/alphaTypeSize*apptypesize,false);    

      //numobs
      vcl_string num_obs_type = boxm2_data_traits<BOXM2_NUM_OBS>::prefix(); 
      int nobsTypeSize        = (int)boxm2_data_info::datasize(num_obs_type);
      bocl_mem* num_obs       = opencl_cache->get_data(id,num_obs_type,alpha->num_bytes()/alphaTypeSize*nobsTypeSize,false);

      //grab an appropriately sized AUX data buffer
      int auxTypeSize  = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
      bocl_mem *aux0   = opencl_cache->get_data<BOXM2_AUX0>(id, info_buffer->data_buffer_length*auxTypeSize);
      auxTypeSize      = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
      bocl_mem *aux1   = opencl_cache->get_data<BOXM2_AUX1>(id, info_buffer->data_buffer_length*auxTypeSize);
      auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
      bocl_mem *aux2   = opencl_cache->get_data<BOXM2_AUX2>(id, info_buffer->data_buffer_length*auxTypeSize);
      auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
      bocl_mem *aux3   = opencl_cache->get_data<BOXM2_AUX3>(id, info_buffer->data_buffer_length*auxTypeSize);

      // update_alpha boolean buffer
      bool update_alpha = true; 
      cl_int up_alpha[1];
      up_alpha[0] = update_alpha ? 1 : 0; 
      bocl_mem_sptr up_alpha_mem = new bocl_mem(device->context(), up_alpha, sizeof(up_alpha), "update alpha bool buffer");
      up_alpha_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

      // Output Array
      float output_arr[100];
      for (int i=0; i<100; ++i) output_arr[i] = 0.0f;
      bocl_mem_sptr  cl_output=new bocl_mem(device->context(), output_arr, sizeof(float)*100, "output buffer");
      cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

      //workspace size
      vcl_size_t local_threads[2]  = {64,1};
      vcl_size_t global_threads[2] = {RoundUp(info_buffer->data_buffer_length,local_threads[0]), 1};

      //set args and exectue
      kern->set_arg( blk_info );
      kern->set_arg( alpha );
      kern->set_arg( mog );
      kern->set_arg( num_obs );
      kern->set_arg( aux0 );
      kern->set_arg( aux1 );
      kern->set_arg( aux2 );
      kern->set_arg( aux3 );
      kern->set_arg( up_alpha_mem.ptr() );
      kern->set_arg( cl_output.ptr() );

      //execute kernel
      kern->execute(queue, 2, local_threads, global_threads);

      //clear render kernel args so it can reset em on next execution
      kern->clear_args();

      //write info to disk
      //alpha->read_to_buffer(queue);
      //mog->read_to_buffer(queue);
      //num_obs->read_to_buffer(queue);
    } 
  }
  
  //-------------------------------------------------------------------
  //finish execution along each queue (block c++ until all GPUS are done
  //-------------------------------------------------------------------
  for(int i=0; i<queues.size(); ++i) {
    clFinish(queues[i]); 
    clReleaseCommandQueue(queues[i]); 
  }
  
  //TODO need to read all GPU data into CPU somehow
  
}

//MAP function for store aux scene step
float boxm2_multi_update_cell::calc_beta_scene( boxm2_scene_sptr          scene,
                                              bocl_device_sptr          device,
                                              boxm2_opencl_cache*       opencl_cache,
                                              cl_command_queue &        queue,
                                              vpgl_camera_double_sptr & cam,
                                              bocl_mem_sptr &           vis_image,
                                              bocl_mem_sptr &           pre_image,
                                              bocl_mem_sptr &           norm_image,
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
    vcl_cout<<(*id);
    
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
    
    //get num obs
    vcl_string numObsType = boxm2_data_traits<BOXM2_NUM_OBS>::prefix(); 
    int nobsTypeSize = (int)boxm2_data_info::datasize(numObsType);
    bocl_mem* num_obs   = opencl_cache->get_data(*id,numObsType,alpha->num_bytes()/alphaTypeSize*nobsTypeSize,false);
    
    //get two aux - cell_vis and cell_beta
    int auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
    bocl_mem *aux0  = opencl_cache->get_data<BOXM2_AUX0>(*id, info_buffer->data_buffer_length*auxTypeSize);
    auxTypeSize     = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
    bocl_mem *aux1  = opencl_cache->get_data<BOXM2_AUX1>(*id, info_buffer->data_buffer_length*auxTypeSize);
    auxTypeSize     = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
    bocl_mem *aux2  = opencl_cache->get_data<BOXM2_AUX2>(*id, info_buffer->data_buffer_length*auxTypeSize);
    auxTypeSize     = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
    bocl_mem *aux3  = opencl_cache->get_data<BOXM2_AUX3>(*id, info_buffer->data_buffer_length*auxTypeSize);

    //prep aux2 and 3
    aux2->zero_gpu_buffer(queue);
    aux3->zero_gpu_buffer(queue);

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

    //clear render kernel args so it can reset em on next execution
    kern->clear_args();
    
    //TODO Figure out how to force aux2 and aux3 to read to buffer (if necessary)
    //aux2->read_to_buffer(queue);
    //aux3->read_to_buffer(queue);
  }

  //clean up cam
  delete[] ray_origins;
  delete[] ray_directions;
  return 0.0f;
}



//-----------------------------------------------------------------
// returns vector of bocl_kernels for this specific device
//-----------------------------------------------------------------
vcl_vector<bocl_kernel*>& boxm2_multi_update_cell::get_kernels(bocl_device_sptr device, vcl_string opts)
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
  vcl_string options = opts + " -D INTENSITY -D DETERMINISTIC ";

  //push back cast_ray_bit
  bocl_kernel* bayes_main = new bocl_kernel();
  vcl_string bayes_opt = options + " -D BAYES -D STEP_CELL=step_cell_bayes(aux_args,data_ptr,llid,d) ";
  bayes_main->create_kernel(&device->context(),device->device_id(), src_paths, "bayes_main", bayes_opt, "update::bayes_main");

  //may need DIFF LIST OF SOURCES FOR THSI GUY TOO
  bocl_kernel* update = new bocl_kernel();
  update->create_kernel(&device->context(),device->device_id(), non_ray_src, "update_bit_scene_main", options, "update::update_main");

  //vector of kernels:
  vcl_vector<bocl_kernel*> kerns(2); 
  kerns[0] = bayes_main; 
  kerns[1] = update; 

  //cache in map
  kernels_[identifier] = kerns; 
  return kernels_[identifier]; 
}

