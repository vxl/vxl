// This is brl/bseg/boxm2/ocl/algo/boxm2_ocl_update_image_factor.cxx
#include <fstream>
#include <iostream>
#include <algorithm>
#include "boxm2_ocl_update_image_factor.h"
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

//: Map of kernels should persist between process executions
std::map<std::string, std::vector<bocl_kernel*> > boxm2_ocl_compute_image_term::image_kernels_;
std::map<std::string, std::vector<bocl_kernel*> > boxm2_ocl_compute_pre_post::pre_kernels_;
std::map<std::string, std::vector<bocl_kernel*> > boxm2_ocl_compute_pre_post::post_kernels_;
std::map<std::string, std::vector<bocl_kernel*> > boxm2_ocl_update_image_factor::update_image_factor_kernels_;

//Main public method, updates color model
bool boxm2_ocl_compute_image_term::
compute_image_term(const boxm2_scene_sptr&         scene,
                   bocl_device_sptr         device,
                   const boxm2_opencl_cache_sptr&  opencl_cache,
                   vpgl_camera_double_sptr  cam,
                   const vil_image_view_base_sptr& img,
                   const std::string& view_identifier,
                   float resnearfactor,
                   float resfarfactor)
{
    enum {
        UPDATE_SEGLEN = 0,
        COMPUTE_IMAGEDEN = 1,
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
    // compile the kernel if not already compiled
    std::vector<bocl_kernel*>& kernels = get_image_kernels(device, options);

    //grab input image, establish cl_ni, cl_nj (so global size is divisible by local size)
    vil_image_view_base_sptr float_img = boxm2_util::prepare_input_image(img, true);
    auto* img_view = static_cast<vil_image_view<float>*>(float_img.ptr());
    auto cl_ni = (unsigned)RoundUp(img_view->ni(), (int)local_threads[0]);
    auto cl_nj = (unsigned)RoundUp(img_view->nj(), (int)local_threads[1]);
    global_threads[0] = cl_ni;
    global_threads[1] = cl_nj;
    //set generic cam
    auto* ray_origins = new cl_float[4 * cl_ni*cl_nj];
    auto* ray_directions = new cl_float[4 * cl_ni*cl_nj];
    bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins, "ray_origins buffer");
    bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions, "ray_directions buffer");
    boxm2_ocl_camera_converter::compute_ray_image(device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

    float tnearfar[2] = { 0.0f, 1000000 };

    if (cam->type_name() == "vpgl_perspective_camera")
    {

        float f = ((vpgl_perspective_camera<double> *)cam.ptr())->get_calibration().focal_length()*((vpgl_perspective_camera<double> *)cam.ptr())->get_calibration().x_scale();
        std::cout << "Focal Length " << f << std::endl;
        tnearfar[0] = f* scene->finest_resolution() / resnearfactor;
        tnearfar[1] = f* scene->finest_resolution()*resfarfactor;
        std::cout << "Near and Far Clipping planes " << tnearfar[0] << " " << tnearfar[1] << std::endl;
    }
    bocl_mem_sptr tnearfar_mem_ptr = opencl_cache->alloc_mem(2 * sizeof(float), tnearfar, "tnearfar  buffer");
    tnearfar_mem_ptr->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    // input image buffers

    auto* input_buff = new float[cl_ni*cl_nj];
    //copy input vals into image
    int count = 0;
    for (unsigned int j = 0; j < cl_nj; ++j) {
        for (unsigned int i = 0; i < cl_ni; ++i) {
            input_buff[count] = 0.0f;
            if (i < img_view->ni() && j < img_view->nj())
                input_buff[count] = (*img_view)(i, j);
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

    // set arguments
    std::vector<boxm2_block_id> vis_order = scene->get_vis_blocks(cam);
    std::vector<boxm2_block_id>::iterator id;

    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        for (unsigned int i = 0; i < kernels.size(); ++i)
        {
            bocl_kernel* kern = kernels[i];
            //write the image values to the buffer
            vul_timer transfer;
            bocl_mem* blk = opencl_cache->get_block(scene, *id);
            bocl_mem* blk_info = opencl_cache->loaded_block_info();
            bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene, *id, 0, false);
            auto* info_buffer = (boxm2_scene_info*)blk_info->cpu_buffer();
            int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
            info_buffer->data_buffer_length = (int)(alpha->num_bytes() / alphaTypeSize);
            blk_info->write_to_buffer((queue));

            // data type string may contain an identifier so determine the buffer size
            bocl_mem* mog = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_MOG3_GREY>::prefix(view_identifier));
            //grab an appropriately sized AUX data buffer
            bocl_mem *aux0 = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix("image_" + view_identifier), 0, true);
            bocl_mem *aux1 = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX1>::prefix("image_" + view_identifier), 0, true);
            bocl_mem *aux2 = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX2>::prefix("image_" + view_identifier), 0, false);

            transfer_time += (float)transfer.all();
            if (i == UPDATE_SEGLEN)
            {
                aux0->zero_gpu_buffer(queue);
                aux1->zero_gpu_buffer(queue);
                kern->set_arg(blk_info);
                kern->set_arg(blk);
                kern->set_arg(alpha);
                kern->set_arg(aux0);
                kern->set_arg(aux1);
                kern->set_arg(lookup.ptr());
                kern->set_arg(ray_o_buff.ptr());
                kern->set_arg(ray_d_buff.ptr());
                kern->set_arg(tnearfar_mem_ptr.ptr());
                kern->set_arg(img_dim.ptr());
                kern->set_arg(in_image.ptr());
                kern->set_arg(cl_output.ptr());
                kern->set_local_arg(local_threads[0] * local_threads[1] * sizeof(cl_uchar16));//local tree,
                kern->set_local_arg(local_threads[0] * local_threads[1] * sizeof(cl_uchar4)); //ray bundle,
                kern->set_local_arg(local_threads[0] * local_threads[1] * sizeof(cl_int));    //cell pointers,
                kern->set_local_arg(local_threads[0] * local_threads[1] * sizeof(cl_float4)); //cached aux,
                kern->set_local_arg(local_threads[0] * local_threads[1] * 10 * sizeof(cl_uchar)); //cumsum buffer, imindex buffer
                //execute kernel
                kern->execute(queue, 2, local_threads, global_threads);
                int status = clFinish(queue);
                if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
                    return false;
                gpu_time += kern->exec_time();
                //clear render kernel args so it can reset em on next execution
                kern->clear_args();
            }
            else if (i == COMPUTE_IMAGEDEN)
            {
                blk_info->write_to_buffer((queue));
                std::size_t lt[1], gt[1];
                lt[0] = 64;
                gt[0] = RoundUp(info_buffer->data_buffer_length, lt[0]);
                kern->set_arg(blk_info);
                kern->set_arg(mog);
                kern->set_arg(aux0);
                kern->set_arg(aux1);
                kern->set_arg(aux2);
                kern->execute(queue, 1, lt, gt);
                int status = clFinish(queue);
                if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
                    return false;
                gpu_time += kern->exec_time();
                aux2->read_to_buffer(queue);
                //clear render kernel args so it can reset em on next execution
                kern->clear_args();
                //write info to disk
                opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix("image_" + view_identifier), false);
                opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX1>::prefix("image_" + view_identifier), false);
                opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX2>::prefix("image_" + view_identifier), true);
            }
            clFinish(queue);
        }
    }
    delete[] input_buff;
    delete[] ray_origins;
    delete[] ray_directions;
    opencl_cache->unref_mem(in_image.ptr());
    opencl_cache->unref_mem(ray_o_buff.ptr());
    opencl_cache->unref_mem(ray_d_buff.ptr());
    opencl_cache->unref_mem(tnearfar_mem_ptr.ptr());
    std::cout << "Gpu time " << gpu_time << " transfer time " << transfer_time << std::endl;
    clReleaseCommandQueue(queue);
    return true;
}
// Returns vector of image density update kernels(and caches them per device
std::vector<bocl_kernel*>& boxm2_ocl_compute_image_term::get_image_kernels(const bocl_device_sptr& device, const std::string& opts)
{
    // compile kernels if not already compiled
    std::string identifier = device->device_identifier() + opts;
    if (image_kernels_.find(identifier) != image_kernels_.end())
        return image_kernels_[identifier];

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
    src_paths.push_back(source_dir + "bit/update_kernels.cl");
    src_paths.push_back(source_dir + "bit/update_bp_kernels.cl");
    std::vector<std::string> non_ray_src = std::vector<std::string>(src_paths);
    src_paths.push_back(source_dir + "update_functors.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //compilation options
    std::string options = "-D ATOMIC_FLOAT -D MOG_TYPE_8";
    //populate vector of kernels
    std::vector<bocl_kernel*> vec_kernels;
    //seg len pass
    auto* seg_len = new bocl_kernel();
    std::string seg_opts = options + " -D SEGLEN  -D STEP_CELL=step_cell_seglen(aux_args,data_ptr,llid,d)";
    seg_len->create_kernel(&device->context(), device->device_id(), src_paths, "seg_len_main", seg_opts, "update::seg_len");
    vec_kernels.push_back(seg_len);

    auto* compute_imageden_kernel = new bocl_kernel();
    std::string image_den_opts = options + " -D COMPUTE_IMAGEDEN";
    compute_imageden_kernel->create_kernel(&device->context(), device->device_id(), non_ray_src, "compute_imageden_main", image_den_opts, "compute_imageden_main");
    vec_kernels.push_back(compute_imageden_kernel);

    //store and return
    image_kernels_[identifier] = vec_kernels;
    return image_kernels_[identifier];
}


//Main public method, updates color model
bool boxm2_ocl_compute_pre_post::update_pre(const boxm2_scene_sptr&         scene,
                                            bocl_device_sptr         device,
                                            const boxm2_opencl_cache_sptr&  opencl_cache,
                                            vpgl_camera_double_sptr  cam,
                                            unsigned int ni, unsigned int nj,
                                            const std::string& view_identifier,
                                            float resnearfactor,
                                            float resfarfactor)
{
    enum {
        UPDATE_PRE = 0,
        NORMALIZE_PRE = 1,
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
    // compile the kernel if not already compiled
    std::vector<bocl_kernel*>& kernels = get_pre_kernels(device, options);

    auto cl_ni = (unsigned)RoundUp(ni, (int)local_threads[0]);
    auto cl_nj = (unsigned)RoundUp(nj, (int)local_threads[1]);
    global_threads[0] = cl_ni;
    global_threads[1] = cl_nj;
    //set generic cam
    auto* ray_origins = new cl_float[4 * cl_ni*cl_nj];
    auto* ray_directions = new cl_float[4 * cl_ni*cl_nj];
    bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins, "ray_origins buffer");
    bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions, "ray_directions buffer");
    boxm2_ocl_camera_converter::compute_ray_image(device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

    float tnearfar[2] = { 0.0f, 1000000 };

    if (cam->type_name() == "vpgl_perspective_camera")
    {

        float f = ((vpgl_perspective_camera<double> *)cam.ptr())->get_calibration().focal_length()*((vpgl_perspective_camera<double> *)cam.ptr())->get_calibration().x_scale();
        std::cout << "Focal Length " << f << std::endl;
        tnearfar[0] = f* scene->finest_resolution() / resnearfactor;
        tnearfar[1] = f* scene->finest_resolution()*resfarfactor;
        std::cout << "Near and Far Clipping planes " << tnearfar[0] << " " << tnearfar[1] << std::endl;
    }
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

    bocl_mem_sptr pre_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), pre_buff, "pre image buffer");
    pre_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // Image Dimensions
    int img_dim_buff[4];
    img_dim_buff[0] = 0;
    img_dim_buff[1] = 0;
    img_dim_buff[2] = ni;
    img_dim_buff[3] = nj;

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

    // set arguments
    std::vector<boxm2_block_id> vis_order;
    vis_order = scene->get_vis_blocks(cam);
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
            bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene, *id, 0, false);
            auto* info_buffer = (boxm2_scene_info*)blk_info->cpu_buffer();
            int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
            info_buffer->data_buffer_length = (int)(alpha->num_bytes() / alphaTypeSize);
            //grab an appropriately sized AUX data buffer
            bocl_mem *aux0 = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix("pre_" + view_identifier), 0, false);
            bocl_mem *aux1 = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX1>::prefix("pre_" + view_identifier), 0, false);
            bocl_mem *aux2 = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX2>::prefix("vis_" + view_identifier), 0, false);
            bocl_mem *aux3 = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX2>::prefix("image_" + view_identifier), 0, true);

            transfer_time += (float)transfer.all();
            if (i == UPDATE_PRE)
            {
                aux0->zero_gpu_buffer(queue);
                aux1->zero_gpu_buffer(queue);
                aux2->zero_gpu_buffer(queue);

                kern->set_arg(blk_info);
                kern->set_arg(blk);
                kern->set_arg(alpha);
                kern->set_arg(aux0); //seg len
                kern->set_arg(aux1); //pre
                kern->set_arg(aux2); //vis
                kern->set_arg(aux3); //datadensity
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

            }
            else if (i == NORMALIZE_PRE)
            {
                blk_info->write_to_buffer((queue));

                std::size_t lt[1], gt[1];
                lt[0] = 64;
                gt[0] = RoundUp(info_buffer->data_buffer_length, lt[0]);

                kern->set_arg(blk_info);
                kern->set_arg(aux0);
                kern->set_arg(aux1);
                kern->set_arg(aux2);
                kern->execute(queue, 1, lt, gt);
                int status = clFinish(queue);
                if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
                    return false;
                gpu_time += kern->exec_time();
                aux1->read_to_buffer(queue);
                aux2->read_to_buffer(queue);

                //clear render kernel args so it can reset em on next execution
                kern->clear_args();
                opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix("pre_" + view_identifier), false);
                opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX1>::prefix("pre_" + view_identifier), true);
                opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX2>::prefix("vis_" + view_identifier), true);
                opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX2>::prefix("image_" + view_identifier), false);

            }
            //read image out to buffer (from gpu)
            vis_image->read_to_buffer(queue);
            pre_image->read_to_buffer(queue);
            cl_output->read_to_buffer(queue);
            clFinish(queue);
        }
    }

    delete[] vis_buff;
    delete[] pre_buff;
    delete[] ray_origins;
    delete[] ray_directions;
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
bool boxm2_ocl_compute_pre_post::update_post(const boxm2_scene_sptr&         scene,
    bocl_device_sptr         device,
    const boxm2_opencl_cache_sptr&  opencl_cache,
    vpgl_camera_double_sptr  cam,
    unsigned int ni, unsigned int nj,
    const std::string& view_identifier,
    float resnearfactor,
    float resfarfactor)
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
    // compile the kernel if not already compiled
    std::vector<bocl_kernel*>& kernels = get_post_kernels(device, options);

    //grab input image, establish cl_ni, cl_nj (so global size is divisible by local size)

    auto cl_ni = (unsigned)RoundUp(ni, (int)local_threads[0]);
    auto cl_nj = (unsigned)RoundUp(nj, (int)local_threads[1]);
    global_threads[0] = cl_ni;
    global_threads[1] = cl_nj;
    //set generic cam
    auto* ray_origins = new cl_float[4 * cl_ni*cl_nj];
    auto* ray_directions = new cl_float[4 * cl_ni*cl_nj];
    bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins, "ray_origins buffer");
    bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions, "ray_directions buffer");
    boxm2_ocl_camera_converter::compute_ray_image(device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

    float tnearfar[2] = { 0.0f, 1000000 };

    if (cam->type_name() == "vpgl_perspective_camera")
    {

        float f = ((vpgl_perspective_camera<double> *)cam.ptr())->get_calibration().focal_length()*((vpgl_perspective_camera<double> *)cam.ptr())->get_calibration().x_scale();
        std::cout << "Focal Length " << f << std::endl;
        tnearfar[0] = f* scene->finest_resolution() / resnearfactor;
        tnearfar[1] = f* scene->finest_resolution()*resfarfactor;
        std::cout << "Near and Far Clipping planes " << tnearfar[0] << " " << tnearfar[1] << std::endl;
    }
    bocl_mem_sptr tnearfar_mem_ptr = opencl_cache->alloc_mem(2 * sizeof(float), tnearfar, "tnearfar  buffer");
    tnearfar_mem_ptr->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
    //Visibility, Preinf, Norm, and input image buffers
    auto* vis_buff = new float[cl_ni*cl_nj];
    auto* post_buff = new float[cl_ni*cl_nj];


    std::fill(vis_buff, vis_buff + cl_ni*cl_nj, 1.0);
    std::fill(post_buff, post_buff + cl_ni*cl_nj, 0.0);

    bocl_mem_sptr vis_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), vis_buff, "vis image buffer");
    vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    bocl_mem_sptr post_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), post_buff, "pre image buffer");
    post_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // Image Dimensions
    int img_dim_buff[4];
    img_dim_buff[0] = 0;
    img_dim_buff[1] = 0;
    img_dim_buff[2] = ni;
    img_dim_buff[3] = nj;

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

    // set arguments
    std::vector<boxm2_block_id> vis_order;
    vis_order = scene->get_vis_blocks(cam);
    std::vector<boxm2_block_id>::reverse_iterator  id;
    for (unsigned int i = 0; i < kernels.size(); ++i)
    {
        for (id = vis_order.rbegin(); id != vis_order.rend(); ++id)
        {
            //choose correct render kernel
            boxm2_block_metadata mdata = scene->get_block_metadata(*id);
            bocl_kernel* kern = kernels[i];
            //write the image values to the buffer
            vul_timer transfer;
            bocl_mem* blk = opencl_cache->get_block(scene, *id);
            bocl_mem* blk_info = opencl_cache->loaded_block_info();
            bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene, *id, 0, false);
            auto* info_buffer = (boxm2_scene_info*)blk_info->cpu_buffer();
            int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
            info_buffer->data_buffer_length = (int)(alpha->num_bytes() / alphaTypeSize);
            //grab an appropriately sized AUX data buffer
            bocl_mem *aux0 = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix("post_" + view_identifier), 0, false);
            bocl_mem *aux2 = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX2>::prefix("post_" + view_identifier), 0, false);
            bocl_mem *datadensity = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX2>::prefix("image_" + view_identifier), 0, false);
            transfer_time += (float)transfer.all();
            if (i == UPDATE_POST)
            {
                aux0->zero_gpu_buffer(queue);
                aux2->zero_gpu_buffer(queue);
                kern->set_arg(blk_info);
                kern->set_arg(blk);
                kern->set_arg(alpha);
                kern->set_arg(aux0);  // seglen
                kern->set_arg(aux2);  // post
                kern->set_arg(datadensity);
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
                aux2->read_to_buffer(queue);

                //clear render kernel args so it can reset em on next execution
                kern->clear_args();
            }
            else if (i == NORMALIZE_POST)
            {
                blk_info->write_to_buffer((queue));

                std::size_t lt[1], gt[1];
                lt[0] = 64;
                gt[0] = RoundUp(info_buffer->data_buffer_length, lt[0]);

                kern->set_arg(blk_info);
                kern->set_arg(aux0);

                kern->set_arg(aux2);
                kern->execute(queue, 1, lt, gt);
                int status = clFinish(queue);
                if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
                    return false;
                gpu_time += kern->exec_time();
                aux2->read_to_buffer(queue);

                //clear render kernel args so it can reset em on next execution
                kern->clear_args();
                opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix("post_" + view_identifier), false);
                opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX2>::prefix("post_" + view_identifier), true);
                opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX2>::prefix("image_" + view_identifier), false);
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
bool boxm2_ocl_compute_pre_post::compute_pre_post(const boxm2_scene_sptr&         scene,
    const bocl_device_sptr&         device,
    const boxm2_opencl_cache_sptr&  opencl_cache,
    const vpgl_camera_double_sptr&  cam,
    const vil_image_view_base_sptr& img,
    const std::string& view_identifier,
    float resnearfactor,
    float resfarfactor)
{
  boxm2_ocl_compute_image_term::compute_image_term(scene, device, opencl_cache, cam, img, view_identifier, resnearfactor, resfarfactor);
  if(!boxm2_ocl_compute_pre_post::update_pre(scene, device, opencl_cache, cam, img->ni(),img->nj(), view_identifier, resnearfactor, resfarfactor))
    return false;
  if(!boxm2_ocl_compute_pre_post::update_post(scene, device, opencl_cache, cam, img->ni(), img->nj(), view_identifier, resnearfactor, resfarfactor))
    return false;
  return true;
}
bool boxm2_ocl_update_image_factor::update_image_factor(const boxm2_scene_sptr&         scene,
                                                        const bocl_device_sptr&         device,
                                                        const boxm2_opencl_cache_sptr&  opencl_cache,
                                                        bool add,
                                                        const std::string& view_identifier)
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
    if (does_add_buf == 1)
        std::cout << " Addition " << std::endl;
    else
        std::cout << "Subtract " << std::endl;
    bocl_mem_sptr does_add = new bocl_mem(device->context(), &does_add_buf, sizeof(int) * 1, "add (1) or subtract (0)");
    does_add->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // create a command queue.
    int status = 0;
    cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),CL_QUEUE_PROFILING_ENABLE,&status);
    if (status != 0)
        return false;
    // compile the kernel if not already compiled
    bocl_kernel * kern = get_update_image_factor_kernels(device, options)[0];
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
        bocl_mem *pre  = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX1>::prefix("pre_" + view_identifier),  0, true);
        bocl_mem *vis  = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX2>::prefix("vis_" + view_identifier),  0, true);
        bocl_mem *post = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX2>::prefix("post_" + view_identifier), 0, true);
        bocl_mem *datadensity = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX2>::prefix("image_" + view_identifier), 0, true);
        bocl_mem *aux0_if    = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix("if"), 0, false);


        transfer_time += (float)transfer.all();


        kern->set_arg(blk_info);
        kern->set_arg(does_add.ptr());
        kern->set_arg(pre);
        kern->set_arg(vis);
        kern->set_arg(post);
        kern->set_arg(datadensity);
        kern->set_arg(aux0_if);
        //execute kernel
        kern->execute(queue, 1, local_threads, global_threads);
        int status = clFinish(queue);
        if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
            return false;
        gpu_time += kern->exec_time();
        //clear render kernel args so it can reset em on next execution
        aux0_if->read_to_buffer(queue);
        kern->clear_args();
        opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX1>::prefix("pre_" + view_identifier), false);
        opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX2>::prefix("vis_" + view_identifier), false);
        opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX2>::prefix("post_" + view_identifier), false);
        opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX2>::prefix("image_" + view_identifier), false);
        opencl_cache->deep_remove_data(scene, *id, boxm2_data_traits<BOXM2_AUX0>::prefix("if" ), true);

    }
    clFinish(queue);
    opencl_cache->unref_mem(does_add.ptr());
    return true;
}

//Returns vector of color update kernels (and caches them per device
std::vector<bocl_kernel*>& boxm2_ocl_compute_pre_post::get_pre_kernels(const bocl_device_sptr& device, const std::string& opts)
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
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //compilation options
    std::string options = "-D ATOMIC_FLOAT ";
    //populate vector of kernels
    std::vector<bocl_kernel*> vec_kernels;
    auto* pre = new bocl_kernel();
    std::string pre_opts = options + " -D PRE_CELL  -D STEP_CELL=step_cell_pre(aux_args,data_ptr,llid,d)";
    pre->create_kernel(&device->context(), device->device_id(), src_paths, "pre_cell_main", pre_opts, "update::pre_inf");
    vec_kernels.push_back(pre);

    auto* normalize_pre = new bocl_kernel();
    std::string normalize_pre_opts = options + " -D PRE_CELL  ";
    normalize_pre->create_kernel(&device->context(), device->device_id(), src_paths, "normalize_pre_cell", pre_opts, "update::normalize_pre_cell");
    vec_kernels.push_back(normalize_pre);
    //store and return
    pre_kernels_[identifier] = vec_kernels;
    return pre_kernels_[identifier];
}

std::vector<bocl_kernel*>& boxm2_ocl_compute_pre_post::get_post_kernels(const bocl_device_sptr& device, const std::string& opts)
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
    std::string post_opts = options + " -D POST_CELL  -D STEP_CELL=step_cell_post(aux_args,data_ptr,llid,d)";
    post->create_kernel(&device->context(), device->device_id(), src_paths, "post_cell_main", post_opts, "update::post_Cell");
    vec_kernels.push_back(post);

    auto* normalize_post = new bocl_kernel();
    std::string normalize_post_opts = options + " -D NORMALIZE_POST_CELL  ";
    normalize_post->create_kernel(&device->context(), device->device_id(), non_ray_src, "normalize_post_cell", normalize_post_opts, "update::normalize_post_cell");
    vec_kernels.push_back(normalize_post);
    //store and return
    post_kernels_[identifier] = vec_kernels;
    return post_kernels_[identifier];
}

std::vector<bocl_kernel*>& boxm2_ocl_update_image_factor::get_update_image_factor_kernels(const bocl_device_sptr& device, const std::string& opts)
{
    // compile kernels if not already compiled
    std::string identifier = device->device_identifier() + opts;
    if (update_image_factor_kernels_.find(identifier) != update_image_factor_kernels_.end())
        return update_image_factor_kernels_[identifier];

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
    update_image_factor_kernels_[identifier] = vec_kernels;
    return update_image_factor_kernels_[identifier];
}
