// This is brl/bseg/boxm2/ocl/algo/boxm2_ocl_update_heightmap_factor.cxx
#include <fstream>
#include <iostream>
#include <algorithm>
#include "boxm2_ocl_update_heightmap_factor.h"
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
#include <boct/boct_bit_tree.h>
#include <vnl/vnl_random.h>
//: Map of kernels should persist between process executions
std::map<std::string, std::vector<bocl_kernel*> > boxm2_ocl_compute_heightmap_pre_post::pre_kernels_;
std::map<std::string, std::vector<bocl_kernel*> > boxm2_ocl_compute_heightmap_pre_post::post_kernels_;
std::map<std::string, std::vector<bocl_kernel*> > boxm2_ocl_update_heightmap_factor::update_heightmap_factor_kernels_;
std::map<std::string, std::vector<bocl_kernel*> > boxm2_ocl_smooth_heightmap_pdata::smooth_heightmap_pdata_kernels_;
//Main public method, updates color model
bool boxm2_ocl_compute_heightmap_pre_post::update_pre(const boxm2_scene_sptr&         scene,
                                                      const bocl_device_sptr&         device,
                                                      const boxm2_opencl_cache_sptr&  opencl_cache,
                                                      const vil_image_view_base_sptr& ximg,
                                                      const vil_image_view_base_sptr& yimg,
                                                      float  /*resnearfactor*/,
                                                      float  /*resfarfactor*/)
{
    enum {
        UPDATE_PRE = 0,
        NORMALIZE_PRE = 1
    };
    float transfer_time = 0.0f;
    float gpu_time = 0.0f;
    std::size_t local_threads[2] = { 8, 8 };
    std::size_t global_threads[2] = { 8, 8 };

    //cache size sanity check
    std::size_t binCache = opencl_cache.ptr()->bytes_in_cache();
    std::cout << "Update MBs in cache: " << binCache / (1024.0*1024.0) << std::endl;

    //make correct data types are here
    std::string data_type, num_obs_type, options;
    // create a command queue.
    int status = 0;
    cl_command_queue queue = clCreateCommandQueue(device->context(), *(device->device_id()), CL_QUEUE_PROFILING_ENABLE, &status);
    if (status != 0)
        return false;

    //grab input image, establish cl_ni, cl_nj (so global size is divisible by local size)
    auto* ximg_view = static_cast<vil_image_view<float>*>(ximg.ptr());
    auto* yimg_view = static_cast<vil_image_view<float>*>(yimg.ptr());

    auto cl_ni = (unsigned)RoundUp(ximg->ni(), (int)local_threads[0]);
    auto cl_nj = (unsigned)RoundUp(ximg->nj(), (int)local_threads[1]);
    global_threads[0] = cl_ni;
    global_threads[1] = cl_nj;
    //set generic cam
    auto* ray_origins = new cl_float[4 * cl_ni*cl_nj];
    auto* ray_directions = new cl_float[4 * cl_ni*cl_nj];
    vgl_box_3d<double> bbox = scene->bounding_box();
    float z = bbox.max_z();
    int count = 0;
    for (unsigned int j = 0; j < cl_nj; ++j) {
        for (unsigned int i = 0; i < cl_ni; ++i) {
            if (i < ximg->ni() && j < ximg->nj())
            {
                ray_origins[count * 4 + 0] = (*ximg_view)(i, j);
                ray_origins[count * 4 + 1] = (*yimg_view)(i, j);
                ray_origins[count * 4 + 2] = z + 1.0f;
                ray_origins[count * 4 + 3] = 0.0f;
                ray_directions[count * 4 + 0] = 0.0;
                ray_directions[count * 4 + 1] = 0.0;
                ray_directions[count * 4 + 2] = -1.0;
                ray_directions[count * 4 + 3] = 0.0f;
            }
            ++count;
        }
    }
    bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins, "ray_origins buffer");
    ray_o_buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions, "ray_directions buffer");
    ray_d_buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    float tnearfar[2] = { 0.0f, 1000000 };

    bocl_mem_sptr tnearfar_mem_ptr = opencl_cache->alloc_mem(2 * sizeof(float), tnearfar, "tnearfar  buffer");
    tnearfar_mem_ptr->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    //Visibility, Preinf, Norm, and input image buffers
    auto* vis_buff = new float[cl_ni*cl_nj];
    auto* pre_buff = new float[cl_ni*cl_nj];

    for (unsigned i = 0; i < cl_ni*cl_nj; i++)
    {
        vis_buff[i] = 1.0f;
        pre_buff[i] = 0.0f;
    }

    bocl_mem_sptr vis_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), vis_buff, "vis image buffer");
    vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    //bocl_mem_sptr pre_image=new bocl_mem(device->context(),pre_buff,cl_ni*cl_nj*sizeof(float),"pre image buffer");
    bocl_mem_sptr pre_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), pre_buff, "pre image buffer");
    pre_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // Image Dimensions
    int img_dim_buff[4];
    img_dim_buff[0] = 0;
    img_dim_buff[1] = 0;
    img_dim_buff[2] = ximg->ni();
    img_dim_buff[3] = ximg->nj();

    bocl_mem_sptr img_dim = new bocl_mem(device->context(), img_dim_buff, sizeof(int) * 4, "image dims");
    img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // Output Array
    float output_arr[100];
    for (float & i : output_arr) i = 0.0f;
    bocl_mem_sptr  cl_output = new bocl_mem(device->context(), output_arr, sizeof(float) * 100, "output buffer");
    cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // bit lookup buffer
    cl_uchar lookup_arr[256];
    boxm2_ocl_util::set_bit_lookup(lookup_arr);
    bocl_mem_sptr lookup = new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar) * 256, "bit lookup buffer");
    lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    // compile the kernel if not already compiled
    std::vector<bocl_kernel*>& kernels = get_pre_kernels(device, options);
    // set arguments
    std::vector<boxm2_block_id> vis_order = scene->get_block_ids();
    std::vector<boxm2_block_id>::iterator id;

    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        for (unsigned int i = 0; i < kernels.size(); ++i)
        {
            //choose correct render kernel
            boxm2_block_metadata mdata = scene->get_block_metadata(*id);
            bocl_kernel* kern = kernels[i];
            //write the image values to the buffer
            vul_timer transfer;
            bocl_mem* blk = opencl_cache->get_block(scene, *id);
            bocl_mem* blk_info = opencl_cache->loaded_block_info();
            bocl_mem* alpha = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_ALPHA>::prefix());
            auto* info_buffer = (boxm2_scene_info*)blk_info->cpu_buffer();
            int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
            info_buffer->data_buffer_length = (int)(alpha->num_bytes() / alphaTypeSize);
            //grab an appropriately sized AUX data buffer
            bocl_mem *aux0 = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix("seglen_h"), 0, false);
            bocl_mem *aux1_ph_smooth = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX1>::prefix("pheight_smooth"), 0, true);
            bocl_mem *aux2 = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX1>::prefix("pre_h"), 0, false);
            bocl_mem *aux3 = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX2>::prefix("vis_h"), 0, false);
            transfer_time += (float)transfer.all();
            if (i == UPDATE_PRE)
            {
                aux0->zero_gpu_buffer(queue);
                aux2->zero_gpu_buffer(queue);
                aux3->zero_gpu_buffer(queue);
                kern->set_arg(blk_info);
                kern->set_arg(blk);
                kern->set_arg(alpha);
                kern->set_arg(aux0);
                kern->set_arg(aux1_ph_smooth);
                kern->set_arg(aux2);
                kern->set_arg(aux3);
                kern->set_arg(lookup.ptr());
                kern->set_arg(ray_o_buff.ptr());
                kern->set_arg(ray_d_buff.ptr());
                kern->set_arg(tnearfar_mem_ptr.ptr());
                kern->set_arg(img_dim.ptr());
                kern->set_arg(vis_image.ptr());
                kern->set_arg(pre_image.ptr());
                kern->set_arg(cl_output.ptr());
                kern->set_local_arg(local_threads[0] * local_threads[1] * sizeof(cl_uchar16));//local tree,
                kern->set_local_arg(local_threads[0] * local_threads[1] * 10 * sizeof(cl_uchar)); //cumsum buffer, imindex buffer
                //execute kernel
                kern->execute(queue, 2, local_threads, global_threads);
                int status = clFinish(queue);
                if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
                    return false;
                gpu_time += kern->exec_time();
                //clear render kernel args so it can reset em on next execution
                kern->clear_args();
                //write info to disk
            }
            else if (i == NORMALIZE_PRE)
            {
                blk_info->write_to_buffer((queue));

                std::size_t lt[1], gt[1];
                lt[0] = 64;
                gt[0] = RoundUp(info_buffer->data_buffer_length, lt[0]);

                kern->set_arg(blk_info);
                kern->set_arg(aux0);
                kern->set_arg(aux2);
                kern->set_arg(aux3);
                kern->execute(queue, 1, lt, gt);
                int status = clFinish(queue);
                if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
                    return false;
                gpu_time += kern->exec_time();
                aux2->read_to_buffer(queue);
                aux3->read_to_buffer(queue);
                //clear render kernel args so it can reset em on next execution
                kern->clear_args();
                opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix("seglen_h"), false);
                opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX1>::prefix("pheight_smooth"), false);
                opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX1>::prefix("pre_h"), true);
                opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX2>::prefix("vis_h"), true);
            }
            //read image out to buffer (from gpu)
            vis_image->read_to_buffer(queue);
            pre_image->read_to_buffer(queue);
            clFinish(queue);
        }
    }

    delete[] vis_buff;
    delete[] pre_buff;
    delete[] ray_origins;
    delete[] ray_directions;
    //opencl_cache->unref_mem(hmean_image.ptr());
    //opencl_cache->unref_mem(hvar_image.ptr());
    opencl_cache->unref_mem(vis_image.ptr());
    opencl_cache->unref_mem(pre_image.ptr());
    opencl_cache->unref_mem(ray_o_buff.ptr());
    opencl_cache->unref_mem(ray_d_buff.ptr());
    opencl_cache->unref_mem(tnearfar_mem_ptr.ptr());
    std::cout << "Gpu time " << gpu_time << " transfer time " << transfer_time << std::endl;
    clReleaseCommandQueue(queue);
    return true;
}

//Main public method, updates color model
bool boxm2_ocl_compute_heightmap_pre_post::update_post(const boxm2_scene_sptr&         scene,
    const bocl_device_sptr&         device,
    const boxm2_opencl_cache_sptr&  opencl_cache,
    const vil_image_view_base_sptr& ximg,
    const vil_image_view_base_sptr& yimg,
    float  /*resnearfactor*/,
    float  /*resfarfactor*/)
{
    enum {
        UPDATE_POST = 0,
        NORMALIZE_POST = 1
    };
    float transfer_time = 0.0f;
    float gpu_time = 0.0f;
    std::size_t local_threads[2] = { 8, 8 };
    std::size_t global_threads[2] = { 8, 8 };

    //cache size sanity check
    std::size_t binCache = opencl_cache.ptr()->bytes_in_cache();
    std::cout << "Update MBs in cache: " << binCache / (1024.0*1024.0) << std::endl;

    //make correct data types are here
    std::string data_type, num_obs_type, options;

    // create a command queue.
    int status = 0;
    cl_command_queue queue = clCreateCommandQueue(device->context(),
        *(device->device_id()),
        CL_QUEUE_PROFILING_ENABLE,
        &status);
    if (status != 0)
        return false;

    auto* ximg_view = static_cast<vil_image_view<float>*>(ximg.ptr());
    auto* yimg_view = static_cast<vil_image_view<float>*>(yimg.ptr());
    auto cl_ni = (unsigned)RoundUp(ximg->ni(), (int)local_threads[0]);
    auto cl_nj = (unsigned)RoundUp(ximg->nj(), (int)local_threads[1]);
    global_threads[0] = cl_ni;
    global_threads[1] = cl_nj;
    //set generic cam
    auto* ray_origins = new cl_float[4 * cl_ni*cl_nj];
    auto* ray_directions = new cl_float[4 * cl_ni*cl_nj];
    vgl_box_3d<double> bbox = scene->bounding_box();
    float z = bbox.max_z();
    int count = 0;
    for (unsigned int j = 0; j < cl_nj; ++j) {
        for (unsigned int i = 0; i < cl_ni; ++i) {
            if (i < ximg->ni() && j < ximg->nj())
            {
                ray_origins[count * 4 + 0] = (*ximg_view)(i, j);
                ray_origins[count * 4 + 1] = (*yimg_view)(i, j);
                ray_origins[count * 4 + 2] = z + 1.0f;
                ray_origins[count * 4 + 3] = 0.0f;
                ray_directions[count * 4 + 0] = 0.0;
                ray_directions[count * 4 + 1] = 0.0;
                ray_directions[count * 4 + 2] = -1.0;
                ray_directions[count * 4 + 3] = 0.0f;
            }
            ++count;
        }
    }
    bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins, "ray_origins buffer");
    ray_o_buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions, "ray_directions buffer");
    ray_d_buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    float tnearfar[2] = { 0.0f, 1000000 };
    bocl_mem_sptr tnearfar_mem_ptr = opencl_cache->alloc_mem(2 * sizeof(float), tnearfar, "tnearfar  buffer");
    tnearfar_mem_ptr->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    //Visibility, Preinf, Norm, and input image buffers
    auto* vis_buff = new float[cl_ni*cl_nj];
    auto* post_buff = new float[cl_ni*cl_nj];

    for (unsigned i = 0; i < cl_ni*cl_nj; i++)
    {
        vis_buff[i] = 1.0f;
        post_buff[i] = 0.0f;
    }


    bocl_mem_sptr vis_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), vis_buff, "vis image buffer");
    vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    bocl_mem_sptr post_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), post_buff, "pre image buffer");
    post_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // Image Dimensions
    int img_dim_buff[4];
    img_dim_buff[0] = 0;
    img_dim_buff[1] = 0;
    img_dim_buff[2] = ximg->ni();
    img_dim_buff[3] = ximg->nj();

    bocl_mem_sptr img_dim = new bocl_mem(device->context(), img_dim_buff, sizeof(int) * 4, "image dims");
    img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // Output Array
    float output_arr[100];
    for (float & i : output_arr) i = 0.0f;
    bocl_mem_sptr  cl_output = new bocl_mem(device->context(), output_arr, sizeof(float) * 100, "output buffer");
    cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // bit lookup buffer
    cl_uchar lookup_arr[256];
    boxm2_ocl_util::set_bit_lookup(lookup_arr);
    bocl_mem_sptr lookup = new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar) * 256, "bit lookup buffer");
    lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    // compile the kernel if not already compiled
    std::vector<bocl_kernel*>& kernels = get_post_kernels(device, options);
    // set arguments
    std::vector<boxm2_block_id> vis_order;
    vis_order = scene->get_block_ids();
    std::vector<boxm2_block_id>::iterator id;
    for (unsigned int i = 0; i < kernels.size(); ++i)
    {
        bocl_kernel* kern = kernels[i];
        for (id = vis_order.begin(); id != vis_order.end(); ++id)
        {
            //choose correct render kernel
            boxm2_block_metadata mdata = scene->get_block_metadata(*id);
            //write the image values to the buffer
            vul_timer transfer;
            bocl_mem* blk = opencl_cache->get_block(scene, *id);
            bocl_mem* blk_info = opencl_cache->loaded_block_info();
            bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene, *id, 0, false);
            auto* info_buffer = (boxm2_scene_info*)blk_info->cpu_buffer();
            int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
            info_buffer->data_buffer_length = (int)(alpha->num_bytes() / alphaTypeSize);
            // data type string may contain an identifier so determine the buffer size
            //grab an appropriately sized AUX data buffer
            bocl_mem *aux0 = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix("post_seglen_h"), 0, false);
            bocl_mem *aux1 = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX1>::prefix("post_h"), 0, false);
            bocl_mem *aux1_ph_smooth = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX1>::prefix("pheight_smooth"), 0, false);
            transfer_time += (float)transfer.all();

            if (i == UPDATE_POST)
            {
                aux0->zero_gpu_buffer(queue);
                aux1->zero_gpu_buffer(queue);
                kern->set_arg(blk_info);
                kern->set_arg(blk);
                kern->set_arg(alpha);
                kern->set_arg(aux0);
                kern->set_arg(aux1_ph_smooth);
                kern->set_arg(aux1);
                kern->set_arg(lookup.ptr());
                kern->set_arg(ray_o_buff.ptr());
                kern->set_arg(ray_d_buff.ptr());
                kern->set_arg(tnearfar_mem_ptr.ptr());
                kern->set_arg(img_dim.ptr());
                kern->set_arg(vis_image.ptr());
                kern->set_arg(post_image.ptr());
                kern->set_arg(cl_output.ptr());
                kern->set_local_arg(local_threads[0] * local_threads[1] * sizeof(cl_uchar16));//local tree,
                kern->set_local_arg(local_threads[0] * local_threads[1] * 10 * sizeof(cl_uchar)); //cumsum buffer, imindex buffer
                //execute kernel
                kern->execute(queue, 2, local_threads, global_threads);
                int status = clFinish(queue);
                if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
                    return false;
                gpu_time += kern->exec_time();
                clFinish(queue);
                //clear render kernel args so it can reset em on next execution
                kern->clear_args();
                //write info to disk

            }
            else if (i == NORMALIZE_POST)
            {
                blk_info->write_to_buffer((queue));

                std::size_t lt[1], gt[1];
                lt[0] = 64;
                gt[0] = RoundUp(info_buffer->data_buffer_length, lt[0]);

                kern->set_arg(blk_info);
                kern->set_arg(aux0);
                kern->set_arg(aux1);
                kern->execute(queue, 1, lt, gt);
                int status = clFinish(queue);
                if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
                    return false;
                gpu_time += kern->exec_time();
                aux1->read_to_buffer(queue);

                //clear render kernel args so it can reset em on next execution
                kern->clear_args();
                aux1->read_to_buffer(queue);
                opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix("post_seglen_h"), false);
                opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX1>::prefix("post_h"), true);
                opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX1>::prefix("pheight_smooth"), false);

            }
            //read image out to buffer (from gpu)
            vis_image->read_to_buffer(queue);
            post_image->read_to_buffer(queue);
            cl_output->read_to_buffer(queue);
            clFinish(queue);
        }
    }
    delete[] vis_buff;
    delete[] post_buff;
    delete[] ray_origins;
    delete[] ray_directions;

    opencl_cache->unref_mem(vis_image.ptr());
    opencl_cache->unref_mem(post_image.ptr());
    opencl_cache->unref_mem(ray_o_buff.ptr());
    opencl_cache->unref_mem(ray_d_buff.ptr());
    opencl_cache->unref_mem(tnearfar_mem_ptr.ptr());
    std::cout << "Gpu time " << gpu_time << " transfer time " << transfer_time << std::endl;
    clReleaseCommandQueue(queue);

    return true;
}

//Main public method, updates color model
bool boxm2_ocl_compute_heightmap_pre_post::compute_pre_post(const boxm2_scene_sptr&         scene,
    const bocl_device_sptr&         device,
    const boxm2_opencl_cache_sptr&  opencl_cache,
    const vil_image_view_base_sptr& hmap_mean,
    const vil_image_view_base_sptr& hmap_var,
    const vil_image_view_base_sptr& ximg,
    const vil_image_view_base_sptr& yimg,
    int smoothingradius,
    float resnearfactor,
    float resfarfactor)
{
    boxm2_ocl_smooth_heightmap_pdata::compute_smooth_heightmap_pdata(scene, device, opencl_cache, hmap_mean, hmap_var, ximg, yimg, smoothingradius);
    boxm2_ocl_compute_heightmap_pre_post::update_pre(scene, device, opencl_cache, ximg, yimg, resnearfactor, resfarfactor);
    boxm2_ocl_compute_heightmap_pre_post::update_post(scene, device, opencl_cache, ximg, yimg, resnearfactor, resfarfactor);
    return true;
}
bool boxm2_ocl_update_heightmap_factor::update_heightmap_factor(const boxm2_scene_sptr&         scene,
                                                                const bocl_device_sptr&         device,
                                                                const boxm2_opencl_cache_sptr&  opencl_cache,
                                                                bool add)
{
    float transfer_time = 0.0f;
    float gpu_time = 0.0f;
    std::size_t local_threads[1] = { 64 };
    std::size_t global_threads[1] = { 64 };
    //cache size sanity check
    std::size_t binCache = opencl_cache.ptr()->bytes_in_cache();
    std::cout << "Update MBs in cache: " << binCache / (1024.0*1024.0) << std::endl;

    //make correct data types are here
    std::string data_type, num_obs_type, options;
    int does_add_buf = add ? 1 : 0;
    bocl_mem_sptr does_add = new bocl_mem(device->context(), &does_add_buf, sizeof(int) * 1, "add (1) or subtract (0)");
    does_add->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // create a command queue.
    int status = 0;
    cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),CL_QUEUE_PROFILING_ENABLE,&status);
    if (status != 0)
        return false;
    // compile the kernel if not already compiled
    bocl_kernel * kern = get_update_heightmap_factor_kernels(device, options)[0];
    std::vector<boxm2_block_id> blks_order;
    blks_order = scene->get_block_ids();
    std::vector<boxm2_block_id>::iterator  id;

    for (id = blks_order.begin(); id != blks_order.end(); ++id)
    {
        //choose correct render kernel
        boxm2_block_metadata mdata = scene->get_block_metadata(*id);
        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk = opencl_cache->get_block(scene, *id);
        bocl_mem* blk_info = opencl_cache->loaded_block_info();
        bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene, *id, 0, false);
        auto* info_buffer = (boxm2_scene_info*)blk_info->cpu_buffer();
        int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        info_buffer->data_buffer_length = (int)(alpha->num_bytes() / alphaTypeSize);
        blk_info->write_to_buffer((queue));
        local_threads[0] = 64;
        global_threads[0] = RoundUp(info_buffer->data_buffer_length, local_threads[0]);
        //grab an appropriately sized AUX data buffer
        bocl_mem *aux1_pre = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX1>::prefix("pre_h"), 0, true);
        bocl_mem *aux2_pre = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX2>::prefix("vis_h"), 0, true);
        bocl_mem *aux1 = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX1>::prefix("post_h"), 0, false);
        bocl_mem *aux1_ph_smooth = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX1>::prefix("pheight_smooth"), 0, false);
        bocl_mem *aux0_hf = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix("hf"), 0, false);

        transfer_time += (float)transfer.all();
        kern->set_arg(blk_info);
        kern->set_arg(does_add.ptr());
        kern->set_arg(aux1_pre);
        kern->set_arg(aux2_pre);
        kern->set_arg(aux1);
        kern->set_arg(aux1_ph_smooth);
        kern->set_arg(aux0_hf);
        //execute kernel
        kern->execute(queue, 1, local_threads, global_threads);
        int status = clFinish(queue);
        if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
            return false;
        gpu_time += kern->exec_time();
        //clear render kernel args so it can reset em on next execution
        aux0_hf->read_to_buffer(queue);
        kern->clear_args();
        opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX1>::prefix("pre_h"), false);
        opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX2>::prefix("vis_h"), false);
        opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX2>::prefix("post_h"), false);
        opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX3>::prefix("post_h"), false);
        opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix("hf"), true);
    }
    clFinish(queue);
    opencl_cache->unref_mem(does_add.ptr());
    return true;
}

//Returns vector of color update kernels (and caches them per device
std::vector<bocl_kernel*>& boxm2_ocl_compute_heightmap_pre_post::get_pre_kernels(const bocl_device_sptr& device, const std::string& opts)
{
    // compile kernels if not already compiled
    std::string identifier = device->device_identifier() + opts;
    if (pre_kernels_.find(identifier) != pre_kernels_.end())
        return pre_kernels_[identifier];

    //otherwise compile the kernels
    std::cout << "=== boxm2_ocl_update_process::compiling kernels on device " << identifier << "===" << std::endl;
    std::vector<std::string> src_paths;
    std::string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "pixel_conversion.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "atomics_util.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/update_bp_kernels.cl");
    std::vector<std::string> non_ray_src = std::vector<std::string>(src_paths);
    src_paths.push_back(source_dir + "update_functors.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //compilation options
    std::string options = "-D ATOMIC_FLOAT ";
    //populate vector of kernels
    std::vector<bocl_kernel*> vec_kernels;

    auto* pre = new bocl_kernel();
    std::string pre_opts = options + " -D PRE_HMAP_CELL  -D STEP_CELL=step_cell_pre_hmap(aux_args,data_ptr,llid,d)";
    pre->create_kernel(&device->context(), device->device_id(), src_paths, "pre_hmap_main", pre_opts, "update::pre_hmap_main");
    vec_kernels.push_back(pre);
    auto* normalize_pre = new bocl_kernel();
    std::string normalize_pre_opts = options + " -D PRE_HMAP_CELL ";
    normalize_pre->create_kernel(&device->context(), device->device_id(), src_paths, "normalize_prehmap_main", pre_opts, "update::normalize_prehmap_main");
    vec_kernels.push_back(normalize_pre);
    //store and return
    pre_kernels_[identifier] = vec_kernels;
    return pre_kernels_[identifier];
}

std::vector<bocl_kernel*>& boxm2_ocl_compute_heightmap_pre_post::get_post_kernels(const bocl_device_sptr& device, const std::string& opts)
{
    // compile kernels if not already compiled
    std::string identifier = device->device_identifier() + opts;
    if (post_kernels_.find(identifier) != post_kernels_.end())
        return post_kernels_[identifier];

    //otherwise compile the kernels
    std::cout << "=== boxm2_ocl_update_process::compiling kernels on device " << identifier << "===" << std::endl;
    std::vector<std::string> src_paths;
    std::string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "pixel_conversion.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "atomics_util.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/update_bp_kernels.cl");
    std::vector<std::string> non_ray_src = std::vector<std::string>(src_paths);
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //compilation options
    std::string options = "-D ATOMIC_FLOAT  -D REVERSE";
    //populate vector of kernels
    std::vector<bocl_kernel*> vec_kernels;
    //seg len pass

    auto* post = new bocl_kernel();
    std::string post_opts = options + " -D POST_HMAP_CELL  -D STEP_CELL=step_cell_post_hmap(aux_args,data_ptr,llid,d)";
    post->create_kernel(&device->context(), device->device_id(), src_paths, "post_hmap_main", post_opts, "update::post_Cell");
    vec_kernels.push_back(post);

    auto* normalize_post = new bocl_kernel();
    std::string normalize_post_opts = options + " -D NORMALIZE_POST_CELL ";
    normalize_post->create_kernel(&device->context(), device->device_id(), non_ray_src, "normalize_post_cell", normalize_post_opts, "update::normalize_post_cell");
    vec_kernels.push_back(normalize_post);
    //store and return
    post_kernels_[identifier] = vec_kernels;
    return post_kernels_[identifier];
}

std::vector<bocl_kernel*>& boxm2_ocl_update_heightmap_factor::get_update_heightmap_factor_kernels(const bocl_device_sptr& device, const std::string& opts)
{
    // compile kernels if not already compiled
    std::string identifier = device->device_identifier() + opts;
    if (update_heightmap_factor_kernels_.find(identifier) != update_heightmap_factor_kernels_.end())
        return update_heightmap_factor_kernels_[identifier];

    //otherwise compile the kernels
    std::cout << "=== boxm2_ocl_update_process::compiling kernels on device " << identifier << "===" << std::endl;
    std::vector<std::string> src_paths;
    std::string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "pixel_conversion.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "atomics_util.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/update_bp_kernels.cl");

    //compilation options
    std::string options = "-D ATOMIC_FLOAT -D ADD_SUBTRACT_FACTOR";
    //populate vector of kernels
    std::vector<bocl_kernel*> vec_kernels;
    auto* computez = new bocl_kernel();
    const std::string& computez_opts = options;
    computez->create_kernel(&device->context(), device->device_id(), src_paths, "add_subtract_factor_main", computez_opts, "update::add_subtract_factor_main");
    vec_kernels.push_back(computez);

    //store and return
    update_heightmap_factor_kernels_[identifier] = vec_kernels;
    return update_heightmap_factor_kernels_[identifier];
}


bool boxm2_ocl_smooth_heightmap_pdata::
compute_smooth_heightmap_pdata(boxm2_scene_sptr         scene,
                                const bocl_device_sptr&         device,
                                const boxm2_opencl_cache_sptr&  opencl_cache,
                                const vil_image_view_base_sptr& hmap_mean,
                                const vil_image_view_base_sptr& hmap_var,
                                const vil_image_view_base_sptr& ximg,
                                const vil_image_view_base_sptr& yimg,
                                int smoothingradius = 16,
                                float  /*resnearfactor*/,
                                float  /*resfarfactor*/)
{

    float transfer_time = 0.0f;
    float gpu_time = 0.0f;
    std::size_t local_threads[2] = { 8, 8 };
    std::size_t global_threads[2] = { 8, 8 };

    //cache size sanity check
    std::size_t binCache = opencl_cache.ptr()->bytes_in_cache();
    std::cout << "Update MBs in cache: " << binCache / (1024.0*1024.0) << std::endl;

    //make correct data types are here
    std::string data_type, num_obs_type, options;

    // create a command queue.
    int status = 0;
    cl_command_queue queue = clCreateCommandQueue(device->context(),
        *(device->device_id()),
        CL_QUEUE_PROFILING_ENABLE,
        &status);
    if (status != 0)
        return false;

    //grab input image, establish cl_ni, cl_nj (so global size is divisible by local size)

    auto* hmap_mean_view = static_cast<vil_image_view<float>*>(hmap_mean.ptr());
    auto* hmap_var_view = static_cast<vil_image_view<float>*>(hmap_var.ptr());
    auto* ximg_view = static_cast<vil_image_view<float>*>(ximg.ptr());
    auto* yimg_view = static_cast<vil_image_view<float>*>(yimg.ptr());

    auto cl_ni = (unsigned)RoundUp(hmap_mean_view->ni(), (int)local_threads[0]);
    auto cl_nj = (unsigned)RoundUp(hmap_mean_view->nj(), (int)local_threads[1]);
    global_threads[0] = cl_ni;
    global_threads[1] = cl_nj;
    //set generic cam
    auto* ray_origins = new cl_float[4 * cl_ni*cl_nj];
    auto* ray_directions = new cl_float[4 * cl_ni*cl_nj];
    vgl_box_3d<double> bbox = scene->bounding_box();
    float z = bbox.max_z();
    int count = 0;
    for (unsigned int j = 0; j < cl_nj; ++j) {
        for (unsigned int i = 0; i < cl_ni; ++i) {
            if (i < hmap_mean_view->ni() && j < hmap_mean_view->nj())
            {
                ray_origins[count * 4 + 0] = (*ximg_view)(i, j);
                ray_origins[count * 4 + 1] = (*yimg_view)(i, j);
                ray_origins[count * 4 + 2] = z + 1.0f;
                ray_origins[count * 4 + 3] = 0.0f;
                ray_directions[count * 4 + 0] = 0.0;
                ray_directions[count * 4 + 1] = 0.0;
                ray_directions[count * 4 + 2] = -1.0;
                ray_directions[count * 4 + 3] = 0.0f;
            }
            ++count;
        }
    }
    bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins, "ray_origins buffer");
    ray_o_buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions, "ray_directions buffer");
    ray_d_buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    float tnearfar[2] = { 0.0f, 1000000 };
    bocl_mem_sptr tnearfar_mem_ptr = opencl_cache->alloc_mem(2 * sizeof(float), tnearfar, "tnearfar  buffer");
    tnearfar_mem_ptr->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    //Visibility, Preinf, Norm, and input image buffers
    auto* hmean_buff = new float[cl_ni*cl_nj];
    auto* hvar_buff = new float[cl_ni*cl_nj];

    //copy input vals into image
    count = 0;
    for (unsigned int j = 0; j < cl_nj; ++j) {
        for (unsigned int i = 0; i < cl_ni; ++i) {
            hmean_buff[count] = 0.0f;
            hvar_buff[count] = 0.0f;
            if (i < hmap_mean_view->ni() && j < hmap_mean_view->nj())
            {
                hmean_buff[count] = (*hmap_mean_view)(i, j);
                hvar_buff[count] = (*hmap_var_view)(i, j);
            }
            ++count;
        }
    }
    bocl_mem_sptr hmean_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), hmean_buff, "input image buffer");
    hmean_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    bocl_mem_sptr hvar_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), hvar_buff, "input image buffer");
    hvar_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    // Image Dimensions
    int img_dim_buff[4];
    img_dim_buff[0] = 0;
    img_dim_buff[1] = 0;
    img_dim_buff[2] = hmap_mean_view->ni();
    img_dim_buff[3] = hmap_mean_view->nj();

    bocl_mem_sptr img_dim = new bocl_mem(device->context(), img_dim_buff, sizeof(int) * 4, "image dims");
    img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // Output Array
    float output_arr[100];
    for (float & i : output_arr) i = 0.0f;
    bocl_mem_sptr  cl_output = new bocl_mem(device->context(), output_arr, sizeof(float) * 100, "output buffer");
    cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // bit lookup buffer
    cl_uchar lookup_arr[256];
    boxm2_ocl_util::set_bit_lookup(lookup_arr);
    bocl_mem_sptr lookup = new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar) * 256, "bit lookup buffer");
    lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    //: avg the Pheight with the neighbors
    vnl_random rand;
    int numsamples = 16;
    int rad = smoothingradius;
    auto * weights = new float[numsamples];
    int * pts = new int[2 * numsamples];
    for (int i = 0; i < numsamples;)
    {
        int x = -rad + rand.lrand32(0, 2 * rad + 1);
        int y = -rad + rand.lrand32(0, 2 * rad + 1);
        if (x == 0 && y == 0) x = 1;
        pts[2 * i] = x;
        pts[2 * i + 1] = y;
        weights[i] = 1 - std::sqrt(float(x*x + y*y)) / (float(rad));
        i++;
    }
    typedef vnl_vector_fixed<unsigned char, 16> uchar16;
    // compile the kernel if not already compiled
    std::vector<bocl_kernel*>& kernels = get_smooth_heightmap_pdata_kernels(device, options);
    // set arguments
    std::vector<boxm2_block_id> vis_order;
    vis_order = scene->get_block_ids();
    std::vector<boxm2_block_id>::iterator id;
    bocl_kernel* kern = kernels[0];
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        //choose correct render kernel
        boxm2_block_metadata mdata = scene->get_block_metadata(*id);
        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk = opencl_cache->get_block(scene, *id);
        bocl_mem* blk_info = opencl_cache->loaded_block_info();
        //grab an appropriately sized AUX data buffer
        bocl_mem *aux0 = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix("length"), 0, false);
        bocl_mem *aux1 = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX1>::prefix("pheight"), 0, false);

        transfer_time += (float)transfer.all();
        aux0->zero_gpu_buffer(queue);
        aux1->zero_gpu_buffer(queue);
        kern->set_arg(blk_info);
        kern->set_arg(blk);
        kern->set_arg(aux0);
        kern->set_arg(aux1);
        kern->set_arg(lookup.ptr());
        kern->set_arg(ray_o_buff.ptr());
        kern->set_arg(ray_d_buff.ptr());
        kern->set_arg(tnearfar_mem_ptr.ptr());
        kern->set_arg(img_dim.ptr());
        kern->set_arg(hmean_image.ptr());
        kern->set_arg(hvar_image.ptr());
        kern->set_arg(cl_output.ptr());
        kern->set_local_arg(local_threads[0] * local_threads[1] * sizeof(cl_uchar16));
        kern->set_local_arg(local_threads[0] * local_threads[1] * 10 * sizeof(cl_uchar));
        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
            return false;
        gpu_time += kern->exec_time();
        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
        aux0->read_to_buffer(queue);
        aux1->read_to_buffer(queue);
       // boxm2_block_metadata data = scene->get_block_metadata(*id);
        boxm2_block * cblk = opencl_cache->get_cpu_cache()->get_block(scene, *id);
        //get data from cache
        boxm2_data_base * aux0_len_b = opencl_cache->get_cpu_cache()->get_data_base(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix("length"));
        boxm2_data_base * aux1_ph_b = opencl_cache->get_cpu_cache()->get_data_base(scene, *id, boxm2_data_traits<BOXM2_AUX1>::prefix("pheight"));
        boxm2_data_base * aux1_ph_smooth_b = opencl_cache->get_cpu_cache()->get_data_base(scene, *id, boxm2_data_traits<BOXM2_AUX1>::prefix("pheight_smooth"), 0, false);
        //3d array of trees
        const boxm2_array_3d<uchar16>& trees = cblk->trees();
        auto * aux0_len = (boxm2_data_traits<BOXM2_AUX0>::datatype*) aux0_len_b->data_buffer();
        auto * aux1_ph = (boxm2_data_traits<BOXM2_AUX1>::datatype*) aux1_ph_b->data_buffer();
        auto * aux1_ph_smooth = (boxm2_data_traits<BOXM2_AUX1>::datatype*) aux1_ph_smooth_b->data_buffer();

        //iterate through each tree
        for (unsigned int x = 0; x < trees.get_row1_count(); ++x) {
            for (unsigned int y = 0; y < trees.get_row2_count(); ++y) {
                for (unsigned int z = 0; z < trees.get_row3_count(); ++z) {
                    //load current block/tree
                    uchar16 tree = trees(x, y, z);
                    boct_bit_tree bit_tree((unsigned char*)tree.data_block(), mdata.max_level_);
                    //iterate through leaves of the tree
                    std::vector<int> leafBits = bit_tree.get_leaf_bits(0);
                    std::vector<int>::iterator iter;
                    for (iter = leafBits.begin(); iter != leafBits.end(); ++iter)
                    {
                        int currIdx = bit_tree.get_data_index((*iter)); //data index
                        vgl_point_3d<double> localCenter = bit_tree.cell_center((*iter));
                        float px = (localCenter.x() + x)*mdata.sub_block_dim_.x() + mdata.local_origin_.x();
                        float py = (localCenter.y() + y)*mdata.sub_block_dim_.y() + mdata.local_origin_.y();
                        float pz = (localCenter.z() + z)*mdata.sub_block_dim_.z() + mdata.local_origin_.z();
                        double sumweight = 1.0;
                        if (aux0_len[currIdx] > 1e-10)
                            aux1_ph_smooth[currIdx] = aux1_ph[currIdx] / aux0_len[currIdx];
                        else
                            aux1_ph_smooth[currIdx] = 0.0;
                        //: iterate over neighbors
                        for (unsigned k = 0; k < numsamples; k++)
                        {
                            vgl_point_3d<double> neighbor_pt(px + pts[2 * k] * mdata.sub_block_dim_.x(),
                                                             py + pts[2 * k + 1] * mdata.sub_block_dim_.y(), pz);
                            unsigned int data_index = 0;
                            if (cblk->data_index(neighbor_pt, data_index))
                            {
                                if (aux0_len[data_index] > 1e-10)
                                {
                                    aux1_ph_smooth[currIdx] +=  (aux1_ph[data_index] / aux0_len[data_index]);
                                    sumweight += weights[k];
                                }
                            }
                        }
                        aux1_ph_smooth[currIdx] = aux1_ph_smooth[currIdx] / sumweight;
                    }
                }
            }
        }
        opencl_cache->get_cpu_cache()->remove_data_base(scene, *id, boxm2_data_traits<BOXM2_AUX1>::prefix("pheight_smooth"), true);
        opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX1>::prefix("pheight"), false);
        opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix("length"), false);
    }
    clFinish(queue);

    delete[] hmean_buff;
    delete[] hvar_buff;
    delete[] ray_origins;
    delete[] ray_directions;
    opencl_cache->unref_mem(hmean_image.ptr());
    opencl_cache->unref_mem(hvar_image.ptr());
    opencl_cache->unref_mem(ray_o_buff.ptr());
    opencl_cache->unref_mem(ray_d_buff.ptr());
    opencl_cache->unref_mem(tnearfar_mem_ptr.ptr());
    std::cout << "Gpu time " << gpu_time << " transfer time " << transfer_time << std::endl;
    clReleaseCommandQueue(queue);

    delete[] weights;
    delete[] pts;
    return true;
}
std::vector<bocl_kernel*>& boxm2_ocl_smooth_heightmap_pdata::
get_smooth_heightmap_pdata_kernels(const bocl_device_sptr& device, const std::string& opts)
{
    // compile kernels if not already compiled
    std::string identifier = device->device_identifier() + opts;
    if (smooth_heightmap_pdata_kernels_.find(identifier) != smooth_heightmap_pdata_kernels_.end())
        return smooth_heightmap_pdata_kernels_[identifier];

    //otherwise compile the kernels
    std::cout << "=== boxm2_ocl_update_process::compiling kernels on device " << identifier << "===" << std::endl;
    std::vector<std::string> src_paths;
    std::string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "pixel_conversion.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "atomics_util.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/update_bp_kernels.cl");
    std::vector<std::string> non_ray_src = std::vector<std::string>(src_paths);
    src_paths.push_back(source_dir + "update_functors.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //compilation options
    std::string options = "-D ATOMIC_FLOAT ";
    //populate vector of kernels
    std::vector<bocl_kernel*> vec_kernels;
    //seg len pass
    auto* seg_len = new bocl_kernel();
    std::string seg_opts = options + " -D HMAP_DENSITY_CELL  -D STEP_CELL=step_cell_hmap_density(aux_args,data_ptr,llid,d,tblock)";
    seg_len->create_kernel(&device->context(), device->device_id(), src_paths, "compute_hmap_density_main", seg_opts, "update::hmap_density_main");
    vec_kernels.push_back(seg_len);

    //store and return
    smooth_heightmap_pdata_kernels_[identifier] = vec_kernels;
    return smooth_heightmap_pdata_kernels_[identifier];
}
