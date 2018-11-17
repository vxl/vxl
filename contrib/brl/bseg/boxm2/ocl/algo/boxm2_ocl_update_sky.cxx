// This is brl/bseg/boxm2/ocl/algo/boxm2_ocl_update_sky_sky.cxx
#include <fstream>
#include <iostream>
#include <algorithm>
#include "boxm2_ocl_update_sky.h"
//:
// \file
// \brief  A process for updating a color model
//
// \author Vishal Jain
// \date Mar 25, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <vil/vil_image_view.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <vnl/vnl_numeric_traits.h>

//: Map of kernels should persist between process executions
std::map<std::string,std::vector<bocl_kernel*> > boxm2_ocl_update_sky::kernels_;

//Main public method, updates color model
bool boxm2_ocl_update_sky::update_sky(const boxm2_scene_sptr&         scene,
                                  bocl_device_sptr         device,
                                  const boxm2_opencl_cache_sptr&  opencl_cache,
                                  vpgl_camera_double_sptr  cam,
                                  const vil_image_view_base_sptr& skyimg)
{
    float transfer_time=0.0f;
    float gpu_time=0.0f;
    std::size_t local_threads[2]={8,8};
    std::size_t global_threads[2]={8,8};

    //cache size sanity check
    std::size_t binCache = opencl_cache.ptr()->bytes_in_cache();
    std::cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;

    // create a command queue.
    int status=0;
    cl_command_queue queue = clCreateCommandQueue( device->context(),*(device->device_id()),
        CL_QUEUE_PROFILING_ENABLE,
        &status);
    if (status!=0)
        return false;

    // compile the kernel if not already compiled
    std::vector<bocl_kernel*>& kernels = get_kernels(device);

    //grab input image, establish cl_ni, cl_nj (so global size is divisible by local size)
    vil_image_view_base_sptr float_img = boxm2_util::prepare_input_image(skyimg, true);
    auto* img_view = static_cast<vil_image_view<float>* >(float_img.ptr());
    auto cl_ni=(unsigned)RoundUp(img_view->ni(),(int)local_threads[0]);
    auto cl_nj=(unsigned)RoundUp(img_view->nj(),(int)local_threads[1]);
    global_threads[0]=cl_ni;
    global_threads[1]=cl_nj;

    //set generic cam
    auto* ray_origins    = new cl_float[4*cl_ni*cl_nj];
    auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
    bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins, "ray_origins buffer");
    bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions, "ray_directions buffer");
    boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

    float tnearfar[2] = { 0.0f, 1000000} ;
    bocl_mem_sptr tnearfar_mem_ptr = opencl_cache->alloc_mem(2*sizeof(float), tnearfar, "tnearfar  buffer");
    tnearfar_mem_ptr->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    auto* input_buff=new float[cl_ni*cl_nj];
    //copy input vals into image
    int count=0;
    for (unsigned int j=0;j<cl_nj;++j) {
        for (unsigned int i=0;i<cl_ni;++i) {
            input_buff[count] = 0.0f;
            if ( i<img_view->ni() && j< img_view->nj() )
                input_buff[count] = (*img_view)(i,j);
            ++count;
        }
    }
    bocl_mem_sptr in_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), input_buff, "input image buffer");
    in_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // Image Dimensions
    int img_dim_buff[4];
    img_dim_buff[0] = 0;
    img_dim_buff[1] = 0;
    img_dim_buff[2] = img_view->ni();
    img_dim_buff[3] = img_view->nj();
    bocl_mem_sptr img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(int)*4, "image dims");
    img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

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

    // set arguments
    std::vector<boxm2_block_id> vis_order;
    if(cam->type_name() == "vpgl_perspective_camera")
        vis_order= scene->get_vis_blocks_opt((vpgl_perspective_camera<double>*)cam.ptr(),img_view->ni(),img_view->nj());
    else
        vis_order= scene->get_vis_blocks(cam);

    std::vector<boxm2_block_id>::iterator id;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        //choose correct render kernel
        boxm2_block_metadata mdata = scene->get_block_metadata(*id);
        bocl_kernel* kern = kernels[0];
        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk       = opencl_cache->get_block(scene,*id);
        bocl_mem* blk_info  = opencl_cache->loaded_block_info();
        bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(scene,*id,0,false);
        auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
        int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
        blk_info->write_to_buffer((queue));

        transfer_time += (float) transfer.all();
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( lookup.ptr() );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );
        kern->set_arg( tnearfar_mem_ptr.ptr() );
        kern->set_arg( img_dim.ptr() );
        kern->set_arg( in_image.ptr() );
        kern->set_arg( cl_output.ptr() );
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar4) ); //ray bundle,
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_int) );    //cell pointers,
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_float4) ); //cached aux,
        kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer

        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
            return false;
        gpu_time += kern->exec_time();
        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
        //read image out to buffer (from gpu)
        alpha->read_to_buffer(queue);
        cl_output->read_to_buffer(queue);
        clFinish(queue);
    }
    delete [] input_buff;
    delete [] ray_origins;
    delete [] ray_directions;
    opencl_cache->unref_mem(in_image.ptr());
    opencl_cache->unref_mem(ray_o_buff.ptr());
    opencl_cache->unref_mem(ray_d_buff.ptr());
    opencl_cache->unref_mem(tnearfar_mem_ptr.ptr());
    std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
    clReleaseCommandQueue(queue);
    return true;
}


//Returns vector of color update kernels (and caches them per device
std::vector<bocl_kernel*>& boxm2_ocl_update_sky::get_kernels(const bocl_device_sptr& device, const std::string& opts, bool  /*isRGB*/)
{
    // compile kernels if not already compiled
    std::string identifier = device->device_identifier() + opts;
    if (kernels_.find(identifier) != kernels_.end())
        return kernels_[identifier];

    //otherwise compile the kernels
    std::cout<<"=== boxm2_ocl_update_sky_process::compiling kernels on device "<<identifier<<"==="<<std::endl;

    std::vector<std::string> src_paths;
    std::string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "pixel_conversion.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "atomics_util.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/update_sky_kernels.cl");
    std::vector<std::string> non_ray_src = std::vector<std::string>(src_paths);


    src_paths.push_back(source_dir + "update_functors.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //compilation options
    const std::string& options = opts;

    //populate vector of kernels
    std::vector<bocl_kernel*> vec_kernels;

    //seg len pass
    auto* update_sky_kernel = new bocl_kernel();
    std::string seg_opts = options + " -D UPDATE_SKY  -D STEP_CELL=step_cell_update_sky(aux_args,data_ptr,llid,d)";
    update_sky_kernel->create_kernel(&device->context(), device->device_id(), src_paths, "update_sky_main", seg_opts, "update_sky_main");
    vec_kernels.push_back(update_sky_kernel);


    //store and return
    kernels_[identifier] = vec_kernels;
    return kernels_[identifier];
}


//: Map of kernels should persist between process executions
std::map<std::string,std::vector<bocl_kernel*> > boxm2_ocl_update_sky2::kernels_;

//Main public method, updates color model
bool boxm2_ocl_update_sky2::accumulate_sky_evidence(const boxm2_scene_sptr&         scene,
                                                     bocl_device_sptr         device,
                                                     const boxm2_opencl_cache_sptr&  opencl_cache,
                                                     vpgl_camera_double_sptr  cam,
                                                     const vil_image_view_base_sptr& skyimg)
{
    float transfer_time=0.0f;
    float gpu_time=0.0f;
    std::size_t local_threads[2]={8,8};
    std::size_t global_threads[2]={8,8};

    //cache size sanity check
    std::size_t binCache = opencl_cache.ptr()->bytes_in_cache();
    std::cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;

    // create a command queue.
    int status=0;
    cl_command_queue queue = clCreateCommandQueue( device->context(),*(device->device_id()),
                                                   CL_QUEUE_PROFILING_ENABLE,
                                                   &status);
    if (status!=0)
        return false;

    // compile the kernel if not already compiled
    std::vector<bocl_kernel*>& kernels =  boxm2_ocl_update_sky2::get_kernels(device);

    //grab input image, establish cl_ni, cl_nj (so global size is divisible by local size)
    vil_image_view_base_sptr float_img = boxm2_util::prepare_input_image(skyimg, true);
    auto* img_view = static_cast<vil_image_view<float>* >(float_img.ptr());
    auto cl_ni=(unsigned)RoundUp(img_view->ni(),(int)local_threads[0]);
    auto cl_nj=(unsigned)RoundUp(img_view->nj(),(int)local_threads[1]);
    global_threads[0]=cl_ni;
    global_threads[1]=cl_nj;

    //set generic cam
    auto* ray_origins    = new cl_float[4*cl_ni*cl_nj];
    auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
    bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins, "ray_origins buffer");
    bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions, "ray_directions buffer");
    boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

  float tnearfar[2] = { 0.0f, 1000000} ;

  if(cam->type_name() == "vpgl_perspective_camera")
  {

      float f  = ((vpgl_perspective_camera<double> *)cam.ptr())->get_calibration().focal_length()*((vpgl_perspective_camera<double> *)cam.ptr())->get_calibration().x_scale();
      tnearfar[0] = f* scene->finest_resolution()/32 ;
      tnearfar[1] = f* scene->finest_resolution()/0.125 ;

      std::cout<<"Near and Far Clipping planes "<<tnearfar[0]<<" "<<tnearfar[1]<<std::endl;
  }
  bocl_mem_sptr tnearfar_mem_ptr = opencl_cache->alloc_mem(2*sizeof(float), tnearfar, "tnearfar  buffer");
  tnearfar_mem_ptr->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    auto* input_buff=new float[cl_ni*cl_nj];
    auto* vis_buff=new float[cl_ni*cl_nj];
    //copy input vals into image
    int count=0;
    for (unsigned int j=0;j<cl_nj;++j) {
        for (unsigned int i=0;i<cl_ni;++i) {
            input_buff[count] = 0.0f;
            if ( i<img_view->ni() && j< img_view->nj() )
                {
                    input_buff[count] = (*img_view)(i,j);
                    vis_buff[count] = 1;
            }
            ++count;
        }
    }
    bocl_mem_sptr in_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), input_buff, "input image buffer");
    in_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    bocl_mem_sptr vis_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), vis_buff, "visibility image buffer");
    vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);    // Image Dimensions

    int img_dim_buff[4];
    img_dim_buff[0] = 0;
    img_dim_buff[1] = 0;
    img_dim_buff[2] = img_view->ni();
    img_dim_buff[3] = img_view->nj();
    bocl_mem_sptr img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(int)*4, "image dims");
    img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

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

    // set arguments
    std::vector<boxm2_block_id> vis_order;
    if(cam->type_name() == "vpgl_perspective_camera")
        vis_order= scene->get_vis_blocks_opt((vpgl_perspective_camera<double>*)cam.ptr(),img_view->ni(),img_view->nj());
    else
        vis_order= scene->get_vis_blocks(cam);

    std::vector<boxm2_block_id>::iterator id;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        //choose correct render kernel
        boxm2_block_metadata mdata = scene->get_block_metadata(*id);
        bocl_kernel* kern = kernels[0];
        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk       = opencl_cache->get_block(scene,*id);
        bocl_mem* blk_info  = opencl_cache->loaded_block_info();
        bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(scene,*id,0,false);
        auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
        int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
        blk_info->write_to_buffer((queue));
        int auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
        bocl_mem * aux0   = opencl_cache->get_data<BOXM2_AUX0>(scene,*id, info_buffer->data_buffer_length*auxTypeSize,false,"temp");
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
        bocl_mem *aux1   = opencl_cache->get_data<BOXM2_AUX1>(scene,*id, info_buffer->data_buffer_length*auxTypeSize,false,"temp");


        transfer_time += (float) transfer.all();
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( lookup.ptr() );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );
        kern->set_arg( tnearfar_mem_ptr.ptr() );
        kern->set_arg( img_dim.ptr() );
        kern->set_arg( in_image.ptr() );
        kern->set_arg( vis_image.ptr() );
        kern->set_arg( cl_output.ptr() );
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar4) ); //ray bundle,
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_int) );    //cell pointers,
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_float4) ); //cached aux,
        kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer

        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
            return false;
        gpu_time += kern->exec_time();
        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
        //read image out to buffer (from gpu)
        //: Averge of information per cell
        kern = kernels[1];
        int numTrees = mdata.sub_block_num_.x() * mdata.sub_block_num_.y() * mdata.sub_block_num_.z();

        std::size_t local_threads_2[] = {16};
        std::size_t global_threads_2[] = {RoundUp(numTrees,local_threads_2[0])};

        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
        bocl_mem *aux0_cum   = opencl_cache->get_data<BOXM2_AUX0>(scene,*id, info_buffer->data_buffer_length*auxTypeSize,false,"cum");
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
        bocl_mem *aux1_count   = opencl_cache->get_data<BOXM2_AUX1>(scene,*id, info_buffer->data_buffer_length*auxTypeSize,false,"count");
        transfer_time += (float) transfer.all();
        kern->set_arg( blk_info );
        kern->set_arg(lookup.ptr());
        kern->set_arg( blk );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( aux0_cum );
        kern->set_arg( aux1_count );
        kern->set_local_arg( local_threads_2[0]*sizeof(cl_uchar16) );//local tree,
        kern->set_local_arg( local_threads_2[0]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer

        //execute kernel
        kern->execute(queue, 1, local_threads_2, global_threads_2);
        status = clFinish(queue);
        if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
            return false;
        gpu_time += kern->exec_time();
        //clear render kernel args so it can reset em on next execution

        kern->clear_args();
        aux0_cum->read_to_buffer(queue);
        aux1_count->read_to_buffer(queue);
        opencl_cache->unref_mem(aux0);
        opencl_cache->unref_mem(aux1);
    }
    delete [] input_buff;
    delete [] vis_buff;
    delete [] ray_origins;
    delete [] ray_directions;
    opencl_cache->unref_mem(in_image.ptr());
    opencl_cache->unref_mem(vis_image.ptr());
    opencl_cache->unref_mem(ray_o_buff.ptr());
    opencl_cache->unref_mem(ray_d_buff.ptr());
    opencl_cache->unref_mem(tnearfar_mem_ptr.ptr());
    std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
    clReleaseCommandQueue(queue);
    return true;
}
bool boxm2_ocl_update_sky2::update_sky2( const boxm2_scene_sptr&         scene,
                                         const bocl_device_sptr&         device,
                                         const boxm2_opencl_cache_sptr&  opencl_cache)
{
    float transfer_time=0.0f;
    float gpu_time=0.0f;

    //cache size sanity check
    std::size_t binCache = opencl_cache.ptr()->bytes_in_cache();
    std::cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;

    // create a command queue.
    int status=0;
    cl_command_queue queue = clCreateCommandQueue( device->context(),*(device->device_id()),
                                                   CL_QUEUE_PROFILING_ENABLE,
                                                   &status);
    if (status!=0)
        return false;
    // compile the kernel if not already compiled
    std::vector<bocl_kernel*>& kernels = boxm2_ocl_update_sky2::get_kernels(device);
    // bit lookup buffer
    cl_uchar lookup_arr[256];
    boxm2_ocl_util::set_bit_lookup(lookup_arr);
    bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
    lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    cl_float chisqr_lookup_arr[10000];
    boxm2_ocl_util::set_cdf_chisqr_lookup(chisqr_lookup_arr);
    bocl_mem_sptr chisqr_lookup=new bocl_mem(device->context(), chisqr_lookup_arr, sizeof(cl_float)*10000, "chisqr cdf lookup buffer");
    chisqr_lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    // set arguments
    std::vector<boxm2_block_id> vis_order = scene->get_block_ids();
    std::vector<boxm2_block_id>::iterator id;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        //choose correct render kernel
        boxm2_block_metadata mdata = scene->get_block_metadata(*id);
        int numTrees = mdata.sub_block_num_.x() * mdata.sub_block_num_.y() * mdata.sub_block_num_.z();
        bocl_kernel* kern = kernels[2];
        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk       = opencl_cache->get_block(scene,*id);
        bocl_mem* blk_info  = opencl_cache->loaded_block_info();
        bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(scene,*id,0,false);
        auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
        int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        // check for invalid parameters
        if( alphaTypeSize == 0 ) //This should never happen, it will result in division by zero later
        {
            std::cout << "ERROR: alphaTypeSize == 0 in " << __FILE__ << __LINE__ << std::endl;
            return false;
        }

        info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
        int auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
        bocl_mem *aux0   = opencl_cache->get_data<BOXM2_AUX0>(scene,*id, info_buffer->data_buffer_length*auxTypeSize,false,"cum");
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
        bocl_mem *aux1   = opencl_cache->get_data<BOXM2_AUX1>(scene,*id, info_buffer->data_buffer_length*auxTypeSize,false,"count");
        transfer_time += (float) transfer.all();
        std::size_t lThreads[] = {16};
        std::size_t gThreads[] = {RoundUp(numTrees,lThreads[0])};

        transfer_time += (float) transfer.all();
        kern->set_arg( blk_info );

        kern->set_arg(lookup.ptr());
        kern->set_arg(chisqr_lookup.ptr());
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_local_arg( lThreads[0]*sizeof(cl_uchar16) );//local tree,
        kern->set_local_arg( lThreads[0]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer

        //execute kernel
        kern->execute(queue, 1, lThreads, gThreads);
        int status = clFinish(queue);
        if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
            return false;
        gpu_time += kern->exec_time();
        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
        //read image out to buffer (from gpu)
        alpha->read_to_buffer(queue);
        clFinish(queue);
    }
    std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
    clReleaseCommandQueue(queue);
    return true;
}

//Returns vector of color update kernels (and caches them per device
std::vector<bocl_kernel*>& boxm2_ocl_update_sky2::get_kernels(const bocl_device_sptr& device, const std::string& opts, bool  /*isRGB*/)
{
    // compile kernels if not already compiled
    std::string identifier = device->device_identifier() + opts;
    if (kernels_.find(identifier) != kernels_.end())
        return kernels_[identifier];

    //otherwise compile the kernels
    std::cout<<"=== boxm2_ocl_update_sky_process::compiling kernels on device "<<identifier<<"==="<<std::endl;

    std::vector<std::string> src_paths;
    std::string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "pixel_conversion.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "atomics_util.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
     src_paths.push_back(source_dir +"stat/chisqr_cdf.cl");
    src_paths.push_back(source_dir + "bit/update_sky_kernels.cl");

    std::vector<std::string> non_ray_src = std::vector<std::string>(src_paths);
    src_paths.push_back(source_dir + "update_functors.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //compilation options
    const std::string& options = opts;

    //populate vector of kernels
    std::vector<bocl_kernel*> vec_kernels;

    //seg len pass
    auto* update_sky_accum_ray_kernel = new bocl_kernel();
    std::string accum_opts = options + " -D UPDATE_SKY2  -D STEP_CELL=step_cell_accumulate_evidence_per_ray(aux_args,data_ptr,llid,d)";
    update_sky_accum_ray_kernel->create_kernel(&device->context(), device->device_id(), src_paths, "accumulate_evidence_sky_ray_main", accum_opts, "accumulate_evidence_sky_ray_main");
    vec_kernels.push_back(update_sky_accum_ray_kernel);

    //: average per cell
    auto* update_sky_accum_cell_kernel = new bocl_kernel();
    std::string cell_accum_opts = options + " -D UPDATE_SKY2_MAIN";
    update_sky_accum_cell_kernel->create_kernel(&device->context(), device->device_id(), non_ray_src, "accumulate_evidence_sky_cell_main", cell_accum_opts, "accumulate_evidence_sky_cell_main");
    vec_kernels.push_back(update_sky_accum_cell_kernel);


    auto* update_sky_main_kernel = new bocl_kernel();
    std::string update_opts = options + " -D UPDATE_SKY2_MAIN  ";
    update_sky_main_kernel->create_kernel(&device->context(), device->device_id(), non_ray_src, "update_sky_main", update_opts, "update_sky_main");
    vec_kernels.push_back(update_sky_main_kernel);

    //store and return
    kernels_[identifier] = vec_kernels;
    return kernels_[identifier];
}
