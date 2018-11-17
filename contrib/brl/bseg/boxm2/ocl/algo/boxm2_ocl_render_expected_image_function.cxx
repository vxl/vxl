#include "boxm2_ocl_render_expected_image_function.h"
//
#include <vul/vul_timer.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <vsph/vsph_camera_bounds.h>
#include <vgl/vgl_ray_3d.h>
#include <boct/boct_bit_tree.h>

#include <brad/brad_image_metadata.h>
#include <brad/brad_atmospheric_parameters.h>
#include <brad/brad_illum_util.h>

float render_expected_image( boxm2_scene_sptr & scene,
                             bocl_device_sptr & device,
                             boxm2_opencl_cache_sptr & opencl_cache,
                             cl_command_queue & queue,
                             vpgl_camera_double_sptr & cam,
                             bocl_mem_sptr & exp_image,
                             bocl_mem_sptr & vis_image,
                             bocl_mem_sptr & max_omega_image,
                             bocl_mem_sptr & exp_img_dim,
                             const std::string& data_type,
                             bocl_kernel* kernel,
                             std::size_t * lthreads,
                             unsigned cl_ni,
                             unsigned cl_nj,
                             int apptypesize,
                             bocl_mem_sptr & tnearfar_mem_ptr,
                             std::size_t startI,
                             std::size_t startJ)
{
    float transfer_time=0.0f;
    float gpu_time=0.0f;

    //camera check
    if (cam->type_name()!= "vpgl_perspective_camera" &&
        cam->type_name()!= "vpgl_generic_camera" &&
        cam->type_name()!= "vpgl_affine_camera") {
      std::cout<<"Cannot render with camera of type "<<cam->type_name()<<std::endl;
      return 0.0f;
    }

    // create all buffers
    auto* ray_origins = new cl_float[4*cl_ni*cl_nj];
    auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
    bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins, "ray_origins buffer");
    bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions, "ray_directions buffer");
    boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff, startI, startJ);

    // Output Array
    float output_arr[100];
    for (float & i : output_arr) i = 0.0f;
    bocl_mem_sptr  cl_output=opencl_cache->alloc_mem(sizeof(float)*100,output_arr,  "output buffer");
    cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // bit lookup buffer
    cl_uchar lookup_arr[256];
    boxm2_ocl_util::set_bit_lookup(lookup_arr);
    bocl_mem_sptr lookup=opencl_cache->alloc_mem(  sizeof(cl_uchar)*256,lookup_arr, "bit lookup buffer");
    lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    //2. set global thread size
    std::size_t gThreads[] = {cl_ni,cl_nj};

    // set arguments
    std::vector<boxm2_block_id> vis_order;
    if(cam->type_name() == "vpgl_perspective_camera")
      vis_order= scene->get_vis_blocks_opt((vpgl_perspective_camera<double>*)cam.ptr(),cl_ni,cl_nj);
    else
      vis_order= scene->get_vis_blocks(cam);

    std::cout<<"Scene : "<<scene->data_path()<<' '<<vis_order.size()<<" cache size "<<opencl_cache->bytes_in_cache()<< std::endl;
    std::vector<boxm2_block_id>::iterator id;
    for (id = vis_order.begin(); id !=  vis_order.end(); ++id)
    {
        //choose correct render kernel
        boxm2_block_metadata mdata = scene->get_block_metadata(*id);
        bocl_kernel* kern =  kernel;
        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk       = opencl_cache->get_block(scene, *id);
        bocl_mem* blk_info  = opencl_cache->loaded_block_info();
        bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(scene, *id);
        int alphaTypeSize   = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        // data type string may contain an identifier so determine the buffer size
        bocl_mem* mog       = opencl_cache->get_data(scene, *id,data_type,alpha->num_bytes()/alphaTypeSize*apptypesize,true);
        transfer_time += (float) transfer.all();

        ////3. SET args
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( mog );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );
        kern->set_arg( tnearfar_mem_ptr.ptr() );
        kern->set_arg( exp_image.ptr() );
        kern->set_arg( exp_img_dim.ptr());
        kern->set_arg( cl_output.ptr() );
        kern->set_arg( lookup.ptr() );
        kern->set_arg( vis_image.ptr() );
        kern->set_arg( max_omega_image.ptr() );

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
    opencl_cache->unref_mem(ray_o_buff.ptr());
    opencl_cache->unref_mem(ray_d_buff.ptr());
    opencl_cache->unref_mem(cl_output.ptr());
    opencl_cache->unref_mem(lookup.ptr());
    std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
    return gpu_time + transfer_time;
}

float render_expected_image2( boxm2_scene_sptr & scene,
                              bocl_device_sptr & device,
                              boxm2_opencl_cache_sptr & opencl_cache,
                              cl_command_queue & queue,
                              vpgl_camera_double_sptr & cam,
                              bocl_mem_sptr & exp_image,
                              bocl_mem_sptr & vis_image,
                              bocl_mem_sptr & max_omega_image,
                              bocl_mem_sptr & exp_img_dim,
                              const std::string& data_type,
                              bocl_kernel* kernel,
                              std::size_t * lthreads,
                              unsigned cl_ni,
                              unsigned cl_nj,
                              int apptypesize,
                              bocl_mem_sptr & tnearfar_mem_ptr )
{
    float transfer_time=0.0f;
    float gpu_time=0.0f;

    //camera check
    if (cam->type_name()!= "vpgl_perspective_camera" &&
        cam->type_name()!= "vpgl_generic_camera" &&
        cam->type_name()!= "vpgl_affine_camera" ) {
      std::cout<<"Cannot render with camera of type "<<cam->type_name()<<std::endl;
      return 0.0f;
    }

    // create all buffers
    auto* ray_origins = new cl_float[4*cl_ni*cl_nj];
    auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
    bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins, "ray_origins buffer");
    bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions, "ray_directions buffer");
    boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

    // Output Array
    float output_arr[100];
    for (float & i : output_arr) i = 0.0f;
    bocl_mem_sptr  cl_output=opencl_cache->alloc_mem(sizeof(float)*100,output_arr,  "output buffer");
    cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // bit lookup buffer
    cl_uchar lookup_arr[256];
    boxm2_ocl_util::set_bit_lookup(lookup_arr);
    bocl_mem_sptr lookup=opencl_cache->alloc_mem(  sizeof(cl_uchar)*256,lookup_arr, "bit lookup buffer");
    lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    //2. set global thread size
    std::size_t gThreads[] = {cl_ni,cl_nj};

    // set arguments
    std::vector<boxm2_block_id> vis_order;
    if(cam->type_name() == "vpgl_perspective_camera")
      vis_order= scene->get_vis_blocks_opt((vpgl_perspective_camera<double>*)cam.ptr(),cl_ni,cl_nj);
    else
      vis_order= scene->get_vis_blocks(cam);
    std::cout<<"Scene : "<<scene->data_path()<<' '<<vis_order.size()<<" cache size "<<opencl_cache->bytes_in_cache()<< std::endl;
    std::vector<boxm2_block_id>::iterator id;
    for (id = vis_order.begin(); id !=  vis_order.end(); ++id)
    {
        //choose correct render kernel
        boxm2_block_metadata mdata = scene->get_block_metadata(*id);
        bocl_kernel* kern =  kernel;
        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk       = opencl_cache->get_block(scene,*id);
        bocl_mem* blk_info  = opencl_cache->loaded_block_info();
        bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(scene,*id);
        int alphaTypeSize   = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        // data type string may contain an identifier so determine the buffer size
        bocl_mem* mog       = opencl_cache->get_data(scene,*id,data_type,alpha->num_bytes()/alphaTypeSize*apptypesize,true);
        transfer_time += (float) transfer.all();

        ////3. SET args
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( mog );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );
        kern->set_arg( tnearfar_mem_ptr.ptr() );
        kern->set_arg( exp_image.ptr() );
        kern->set_arg( exp_img_dim.ptr());
        kern->set_arg( cl_output.ptr() );
        kern->set_arg( lookup.ptr() );
        kern->set_arg( vis_image.ptr() );
        kern->set_arg( max_omega_image.ptr() );

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
    opencl_cache->unref_mem(ray_o_buff.ptr());
    opencl_cache->unref_mem(ray_d_buff.ptr());
    opencl_cache->unref_mem(cl_output.ptr());
    opencl_cache->unref_mem(lookup.ptr());
    std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
    return gpu_time + transfer_time;
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
                                  bocl_mem_sptr & ray_level_image,
                                  bocl_mem_sptr & exp_img_dim,
                                  const std::string& data_type,
                                  bocl_kernel* kernel,
                                  std::size_t * lthreads,
                                  unsigned cl_ni,
                                  unsigned cl_nj )
{
    float transfer_time=0.0f;
    float gpu_time=0.0f;

    //camera check
    if (cam->type_name()!= "vpgl_perspective_camera" &&
        cam->type_name() != "vpgl_generic_camera" &&
        cam->type_name() != "vpgl_affine_camera" ) {
      std::cout<<"Cannot render with camera of type "<<cam->type_name()<<std::endl;
      return 0.0f;
    }

    //set generic cam and get visible block order
    auto* ray_origins = new cl_float[4*cl_ni*cl_nj];
    auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
    bocl_mem_sptr ray_o_buff = new bocl_mem(device->context(), ray_origins, cl_ni*cl_nj * sizeof(cl_float4) , "ray_origins buffer");
    bocl_mem_sptr ray_d_buff = new bocl_mem(device->context(), ray_directions,  cl_ni*cl_nj * sizeof(cl_float4), "ray_directions buffer");
    boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);
    ray_d_buff->read_to_buffer(queue);

    ////////////////////////////////////////////////////////////////////////////////
    //gotta do this the old fashion way for debuggin....
    std::cout<<"  DEBUG: COMPUTING CONE HALF ANGLES ON CPU"<<std::endl;
    int cnt = 0;
    for (unsigned j=0;j<cl_nj;++j) {
      for (unsigned i=0;i<cl_ni;++i) {
        //calculate ray and ray angles at pixel ij
        vgl_ray_3d<double> ray_ij; //= cam->ray(i,j);
        double cone_half_angle, solid_angle;
        auto* pcam = (vpgl_perspective_camera<double>*) cam.ptr();
        vsph_camera_bounds::pixel_solid_angle(*pcam, i, j, ray_ij, cone_half_angle, solid_angle);
        ray_directions[4*cnt+3] = (cl_float) cone_half_angle;
        cnt++;
      }
    }
    ray_d_buff->write_to_buffer(queue);
    std::cout<<"opencl Half angle: "
            <<ray_directions[0]<<','
            <<ray_directions[1]<<','
            <<ray_directions[2]<<','
            <<ray_directions[3]<<'\n'
            <<"  DEBUG: FINISHED CONE HALF ANGLES ON CPU"<<std::endl;
    ////////////////////////////////////////////////////////////////////////////////


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

    //center buffer
    bocl_mem_sptr centerX = new bocl_mem(device->context(), boct_bit_tree::centerX, sizeof(cl_float)*585, "centersX lookup buffer");
    bocl_mem_sptr centerY = new bocl_mem(device->context(), boct_bit_tree::centerY, sizeof(cl_float)*585, "centersY lookup buffer");
    bocl_mem_sptr centerZ = new bocl_mem(device->context(), boct_bit_tree::centerZ, sizeof(cl_float)*585, "centersZ lookup buffer");
    centerX->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    centerY->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    centerZ->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    //2. set global thread size
    std::size_t gThreads[] = {cl_ni,cl_nj};

    // set arguments
    std::vector<boxm2_block_id> vis_order = scene->get_vis_blocks(cam);
    std::vector<boxm2_block_id>::iterator id;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        //choose correct render kernel
        boxm2_block_metadata mdata = scene->get_block_metadata(*id);
        bocl_kernel* kern =  kernel;

        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk       = opencl_cache->get_block(scene, *id);
        bocl_mem* blk_info  = opencl_cache->loaded_block_info();
        bocl_mem* alpha     = opencl_cache->get_data<BOXM2_GAMMA>(scene, *id); //, numCells*gammaTypeSize); //opencl_cache->get_data<BOXM2_GAMMA>(*id);
        bocl_mem* mog       = opencl_cache->get_data(scene, *id,data_type);
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
        kern->set_arg( ray_level_image.ptr() );

        //set centers args
        kern->set_arg( centerX.ptr() );
        kern->set_arg( centerY.ptr() );
        kern->set_arg( centerZ.ptr() );

        //local tree , cumsum buffer, tree list
        kern->set_local_arg( lthreads[0]*lthreads[1]*sizeof(cl_uchar16) );
        kern->set_local_arg( lthreads[0]*lthreads[1]*10*sizeof(cl_uchar) );
        kern->set_local_arg( lthreads[0]*lthreads[1]*73*sizeof(cl_uchar) ); //to visit lists

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

    std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
    return gpu_time + transfer_time;
}

float render_expected_shadow_map(boxm2_scene_sptr & scene,
                                 bocl_device_sptr & device,
                                 boxm2_opencl_cache_sptr & opencl_cache,
                                 cl_command_queue & queue,
                                 vpgl_camera_double_sptr & cam,
                                 bocl_mem_sptr & exp_image,
                                 bocl_mem_sptr & vis_image,
                                 bocl_mem_sptr & exp_img_dim,
                                 const std::string& data_type,
                                 bocl_kernel* kernel,
                                 std::size_t * lthreads,
                                 unsigned cl_ni,
                                 unsigned cl_nj )
{
    float transfer_time=0.0f;
    float gpu_time=0.0f;

    //camera check
    if (cam->type_name()!= "vpgl_perspective_camera" &&
        cam->type_name() != "vpgl_generic_camera" &&
        cam->type_name() != "vpgl_affine_camera") {
      std::cout<<"Cannot render with camera of type "<<cam->type_name()<<std::endl;
      return 0.0f;
    }

    //set generic cam and get visible block order
    auto* ray_origins    = new cl_float[4*cl_ni*cl_nj];
    auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
    bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins,"ray_origins buffer");
    bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions,"ray_directions buffer");
    boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

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

    //2. set global thread size
    std::size_t gThreads[] = {cl_ni,cl_nj};

    // set arguments
    std::vector<boxm2_block_id> vis_order = scene->get_vis_blocks(cam);
    std::vector<boxm2_block_id>::iterator id;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        //choose correct render kernel
        boxm2_block_metadata mdata = scene->get_block_metadata(*id);
        bocl_kernel* kern =  kernel;
        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk       = opencl_cache->get_block(scene, *id);
        bocl_mem* blk_info  = opencl_cache->loaded_block_info();
        bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(scene, *id);
        bocl_mem *aux_sun   = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix(data_type));
        //std::cout << "id = " << id << std::endl;
        std::cout << "blk = " << blk->cpu_buffer() << '\n'
                 << "alpha = " << alpha->cpu_buffer() << '\n'
                 << "aux_sun = " << aux_sun->cpu_buffer() << std::endl;

        transfer_time += (float) transfer.all();
        ////3. SET args
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( aux_sun );
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

    opencl_cache->unref_mem(ray_d_buff.ptr());
    opencl_cache->unref_mem(ray_o_buff.ptr());

    //clean up cam
    delete[] ray_origins;
    delete[] ray_directions;

    std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
    return gpu_time + transfer_time;
}

float render_expected_phongs_image( boxm2_scene_sptr & scene,
                                    bocl_device_sptr & device,
                                    boxm2_opencl_cache_sptr & opencl_cache,
                                    cl_command_queue & queue,
                                    vpgl_camera_double_sptr & cam,
                                    bocl_mem_sptr & exp_image,
                                    bocl_mem_sptr & vis_image,
                                    bocl_mem_sptr & exp_img_dim,
                                    const std::string&  /*data_type*/,
                                    bocl_kernel* kernel,
                                    std::size_t * lthreads,
                                    unsigned cl_ni,
                                    unsigned cl_nj,
                                    const bocl_mem_sptr& sundir)
{
    float transfer_time=0.0f;
    float gpu_time=0.0f;

    //camera check
    if (cam->type_name()!= "vpgl_perspective_camera" &&
        cam->type_name()!= "vpgl_generic_camera" &&
        cam->type_name()!= "vpgl_affine_camera" ) {
      std::cout<<"Cannot render with camera of type "<<cam->type_name()<<std::endl;
      return 0.0f;
    }

    //set generic cam and get visible block order
    auto* ray_origins    = new cl_float[4*cl_ni*cl_nj];
    auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
    bocl_mem_sptr ray_o_buff = new bocl_mem(device->context(), ray_origins   ,  cl_ni*cl_nj * sizeof(cl_float4), "ray_origins buffer");
    bocl_mem_sptr ray_d_buff = new bocl_mem(device->context(), ray_directions,  cl_ni*cl_nj * sizeof(cl_float4), "ray_directions buffer");
    boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

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

    //2. set global thread size
    std::size_t gThreads[] = {cl_ni,cl_nj};

    // set arguments
    std::vector<boxm2_block_id> vis_order = scene->get_vis_blocks(cam);
    std::vector<boxm2_block_id>::iterator id;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        std::cout<<(*id);
        //choose correct render kernel
        boxm2_block_metadata mdata = scene->get_block_metadata(*id);
        bocl_kernel* kern =  kernel;

        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk       = opencl_cache->get_block(scene, *id);
        bocl_mem* blk_info  = opencl_cache->loaded_block_info();
        bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(scene, *id);
        bocl_mem* mog       = opencl_cache->get_data(scene, *id,"float8_phongs_model");
        transfer_time += (float) transfer.all();

        ////3. SET args
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( mog );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );
        kern->set_arg( sundir.ptr() );
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

    std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
    return gpu_time + transfer_time;
}


float render_expected_image_naa(  boxm2_scene_sptr & scene,
                                  bocl_device_sptr & device,
                                  boxm2_opencl_cache_sptr & opencl_cache,
                                  cl_command_queue & queue,
                                  vpgl_camera_double_sptr & cam,
                                  bocl_mem_sptr & exp_image,
                                  bocl_mem_sptr & vis_image,
                                  bocl_mem_sptr & exp_img_dim,
                                  bocl_kernel* kernel,
                                  std::size_t * lthreads,
                                  unsigned cl_ni,
                                  unsigned cl_nj,
                                  const brad_image_metadata_sptr&  metadata,
                                  const brad_atmospheric_parameters_sptr& atm_params)
{
    float transfer_time=0.0f;
    float gpu_time=0.0f;

    //camera check
    if (cam->type_name()!= "vpgl_perspective_camera" &&
        cam->type_name()!= "vpgl_generic_camera" &&
        cam->type_name()!= "vpgl_affine_camera" )  {
      std::cout<<"Cannot render with camera of type "<<cam->type_name()<<std::endl;
      return 0.0f;
    }

    //set generic cam and get visible block order
    auto* ray_origins    = new cl_float[4*cl_ni*cl_nj];
    auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
    bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj * sizeof(cl_float4),ray_origins   , "ray_origins buffer");
    bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj * sizeof(cl_float4),ray_directions, "ray_directions buffer");
    boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

    // get normal directions
    std::vector<vgl_vector_3d<double> > normals = boxm2_normal_albedo_array::get_normals();
    unsigned int num_normals = normals.size();
    // opencl code depends on there being exactly 16 normal directions - do sanity check here
    if (num_normals != 16) {
      std::cerr << "ERROR: boxm2_ocl_update_alpha_naa_process: num_normals = " << num_normals << ".  Expected 16\n";
      return false;
    }

   double deg2rad = vnl_math::pi_over_180;
   double sun_az = metadata->sun_azimuth_ * deg2rad;
   double sun_el = metadata->sun_elevation_ * deg2rad;
   vgl_vector_3d<double> sun_dir(std::sin(sun_az)*std::cos(sun_el),
                                 std::cos(sun_az)*std::cos(sun_el),
                                 std::sin(sun_el));

   // buffers for holding radiance scales and offsets per normal
   auto* radiance_scales_buff = new float[num_normals];
   auto* radiance_offsets_buff = new float[num_normals];

   // compute offsets and scale for linear radiance model
   for (unsigned n=0; n < num_normals; ++n) {
      // compute offset as radiance of surface with 0 reflectance
      double offset = brad_expected_radiance_chavez(0.0, normals[n], *metadata, *atm_params);
      radiance_offsets_buff[n] = offset;
      // use perfect reflector to compute radiance scale
      double radiance = brad_expected_radiance_chavez(1.0, normals[n], *metadata, *atm_params);
      radiance_scales_buff[n] = radiance - offset;
#if 0
      brad_image_metadata shadow_metadata = *metadata;
      shadow_metadata.sun_irradiance_ = 0;
      double radiance_shadow = brad_expected_radiance_chavez(1.0, normals[n], shadow_metadata, *atm_params);
      radiance_shadow_scales_buff[n] = radiance_shadow - offset;
#endif
      std::cout << "radiance_scales["<<n<<"] = " << radiance_scales_buff[n] << std::endl;
   }

    bocl_mem_sptr radiance_scales = new bocl_mem(device->context(), radiance_scales_buff, sizeof(float)*num_normals,"radiance scales buffer");
    radiance_scales->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    bocl_mem_sptr radiance_offsets = new bocl_mem(device->context(), radiance_offsets_buff, sizeof(float)*num_normals,"radiance offset buffer");
    radiance_offsets->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

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

    //2. set global thread size
    std::size_t gThreads[] = {cl_ni,cl_nj};

    // set arguments
    std::vector<boxm2_block_id> vis_order = scene->get_vis_blocks(cam);
    std::vector<boxm2_block_id>::iterator id;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        std::cout << (*id) << std::endl;
        //choose correct render kernel
        boxm2_block_metadata mdata = scene->get_block_metadata(*id);
        bocl_kernel* kern =  kernel;

        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk       = opencl_cache->get_block(scene, *id);
        bocl_mem* blk_info  = opencl_cache->loaded_block_info();
        bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(scene, *id);
        int alphaTypeSize   = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        // data type string may contain an identifier so determine the buffer size
        unsigned int num_cells = alpha->num_bytes()/alphaTypeSize;
        std::string data_type = boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::prefix();
        int appTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::prefix());

        bocl_mem* naa_apm  = opencl_cache->get_data(scene, *id,data_type,num_cells*appTypeSize, true);
        transfer_time += (float) transfer.all();

        ////3. SET args
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( naa_apm );
        kern->set_arg( radiance_scales.ptr() );
        kern->set_arg( radiance_offsets.ptr() );
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
        std::cout << "executing kernel.." << std::endl;
        kern->execute(queue, 2, lthreads, gThreads);
        clFinish(queue);
        float kern_time = kern->exec_time();
        std::cout << "..exec_time = " << kern_time << std::endl;
        gpu_time += kern_time;

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
    }

    //clean up cam
    opencl_cache->unref_mem(ray_o_buff.ptr());
    opencl_cache->unref_mem(ray_d_buff.ptr());

    delete[] ray_origins;
    delete[] ray_directions;
    delete[] radiance_scales_buff;
    //delete[] radiance_scales_shadow_buff;
    delete[] radiance_offsets_buff;

    std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<'\n'
            << "Returning" << std::endl;
    return gpu_time + transfer_time;
}

float render_expected_albedo_normal( boxm2_scene_sptr & scene,
                                     bocl_device_sptr & device,
                                     boxm2_opencl_cache_sptr & opencl_cache,
                                     cl_command_queue & queue,
                                     vpgl_camera_double_sptr & cam,
                                     bocl_mem_sptr & exp_image,
                                     bocl_mem_sptr & vis_image,
                                     bocl_mem_sptr & exp_img_dim,
                                     bocl_kernel* kernel,
                                     std::size_t * lthreads,
                                     unsigned cl_ni,
                                     unsigned cl_nj)
{
    float transfer_time=0.0f;
    float gpu_time=0.0f;

    //camera check
    if (cam->type_name()!= "vpgl_perspective_camera" &&
        cam->type_name()!= "vpgl_generic_camera" &&
        cam->type_name()!= "vpgl_affine_camera" ) {
      std::cout<<"Cannot render with camera of type "<<cam->type_name()<<std::endl;
      return 0.0f;
    }

    //set generic cam and get visible block order
    auto* ray_origins    = new cl_float[4*cl_ni*cl_nj];
    auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
    bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj * sizeof(cl_float4),ray_origins   , "ray_origins buffer");
    bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj * sizeof(cl_float4),ray_directions, "ray_directions buffer");
    boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

    // get normals
    std::vector<vgl_vector_3d<double> > normals = boxm2_normal_albedo_array::get_normals();
    cl_float16 normals_x, normals_y, normals_z;
#ifdef CL_ALIGNED
    for (unsigned int i=0; i<16; ++i) {
       normals_x.s[i] = normals[i].x();
       normals_y.s[i] = normals[i].y();
       normals_z.s[i] = normals[i].z();
    }
#else // assuming cl_float16 is a typedef for float[16]
    float* n_x = static_cast<float*>(normals_x);
    float* n_y = static_cast<float*>(normals_y);
    float* n_z = static_cast<float*>(normals_z);
    for (unsigned int i=0; i<16; ++i) {
       n_x[i] = normals[i].x();
       n_y[i] = normals[i].y();
       n_z[i] = normals[i].z();
    }
#endif // CL_ALIGNED
    bocl_mem_sptr normals_x_buff = new bocl_mem(device->context(), &normals_x, sizeof(cl_float16), "normals_x buffer");
    normals_x_buff->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    bocl_mem_sptr normals_y_buff = new bocl_mem(device->context(), &normals_y, sizeof(cl_float16), "normals_y buffer");
    normals_y_buff->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    bocl_mem_sptr normals_z_buff = new bocl_mem(device->context(), &normals_z, sizeof(cl_float16), "normals_z buffer");
    normals_z_buff->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

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

    //2. set global thread size
    std::size_t gThreads[] = {cl_ni,cl_nj};

    // set arguments
    std::vector<boxm2_block_id> vis_order = scene->get_vis_blocks(cam);
    std::vector<boxm2_block_id>::iterator id;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        std::cout << (*id) << std::endl;

        //choose correct render kernel
        boxm2_block_metadata mdata = scene->get_block_metadata(*id);
        bocl_kernel* kern =  kernel;

        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk       = opencl_cache->get_block(scene, *id);
        bocl_mem* blk_info  = opencl_cache->loaded_block_info();
        bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(scene, *id);
        int alphaTypeSize   = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        // data type string may contain an identifier so determine the buffer size
        unsigned int num_cells = alpha->num_bytes()/alphaTypeSize;
        std::string data_type = boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::prefix();
        int appTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NORMAL_ALBEDO_ARRAY>::prefix());

        bocl_mem* naa_apm  = opencl_cache->get_data(scene, *id,data_type,num_cells*appTypeSize, true);
        transfer_time += (float) transfer.all();

        ////3. SET args
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( naa_apm );
        kern->set_arg( normals_x_buff.ptr() );
        kern->set_arg( normals_y_buff.ptr() );
        kern->set_arg( normals_z_buff.ptr() );
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
        std::cout << "executing kernel.." << std::endl;
        kern->execute(queue, 2, lthreads, gThreads);
        clFinish(queue);
        float kern_time = kern->exec_time();
        std::cout << "..exec_time = " << kern_time << std::endl;
        gpu_time += kern_time;

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
    }

    //clean up cam
    opencl_cache->unref_mem(ray_o_buff.ptr());
    opencl_cache->unref_mem(ray_d_buff.ptr());

    delete[] ray_origins;
    delete[] ray_directions;

    std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
    return gpu_time + transfer_time;
}
