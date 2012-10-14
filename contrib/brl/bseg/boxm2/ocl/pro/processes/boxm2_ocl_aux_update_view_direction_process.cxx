// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_aux_update_view_direction_process.cxx
//:
// \file
// \brief  A process for updating the aux with viewing directions.
//
// \author Vishal Jain
// \date Aug 08, 2011

#include <bprb/bprb_func_process.h>

#include <vcl_fstream.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_util.h>
#include <vil/vil_image_view.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace boxm2_ocl_aux_update_view_direction_process_globals
{
    const unsigned n_inputs_  = 9;
    const unsigned n_outputs_ = 0;

    void compile_kernel(bocl_device_sptr device,vcl_vector<bocl_kernel*> & vec_kernels,vcl_string opts)
    {
        //gather all render sources... seems like a lot for rendering...
        vcl_vector<vcl_string> src_paths;
        vcl_string source_dir = boxm2_ocl_util::ocl_src_root();
        src_paths.push_back(source_dir + "scene_info.cl");
        src_paths.push_back(source_dir + "cell_utils.cl");
        src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
        src_paths.push_back(source_dir + "backproject.cl");
        src_paths.push_back(source_dir + "statistics_library_functions.cl");
        src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
        src_paths.push_back(source_dir + "bit/batch_update_kernels.cl");
        vcl_vector<vcl_string> second_pass_src = vcl_vector<vcl_string>(src_paths);
        src_paths.push_back(source_dir + "batch_update_functors.cl");
        src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");
        //compilation options
        vcl_string options = opts;
        //create all passes
        bocl_kernel* aux_direction_kernel = new bocl_kernel();
        vcl_string seg_opts = options + " -D UPDATE_AUX_DIRECTION -D STEP_CELL=step_cell_directions(aux_args,data_ptr,d) ";
        aux_direction_kernel->create_kernel(&device->context(),device->device_id(), src_paths, "aux_directions_main", seg_opts, "batch_update::viewing directions");
        vec_kernels.push_back(aux_direction_kernel);

        bocl_kernel* convert_aux_int_float = new bocl_kernel();
        convert_aux_int_float->create_kernel(&device->context(),device->device_id(), second_pass_src, "convert_aux_int_to_float"," -D CONVERT_AUX", "batch_update::convert_aux_int_to_float");
        vec_kernels.push_back(convert_aux_int_float);


        bocl_kernel* convert_aux_xyz_to_thetaphi = new bocl_kernel();
        convert_aux_xyz_to_thetaphi->create_kernel(&device->context(),device->device_id(), second_pass_src, "convert_aux_xyz_to_thetaphi"," -D CONVERT_AUX_XYZ_THETAPHI ", "batch_update::convert_aux_xyz_to_thetaphi");
        vec_kernels.push_back(convert_aux_xyz_to_thetaphi);


        return ;
    }


    static vcl_map<vcl_string,vcl_vector<bocl_kernel*> > kernels;
}

bool boxm2_ocl_aux_update_view_direction_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_ocl_aux_update_view_direction_process_globals;

    //process takes 1 input
    vcl_vector<vcl_string> input_types_(n_inputs_);
    input_types_[0] = "bocl_device_sptr";
    input_types_[1] = "boxm2_scene_sptr";
    input_types_[2] = "boxm2_cache_sptr";
    input_types_[3] = "boxm2_opencl_cache_sptr";
    input_types_[4] = "vpgl_camera_double_sptr";
    input_types_[5] = "unsigned";
    input_types_[6] = "unsigned";
    input_types_[7] = "vcl_string";
    input_types_[8] = "vcl_string";

    // process has 1 output:
    // output[0]: scene sptr
    vcl_vector<vcl_string>  output_types_(n_outputs_);

    bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
    // in case the 6th input is not set
    brdb_value_sptr idx = new brdb_value_t<vcl_string>("");
    pro.set_input(7, idx);
    brdb_value_sptr idx_coordinate_type = new brdb_value_t<vcl_string>("Cartesian");
    pro.set_input(8, idx_coordinate_type);
    return good;
}

bool boxm2_ocl_aux_update_view_direction_process(bprb_func_process& pro)
{
    using namespace boxm2_ocl_aux_update_view_direction_process_globals;
    vcl_size_t local_threads[2]={8,8};
    vcl_size_t global_threads[2]={8,8};

    if ( pro.n_inputs() < n_inputs_ ) {
        vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
        return false;
    }
    float transfer_time=0.0f;
    float gpu_time=0.0f;
    //get the inputs
    unsigned i = 0;
    bocl_device_sptr device               = pro.get_input<bocl_device_sptr>(i++);
    boxm2_scene_sptr scene                = pro.get_input<boxm2_scene_sptr>(i++);
    boxm2_cache_sptr cache                = pro.get_input<boxm2_cache_sptr>(i++);
    boxm2_opencl_cache_sptr opencl_cache  = pro.get_input<boxm2_opencl_cache_sptr>(i++);
    vpgl_camera_double_sptr cam           = pro.get_input<vpgl_camera_double_sptr>(i++);
    unsigned int ni                       = pro.get_input<unsigned>(i++);
    unsigned int nj                       = pro.get_input<unsigned>(i++);
    vcl_string suffix                     = pro.get_input<vcl_string>(i++);
    vcl_string coordinate_type            = pro.get_input<vcl_string>(i++);


    //  opencl_cache->get_cpu_cache()->clear_cache();

    long binCache = opencl_cache.ptr()->bytes_in_cache();
    vcl_cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<vcl_endl;


    // create a command queue.
    int status=0;
    cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
        CL_QUEUE_PROFILING_ENABLE,&status);
    if (status!=0) 
        return false;

    vcl_string identifier=device->device_identifier();
    // compile the kernel if not already compiled
    if (kernels.find(identifier)==kernels.end())
    {
        vcl_cout<<"===========Compiling kernels==========="<<vcl_endl;
        vcl_vector<bocl_kernel*> ks;
        compile_kernel(device,ks,"");
        kernels[identifier]=ks;
    }

    //grab input image, establish cl_ni, cl_nj (so global size is divisible by local size)
    //vil_image_view_base_sptr float_img=boxm2_util::prepare_input_image(img);
    //vil_image_view<float>* img_view = static_cast<vil_image_view<float>* >(float_img.ptr());
    unsigned cl_ni=(unsigned)RoundUp(ni,(int)local_threads[0]);
    unsigned cl_nj=(unsigned)RoundUp(nj,(int)local_threads[1]);

    global_threads[0]=cl_ni;
    global_threads[1]=cl_nj;

    //set generic cam
    cl_float* ray_origins    = new cl_float[4*cl_ni*cl_nj];
    cl_float* ray_directions = new cl_float[4*cl_ni*cl_nj];
    bocl_mem_sptr ray_o_buff = new bocl_mem(device->context(), ray_origins, cl_ni*cl_nj * sizeof(cl_float4) , "ray_origins buffer");
    bocl_mem_sptr ray_d_buff = new bocl_mem(device->context(), ray_directions,  cl_ni*cl_nj * sizeof(cl_float4), "ray_directions buffer");
    boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

    // Image Dimensions
    int img_dim_buff[4];
    img_dim_buff[0] = 0;
    img_dim_buff[1] = 0;
    img_dim_buff[2] = ni;
    img_dim_buff[3] = nj;
    bocl_mem_sptr img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(int)*4, "image dims");
    img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

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

    // set arguments
    vcl_vector<boxm2_block_id> vis_order = scene->get_vis_blocks(cam);
    vcl_vector<boxm2_block_id>::iterator id;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        boxm2_block_metadata mdata = scene->get_block_metadata(*id);

        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk       = opencl_cache->get_block(*id);
        bocl_mem* blk_info  = opencl_cache->loaded_block_info();
        bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(*id,0,false);
        boxm2_scene_info* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
        int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
        blk_info->write_to_buffer((queue));
        //grab an appropriately sized AUX data buffer
        int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
        bocl_mem *aux0  = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_AUX0>::prefix(suffix),info_buffer->data_buffer_length*auxTypeSize,false);
        auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
        bocl_mem *aux1  = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_AUX1>::prefix(suffix),info_buffer->data_buffer_length*auxTypeSize,false);
        auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
        bocl_mem *aux2  = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_AUX2>::prefix(suffix),info_buffer->data_buffer_length*auxTypeSize,false);
        auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
        bocl_mem *aux3  = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_AUX3>::prefix(suffix),info_buffer->data_buffer_length*auxTypeSize,false);
        for (unsigned int i=0; i<2; ++i)
        {

            if( i ==0 )
            {
                bocl_kernel* kern =  kernels[identifier][i];
                //choose correct render kernel
                local_threads[0] = 8;
                local_threads[1] = 8 ;
                global_threads[0]=cl_ni;
                global_threads[1]=cl_nj;

                kern->set_arg( blk_info );
                kern->set_arg( blk );
                kern->set_arg( aux0 );
                kern->set_arg( aux1 );
                kern->set_arg( aux2 );
                kern->set_arg( aux3 );
                kern->set_arg( lookup.ptr() );

                kern->set_arg( ray_o_buff.ptr() );
                kern->set_arg( ray_d_buff.ptr() );

                kern->set_arg( img_dim.ptr() );
                kern->set_arg( cl_output.ptr() );
                kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
                kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar4) ); //ray bundle,
                kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_int) );    //cell pointers,
                kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_float4) ); //cached aux,
                kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer

                //execute kernel
                kern->execute(queue, 2, local_threads, global_threads);
                int status = clFinish(queue);
                check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
                gpu_time += kern->exec_time();

                //clear render kernel args so it can reset em on next execution
                kern->clear_args();
                //read image out to buffer (from gpu)
                clFinish(queue);
            }
            else if ( i == 1)
            {
                bocl_kernel* kern ;
                if(coordinate_type == "spherical")
                    kern =  kernels[identifier][2];
                else
                    kern =  kernels[identifier][1];

                local_threads[0] = 64;
                local_threads[1] = 1 ;
                global_threads[0]=RoundUp(info_buffer->data_buffer_length,local_threads[0]);
                global_threads[1]=1;

                kern->set_arg( blk_info );
                kern->set_arg( aux0 );
                kern->set_arg( aux1 );
                kern->set_arg( aux2 );
                kern->set_arg( aux3 );
                //execute kernel
                kern->execute(queue, 2, local_threads, global_threads);
                int status = clFinish(queue);
                check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
                gpu_time += kern->exec_time();

                //clear render kernel args so it can reset em on next execution
                kern->clear_args();

                //write info to disk
                aux0->read_to_buffer(queue);
                aux1->read_to_buffer(queue);
                if(coordinate_type != "spherical")
                {
                    aux2->read_to_buffer(queue);
                    aux3->read_to_buffer(queue);
                }
            }
        }

        opencl_cache->deep_remove_data(*id,boxm2_data_traits<BOXM2_AUX0>::prefix(suffix),true);
        opencl_cache->deep_remove_data(*id,boxm2_data_traits<BOXM2_AUX1>::prefix(suffix),true);
        if(coordinate_type != "spherical")
        {
            opencl_cache->deep_remove_data(*id,boxm2_data_traits<BOXM2_AUX2>::prefix(suffix),true);
            opencl_cache->deep_remove_data(*id,boxm2_data_traits<BOXM2_AUX3>::prefix(suffix),true);
        }
        else
        {
            opencl_cache->deep_remove_data(*id,boxm2_data_traits<BOXM2_AUX2>::prefix(suffix),false);
            opencl_cache->deep_remove_data(*id,boxm2_data_traits<BOXM2_AUX3>::prefix(suffix),false);
        }
    }
    delete [] ray_origins;
    delete [] ray_directions;
    vcl_cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<vcl_endl;
    clReleaseCommandQueue(queue);
    return true;
}
