// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_compute_visibility_process.cxx
#include <fstream>
#include <iostream>
#include <cstdio>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for rendering visibilty of the scene from a given point.
//
// \author Vishal Jain
// \date Sept 15, 2012

#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <vil/vil_image_view.h>
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <vul/vul_timer.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <boxm2/boxm2_util.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_save.h>
#include <vil/vil_resample_nearest.h>

#include <vil/algo/vil_structuring_element.h>
#include <vil/algo/vil_binary_closing.h>

namespace boxm2_ocl_compute_visibility_process_globals
{
    constexpr unsigned n_inputs_ = 10;
    constexpr unsigned n_outputs_ = 0;
    std::size_t lthreads[2]={8,8};
    void compile_kernel(const bocl_device_sptr& device,std::vector<bocl_kernel*> & vec_kernels)
    {
        //gather all render sources... seems like a lot for rendering...
        std::vector<std::string> src_paths;
        std::string source_dir = boxm2_ocl_util::ocl_src_root();
        src_paths.push_back(source_dir + "scene_info.cl");
        src_paths.push_back(source_dir + "pixel_conversion.cl");
        src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
        src_paths.push_back(source_dir + "backproject.cl");
        src_paths.push_back(source_dir + "statistics_library_functions.cl");
        src_paths.push_back(source_dir + "expected_functor.cl");
        src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
        src_paths.push_back(source_dir + "bit/render_bit_scene.cl");
        src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

        //set kernel options
        std::string options = " -D RENDER_VISIBILITY ";
        options += " -D STEP_CELL=step_cell_vis(aux_args.alpha,data_ptr,d*linfo->block_len,aux_args.vis)";

        //have kernel construct itself using the context and device
        auto * ray_trace_kernel=new bocl_kernel();

        ray_trace_kernel->create_kernel( &device->context(),
                                         device->device_id(),
                                         src_paths,
                                         "render_visibiltiy",   //kernel name
                                         options,              //options
                                         "boxm2 opencl render visibility"); //kernel identifier (for error checking)
        vec_kernels.push_back(ray_trace_kernel);
    }
    static std::map<std::string,std::vector<bocl_kernel*> > kernels;
}

bool boxm2_ocl_compute_visibility_process_cons(bprb_func_process& pro)
{
    using namespace boxm2_ocl_compute_visibility_process_globals;

    //process takes 1 input
    std::vector<std::string> input_types_(n_inputs_);
    input_types_[0] = "bocl_device_sptr";
    input_types_[1] = "boxm2_scene_sptr";
    input_types_[2] = "boxm2_opencl_cache_sptr";
    input_types_[3] = "vcl_string"; // cams file
    input_types_[4] = "vcl_string"; // directory for depth images
    input_types_[5] = "float";
    input_types_[6] = "float";
    input_types_[7] = "float";
    input_types_[8] = "vcl_string"; // output directory for visibility images
    input_types_[9] = "int"; // scale parameter for increasing speed, should be a power of 2.
    // process has 1 output:
    // output[0]: scene sptr
    std::vector<std::string>  output_types_(n_outputs_);
    return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_compute_visibility_process(bprb_func_process& pro)
{
    using namespace boxm2_ocl_compute_visibility_process_globals;
    vul_timer t;
    t.mark();
    if ( pro.n_inputs() < n_inputs_ ) {
        std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
        return false;
    }
    float transfer_time=0.0f;
    float gpu_time=0.0f;
    //get the inputs
    unsigned i = 0;
    bocl_device_sptr device= pro.get_input<bocl_device_sptr>(i++);
    boxm2_scene_sptr scene =pro.get_input<boxm2_scene_sptr>(i++);
    boxm2_opencl_cache_sptr opencl_cache= pro.get_input<boxm2_opencl_cache_sptr>(i++);
    std::string camsfile= pro.get_input<std::string>(i++);
    std::string indir= pro.get_input<std::string>(i++);
    auto x = pro.get_input<float>(i++);
    auto y = pro.get_input<float>(i++);
    auto z = pro.get_input<float>(i++);
    std::string outdir= pro.get_input<std::string>(i++);
    int scale = pro.get_input<int>(i++);

    std::string identifier=device->device_identifier();

    // create a command queue.
    int status=0;
    cl_command_queue queue = clCreateCommandQueue(device->context(),
                                                  *(device->device_id()),
                                                  CL_QUEUE_PROFILING_ENABLE,
                                                  &status);
    if (status!=0)
        return false;
    // compile the kernel
    if (kernels.find(identifier)==kernels.end())
    {
        std::cout<<"===========Compiling kernels==========="<<std::endl;
        std::vector<bocl_kernel*> ks;
        compile_kernel(device,ks);
        kernels[identifier]=ks;
    }
    std::vector<vil_image_view<float>*>  raydirs;
    std::vector<vil_image_view<float>*>  tfinals;
    float rayo_buff[4];
    rayo_buff[0] = x;
    rayo_buff[1] = y;
    rayo_buff[2] = z+2;
    rayo_buff[3] = 0;
    bocl_mem_sptr ray_o_buff=opencl_cache->alloc_mem(sizeof(float)*4, rayo_buff,  "image dims");
    ray_o_buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);


    boxm2_util::get_raydirs_tfinal(indir,camsfile,vgl_point_3d<double>(x,y,z),raydirs,tfinals,scale );
    std::cout<<"Read Depth Images in "<<t.all()<<" ms"<<std::endl;
    t.mark();
    unsigned cl_ni =0;
    unsigned cl_nj =0;
    std::vector<bocl_mem_sptr> bocl_raydirs;
    std::vector<bocl_mem_sptr> bocl_vis;

    std::vector<float*> rayd_buffs;
    std::vector<float*> vis_buffs;
    int img_dim_buff[4];
    std::size_t gThreads[]={8,8};
    for (unsigned int count = 0 ; count < tfinals.size(); count ++)
    {
        cl_ni=RoundUp(tfinals[count]->ni(),lthreads[0]);
        cl_nj=RoundUp(tfinals[count]->nj(),lthreads[1]);

        auto* rayd_buff = new float[cl_ni*cl_nj*4];
        auto* vis_buff = new float[cl_ni*cl_nj];
        int num=0;
        for (unsigned int j=0;j<cl_nj;++j)
            for (unsigned int i=0;i<cl_ni;++i)
            {
                if (i<tfinals[count]->ni() && j< tfinals[count]->nj())
                {
                    rayd_buff[num*4+0]=(* raydirs[count])(i,j,0);
                    rayd_buff[num*4+1]=(* raydirs[count])(i,j,1);
                    rayd_buff[num*4+2]=(* raydirs[count])(i,j,2);
                    rayd_buff[num*4+3]=(* tfinals[count])(i,j);
                }
                vis_buff[num] = 1.0f;
                ++num;
            }
            rayd_buffs.push_back(rayd_buff);
            vis_buffs.push_back(vis_buff);
            if (count ==0)
            {
                img_dim_buff[0] = 0;
                img_dim_buff[1] = 0;
                img_dim_buff[2] = tfinals[count]->ni();
                img_dim_buff[3] = tfinals[count]->nj();

                gThreads[0] = cl_ni;
                gThreads[1] = cl_nj;
            }
    }

    // Image Dimensions
    bocl_mem_sptr exp_img_dim=opencl_cache->alloc_mem(sizeof(int)*4, img_dim_buff,  "image dims");
    exp_img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // Output Array
    float output_arr[100];
    for (float & i : output_arr) i = 0.0f;
    bocl_mem_sptr  cl_output=opencl_cache->alloc_mem(sizeof(float)*100, output_arr, "output buffer");
    cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // bit lookup buffer
    cl_uchar lookup_arr[256];
    boxm2_ocl_util::set_bit_lookup(lookup_arr);
    bocl_mem_sptr lookup=opencl_cache->alloc_mem( sizeof(cl_uchar)*256, lookup_arr,  "bit lookup buffer");
    lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    bocl_kernel* kern =  kernels[identifier][0];
    // set arguments
    boxm2_block_id pt_id;
    vgl_point_3d<double> local_coords;
    scene->contains(vgl_point_3d<double>(x,y,z),pt_id,local_coords);

    std::vector<boxm2_block_id> vis_order = boxm2_util::order_about_a_block(scene,pt_id);
    std::vector<boxm2_block_id>::iterator id;
    bocl_mem * ray_image = opencl_cache->alloc_mem(4*cl_ni*cl_nj*sizeof(float),nullptr,"ray direction buffer");
    ray_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR);

    bocl_mem * vis_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), nullptr,"ray direction buffer");
    vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_ALLOC_HOST_PTR);
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        boxm2_block_metadata mdata = scene->get_block_metadata(*id);
        vul_timer transfer;
        bocl_mem* blk = opencl_cache->get_block(scene,*id);
        bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene,*id);
        bocl_mem * blk_info = opencl_cache->loaded_block_info();
        transfer_time          += (float) transfer.all();

        //std::cout<<"bytes in cache "<<opencl_cache->bytes_in_cache()<<std::endl;
        for (unsigned int frame = 0 ; frame < rayd_buffs.size(); frame ++)
        {
            //choose correct render kernel
            ray_image->write_to_gpu_mem(queue,rayd_buffs[frame],4*cl_ni*cl_nj*sizeof(float));
            vis_image->write_to_gpu_mem(queue,vis_buffs[frame],cl_ni*cl_nj*sizeof(float));

            ////3. SET args
            kern->set_arg( blk_info );
            kern->set_arg( blk );
            kern->set_arg( alpha );
            kern->set_arg( ray_o_buff.ptr() );
            kern->set_arg( ray_image );
            kern->set_arg( exp_img_dim.ptr());
            kern->set_arg( cl_output.ptr() );
            kern->set_arg( lookup.ptr() );
            kern->set_arg( vis_image );
            //local tree , cumsum buffer, imindex buffer
            kern->set_local_arg( lthreads[0]*lthreads[1]*sizeof(cl_uchar16) );
            kern->set_local_arg( lthreads[0]*lthreads[1]*10*sizeof(cl_uchar) );
            kern->set_local_arg( lthreads[0]*lthreads[1]*sizeof(cl_int) );
            //execute kernel
            kern->execute(queue, 2, lthreads, gThreads);
            clFinish(queue);

            gpu_time += kern->exec_time();
            std::cout.flush();
            vis_image->read_from_gpu_mem(queue,vis_buffs[frame],cl_ni*cl_nj*sizeof(float));
             clFinish(queue);
            // clear render kernel args so it can reset em on next execution
            kern->clear_args();
        }

        opencl_cache->shallow_remove_data(scene,*id,"alpha");
    }
    opencl_cache->free_mem(ray_image);
    opencl_cache->free_mem(vis_image);
    opencl_cache->unref_mem(exp_img_dim.ptr());
    opencl_cache->unref_mem(cl_output.ptr());
    opencl_cache->unref_mem(lookup.ptr());
    opencl_cache->unref_mem(ray_o_buff.ptr());
    //opencl_cache->free_mem_pool();
    clReleaseCommandQueue(queue);
    std::cout<<"GPU TIME "<<gpu_time<<" and total time "<<t.all()<<std::endl;
    t.mark();
    std::cout<<"Writing Vis Images"<<std::endl;
    for (unsigned int count = 0 ; count < tfinals.size(); count ++)
    {
        auto * vis_buf = (float*)vis_buffs[count];
        vil_image_view<bool> visbool(tfinals[count]->ni(),tfinals[count]->nj());
        visbool.fill(false);
        for (unsigned c=0;c<tfinals[count]->nj();c++)
            for (unsigned r=0;r<tfinals[count]->ni();r++)
            {
                if (vis_buf[c*cl_ni+r] > 0.5)
                    visbool(r,c)=true;
            }
        vil_structuring_element s;
        s.set_to_disk(2.0);
        vil_binary_closing(visbool,visbool,s);
        vil_image_view<unsigned char> visout(tfinals[count]->ni(),tfinals[count]->nj(),3);
        visout.fill(0);
        for (unsigned c=0;c<visbool.nj();c++)
            for (unsigned r=0;r<visbool.ni();r++)
            {
                if (visbool(r,c))
                    visout(r,c,1)=255;
            }
       int rescaled_ni = tfinals[count]->ni() * scale;
       int rescaled_nj = tfinals[count]->nj() * scale;
       vil_image_view<unsigned char> visout_rescaled(rescaled_ni,rescaled_nj,3);
       vil_resample_nearest<unsigned char,unsigned char>(visout, visout_rescaled, rescaled_ni, rescaled_nj);

       char filename[1000];
       std::sprintf(filename,"vis_%d.jpg",count);
       std::string outfile = outdir +"/" + filename;
       std::cout<<"filename"<<outfile<<std::endl;
       vil_save(visout_rescaled,outfile.c_str());
    }
    std::cout<<"it took "<<t.all()<<std::endl;
    for (unsigned int count = 0 ; count < tfinals.size(); count ++)
    {
       delete [] rayd_buffs[count];
       delete [] vis_buffs[count];
    }
    std::cout<<"Time taken is "<<t.all()<<std::endl;
    return true;
}
