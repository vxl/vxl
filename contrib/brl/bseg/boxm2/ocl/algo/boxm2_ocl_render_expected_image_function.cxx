#include "boxm2_ocl_render_expected_image_function.h"
//
#include <vul/vul_timer.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <vpgl/algo/vpgl_camera_bounds.h>
#include <vgl/vgl_ray_3d.h>
#include <boct/boct_bit_tree2.h>

float render_expected_image(  boxm2_scene_sptr & scene,
                              bocl_device_sptr & device,
                              boxm2_opencl_cache_sptr & opencl_cache,
                              cl_command_queue & queue,
                              vpgl_camera_double_sptr & cam,
                              bocl_mem_sptr & exp_image,
                              bocl_mem_sptr & vis_image, 
                              bocl_mem_sptr & exp_img_dim,
                              vcl_string data_type,
                              bocl_kernel* kernel,
                              vcl_size_t * lthreads,
                              unsigned cl_ni,
                              unsigned cl_nj )
{
    float transfer_time=0.0f;
    float gpu_time=0.0f;
    
    //camera check
    if(cam->type_name()!= "vpgl_perspective_camera" && cam->type_name() != "vpgl_generic_camera" ) {
      vcl_cout<<"Cannot render with camera of type "<<cam->type_name()<<vcl_endl;
      return 0.0f; 
    }
    
    //set generic cam and get visible block order 
    cl_float* ray_origins = new cl_float[4*cl_ni*cl_nj]; 
    cl_float* ray_directions = new cl_float[4*cl_ni*cl_nj]; 
    bocl_mem_sptr ray_o_buff = new bocl_mem(device->context(), ray_origins, cl_ni*cl_nj * sizeof(cl_float4) , "ray_origins buffer");
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
        //choose correct render kernel
        boxm2_block_metadata mdata = scene->get_block_metadata(*id);
        bocl_kernel* kern =  kernel;

        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk       = opencl_cache->get_block(*id);
        bocl_mem* blk_info  = opencl_cache->loaded_block_info();
        bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(*id);
        bocl_mem* mog       = opencl_cache->get_data(*id,data_type);
        transfer_time += (float) transfer.all();

        ////3. SET args
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( mog );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );
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

    //clean up cam
    delete[] ray_origins; 
    delete[] ray_directions;

    vcl_cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<vcl_endl;
    return (gpu_time + transfer_time); 
}


//render_cone_expected_image - pretty much the same as above but one of the local
//memory arguments is expanded
float render_cone_expected_image( boxm2_scene_sptr & scene,
                                  bocl_device_sptr & device,
                                  boxm2_opencl_cache_sptr & opencl_cache,
                                  cl_command_queue & queue,
                                  vpgl_camera_double_sptr & cam,
                                  bocl_mem_sptr & exp_image,
                                  bocl_mem_sptr & vis_image, 
                                  bocl_mem_sptr & exp_img_dim,
                                  vcl_string data_type,
                                  bocl_kernel* kernel,
                                  vcl_size_t * lthreads,
                                  unsigned cl_ni,
                                  unsigned cl_nj )
{
    float transfer_time=0.0f;
    float gpu_time=0.0f;
    
    //camera check
    if(cam->type_name()!= "vpgl_perspective_camera" && cam->type_name() != "vpgl_generic_camera" ) {
      vcl_cout<<"Cannot render with camera of type "<<cam->type_name()<<vcl_endl;
      return 0.0f; 
    }
    
    //set generic cam and get visible block order 
    cl_float* ray_origins = new cl_float[4*cl_ni*cl_nj]; 
    cl_float* ray_directions = new cl_float[4*cl_ni*cl_nj]; 
    bocl_mem_sptr ray_o_buff = new bocl_mem(device->context(), ray_origins, cl_ni*cl_nj * sizeof(cl_float4) , "ray_origins buffer");
    bocl_mem_sptr ray_d_buff = new bocl_mem(device->context(), ray_directions,  cl_ni*cl_nj * sizeof(cl_float4), "ray_directions buffer");
    boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff); 
    
    ray_d_buff->read_to_buffer(queue); 
    
    //gotta do this the old fashion way for debuggin....
    int count = 0; 
    for (unsigned j=0;j<cl_nj;++j) {
      for (unsigned i=0;i<cl_ni;++i) {

        //calculate ray and ray angles at pixel ij
        vgl_ray_3d<double> ray_ij; //= cam->ray(i,j);
        double cone_half_angle, solid_angle;
        vpgl_perspective_camera<double>* pcam = (vpgl_perspective_camera<double>*) cam.ptr();
        vpgl_camera_bounds::pixel_solid_angle(*pcam, i, j, ray_ij, cone_half_angle, solid_angle);
        ray_directions[4*count+3] = (cl_float) cone_half_angle; 
        count++;
      }
    }
    ray_d_buff->write_to_buffer(queue);
    vcl_cout<<"opencl Half angle: "
            <<ray_directions[0]<<','
            <<ray_directions[1]<<','
            <<ray_directions[2]<<','
            <<ray_directions[3]<<vcl_endl;
   
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
    
    //center buffer
    bocl_mem_sptr centerX = new bocl_mem(device->context(), boct_bit_tree2::centerX, sizeof(cl_float)*585, "centersX lookup buffer"); 
    bocl_mem_sptr centerY = new bocl_mem(device->context(), boct_bit_tree2::centerY, sizeof(cl_float)*585, "centersY lookup buffer"); 
    bocl_mem_sptr centerZ = new bocl_mem(device->context(), boct_bit_tree2::centerZ, sizeof(cl_float)*585, "centersZ lookup buffer"); 
    centerX->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR); 
    centerY->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);     
    centerZ->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR); 
    
    //2. set global thread size
    vcl_size_t gThreads[] = {cl_ni,cl_nj};

    // set arguments
    vcl_vector<boxm2_block_id> vis_order = scene->get_vis_blocks(cam);
    vcl_vector<boxm2_block_id>::iterator id;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        //choose correct render kernel
        boxm2_block_metadata mdata = scene->get_block_metadata(*id);
        bocl_kernel* kern =  kernel;

        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk       = opencl_cache->get_block(*id);
        bocl_mem* blk_info  = opencl_cache->loaded_block_info();
        bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(*id);
        bocl_mem* mog       = opencl_cache->get_data(*id,data_type);
        transfer_time += (float) transfer.all();

        ////3. SET args
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( mog );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );
        kern->set_arg( exp_image.ptr() );
        kern->set_arg( exp_img_dim.ptr());
        kern->set_arg( cl_output.ptr() );
        kern->set_arg( lookup.ptr() );
        kern->set_arg( vis_image.ptr() );
        
        //set centers args
        kern->set_arg( centerX.ptr() );
        kern->set_arg( centerY.ptr() );
        kern->set_arg( centerZ.ptr() );

        //local tree , cumsum buffer, imindex buffer
        kern->set_local_arg( lthreads[0]*lthreads[1]*sizeof(cl_uchar16) );
        kern->set_local_arg( lthreads[0]*lthreads[1]*10*sizeof(cl_uchar) );
        kern->set_local_arg( lthreads[0]*lthreads[1]*73*sizeof(cl_uchar) );

        //execute kernel
        kern->execute(queue, 2, lthreads, gThreads);
        clFinish(queue);
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
    }

    //clean up cam
    delete[] ray_origins; 
    delete[] ray_directions;

    vcl_cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<vcl_endl;
    return (gpu_time + transfer_time); 
}
