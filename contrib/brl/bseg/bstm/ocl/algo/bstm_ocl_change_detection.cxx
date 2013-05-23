// This is brl/bseg/bstm/ocl/algo/bstm_ocl_change_detection.cxx
//:
// \file
// \brief  A process for change detection
//
// \author Ali Osman Ulusoy
// \date May 15, 2013

#include "bstm_ocl_change_detection.h"

#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <vcl_sstream.h>
#include <bstm/ocl/bstm_opencl_cache.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_data_base.h>
#include <bstm/ocl/bstm_ocl_util.h>
#include <bstm/bstm_util.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/vil_pyramid_image_view.h>
#include <vil/vil_resample_bilin.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

//using variables in global namespace
using namespace bstm_ocl_change_detection_globals;



//declare static map
vcl_map<vcl_string, vcl_vector<bocl_kernel*> > bstm_ocl_change_detection::kernels_;


//main change detect function
bool bstm_ocl_change_detection::change_detect( vil_image_view<float>&    change_img,
                                                bocl_device_sptr          device,
                                                bstm_scene_sptr          scene,
                                                bstm_opencl_cache_sptr   opencl_cache,
                                                vpgl_camera_double_sptr   cam,
                                                vil_image_view_base_sptr  img,
                                                vil_image_view_base_sptr  mask_sptr,
                                                vcl_string                norm_type,
                                                bool                      pmax,
                                                float                     time)
{
    float transfer_time=0.0f;
    float gpu_time=0.0f;

    //img dimensions, global and local threads (will be correctly set)
    unsigned ni=img->ni();
    unsigned nj=img->nj();
    vcl_size_t  local_threads [2] = {8,8};
    vcl_size_t  global_threads[2] = {8,8};

    //---- get scene info -----
    vcl_string data_type,label_data_type;
    int apptypesize,label_apptypesize;
    vcl_vector<vcl_string> valid_types;
    valid_types.push_back(bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix());
    if ( !bstm_util::verify_appearance( *scene, valid_types, data_type, apptypesize ) ) {
      vcl_cout<<"bstm_ocl_render_gl_expected_image_process ERROR: scene doesn't have correct appearance model data type"<<vcl_endl;
      return false;
    }
    vcl_string options = bstm_ocl_util::mog_options(data_type);
    //specify max mode options
    if ( pmax )
        options += " -D USE_MAX_MODE  ";

    //grab kernel
    vcl_vector<bocl_kernel*>& kerns = get_kernels(device, options);

    //create a command queue.
    int status=0;
    cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
        CL_QUEUE_PROFILING_ENABLE,&status);
    if (status!=0)
        return false;

    //catch a "null" mask (not really null because that throws an error)
    bool use_mask = false;
    if ( mask_sptr->ni() == img->ni() && mask_sptr->nj() == img->nj() ) {
      vcl_cout<<"Change detection using mask."<<vcl_endl;
      use_mask = true;
    }
    vil_image_view<unsigned char >* mask_map = 0;
    if (use_mask) {
      mask_map = dynamic_cast<vil_image_view<unsigned char> *>(mask_sptr.ptr());
      if (!mask_map) {
        vcl_cout<<"bstm_ocl_change_detection:: mask map is not an unsigned char map"<<vcl_endl;
        return false;
      }
    }
    //----- PREP INPUT BUFFERS -------------
    //prepare input images
    vil_image_view_base_sptr float_img     = bstm_util::prepare_input_image(img, true); //true for force gray scale
    vil_image_view<float>*   img_view      = static_cast<vil_image_view<float>* >(float_img.ptr());

    //prepare workspace size
    unsigned cl_ni    = RoundUp(img_view->ni(),local_threads[0]);
    unsigned cl_nj    = RoundUp(img_view->nj(),local_threads[1]);
    global_threads[0] = cl_ni;
    global_threads[1] = cl_nj;

    // create all buffers
    cl_float* ray_origins = new cl_float[4*cl_ni*cl_nj];
    cl_float* ray_directions = new cl_float[4*cl_ni*cl_nj];
    bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins, "ray_origins buffer");
    bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions, "ray_directions buffer");
    boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

    //prepare image buffers (cpu)
    float* vis_buff               = new float[cl_ni*cl_nj];
    float* change_image_buff      = new float[cl_ni*cl_nj];
    unsigned char* mask_image_buff   = new unsigned char[cl_ni*cl_nj];
    float* input_buff             = new float[4*cl_ni*cl_nj];
    full_pyramid(float_img, input_buff, cl_ni, cl_nj);

    for (unsigned i=0;i<cl_ni*cl_nj;i++) {
        vis_buff[i]=1.0f;
        change_image_buff[i]=0.0f;
    }

    int count=0;
    for (unsigned int j=0;j<cl_nj;++j) {
        for (unsigned int i=0;i<cl_ni;++i) {
          mask_image_buff[count]=0;
            if (i<img_view->ni() && j< img_view->nj())
              mask_image_buff[count]=(*mask_map)(i,j);
            ++count;
        }
    }

    //prepare image buffers (GPU)
    //bocl_mem_sptr in_image=new bocl_mem(device->context(),input_buff, 4*cl_ni*cl_nj*sizeof(float),"input image buffer");
    bocl_mem_sptr in_image = opencl_cache->alloc_mem(4*cl_ni*cl_nj*sizeof(float), input_buff, "input image buffer");
    in_image->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    bocl_mem_sptr mask_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(char), mask_image_buff, "mask image buffer");
    mask_image->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    //bocl_mem_sptr change_image=new bocl_mem(device->context(),change_image_buff,cl_ni*cl_nj*sizeof(float),"change image buffer");
    bocl_mem_sptr change_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), change_image_buff, "change image buffer");
    change_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    //bocl_mem_sptr vis_image=new bocl_mem(device->context(),vis_buff,cl_ni*cl_nj*sizeof(float),"vis image buffer");
    bocl_mem_sptr vis_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), vis_buff, "vis image buffer");
    vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // Image Dimensions
    unsigned int img_dim_buff[] = { 0, 0, img_view->ni(), img_view->nj() };
    bocl_mem_sptr img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(unsigned int)*4, "image dims");
    img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // Output Array
    float output_arr[100];
    for (int i=0; i<100; ++i) output_arr[i] = 0.0f;
    bocl_mem_sptr  cl_output=new bocl_mem(device->context(), output_arr, sizeof(float)*100, "output buffer");
    cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // bit lookup buffer
    cl_uchar lookup_arr[256];
    bstm_ocl_util::set_bit_lookup(lookup_arr);
    bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
    lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    cl_float cl_time = 0;
    bocl_mem_sptr time_mem =new bocl_mem(device->context(), &cl_time, sizeof(cl_float), "time instance buffer");
    time_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);


    //----------------------------------------------------------------------------
    // STEP ONE: Do 1x1 Change Detection Pass
    //----------------------------------------------------------------------------
    //For each ID in the visibility order, grab that block
    vcl_vector<bstm_block_id> vis_order = scene->get_vis_blocks(cam);
    vcl_vector<bstm_block_id>::iterator id;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        //choose correct render kernel
        bstm_block_metadata mdata = scene->get_block_metadata(*id);
        bocl_kernel* kern =  kerns[0];

        //if the current blk does not contain the queried time, no need to ray cast
        double local_time;
        if(!mdata.contains_t(time,local_time))
          continue;
        //write cl_time
        cl_time = (cl_float)local_time;
        time_mem->write_to_buffer(queue);

        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk       = opencl_cache->get_block(*id);
        bocl_mem* blk_t     = opencl_cache->get_time_block(*id);
        bocl_mem* blk_info  = opencl_cache->loaded_block_info();
        bocl_mem* alpha     = opencl_cache->get_data<BSTM_ALPHA>(*id);
        bocl_mem* mog       = opencl_cache->get_data(*id,data_type);
        transfer_time += (float) transfer.all();

        ////3. SET args
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( blk_t );
        kern->set_arg( alpha );
        kern->set_arg( mog );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );
        kern->set_arg( in_image.ptr() );
        kern->set_arg( change_image.ptr() );
        kern->set_arg( img_dim.ptr());
        kern->set_arg( cl_output.ptr() );
        kern->set_arg( lookup.ptr() );
        kern->set_arg( vis_image.ptr() );
        kern->set_arg( time_mem.ptr() );

        //local tree , cumsum buffer, imindex buffer
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar8) );
        kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) );
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_int) );

        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        clFinish(queue);
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
    }
    change_image->read_to_buffer(queue);
    vis_image->read_to_buffer(queue);


    //----------------------------------------------------------------------------
    // STEP TWO: Do normalize pass on change image
    //----------------------------------------------------------------------------
    bocl_kernel* normalize_change_kernel =  kerns[1];
    {
        normalize_change_kernel->set_arg( change_image.ptr() );
        normalize_change_kernel->set_arg( vis_image.ptr() );
        normalize_change_kernel->set_arg( mask_image.ptr() );
        normalize_change_kernel->set_arg( img_dim.ptr());
        normalize_change_kernel->execute( queue, 2, local_threads, global_threads);
        clFinish(queue);
        gpu_time += normalize_change_kernel->exec_time();

        //clear render kernel args so it can reset em on next execution
        normalize_change_kernel->clear_args();
    }
    // read out expected image
    change_image->read_to_buffer(queue);
    vcl_cout<<"Change Detection GPU Time: " << gpu_time << " ms" << vcl_endl;

    //----------------------------------------------------------------------------
    //prep output images
    //----------------------------------------------------------------------------
    //store change image
    for (unsigned c=0;c<nj;c++)
        for (unsigned r=0;r<ni;r++)
            change_img(r,c) = change_image_buff[c*cl_ni+r];

    //cleanup the image buffers
    delete [] ray_origins;
    delete [] ray_directions;
    delete [] change_image_buff;
    delete [] vis_buff;
    delete [] input_buff;
    opencl_cache->unref_mem(in_image.ptr());
    opencl_cache->unref_mem(vis_image.ptr());
    opencl_cache->unref_mem(change_image.ptr());
    opencl_cache->unref_mem(ray_o_buff.ptr());
    opencl_cache->unref_mem(ray_d_buff.ptr());
    clReleaseCommandQueue(queue);
    return true;
}



//-------------------------------------------------
// Creates full in image (float4 pixels, each along one level of the pyramid
//-------------------------------------------------
void bstm_ocl_change_detection::full_pyramid(vil_image_view_base_sptr in_img, float* img_buff, unsigned cl_ni, unsigned cl_nj)
{
    // half the size of the previous image
    vil_pyramid_image_view<float> pyramid(in_img, 4);

    // resized images
    unsigned ni = in_img->ni(), nj = in_img->nj();
    vil_image_view<float> half(ni,nj), quarter(ni,nj), eighth(ni,nj);
    vil_resample_bilin(pyramid(1), half, ni, nj);
    vil_resample_bilin(pyramid(2), quarter, ni, nj);
    vil_resample_bilin(pyramid(3), eighth, ni, nj);
#if 0
    vil_save(half, "half.tiff");
    vil_save(quarter, "quarter.tiff");
    vil_save(eighth, "eighth.tiff");
#endif
    //load up each level of pixels
    int idx=0;
    for (unsigned c=0; c<cl_nj; ++c) {
        for (unsigned r=0; r<cl_ni; ++r) {
            if (c<nj && r<ni) {
                img_buff[idx] = pyramid(0)(r,c);
                img_buff[idx+1] = half(r,c);
                img_buff[idx+2] = quarter(r,c);
                img_buff[idx+3] = eighth(r,c);
            }
            idx += 4;
        }
    }
}


//---------------------------------------------------
// compiles, caches and returns list of kernels
//---------------------------------------------------
vcl_vector<bocl_kernel*>& bstm_ocl_change_detection::get_kernels(bocl_device_sptr device, vcl_string opts)
{
    // check to see if this device has compiled kernels already
    vcl_string identifier = device->device_identifier() + opts;
    if (kernels_.find(identifier) != kernels_.end())
        return kernels_[identifier];

    //if not, compile and cache them
    vcl_cout<<"===========Compiling change detection kernels===========\n"
            <<"  for device: "<<device->device_identifier()<<vcl_endl;

    //gather all render sources... seems like a lot for rendering...
    vcl_vector<vcl_string> src_paths;
    vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/bstm/ocl/cl/";
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "bit/time_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "expected_functor.cl");
    src_paths.push_back(source_dir + "view_dep_app_helper_functions.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "change/change_detection.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //set kernel options
    opts += " -D CHANGE ";
    vcl_string options=opts;
    opts += " -D STEP_CELL=step_cell_change(aux_args,data_ptr_tt,d*linfo->block_len) ";

    //have kernel construct itself using the context and device
    bocl_kernel * ray_trace_kernel = new bocl_kernel();
    ray_trace_kernel->create_kernel( &device->context(),
                                     device->device_id(),
                                     src_paths,
                                     "change_detection_bit_scene",   //kernel name
                                     opts,              //options
                                     "bstm 1x1 ocl change detection"); //kernel identifier (for error checking)



    //create normalize image kernel
    vcl_vector<vcl_string> norm_src_paths;
    norm_src_paths.push_back(source_dir + "pixel_conversion.cl");
    norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
    bocl_kernel * normalize_render_kernel=new bocl_kernel();
    normalize_render_kernel->create_kernel( &device->context(),
                                            device->device_id(),
                                            norm_src_paths,
                                            "normalize_change_kernel",   //kernel name
                                            options + " -D NORMALIZE_CHANGE ",              //options
                                            "normalize change detection kernel"); //kernel identifier (for error checking)

    //store in a vector in the map and return
    vcl_vector<bocl_kernel*> vec_kernels;
    vec_kernels.push_back(ray_trace_kernel);
    vec_kernels.push_back(normalize_render_kernel);
    kernels_[identifier] = vec_kernels;
    return kernels_[identifier];
}



vcl_map<vcl_string, vcl_vector<bocl_kernel*> > bstm_ocl_aux_pass_change::kernels_;

bool bstm_ocl_aux_pass_change::change_detect(vil_image_view<float>&    change_img,
                                              bocl_device_sptr          device,
                                              bstm_scene_sptr           scene,
                                              bstm_opencl_cache_sptr   opencl_cache,
                                              vpgl_camera_double_sptr   cam,
                                              vil_image_view_base_sptr  img,
                                              vil_image_view_base_sptr  mask_sptr,
                                              float                     time,
                                              bool max_density)
{
    float transfer_time=0.0f;
    float gpu_time=0.0f;

    //img dimensions, global and local threads (will be correctly set)
    unsigned ni=img->ni();
    unsigned nj=img->nj();
    vcl_size_t  local_threads [2] = {8,8};
    vcl_size_t  global_threads[2] = {8,8};

    //---- get scene info -----
    vcl_string data_type,label_data_type;
    int apptypesize,label_apptypesize;
    vcl_vector<vcl_string> valid_types;
    valid_types.push_back(bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix());
    if ( !bstm_util::verify_appearance( *scene, valid_types, data_type, apptypesize ) ) {
      vcl_cout<<"bstm_ocl_render_gl_expected_image_process ERROR: scene doesn't have correct appearance model data type"<<vcl_endl;
      return false;
    }
    vcl_string options = bstm_ocl_util::mog_options(data_type);

    //grab kernel
    vcl_vector<bocl_kernel*>& kerns = get_kernels(device, options,max_density);

    //create a command queue.
    int status=0;
    cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
        CL_QUEUE_PROFILING_ENABLE,&status);
    if (status!=0)
        return false;

    //----- PREP INPUT BUFFERS -------------
    //prepare input images
    vil_image_view_base_sptr float_img     = bstm_util::prepare_input_image(img, true); //true for force gray scale
    vil_image_view<float>*   img_view      = static_cast<vil_image_view<float>* >(float_img.ptr());

    //catch a "null" mask (not really null because that throws an error)
    bool use_mask = false;
    if ( mask_sptr->ni() == img->ni() && mask_sptr->nj() == img->nj() ) {
      vcl_cout<<"Change detection using mask."<<vcl_endl;
      use_mask = true;
    }
    vil_image_view<unsigned char >* mask_map = 0;
    if (use_mask) {
      mask_map = dynamic_cast<vil_image_view<unsigned char> *>(mask_sptr.ptr());
      if (!mask_map) {
        vcl_cout<<"bstm_ocl_change_detection:: mask map is not an unsigned char map"<<vcl_endl;
        return false;
      }
    }

    //prepare workspace size
    unsigned cl_ni    = RoundUp(img_view->ni(),local_threads[0]);
    unsigned cl_nj    = RoundUp(img_view->nj(),local_threads[1]);
    global_threads[0] = cl_ni;
    global_threads[1] = cl_nj;

    //set generic cam
    cl_float* ray_origins    = new cl_float[4*cl_ni*cl_nj];
    cl_float* ray_directions = new cl_float[4*cl_ni*cl_nj];
    bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(   cl_ni*cl_nj * sizeof(cl_float4),  ray_origins, "ray_origins buffer");
    bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem( cl_ni*cl_nj * sizeof(cl_float4),ray_directions, "ray_directions buffer");
    boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);



    //prepare image buffers (cpu)
    float* vis_buff               = new float[cl_ni*cl_nj];
    float* change_image_buff      = new float[cl_ni*cl_nj];
    float* input_buff             = new float[cl_ni*cl_nj];
    unsigned char* mask_image_buff   = new unsigned char[cl_ni*cl_nj];

    for (unsigned i=0;i<cl_ni*cl_nj;i++) {
        vis_buff[i]              = 1.0f;
        change_image_buff[i]     = 0.0f;
    }

    int count=0;
    for (unsigned int j=0;j<cl_nj;++j) {
        for (unsigned int i=0;i<cl_ni;++i) {
            input_buff[count] = 0.0f;
            if (i<img_view->ni() && j< img_view->nj()) {
                input_buff[count]     = (*img_view)(i,j);
                mask_image_buff[count]=(*mask_map)(i,j);
            }
            ++count;
        }
    }

    //prepare image buffers (GPU)
    bocl_mem_sptr in_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), input_buff, "input image buffer");
    in_image->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    bocl_mem_sptr change_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), change_image_buff, "change image buffer");
    change_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    bocl_mem_sptr mask_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(char), mask_image_buff, "mask image buffer");
    mask_image->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    bocl_mem_sptr vis_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), vis_buff, "vis image buffer");
    vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // Image Dimensions
    unsigned int img_dim_buff[] = { 0, 0, img_view->ni(), img_view->nj() };
    bocl_mem_sptr img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(unsigned int)*4, "image dims");
    img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // Output Array
    float* output_arr = new float[cl_ni*cl_nj];
    bocl_mem_sptr cl_output=new bocl_mem(device->context(), output_arr, sizeof(float)*cl_ni*cl_nj, "output buffer");
    cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // bit lookup buffer
    cl_uchar lookup_arr[256];
    bstm_ocl_util::set_bit_lookup(lookup_arr);
    bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
    lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    cl_float cl_time = 0;
    bocl_mem_sptr time_mem =new bocl_mem(device->context(), &cl_time, sizeof(cl_float), "time instance buffer");
    time_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);


    //----- STEP ONE: per cell mean obs pass ---------
    //For each ID in the visibility order, grab that block
    vcl_vector<bstm_block_id> vis_order = scene->get_vis_blocks(cam);
    vcl_vector<bstm_block_id>::iterator id;
    vcl_cout<<"  STEP ONE"<<vcl_endl;
    bocl_kernel* kern =  kerns[0];
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

        //-----write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk       = opencl_cache->get_block(*id);
        bocl_mem* blk_t     = opencl_cache->get_time_block(*id);

        bocl_mem* blk_info  = opencl_cache->loaded_block_info();
        bocl_mem* alpha     = opencl_cache->get_data<BSTM_ALPHA>(*id);

        // aux buffers (determine length first)
        bstm_scene_info* info_buffer = (bstm_scene_info*) blk_info->cpu_buffer();
        int alphaTypeSize = (int) bstm_data_info::datasize(bstm_data_traits<BSTM_ALPHA>::prefix());
        info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
        blk_info->write_to_buffer(queue);

        int auxTypeSize = (int) bstm_data_info::datasize(bstm_data_traits<BSTM_AUX0>::prefix());
        bocl_mem *aux0  = opencl_cache->get_data<BSTM_AUX0>(*id, info_buffer->data_buffer_length*auxTypeSize);
        auxTypeSize     = (int) bstm_data_info::datasize(bstm_data_traits<BSTM_AUX1>::prefix());
        bocl_mem *aux1  = opencl_cache->get_data<BSTM_AUX1>(*id, info_buffer->data_buffer_length*auxTypeSize);
        transfer_time += (float) transfer.all();


        //3. SET args
        aux0->zero_gpu_buffer(queue,true);
        aux1->zero_gpu_buffer(queue,true);

        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( blk_t);
        kern->set_arg( alpha );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( lookup.ptr() );

        // kern->set_arg( persp_cam.ptr() );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );

        kern->set_arg( img_dim.ptr() );
        kern->set_arg( in_image.ptr() );
        kern->set_arg( time_mem.ptr() );
        kern->set_arg( cl_output.ptr() );
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar8) );
        kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) );

        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        clFinish(queue);
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
    }
    kern =  kerns[1];
    //----- STEP TWO: prob background pas --------
    vcl_cout<<"  STEP TWO CHANGE PASS"<<vcl_endl;
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

        //-----write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk       = opencl_cache->get_block(*id);
        bocl_mem* blk_t     = opencl_cache->get_time_block(*id);
        bocl_mem* blk_info  = opencl_cache->loaded_block_info();
        bocl_mem* alpha     = opencl_cache->get_data<BSTM_ALPHA>(*id);
        bocl_mem* mog       = opencl_cache->get_data(*id,data_type);

        // aux buffers (determine length first)
        bstm_scene_info* info_buffer = (bstm_scene_info*) blk_info->cpu_buffer();
        int alphaTypeSize = (int) bstm_data_info::datasize(bstm_data_traits<BSTM_ALPHA>::prefix());
        info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
        blk_info->write_to_buffer((queue));
        int auxTypeSize = (int) bstm_data_info::datasize(bstm_data_traits<BSTM_AUX0>::prefix());
        bocl_mem *aux0  = opencl_cache->get_data<BSTM_AUX0>(*id, info_buffer->data_buffer_length*auxTypeSize);
        auxTypeSize     = (int) bstm_data_info::datasize(bstm_data_traits<BSTM_AUX1>::prefix());
        bocl_mem *aux1  = opencl_cache->get_data<BSTM_AUX1>(*id, info_buffer->data_buffer_length*auxTypeSize);
        transfer_time += (float) transfer.all();

        //3. SET args
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( blk_t);
        kern->set_arg( alpha );
        kern->set_arg( mog );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );
        kern->set_arg( change_image.ptr() );
        kern->set_arg( img_dim.ptr());
        kern->set_arg( time_mem.ptr() );
        kern->set_arg( cl_output.ptr() );
        kern->set_arg( lookup.ptr() );
        kern->set_arg( vis_image.ptr() );

        //local tree , cumsum buffer, imindex buffer
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar8) );
        kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) );
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_int) );

        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        clFinish(queue);
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
    }

    bocl_kernel* normalize_change_kernel =  kerns[2];
    {

        normalize_change_kernel->set_arg( change_image.ptr() );
        normalize_change_kernel->set_arg( vis_image.ptr() );
        normalize_change_kernel->set_arg( mask_image.ptr() );
        normalize_change_kernel->set_arg( img_dim.ptr());
        normalize_change_kernel->execute( queue, 2, local_threads, global_threads);
        clFinish(queue);
        gpu_time += normalize_change_kernel->exec_time();

        //clear render kernel args so it can reset em on next execution
        normalize_change_kernel->clear_args();
    }
    change_image->read_to_buffer(queue);

    vcl_cout<<"Change Detection GPU Time: " << gpu_time << " ms" << vcl_endl;

    //store change image
    for (unsigned c=0;c<nj;c++)
        for (unsigned r=0;r<ni;r++)
            change_img(r,c) = change_image_buff[c*cl_ni+r];

    //cleanup the image buffers
    delete [] vis_buff;
    delete [] input_buff;
    delete [] output_arr;
    delete [] change_image_buff;
    delete [] ray_origins;
    delete [] ray_directions;
    opencl_cache->unref_mem(in_image.ptr());
    opencl_cache->unref_mem(vis_image.ptr());
    opencl_cache->unref_mem(change_image.ptr());
    opencl_cache->unref_mem(ray_o_buff.ptr());
    opencl_cache->unref_mem(ray_d_buff.ptr());

    clReleaseCommandQueue(queue);
    return true;
}

vcl_vector<bocl_kernel*>& bstm_ocl_aux_pass_change::get_kernels(bocl_device_sptr device, vcl_string opts, bool maxdensity)
{
    // check to see if this device has compiled kernels already
    vcl_string identifier = device->device_identifier() + opts;
    if (kernels_.find(identifier) != kernels_.end())
        return kernels_[identifier];

    //if not, compile and cache them
    vcl_cout<<"===========Compiling two pass change kernels===========\n"
            <<"  for device: "<<device->device_identifier()<<vcl_endl;

    //gather all render sources... seems like a lot for rendering...
    vcl_vector<vcl_string> src_paths;
    vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/bstm/ocl/cl/";
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "bit/time_tree_library_functions.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "view_dep_app_helper_functions.cl");
    src_paths.push_back(source_dir + "change/two_pass_change_detection.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");


    //pass one, seglen
    vcl_string seg_options = opts + "  -D CHANGE_SEGLEN -D STEP_CELL=step_cell_seglen(aux_args,data_ptr_tt,d) ";
    bocl_kernel* seg_len = new bocl_kernel();
    seg_len->create_kernel( &device->context(),
                            device->device_id(),
                            src_paths,
                            "seg_len_main",   //kernel name
                            seg_options,              //options
                            "bstm accumulate intensity (pass 1)"); //kernel identifier (for error checking)

    //pass two, change detection
    vcl_string change_options;


    if (maxdensity)
        change_options = opts + " -D AUX_CHANGE -D STEP_CELL=step_cell_change2_maxdensity(aux_args,data_ptr_tt,d) ";
    else
        change_options = opts + " -D AUX_CHANGE -D STEP_CELL=step_cell_change2(aux_args,data_ptr_tt,d) ";

    bocl_kernel* change_kernel = new bocl_kernel();
    change_kernel->create_kernel( &device->context(),
                                  device->device_id(),
                                  src_paths,
                                  "aux_pass_change_kernel",   //kernel name
                                  opts + " -D NORMALIZE_CHANGE ",              //options
                                  "bstm aux pass change kernel (pass 2)"); //kernel identifier (for error checking)


    //create normalize image kernel
    vcl_vector<vcl_string> norm_src_paths;
    norm_src_paths.push_back(source_dir + "pixel_conversion.cl");
    norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
    bocl_kernel * normalize_render_kernel=new bocl_kernel();
    normalize_render_kernel->create_kernel( &device->context(),
                                            device->device_id(),
                                            norm_src_paths,
                                            "normalize_change_kernel",   //kernel name
                                            opts + " -D NORMALIZE_CHANGE ",              //options
                                            "normalize change detection kernel"); //kernel identifier (for error checking)

    //store in a vector in the map and return
    vcl_vector<bocl_kernel*> vec_kernels;
    vec_kernels.push_back(seg_len);
    vec_kernels.push_back(change_kernel);
    vec_kernels.push_back(normalize_render_kernel);
    kernels_[identifier] = vec_kernels;
    return kernels_[identifier];
}
