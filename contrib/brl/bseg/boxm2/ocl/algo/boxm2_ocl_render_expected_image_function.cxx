 
#include <boxm2/ocl/algo/boxm2_ocl_render_expected_image_function.h>
#include <vul/vul_timer.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
void render_expected_image(bocl_device_sptr & device,
                              boxm2_scene_sptr & scene,
                              boxm2_opencl_cache_sptr & opencl_cache,
                              vpgl_camera_double_sptr & cam,
                              cl_command_queue & queue,
                              bocl_mem_sptr & exp_image,
                              bocl_mem_sptr & exp_img_dim,
                              vcl_string identifier,
                              vcl_string data_type,
                              vcl_map<vcl_string,vcl_vector<bocl_kernel*> > & kernels,
                              vcl_size_t * lthreads,
                              unsigned cl_ni,unsigned cl_nj)
  {
      float transfer_time=0.0f;
      float gpu_time=0.0f;
      //: create all buffers
      cl_float cam_buffer[48];
      boxm2_ocl_util::set_persp_camera(cam, cam_buffer);
      bocl_mem_sptr persp_cam=new bocl_mem(device->context(), cam_buffer, 3*sizeof(cl_float16), "persp cam buffer");
      persp_cam->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
      //: visibility image
      float* vis_buff = new float[cl_ni*cl_nj];
      for (unsigned i=0;i<cl_ni*cl_nj;i++) vis_buff[i]=1.0f;

      bocl_mem_sptr vis_image=new bocl_mem(device->context(),vis_buff,cl_ni*cl_nj*sizeof(float),"exp image buffer");
      vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

      //: Output Array
      float output_arr[100];
      for (int i=0; i<100; ++i) output_arr[i] = 0.0f;
      bocl_mem_sptr  cl_output=new bocl_mem(device->context(), output_arr, sizeof(float)*100, "output buffer");
      cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

      //: bit lookup buffer
      cl_uchar lookup_arr[256];
      boxm2_ocl_util::set_bit_lookup(lookup_arr);
      bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
      lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

      //2. set global thread size
      vcl_size_t gThreads[] = {cl_ni,cl_nj};

      //: set arguments
      vcl_vector<boxm2_block_id> vis_order = scene->get_vis_blocks( (vpgl_perspective_camera<double>*) cam.ptr());
      vcl_vector<boxm2_block_id>::iterator id;
      for (id = vis_order.begin(); id != vis_order.end(); ++id)
      {
          //choose correct render kernel
          boxm2_block_metadata mdata = scene->get_block_metadata(*id);
          bocl_kernel* kern =  kernels[identifier][0];

          //write the image values to the buffer
          vul_timer transfer;
          bocl_mem* blk       = opencl_cache->get_block(*id);
          bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(*id);
          bocl_mem* mog       = opencl_cache->get_data(*id,data_type);
          bocl_mem * blk_info  = opencl_cache->loaded_block_info();
          transfer_time += (float) transfer.all();

          ////3. SET args
          kern->set_arg( blk_info );
          kern->set_arg( blk );
          kern->set_arg( alpha );
          kern->set_arg( mog );
          kern->set_arg( persp_cam.ptr() );
          kern->set_arg( exp_image.ptr() );
          kern->set_arg( exp_img_dim.ptr());
          kern->set_arg( cl_output.ptr() );
          kern->set_arg( lookup.ptr() );
          kern->set_arg( vis_image.ptr() );

          //local tree , cumsum buffer, imindex buffer
          kern->set_local_arg( lthreads[0]*lthreads[1]*sizeof(cl_uchar16) );
          kern->set_local_arg( lthreads[0]*lthreads[1]*10*sizeof(cl_uchar) );
          kern->set_local_arg( lthreads[0]*lthreads[1]*sizeof(cl_int) );

          //execute kernel
          kern->execute(queue, 2, lthreads, gThreads);
          clFinish(queue);
          gpu_time += kern->exec_time();

          //clear render kernel args so it can reset em on next execution
          kern->clear_args();
      }
      // normalize
      {
          bocl_kernel* normalize_kern= kernels[identifier][1];
          normalize_kern->set_arg( exp_image.ptr() );
          normalize_kern->set_arg( vis_image.ptr() );
          normalize_kern->set_arg( exp_img_dim.ptr());
          normalize_kern->execute( queue, 2, lthreads, gThreads);
          clFinish(queue);
          gpu_time += normalize_kern->exec_time();

          //clear render kernel args so it can reset em on next execution
          normalize_kern->clear_args();
      }

      vcl_cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<vcl_endl;
  }