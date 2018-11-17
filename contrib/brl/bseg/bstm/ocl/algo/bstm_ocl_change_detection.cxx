// This is brl/bseg/bstm/ocl/algo/bstm_ocl_change_detection.cxx
//:
// \file
// \brief  A process for change detection
//
// \author Ali Osman Ulusoy
// \date May 15, 2013

#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include "bstm_ocl_change_detection.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bstm/ocl/bstm_opencl_cache.h>
#include <bstm/bstm_scene.h>
#include <bstm/bstm_block.h>
#include <bstm/bstm_data_base.h>
#include <bstm/ocl/bstm_ocl_util.h>
#include <bstm/bstm_util.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <vil/vil_image_view.h>
#include <vil/algo/vil_median.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

//using variables in global namespace
using namespace bstm_ocl_change_detection_globals;



//declare static map
std::map<std::string, std::vector<bocl_kernel*> > bstm_ocl_change_detection::kernels_;


//main change detect function
bool bstm_ocl_change_detection::change_detect( vil_image_view<float>&    change_img,
                                                bocl_device_sptr          device,
                                                const bstm_scene_sptr&          scene,
                                                const bstm_opencl_cache_sptr&   opencl_cache,
                                                vpgl_camera_double_sptr   cam,
                                                const vil_image_view_base_sptr&  img,
                                                const vil_image_view_base_sptr&  mask_sptr,
                                                const std::string&                 /*norm_type*/,
                                                float                     time)
{
    float transfer_time=0.0f;
    float gpu_time=0.0f;

    //img dimensions, global and local threads (will be correctly set)
    unsigned ni=img->ni();
    unsigned nj=img->nj();
    std::size_t  local_threads [2] = {8,8};
    std::size_t  global_threads[2] = {8,8};

    //---- get scene info -----
    std::string data_type,label_data_type;
    int apptypesize,label_apptypesize;
    std::vector<std::string> valid_types;
    valid_types.push_back(bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix());
    valid_types.push_back(bstm_data_traits<BSTM_MOG3_GREY>::prefix());
    valid_types.push_back(bstm_data_traits<BSTM_GAUSS_RGB>::prefix());
    if ( !bstm_util::verify_appearance( *scene, valid_types, data_type, apptypesize ) ) {
      std::cout<<"bstm_ocl_change_detection ERROR: scene doesn't have correct appearance model data type"<<std::endl;
      return false;
    }
    std::string options = bstm_ocl_util::mog_options(data_type);
    bool isColor = (data_type == bstm_data_traits<BSTM_GAUSS_RGB>::prefix());

    //grab kernel
    std::vector<bocl_kernel*>& kerns = get_kernels(device, options, isColor);

    //create a command queue.
    int status=0;
    cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
        CL_QUEUE_PROFILING_ENABLE,&status);
    if (status!=0)
        return false;

    //catch a "null" mask (not really null because that throws an error)
    bool use_mask = false;
    if ( mask_sptr->ni() == img->ni() && mask_sptr->nj() == img->nj() ) {
      std::cout<<"Change detection using mask."<<std::endl;
      use_mask = true;
    }
    vil_image_view<unsigned char >* mask_map = nullptr;
    if (use_mask) {
      mask_map = dynamic_cast<vil_image_view<unsigned char> *>(mask_sptr.ptr());
      if (!mask_map) {
        std::cout<<"bstm_ocl_change_detection:: mask map is not an unsigned char map"<<std::endl;
        return false;
      }
    }


    unsigned cl_ni;
    unsigned cl_nj;
    float* input_buff;
    unsigned char* mask_image_buff;
    if(!isColor){
      vil_image_view_base_sptr float_img     = bstm_util::prepare_input_image(img, true); //true for force gray scale
      auto* img_view = static_cast<vil_image_view<float>* >(float_img.ptr());

      //prepare workspace size
      cl_ni    = RoundUp(img_view->ni(),local_threads[0]);
      cl_nj    = RoundUp(img_view->nj(),local_threads[1]);
      global_threads[0] = cl_ni;
      global_threads[1] = cl_nj;

      input_buff  = new float[cl_ni*cl_nj];
      mask_image_buff = new unsigned char[cl_ni*cl_nj];

      int count=0;
      for (unsigned int j=0;j<cl_nj;++j) {
          for (unsigned int i=0;i<cl_ni;++i) {
            mask_image_buff[count]=0;
            input_buff[count]     = 0;
              if (i<img_view->ni() && j< img_view->nj()) {
                input_buff[count]     = (*img_view)(i,j);
                if (use_mask)
                  mask_image_buff[count]=(*mask_map)(i,j);
                else
                  mask_image_buff[count]= 255;
              }
              ++count;
          }
      }

    }
    else
    {
      vil_image_view_base_sptr float_img = bstm_util::prepare_input_image(img, false);
      if ( float_img->pixel_format() != VIL_PIXEL_FORMAT_RGBA_BYTE ) {
        std::cout<<"bstm_ocl_update_color_process::using a non RGBA image!!"<<std::endl;
        return false;
      }
      auto* img_view = static_cast<vil_image_view<vil_rgba<vxl_byte> >* >(float_img.ptr());

      cl_ni=(unsigned)RoundUp(img_view->ni(),(int)local_threads[0]);
      cl_nj=(unsigned)RoundUp(img_view->nj(),(int)local_threads[1]);
      global_threads[0]=cl_ni;
      global_threads[1]=cl_nj;

      //initialize input image buffer
      int numFloats = 4;
      input_buff = new float[numFloats*cl_ni*cl_nj];  //need to store RGB (or YUV values)
      mask_image_buff = new unsigned char[cl_ni*cl_nj];

      int count=0;
      for (unsigned int j=0;j<cl_nj;++j) {
        for (unsigned int i=0;i<cl_ni;++i) {
          //rgba values
          input_buff[numFloats*count] = 0.0f;
          input_buff[numFloats*count + 1] = 0.0f;
          input_buff[numFloats*count + 2] = 0.0f;
          input_buff[numFloats*count + 3] = 1.0f;
          if (i<img_view->ni() && j< img_view->nj()) {
            vil_rgba<vxl_byte> rgba = (*img_view)(i,j);
            input_buff[numFloats*count + 0] = (float) rgba.R() / 255.0f;
            input_buff[numFloats*count + 1] = (float) rgba.G() / 255.0f;
            input_buff[numFloats*count + 2] = (float) rgba.B() / 255.0f;
            input_buff[numFloats*count + 3] = (float) 1.0f;
            if (use_mask)
              mask_image_buff[count]=(*mask_map)(i,j);
            else
              mask_image_buff[count]= 255;
          }
          ++count;
        }
      }
    }

    // create all buffers
    auto* ray_origins = new cl_float[4*cl_ni*cl_nj];
    auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
    bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins, "ray_origins buffer");
    bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions, "ray_directions buffer");
    boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

    //prepare image buffers (cpu)
    auto* vis_buff               = new float[cl_ni*cl_nj];
    auto* change_image_buff      = new float[cl_ni*cl_nj];

    for (unsigned i=0;i<cl_ni*cl_nj;i++) {
        vis_buff[i]=1.0f;
        change_image_buff[i]=0.0f;
    }


    //prepare image buffers (GPU)
    //bocl_mem_sptr in_image=new bocl_mem(device->context(),input_buff, 4*cl_ni*cl_nj*sizeof(float),"input image buffer");
    bocl_mem_sptr in_image;
    if(!isColor)
      in_image= opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), input_buff, "input image buffer");
    else
      in_image= opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float)*4, input_buff, "input image buffer");
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
    unsigned int img_dim_buff[] = { 0, 0, ni, nj };
    bocl_mem_sptr img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(unsigned int)*4, "image dims");
    img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

     //Output Array
    float output_arr[100];
    for (float & i : output_arr) i = 0.0f;
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

    cl_int cl_use_mask = (use_mask);
    bocl_mem_sptr use_mask_mem =new bocl_mem(device->context(), &cl_use_mask, sizeof(cl_int), "use mask ");
    use_mask_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    //----------------------------------------------------------------------------
    // STEP ONE: Do 1x1 Change Detection Pass
    //----------------------------------------------------------------------------
    //For each ID in the visibility order, grab that block
    std::vector<bstm_block_id> vis_order = scene->get_vis_blocks(cam);
    std::vector<bstm_block_id>::iterator id;
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
        normalize_change_kernel->set_arg( use_mask_mem.ptr());
        normalize_change_kernel->execute( queue, 2, local_threads, global_threads);
        clFinish(queue);
        gpu_time += normalize_change_kernel->exec_time();

        //clear render kernel args so it can reset em on next execution
        normalize_change_kernel->clear_args();
    }
    // read out expected image
    change_image->read_to_buffer(queue);
    std::cout<<"Change Detection GPU Time: " << gpu_time << " ms" << std::endl;

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
    opencl_cache->unref_mem(mask_image.ptr());

    clReleaseCommandQueue(queue);
    return true;
}


//---------------------------------------------------
// compiles, caches and returns list of kernels
//---------------------------------------------------
std::vector<bocl_kernel*>& bstm_ocl_change_detection::get_kernels(const bocl_device_sptr& device, std::string opts, bool isColor)
{
    // check to see if this device has compiled kernels already
    std::string identifier = device->device_identifier() + opts;
    if (kernels_.find(identifier) != kernels_.end())
        return kernels_[identifier];

    //if not, compile and cache them
    std::cout<<"===========Compiling change detection kernels===========\n"
            <<"  for device: "<<device->device_identifier()<<std::endl;

    //gather all render sources... seems like a lot for rendering...
    std::vector<std::string> src_paths;
    std::string source_dir = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/bstm/ocl/cl/";
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
    opts += " -D CHANGE_DETECT ";
    if(isColor)
      opts += " -D COLOR ";
    std::string options=opts;
    opts += " -D STEP_CELL=step_cell_change(aux_args,data_ptr_tt,d*linfo->block_len) ";

    //have kernel construct itself using the context and device
    auto * ray_trace_kernel = new bocl_kernel();
    ray_trace_kernel->create_kernel( &device->context(),
                                     device->device_id(),
                                     src_paths,
                                     "change_detection_bit_scene",   //kernel name
                                     opts,              //options
                                     "bstm 1x1 ocl change detection"); //kernel identifier (for error checking)



    //create normalize image kernel
    std::vector<std::string> norm_src_paths;
    norm_src_paths.push_back(source_dir + "pixel_conversion.cl");
    norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
    auto * normalize_render_kernel=new bocl_kernel();
    normalize_render_kernel->create_kernel( &device->context(),
                                            device->device_id(),
                                            norm_src_paths,
                                            "normalize_change_kernel",   //kernel name
                                            options + " -D NORMALIZE_CHANGE ",              //options
                                            "normalize change detection kernel"); //kernel identifier (for error checking)

    //store in a vector in the map and return
    std::vector<bocl_kernel*> vec_kernels;
    vec_kernels.push_back(ray_trace_kernel);
    vec_kernels.push_back(normalize_render_kernel);
    kernels_[identifier] = vec_kernels;
    return kernels_[identifier];
}



std::map<std::string, std::vector<bocl_kernel*> > bstm_ocl_aux_pass_change::kernels_;

bool bstm_ocl_aux_pass_change::change_detect(vil_image_view<float>&    change_img,
                                              bocl_device_sptr          device,
                                              const bstm_scene_sptr&           scene,
                                              const bstm_opencl_cache_sptr&   opencl_cache,
                                              vpgl_camera_double_sptr   cam,
                                              const vil_image_view_base_sptr&  img,
                                              const vil_image_view_base_sptr&  mask_sptr,
                                              float                     time)
{
    float transfer_time=0.0f;
    float gpu_time=0.0f;

    //img dimensions, global and local threads (will be correctly set)
    unsigned ni=img->ni();
    unsigned nj=img->nj();
    std::size_t  local_threads [2] = {8,8};
    std::size_t  global_threads[2] = {8,8};

    //---- get scene info -----
    std::string data_type,label_data_type;
    int apptypesize,label_apptypesize;
    std::vector<std::string> valid_types;
    valid_types.push_back(bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix());
    valid_types.push_back(bstm_data_traits<BSTM_MOG3_GREY>::prefix());
    if ( !bstm_util::verify_appearance( *scene, valid_types, data_type, apptypesize ) ) {
      std::cout<<"bstm_ocl_render_gl_expected_image_process ERROR: scene doesn't have correct appearance model data type"<<std::endl;
      return false;
    }
    std::string options = bstm_ocl_util::mog_options(data_type);

    //grab kernel
    std::vector<bocl_kernel*>& kerns = get_kernels(device, options);

    //create a command queue.
    int status=0;
    cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
        CL_QUEUE_PROFILING_ENABLE,&status);
    if (status!=0)
        return false;

    //----- PREP INPUT BUFFERS -------------
    //prepare input images
    vil_image_view_base_sptr float_img     = bstm_util::prepare_input_image(img, true); //true for force gray scale
    auto*   img_view      = static_cast<vil_image_view<float>* >(float_img.ptr());

    //catch a "null" mask (not really null because that throws an error)
    bool use_mask = false;
    if ( mask_sptr->ni() == img->ni() && mask_sptr->nj() == img->nj() ) {
      std::cout<<"Change detection using mask."<<std::endl;
      use_mask = true;
    }
    vil_image_view<unsigned char >* mask_map = nullptr;
    if (use_mask) {
      mask_map = dynamic_cast<vil_image_view<unsigned char> *>(mask_sptr.ptr());
      if (!mask_map) {
        std::cout<<"bstm_ocl_change_detection:: mask map is not an unsigned char map"<<std::endl;
        return false;
      }
    }


    //prepare workspace size
    unsigned cl_ni    = RoundUp(img_view->ni(),local_threads[0]);
    unsigned cl_nj    = RoundUp(img_view->nj(),local_threads[1]);
    global_threads[0] = cl_ni;
    global_threads[1] = cl_nj;

    //set generic cam
    auto* ray_origins    = new cl_float[4*cl_ni*cl_nj];
    auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
    bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(   cl_ni*cl_nj * sizeof(cl_float4),  ray_origins, "ray_origins buffer");
    bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem( cl_ni*cl_nj * sizeof(cl_float4),ray_directions, "ray_directions buffer");
    boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);


    //prepare image buffers (cpu)
    auto* vis_buff               = new float[cl_ni*cl_nj];
    auto* change_image_buff      = new float[cl_ni*cl_nj];
    auto* input_buff             = new float[cl_ni*cl_nj];
    auto* mask_image_buff   = new unsigned char[cl_ni*cl_nj];

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
                if(use_mask)
                  mask_image_buff[count]= (*mask_map)(i,j);
                else
                  mask_image_buff[count]= 255;
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
    auto* output_arr = new float[cl_ni*cl_nj];
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

    cl_int cl_use_mask = (use_mask);
    bocl_mem_sptr use_mask_mem =new bocl_mem(device->context(), &cl_use_mask, sizeof(cl_int), "use mask ");
    use_mask_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    //----- STEP ONE: per cell mean obs pass ---------
    //For each ID in the visibility order, grab that block
    std::vector<bstm_block_id> vis_order = scene->get_vis_blocks(cam);
    std::vector<bstm_block_id>::iterator id;
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
        bocl_mem* alpha     = opencl_cache->get_data<BSTM_ALPHA>(*id,0,false);
        bocl_mem* blk_info  = opencl_cache->loaded_block_info();
        bocl_mem* blk_t_info= opencl_cache->loaded_time_block_info();
        auto* info_buffer_t = (bstm_scene_info*) blk_t_info->cpu_buffer();

        //figure out sizes
        int alphaTypeSize = (int)bstm_data_info::datasize(bstm_data_traits<BSTM_ALPHA>::prefix());
        // check for invalid parameters
        if( alphaTypeSize == 0 ) //This should never happen, it will result in division by zero later
        {
            std::cout << "ERROR: alphaTypeSize == 0 in " << __FILE__ << __LINE__ << std::endl;
            return false;
        }

        int data_buffer_length = alpha->num_bytes() / alphaTypeSize;
        int num_time_trees = info_buffer_t->tree_buffer_length;

        int auxTypeSize = (int) bstm_data_info::datasize(bstm_data_traits<BSTM_AUX0>::prefix());
        bocl_mem *aux0  = opencl_cache->get_data<BSTM_AUX0>(*id, num_time_trees*auxTypeSize);
        auxTypeSize     = (int) bstm_data_info::datasize(bstm_data_traits<BSTM_AUX1>::prefix());
        bocl_mem *aux1  = opencl_cache->get_data<BSTM_AUX1>(*id, num_time_trees*auxTypeSize);
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
        kern->release_current_event();
    }
    kern =  kerns[1];
    //----- STEP TWO: prob background pas --------
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

        vul_timer transfer;
        bocl_mem* blk       = opencl_cache->get_block(*id);
        bocl_mem* blk_t     = opencl_cache->get_time_block(*id);
        bocl_mem* alpha     = opencl_cache->get_data<BSTM_ALPHA>(*id,0,false);
        bocl_mem* mog       = opencl_cache->get_data(*id,data_type);
        bocl_mem* blk_info  = opencl_cache->loaded_block_info();
        bocl_mem* blk_t_info= opencl_cache->loaded_time_block_info();
        auto* info_buffer_t = (bstm_scene_info*) blk_t_info->cpu_buffer();

        //figure out sizes
        int alphaTypeSize = (int)bstm_data_info::datasize(bstm_data_traits<BSTM_ALPHA>::prefix());
        // check for invalid parameters
        if( alphaTypeSize == 0 ) //This should never happen, it will result in division by zero later
        {
            std::cout << "ERROR: alphaTypeSize == 0 in " << __FILE__ << __LINE__ << std::endl;
            return false;
        }

        int data_buffer_length = alpha->num_bytes() / alphaTypeSize;
        int num_time_trees = info_buffer_t->tree_buffer_length;

        int auxTypeSize = (int) bstm_data_info::datasize(bstm_data_traits<BSTM_AUX0>::prefix());
        bocl_mem *aux0  = opencl_cache->get_data<BSTM_AUX0>(*id, num_time_trees*auxTypeSize);
        auxTypeSize     = (int) bstm_data_info::datasize(bstm_data_traits<BSTM_AUX1>::prefix());
        bocl_mem *aux1  = opencl_cache->get_data<BSTM_AUX1>(*id, num_time_trees*auxTypeSize);
        transfer_time += (float) transfer.all();
        //3. SET args

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
        normalize_change_kernel->set_arg( use_mask_mem.ptr());
        normalize_change_kernel->execute( queue, 2, local_threads, global_threads);
        clFinish(queue);
        gpu_time += normalize_change_kernel->exec_time();

        //clear render kernel args so it can reset em on next execution
        normalize_change_kernel->clear_args();
    }
    change_image->read_to_buffer(queue);

    std::cout<<"Change Detection GPU Time: " << gpu_time << " ms" << std::endl;

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
    delete [] mask_image_buff;
    opencl_cache->unref_mem(in_image.ptr());
    opencl_cache->unref_mem(vis_image.ptr());
    opencl_cache->unref_mem(change_image.ptr());
    opencl_cache->unref_mem(ray_o_buff.ptr());
    opencl_cache->unref_mem(ray_d_buff.ptr());
    opencl_cache->unref_mem(mask_image.ptr());

    clReleaseCommandQueue(queue);
    return true;
}

std::vector<bocl_kernel*>& bstm_ocl_aux_pass_change::get_kernels(const bocl_device_sptr& device, const std::string& opts)
{
    // check to see if this device has compiled kernels already
    std::string identifier = device->device_identifier() + opts;
    if (kernels_.find(identifier) != kernels_.end())
        return kernels_[identifier];

    //if not, compile and cache them
    std::cout<<"===========Compiling two pass change kernels===========\n"
            <<"  for device: "<<device->device_identifier()<<std::endl;

    //gather all render sources... seems like a lot for rendering...
    std::vector<std::string> src_paths;
    std::string source_dir = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/bstm/ocl/cl/";
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "bit/time_tree_library_functions.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "view_dep_app_helper_functions.cl");
    src_paths.push_back(source_dir + "change/two_pass_change_detection.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //pass one, seglen
    std::string seg_options = opts + "  -D CHANGE_SEGLEN -D STEP_CELL=step_cell_seglen(aux_args,data_ptr,data_ptr_tt,d) ";
    auto* seg_len = new bocl_kernel();
    seg_len->create_kernel( &device->context(),
                            device->device_id(),
                            src_paths,
                            "seg_len_main",   //kernel name
                            seg_options,              //options
                            "bstm accumulate intensity (pass 1)"); //kernel identifier (for error checking)

    //pass two, change detection
    std::string change_options;
    change_options = opts + " -D AUX_CHANGE -D STEP_CELL=step_cell_change2(aux_args,data_ptr,data_ptr_tt,d) ";

    std::cout << "CHANGE OPTIONS: " << change_options << std::endl;
    auto* change_kernel = new bocl_kernel();
    change_kernel->create_kernel( &device->context(),
                                  device->device_id(),
                                  src_paths,
                                  "change_kernel",   //kernel name
                                  change_options ,              //options
                                  "bstm aux pass change kernel (pass 2)"); //kernel identifier (for error checking)


    //create normalize image kernel
    std::vector<std::string> norm_src_paths;
    norm_src_paths.push_back(source_dir + "pixel_conversion.cl");
    norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
    auto * normalize_render_kernel=new bocl_kernel();
    normalize_render_kernel->create_kernel( &device->context(),
                                            device->device_id(),
                                            norm_src_paths,
                                            "normalize_change_kernel",   //kernel name
                                            opts + " -D NORMALIZE_CHANGE ",              //options
                                            "normalize change detection kernel"); //kernel identifier (for error checking)

    //store in a vector in the map and return
    std::vector<bocl_kernel*> vec_kernels;
    vec_kernels.push_back(seg_len);
    vec_kernels.push_back(change_kernel);
    vec_kernels.push_back(normalize_render_kernel);
    kernels_[identifier] = vec_kernels;
    return kernels_[identifier];
}



///////////////////////////////////////////////////


//declare static map
std::map<std::string, std::vector<bocl_kernel*> > bstm_ocl_update_change::kernels_;


//main change detect function
bool bstm_ocl_update_change::update_change(vil_image_view<float>&    change_img,
                                                bocl_device_sptr          device,
                                                const bstm_scene_sptr&          scene,
                                                const bstm_opencl_cache_sptr&   opencl_cache,
                                                vpgl_camera_double_sptr   cam,
                                                const vil_image_view_base_sptr&  img,
                                                const vil_image_view_base_sptr&  mask_sptr,
                                                float                    time)
{
    float transfer_time=0.0f;
    float gpu_time=0.0f;

    //img dimensions, global and local threads (will be correctly set)
    unsigned ni=img->ni();
    unsigned nj=img->nj();
    std::size_t  local_threads [2] = {8,8};
    std::size_t  global_threads[2] = {8,8};

    //---- get scene info -----
    std::string data_type,label_data_type;
    int apptypesize,label_apptypesize;
    std::vector<std::string> valid_types;
    valid_types.push_back(bstm_data_traits<BSTM_MOG6_VIEW_COMPACT>::prefix());
    valid_types.push_back(bstm_data_traits<BSTM_MOG3_GREY>::prefix());
    valid_types.push_back(bstm_data_traits<BSTM_GAUSS_RGB>::prefix());
    if ( !bstm_util::verify_appearance( *scene, valid_types, data_type, apptypesize ) ) {
      std::cout<<"bstm_ocl_update_change_process ERROR: scene doesn't have correct appearance model data type"<<std::endl;
      return false;
    }

    std::string options = bstm_ocl_util::mog_options(data_type);
    bool isColor = (data_type == bstm_data_traits<BSTM_GAUSS_RGB>::prefix());

    //grab kernel
    std::vector<bocl_kernel*>& kerns = get_kernels(device, options, isColor);
    //create a command queue.
    int status=0;
    cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
        CL_QUEUE_PROFILING_ENABLE,&status);
    if (status!=0)
        return false;

    //catch a "null" mask (not really null because that throws an error)
    bool use_mask = false;
    if ( mask_sptr->ni() == img->ni() && mask_sptr->nj() == img->nj() ) {
      std::cout<<"Change detection using mask."<<std::endl;
      use_mask = true;
    }
    vil_image_view<unsigned char >* mask_map = nullptr;
    if (use_mask) {
      mask_map = dynamic_cast<vil_image_view<unsigned char> *>(mask_sptr.ptr());
      if (!mask_map) {
        std::cout<<"bstm_ocl_update_change:: mask map is not an unsigned char map"<<std::endl;
        return false;
      }
    }
    //----- PREP INPUT BUFFERS -------------
    //prepare input images

    unsigned cl_ni;
    unsigned cl_nj;
    float* input_buff;
    unsigned char* mask_image_buff;
    if(!isColor){
      vil_image_view_base_sptr float_img     = bstm_util::prepare_input_image(img, true); //true for force gray scale
      auto* img_view = static_cast<vil_image_view<float>* >(float_img.ptr());

      //prepare workspace size
      cl_ni    = RoundUp(img_view->ni(),local_threads[0]);
      cl_nj    = RoundUp(img_view->nj(),local_threads[1]);
      global_threads[0] = cl_ni;
      global_threads[1] = cl_nj;

      input_buff  = new float[cl_ni*cl_nj];
      mask_image_buff = new unsigned char[cl_ni*cl_nj];

      int count=0;
      for (unsigned int j=0;j<cl_nj;++j) {
          for (unsigned int i=0;i<cl_ni;++i) {
            mask_image_buff[count]=0;
            input_buff[count]     = 0;
              if (i<img_view->ni() && j< img_view->nj()) {
                input_buff[count]     = (*img_view)(i,j);
                if (use_mask)
                  mask_image_buff[count]=(*mask_map)(i,j);
                else
                  mask_image_buff[count]= 255;
              }
              ++count;
          }
      }

    }
    else
    {
      vil_image_view_base_sptr float_img = bstm_util::prepare_input_image(img, false);
      if ( float_img->pixel_format() != VIL_PIXEL_FORMAT_RGBA_BYTE ) {
        std::cout<<"bstm_ocl_update_color_process::using a non RGBA image!!"<<std::endl;
        return false;
      }
      auto* img_view = static_cast<vil_image_view<vil_rgba<vxl_byte> >* >(float_img.ptr());

      cl_ni=(unsigned)RoundUp(img_view->ni(),(int)local_threads[0]);
      cl_nj=(unsigned)RoundUp(img_view->nj(),(int)local_threads[1]);
      global_threads[0]=cl_ni;
      global_threads[1]=cl_nj;

      //initialize input image buffer
      int numFloats = 4;
      input_buff = new float[numFloats*cl_ni*cl_nj];  //need to store RGB (or YUV values)
      mask_image_buff = new unsigned char[cl_ni*cl_nj];

      int count=0;
      for (unsigned int j=0;j<cl_nj;++j) {
        for (unsigned int i=0;i<cl_ni;++i) {
          //rgba values
          input_buff[numFloats*count] = 0.0f;
          input_buff[numFloats*count + 1] = 0.0f;
          input_buff[numFloats*count + 2] = 0.0f;
          input_buff[numFloats*count + 3] = 1.0f;
          if (i<img_view->ni() && j< img_view->nj()) {
            vil_rgba<vxl_byte> rgba = (*img_view)(i,j);
            input_buff[numFloats*count + 0] = (float) rgba.R() / 255.0f;
            input_buff[numFloats*count + 1] = (float) rgba.G() / 255.0f;
            input_buff[numFloats*count + 2] = (float) rgba.B() / 255.0f;
            input_buff[numFloats*count + 3] = (float) 1.0f;
            if (use_mask)
              mask_image_buff[count]=(*mask_map)(i,j);
            else
              mask_image_buff[count]= 255;
          }
          ++count;
        }
      }
    }


    // create all buffers
    auto* ray_origins = new cl_float[4*cl_ni*cl_nj];
    auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
    bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins, "ray_origins buffer");
    bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions, "ray_directions buffer");
    boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

    //prepare image buffers (cpu)
    auto* vis_buff               = new float[cl_ni*cl_nj];
    auto* change_image_buff      = new float[cl_ni*cl_nj];

    for (unsigned i=0;i<cl_ni*cl_nj;i++) {
        vis_buff[i]=1.0f;
        change_image_buff[i]=0.0f;
    }



    //prepare image buffers (GPU)
    bocl_mem_sptr in_image;
    if(!isColor)
      in_image= opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), input_buff, "input image buffer");
    else
      in_image= opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float)*4, input_buff, "input image buffer");

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
    unsigned int img_dim_buff[] = { 0, 0, ni, nj };
    bocl_mem_sptr img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(unsigned int)*4, "image dims");
    img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // Output Array
    float output_arr[100];
    for (float & i : output_arr) i = -1.0f;
    bocl_mem_sptr  cl_output=new bocl_mem(device->context(), output_arr, sizeof(float)*100, "output buffer");
    cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);


    cl_int cl_use_mask = (use_mask);
    bocl_mem_sptr use_mask_mem =new bocl_mem(device->context(), &cl_use_mask, sizeof(cl_int), "use mask ");
    use_mask_mem->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

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
    std::vector<bstm_block_id> vis_order = scene->get_vis_blocks(cam);
    std::vector<bstm_block_id>::iterator id;
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
//
        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
    }

    //----------------------------------------------------------------------------
    // STEP TWO: Do normalize pass on change image
    //----------------------------------------------------------------------------
    bocl_kernel* normalize_change_kernel =  kerns[1];
    {
        normalize_change_kernel->set_arg( change_image.ptr() );
        normalize_change_kernel->set_arg( vis_image.ptr() );
        normalize_change_kernel->set_arg( mask_image.ptr() );
        normalize_change_kernel->set_arg( img_dim.ptr());
        normalize_change_kernel->set_arg( use_mask_mem.ptr());
        normalize_change_kernel->execute( queue, 2, local_threads, global_threads);
        clFinish(queue);
        gpu_time += normalize_change_kernel->exec_time();

        //clear render kernel args so it can reset em on next execution
        normalize_change_kernel->clear_args();
    }
    change_image->read_to_buffer(queue);


    //median filtering
    //store change image
    for (unsigned c=0;c<nj;c++)
        for (unsigned r=0;r<ni;r++)
            change_img(r,c) = change_image_buff[c*cl_ni+r];

    int medfilt_halfsize = 5;
    //vil_image_view<float> orig_img(image);
    auto* out_img =  new vil_image_view<float>(ni, nj);
    out_img->fill(0.0f);
    std::vector<int> strel_vec_i, strel_vec_j;
    for (int i=-medfilt_halfsize; i <= medfilt_halfsize; ++i)
      for (int j=-medfilt_halfsize; j <= medfilt_halfsize; ++j) {
        strel_vec_i.push_back(i);
        strel_vec_j.push_back(j); }

    vil_structuring_element strel(strel_vec_i,strel_vec_j);
    vil_median(change_img,*out_img,strel);

    for (unsigned c=0;c<nj;c++)
        for (unsigned r=0;r<ni;r++)
          change_image_buff[c*cl_ni+r] = (*out_img)(r,c);
    change_image->write_to_buffer(queue);
    /////////////////////////////////////////


    //----------------------------------------------------------------------------
    // STEP THREE: Do update pass
    //----------------------------------------------------------------------------
    //For each ID in the visibility order, grab that block
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        //choose correct render kernel
        bstm_block_metadata mdata = scene->get_block_metadata(*id);
        bocl_kernel* kern =  kerns[2];

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
        bocl_mem* blk_t_info= opencl_cache->loaded_time_block_info();
        bocl_mem* alpha     = opencl_cache->get_data<BSTM_ALPHA>(*id);

        auto* info_buffer = (bstm_scene_info*) blk_info->cpu_buffer();

        auto* info_buffer_t = (bstm_scene_info*) blk_t_info->cpu_buffer();
        int num_time_trees = info_buffer_t->tree_buffer_length;
        bocl_mem *cum_seglen= opencl_cache->get_data<BSTM_AUX0>(*id, num_time_trees* bstm_data_traits<BSTM_AUX0>::datasize() ,false);
        bocl_mem *cum_vis   = opencl_cache->get_data<BSTM_AUX1>(*id, num_time_trees* bstm_data_traits<BSTM_AUX1>::datasize() ,false);
        bocl_mem *cum_change= opencl_cache->get_data<BSTM_AUX2>(*id, num_time_trees* bstm_data_traits<BSTM_AUX2>::datasize() ,false);
        cum_seglen->zero_gpu_buffer(queue,true);
        cum_vis->zero_gpu_buffer(queue,true);
        cum_change->zero_gpu_buffer(queue,true);

        transfer_time += (float) transfer.all();
        ////3. SET args
        kern->set_arg( blk_t_info );
        kern->set_arg( blk );
        kern->set_arg( blk_t );
        kern->set_arg( alpha );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );
        kern->set_arg( change_image.ptr() );
        kern->set_arg( img_dim.ptr());
        kern->set_arg( cum_seglen );
        kern->set_arg( cum_vis );
        kern->set_arg( cum_change );
        kern->set_arg( lookup.ptr() );
        kern->set_arg( vis_image.ptr() );
        kern->set_arg( time_mem.ptr() );
        kern->set_arg( cl_output.ptr() );

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

#if 0
    //----------------------------------------------------------------------------
    // STEP FOUR: Final data-pass
    //----------------------------------------------------------------------------
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        //choose correct render kernel
        bstm_block_metadata mdata = scene->get_block_metadata(*id);
        bocl_kernel* kern =  kerns[3];

        //if the current blk does not contain the queried time, no need to ray cast
        double local_time;
        if(!mdata.contains_t(time,local_time))
          continue;
        //write cl_time
        cl_time = (cl_float)local_time;
        time_mem->write_to_buffer(queue);

        bocl_mem* blk       = opencl_cache->get_block(*id);
        bocl_mem* blk_t     = opencl_cache->get_time_block(*id);
        bocl_mem* blk_info  = opencl_cache->loaded_block_info();
        bocl_mem* blk_t_info= opencl_cache->loaded_time_block_info();
        bstm_scene_info* info_buffer_t = (bstm_scene_info*) blk_t_info->cpu_buffer();

        bocl_mem *cum_seglen= opencl_cache->get_data<BSTM_AUX0>(*id);
        bocl_mem *cum_vis   = opencl_cache->get_data<BSTM_AUX1>(*id);
        bocl_mem *cum_change= opencl_cache->get_data<BSTM_AUX2>(*id);
        bocl_mem *change_prob   = opencl_cache->get_data<BSTM_CHANGE>(*id, info_buffer_t->tree_buffer_length * bstm_data_traits<BSTM_CHANGE>::datasize() ,false);

        local_threads[0] = 64;
        local_threads[1] = 1 ;
        global_threads[0]= RoundUp(info_buffer_t->tree_buffer_length ,local_threads[0]);
        global_threads[1]=1;

        kern->set_arg( blk_t_info );
        kern->set_arg( cum_seglen );
        kern->set_arg( cum_vis );
        kern->set_arg( cum_change );
        kern->set_arg( change_prob );
        kern->set_arg( cl_output.ptr() );

        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
          return false;
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
        kern->release_current_event();

        //write info to disk
        change_prob->read_to_buffer(queue);
    }
#endif


    //----------------------------------------------------------------------------
    // STEP FOUR: Final data-pass
    //----------------------------------------------------------------------------
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        //choose correct render kernel
        bstm_block_metadata mdata = scene->get_block_metadata(*id);
        bocl_kernel* kern =  kerns[3];

        //if the current blk does not contain the queried time, no need to ray cast
        double local_time;
        if(!mdata.contains_t(time,local_time))
          continue;
        //write cl_time
        cl_time = (cl_float)local_time;
        time_mem->write_to_buffer(queue);

        bocl_mem* blk       = opencl_cache->get_block(*id);
        bocl_mem* blk_t     = opencl_cache->get_time_block(*id);
        bocl_mem* blk_info  = opencl_cache->loaded_block_info();
        bocl_mem* blk_t_info= opencl_cache->loaded_time_block_info();
        auto* info_buffer_t = (bstm_scene_info*) blk_t_info->cpu_buffer();

        bocl_mem *cum_seglen= opencl_cache->get_data<BSTM_AUX0>(*id);
        bocl_mem *cum_vis   = opencl_cache->get_data<BSTM_AUX1>(*id);
        bocl_mem *cum_change= opencl_cache->get_data<BSTM_AUX2>(*id);
        bocl_mem *change_array   = opencl_cache->get_data(*id, bstm_data_traits<BSTM_CHANGE>::prefix(), info_buffer_t->tree_buffer_length * bstm_data_traits<BSTM_CHANGE>::datasize() ,false);
        bocl_mem *change_ll   = opencl_cache->get_data(*id, bstm_data_traits<BSTM_CHANGE>::prefix("pos"), info_buffer_t->tree_buffer_length * bstm_data_traits<BSTM_CHANGE>::datasize() ,false);
        bocl_mem *no_change_ll   = opencl_cache->get_data(*id, bstm_data_traits<BSTM_CHANGE>::prefix("neg"), info_buffer_t->tree_buffer_length * bstm_data_traits<BSTM_CHANGE>::datasize() ,false);

        local_threads[0] = 64;
        local_threads[1] = 1 ;
        global_threads[0]= RoundUp(info_buffer_t->tree_buffer_length ,local_threads[0]);
        global_threads[1]=1;

        kern->set_arg( blk_t_info );
        kern->set_arg( cum_seglen );
        kern->set_arg( cum_vis );
        kern->set_arg( cum_change );
        kern->set_arg( change_ll );
        kern->set_arg( no_change_ll);
        kern->set_arg( change_array);
        kern->set_arg( use_mask_mem.ptr() );

        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
          return false;
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
        kern->release_current_event();

        //write info to disk
        change_ll->read_to_buffer(queue);
        no_change_ll->read_to_buffer(queue);
        change_array->read_to_buffer(queue);

    }

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
    delete [] mask_image_buff;
    opencl_cache->unref_mem(in_image.ptr());
    opencl_cache->unref_mem(vis_image.ptr());
    opencl_cache->unref_mem(change_image.ptr());
    opencl_cache->unref_mem(ray_o_buff.ptr());
    opencl_cache->unref_mem(ray_d_buff.ptr());
    opencl_cache->unref_mem(mask_image.ptr());

    clReleaseCommandQueue(queue);
    return true;
}


//---------------------------------------------------
// compiles, caches and returns list of kernels
//---------------------------------------------------
std::vector<bocl_kernel*>& bstm_ocl_update_change::get_kernels(const bocl_device_sptr& device, const std::string& opts, bool isColor)
{
    // check to see if this device has compiled kernels already
    std::string identifier = device->device_identifier() + opts;
    if (kernels_.find(identifier) != kernels_.end())
        return kernels_[identifier];

    //if not, compile and cache them
    std::cout<<"===========Compiling change detection kernels===========\n"
            <<"  for device: "<<device->device_identifier()<<std::endl;

    //gather all render sources... seems like a lot for rendering...
    std::vector<std::string> src_paths;
    std::string source_dir = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/bstm/ocl/cl/";
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "bit/time_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "pixel_conversion.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "expected_functor.cl");
    src_paths.push_back(source_dir + "atomics_util.cl");
    src_paths.push_back(source_dir + "view_dep_app_helper_functions.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "change/change_detection.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //set kernel options
    std::string change_options=opts;
    change_options += " -D CHANGE_DETECT ";
    if(isColor)
      change_options += " -D COLOR ";

    change_options += " -D STEP_CELL=step_cell_change(aux_args,data_ptr_tt,d*linfo->block_len) ";

    std::cout << "Compiling with options " << change_options << std::endl;
    //have kernel construct itself using the context and device
    auto * ray_trace_kernel = new bocl_kernel();
    ray_trace_kernel->create_kernel( &device->context(),
                                     device->device_id(),
                                     src_paths,
                                     "change_detection_bit_scene",   //kernel name
                                     change_options,              //options
                                     "bstm 1x1 ocl change detection"); //kernel identifier (for error checking)



    //create normalize image kernel
    const std::string& norm_options=opts;
    std::vector<std::string> norm_src_paths;
    norm_src_paths.push_back(source_dir + "pixel_conversion.cl");
    norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
    auto * normalize_render_kernel=new bocl_kernel();
    normalize_render_kernel->create_kernel( &device->context(),
                                            device->device_id(),
                                            norm_src_paths,
                                            "normalize_change_kernel",   //kernel name
                                            norm_options + " -D NORMALIZE_CHANGE ",              //options
                                            "normalize change detection kernel"); //kernel identifier (for error checking)


    //set kernel options
    std::string accum_options=opts;
    accum_options += " -D CHANGE_ACCUM ";
    accum_options += " -D STEP_CELL=step_cell_update(aux_args,data_ptr,data_ptr_tt,d) ";

    //have kernel construct itself using the context and device
    auto * accum_kernel = new bocl_kernel();
    accum_kernel->create_kernel( &device->context(),
                                     device->device_id(),
                                     src_paths,
                                     "update_change",   //kernel name
                                     accum_options,              //options
                                     "bstm 1x1 ocl change detection"); //kernel identifier (for error checking)

    const std::string& update_options=opts;
    std::vector<std::string> update_paths;
    update_paths.push_back(source_dir + "scene_info.cl");
    update_paths.push_back(source_dir + "change/change_detection.cl");
    auto * update_kernel=new bocl_kernel();
    update_kernel->create_kernel( &device->context(),
                                            device->device_id(),
                                            update_paths,
                                            "update_change_kernel",   //kernel name
                                            update_options + " -D ACCUM_CHANGE_LL ",              //options
                                            "update change kernel"); //kernel identifier (for error checking)


    //store in a vector in the map and return
    std::vector<bocl_kernel*> vec_kernels;
    vec_kernels.push_back(ray_trace_kernel);
    vec_kernels.push_back(normalize_render_kernel);
    vec_kernels.push_back(accum_kernel);
    vec_kernels.push_back(update_kernel);

    kernels_[identifier] = vec_kernels;
    return kernels_[identifier];
}
