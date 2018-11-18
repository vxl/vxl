#include "bstm_ocl_render_expected_image_function.h"
//
#include <vul/vul_timer.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <bstm/ocl/bstm_ocl_util.h>
#include <vsph/vsph_camera_bounds.h>
#include <vgl/vgl_ray_3d.h>
#include <boct/boct_bit_tree.h>


float render_expected_image(  bstm_scene_sptr & scene,
                                bocl_device_sptr & device,
                                bstm_opencl_cache_sptr & opencl_cache,
                                cl_command_queue & queue,
                                vpgl_camera_double_sptr & cam,
                                bocl_mem_sptr & exp_image,
                                bocl_mem_sptr & vis_image,
                                bocl_mem_sptr & exp_img_dim,
                                const std::string& data_type,
                                bocl_kernel* kernel,
                                std::size_t * lthreads,
                                unsigned cl_ni,
                                unsigned cl_nj,
                                int apptypesize,
                                float time,
                                const std::string& label_data_type,
                                int label_apptypesize,
                                bool render_label)
  {
    float transfer_time=0.0f;
    float gpu_time=0.0f;

    //camera check
    if (cam->type_name()!= "vpgl_perspective_camera" &&  cam->type_name()!= "vpgl_generic_camera" ) {
      std::cout<<"Cannot render with camera of type "<<cam->type_name()<<std::endl;
      return 0.0f;
    }

    std::cout << "TIME: " << time << std::endl;
    // create all buffers
    auto* ray_origins = new cl_float[4*cl_ni*cl_nj];
    auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
    bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins, "ray_origins buffer");
    bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions, "ray_directions buffer");
    boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

    // Output Array
    float output_arr[100];
    for (float & i : output_arr) i = 0.0f;
    bocl_mem_sptr  cl_output=new bocl_mem(device->context(), output_arr, sizeof(float)*100, "output buffer");
    cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // bit lookup buffer
    cl_uchar lookup_arr[256];
    bstm_ocl_util::set_bit_lookup(lookup_arr);
    bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
    lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    //render label or not?
    cl_int render_label_buf[1];
    render_label_buf[0] = render_label ? 1 : 0;
    bocl_mem_sptr render_label_mem = new bocl_mem(device->context(), render_label_buf, sizeof(cl_int), "render label?");
    render_label_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    cl_float cl_time = 0;
    bocl_mem_sptr time_mem =new bocl_mem(device->context(), &cl_time, sizeof(cl_float), "time instance buffer");
    time_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    //2. set global thread size
    std::size_t gThreads[] = {cl_ni,cl_nj};

    // set arguments
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

        //write cl_time
        cl_time = (cl_float)local_time;
        time_mem->write_to_buffer(queue);


        bocl_kernel* kern =  kernel;

        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk       = opencl_cache->get_block(*id);
        bocl_mem* blk_t     = opencl_cache->get_time_block(*id);
        bocl_mem* blk_info  = opencl_cache->loaded_block_info();


        bocl_mem* alpha     = opencl_cache->get_data<BSTM_ALPHA>(*id);
        int alphaTypeSize   = (int)bstm_data_info::datasize(bstm_data_traits<BSTM_ALPHA>::prefix());
        // data type string may contain an identifier so determine the buffer size
        bocl_mem* mog       = opencl_cache->get_data(*id,data_type,alpha->num_bytes()/alphaTypeSize*apptypesize,false);

        //if rendering label, actually get the data for it, otherwise don't bother.
        bocl_mem* label = mog;
        if(render_label)
          label = opencl_cache->get_data(*id,label_data_type,alpha->num_bytes()/alphaTypeSize*label_apptypesize,true);

        transfer_time += (float) transfer.all();


        ////3. SET args
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( blk_t );
        kern->set_arg( alpha );
        kern->set_arg( mog );
        kern->set_arg( label );
        kern->set_arg( render_label_mem.ptr() );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );
        kern->set_arg( exp_image.ptr() );
        kern->set_arg( exp_img_dim.ptr());
        kern->set_arg( cl_output.ptr() );
        kern->set_arg( lookup.ptr() );
        kern->set_arg( vis_image.ptr() );
        kern->set_arg( time_mem.ptr() );

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
    std::cout << "Ocl cache size: " << opencl_cache->bytes_in_cache() << std::endl;

    //clean up cam
    delete[] ray_origins;
    delete[] ray_directions;
    opencl_cache->unref_mem(ray_o_buff.ptr());
    opencl_cache->unref_mem(ray_d_buff.ptr());

    std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
    return gpu_time + transfer_time;
}
