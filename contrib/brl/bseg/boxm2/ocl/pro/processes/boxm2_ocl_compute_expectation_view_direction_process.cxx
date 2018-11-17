// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_compute_expectation_view_direction_process.cxx
//:
// \file
// \brief  A process for updating the aux with viewing directions.
//
// \author Vishal Jain
// \date Aug 08, 2011

#include <algorithm>
#include <bprb/bprb_func_process.h>
#include <fstream>
#include <iostream>
#include <utility>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_util.h>
#include <vil/vil_image_view.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <vpgl/vpgl_lvcs_sptr.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace boxm2_ocl_compute_expectation_view_direction_process_globals
{
    constexpr unsigned n_inputs_ = 5;
    constexpr unsigned n_outputs_ = 0;

    void compile_kernel(const bocl_device_sptr& device,std::vector<bocl_kernel*> & vec_kernels,std::string opts)
    {
        //gather all render sources... seems like a lot for rendering...
        std::vector<std::string> src_paths;
        std::string source_dir = boxm2_ocl_util::ocl_src_root();
        src_paths.push_back(source_dir + "scene_info.cl");
        src_paths.push_back(source_dir + "stat/directional_statistics.cl");

        //compilation options
        const std::string& options = std::move(opts);
        //create all passes
        std::string accumulate_opts = options + " -D ACCUMULATE_DIRECTION_VECTORS";
        auto* accumulate_direction_kernel = new bocl_kernel();
        accumulate_direction_kernel->create_kernel(&device->context(),device->device_id(), src_paths, "accumulate_direction_vectors", accumulate_opts, "accumulate_direction_vectors");
        vec_kernels.push_back(accumulate_direction_kernel);

        std::string dispersion_opts = options + " -D COMPUTE_DISPERSION";
        auto* compute_dispersion = new bocl_kernel();
        compute_dispersion->create_kernel(&device->context(),device->device_id(), src_paths, "compute_dispersion",dispersion_opts, "compute_dispersion");
        vec_kernels.push_back(compute_dispersion);
        return ;
    }


    static std::map<std::string,std::vector<bocl_kernel*> > kernels;
}

bool boxm2_ocl_compute_expectation_view_direction_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_ocl_compute_expectation_view_direction_process_globals;

    //process takes 1 input
    std::vector<std::string> input_types_(n_inputs_);
    input_types_[0] = "bocl_device_sptr";
    input_types_[1] = "boxm2_scene_sptr";
    input_types_[2] = "boxm2_opencl_cache_sptr";
    input_types_[3] = "vcl_string";   // file with list of identifiers
    input_types_[4] = "vcl_string";   // coordinate type

    // process has 1 output:
    // output[0]: scene sptr
    std::vector<std::string>  output_types_(n_outputs_);

    bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
    brdb_value_sptr idx = new brdb_value_t<std::string>("");
    pro.set_input(3, idx);
    brdb_value_sptr idx_coordinate_type = new brdb_value_t<std::string>("Cartesian");
    pro.set_input(4, idx_coordinate_type);
    return good;
}

bool boxm2_ocl_compute_expectation_view_direction_process(bprb_func_process& pro)
{
    using namespace boxm2_ocl_compute_expectation_view_direction_process_globals;
    std::size_t local_threads[1]={64};
    std::size_t global_threads[1]={64};

    if ( pro.n_inputs() < n_inputs_ ) {
        std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
        return false;
    }
    float transfer_time=0.0f;
    float gpu_time=0.0f;
    //get the inputs
    unsigned i = 0;
    bocl_device_sptr device = pro.get_input<bocl_device_sptr>(i++);
    boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
    boxm2_opencl_cache_sptr opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
    std::string identifier_file = pro.get_input<std::string>(i++);
    std::string coordinate_type = pro.get_input<std::string>(i++);
    long binCache = opencl_cache.ptr()->bytes_in_cache();
    std::cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;

    // create a command queue.
    int status=0;
    cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
        CL_QUEUE_PROFILING_ENABLE,&status);
    if (status!=0)
        return false;

    std::string identifier=device->device_identifier();
    // compile the kernel if not already compiled
    if (kernels.find(identifier)==kernels.end())
    {
        std::cout<<"===========Compiling kernels==========="<<std::endl;
        std::vector<bocl_kernel*> ks;
        compile_kernel(device,ks,"");
        kernels[identifier]=ks;
    }
    std::ifstream ifile( identifier_file.c_str() );
    if(!ifile )
    {
        std::cout<<"Cannot open the files for suffixes "<<std::endl;
        return false;
    }
    std::vector<std::string> suffixes;
    while(!ifile.eof())
    {
        std::string suffix;
        ifile>>suffix;
        suffixes.push_back(suffix);
    }
    ifile.close();
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

    std::vector<boxm2_block_id> vis_order = scene->get_block_ids();
    for (auto & id : vis_order)
    {
        boxm2_block_metadata mdata = scene->get_block_metadata(id);
        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk = opencl_cache->get_block(scene,id);
        bocl_mem* blk_info = opencl_cache->loaded_block_info();
        bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene,id,0,false);
        auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
        int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
        blk_info->write_to_buffer((queue));
        int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
        bocl_mem *aux_direction_expectation_x = opencl_cache->get_data(scene,id, boxm2_data_traits<BOXM2_AUX0>::prefix("direction_expectation_x"),info_buffer->data_buffer_length*auxTypeSize,false);
        auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
        bocl_mem *aux_direction_expectation_y = opencl_cache->get_data(scene,id, boxm2_data_traits<BOXM2_AUX0>::prefix("direction_expectation_y"),info_buffer->data_buffer_length*auxTypeSize,false);
        auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
        bocl_mem *aux_direction_expectation_z = opencl_cache->get_data(scene,id, boxm2_data_traits<BOXM2_AUX0>::prefix("direction_expectation_z"),info_buffer->data_buffer_length*auxTypeSize,false);
        auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
        bocl_mem *aux_vis_expectation = opencl_cache->get_data(scene,id, boxm2_data_traits<BOXM2_AUX0>::prefix("visibility_expectation"),info_buffer->data_buffer_length*auxTypeSize,false);

        bocl_kernel* kern =  kernels[identifier][0];
        //choose correct render kernel
        global_threads[0]=RoundUp(info_buffer->data_buffer_length,local_threads[0]);
        /*for(unsigned int k = 0 ; k < suffixes.size() ; k++)
        {
            std::string suffix = suffixes[k];
            //grab an appropriately sized AUX data buffer
            auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
            bocl_mem *aux0 = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_AUX0>::prefix("viewdir_"+suffix),info_buffer->data_buffer_length*auxTypeSize,false);
            auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
            bocl_mem *aux1 = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_AUX1>::prefix("viewdir_"+suffix),info_buffer->data_buffer_length*auxTypeSize,false);
            auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
            bocl_mem *aux2 = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_AUX2>::prefix("viewdir_"+suffix),info_buffer->data_buffer_length*auxTypeSize,false);
            auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
            bocl_mem *aux3 = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_AUX3>::prefix("viewdir_"+suffix),info_buffer->data_buffer_length*auxTypeSize,false);
            auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
            bocl_mem *aux0_img = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_AUX0>::prefix("img_"+suffix),info_buffer->data_buffer_length*auxTypeSize,false);
            auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
            bocl_mem *aux2_img = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_AUX2>::prefix("img_"+suffix),info_buffer->data_buffer_length*auxTypeSize,false);
            kern->set_arg( blk_info );
            kern->set_arg( aux0 );
            kern->set_arg( aux1 );
            kern->set_arg( aux2 );
            kern->set_arg( aux3 );
            kern->set_arg( aux0_img );
            kern->set_arg( aux2_img );
            kern->set_arg( aux_direction_expectation_x );
            kern->set_arg( aux_direction_expectation_y );
            kern->set_arg( aux_direction_expectation_z );
            kern->set_arg( aux_vis_expectation );
            kern->set_arg( cl_output.ptr() );
            //execute kernel
            kern->execute(queue, 1, local_threads, global_threads);
            int status = clFinish(queue);
            check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
            gpu_time += kern->exec_time();
            //clear render kernel args so it can reset em on next execution
            kern->clear_args();
            //read image out to buffer (from gpu)
            clFinish(queue);
            opencl_cache->deep_remove_data(*id,boxm2_data_traits<BOXM2_AUX0>::prefix("viewdir_"+suffix),false);
            opencl_cache->deep_remove_data(*id,boxm2_data_traits<BOXM2_AUX1>::prefix("viewdir_"+suffix),false);
            opencl_cache->deep_remove_data(*id,boxm2_data_traits<BOXM2_AUX2>::prefix("viewdir_"+suffix),false);
            opencl_cache->deep_remove_data(*id,boxm2_data_traits<BOXM2_AUX3>::prefix("viewdir_"+suffix),false);
            opencl_cache->deep_remove_data(*id,boxm2_data_traits<BOXM2_AUX0>::prefix("img_"+suffix),false);
            opencl_cache->deep_remove_data(*id,boxm2_data_traits<BOXM2_AUX2>::prefix("img_"+suffix),false);
        }*/


        //: Compute dispersion and mean direction
        kern =  kernels[identifier][1];
        {
            auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
            bocl_mem *aux_dispersion = opencl_cache->get_data(scene,id, boxm2_data_traits<BOXM2_AUX0>::prefix("dispersion"),info_buffer->data_buffer_length*auxTypeSize,false);
            kern->set_arg( blk_info );
            kern->set_arg( alpha );
            kern->set_arg( aux_direction_expectation_x );
            kern->set_arg( aux_direction_expectation_y );
            kern->set_arg( aux_direction_expectation_z );
            kern->set_arg( aux_vis_expectation );
            kern->set_arg( aux_dispersion );
            kern->set_arg( cl_output.ptr() );
            //execute kernel
            kern->execute(queue, 1, local_threads, global_threads);
            int status = clFinish(queue);
            check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
            gpu_time += kern->exec_time();
            aux_dispersion->read_to_buffer(queue);
            aux_direction_expectation_x->read_to_buffer(queue);
            aux_direction_expectation_y->read_to_buffer(queue);
            aux_direction_expectation_z->read_to_buffer(queue);
            aux_vis_expectation->read_to_buffer(queue);
            alpha->read_to_buffer(queue);
            opencl_cache->deep_remove_data(scene,id,boxm2_data_traits<BOXM2_AUX0>::prefix("direction_expectation_x"),true);
            opencl_cache->deep_remove_data(scene,id,boxm2_data_traits<BOXM2_AUX0>::prefix("direction_expectation_y"),true);
            opencl_cache->deep_remove_data(scene,id,boxm2_data_traits<BOXM2_AUX0>::prefix("direction_expectation_z"),true);
            opencl_cache->deep_remove_data(scene,id,boxm2_data_traits<BOXM2_AUX0>::prefix("visibility_expectation"),true);
            opencl_cache->deep_remove_data(scene,id,boxm2_data_traits<BOXM2_AUX0>::prefix("dispersion"),true);
            //opencl_cache->deep_remove_data(*id,boxm2_data_traits<BOXM2_ALPHA>::prefix(),true);
            //clear render kernel args so it can reset em on next execution
            kern->clear_args();
            //read image out to buffer (from gpu)
            clFinish(queue);
        }
    }

    std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
    opencl_cache->get_cpu_cache()->write_to_disk();
    clReleaseCommandQueue(queue);
    return true;
}
