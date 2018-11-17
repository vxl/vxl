// This is brl/bseg/boxm2/ocl/algo/boxm2_ocl_render_scene_uncertainty_map.cxx
#include "boxm2_ocl_render_scene_uncertainty_map.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <utility>
//:
// \file
// \brief  A class for rendering scene on a sphaerical map.
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

#include <vnl/vnl_float_4x4.h>
#include <vnl/vnl_inverse.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

//: Map of kernels should persist between process executions
std::map<std::string,std::vector<bocl_kernel*> > boxm2_ocl_render_scene_uncertainty_map::kernels_;

//: Main public method, updates color model
bool
boxm2_ocl_render_scene_uncertainty_map::render_scene_uncertainty_map( boxm2_scene_sptr & scene,
                                                                      bocl_device_sptr & device,
                                                                      boxm2_opencl_cache_sptr & opencl_cache,
                                                                      unsigned ni,
                                                                      unsigned nj,
                                                                      const std::string& ident,
                                                                      vil_image_view<float>* exp_image,
                                                                      vil_image_view<unsigned char>* radial_image,
                                                                      std::string cam_dir_1,
                                                                      std::string cam_dir_2)
{
    float transfer_time=0.0f;
    float gpu_time=0.0f;
    //setup local/global size
    std::size_t local_threads[2]={8,8};
    enum {
        COMPUTE_ALPHA_INTEGRAL = 0,
        RENDER_IMAGE = 1,
        NORMALIZE_KERNEL =2
    };
    bool foundDataType = false;
    std::string data_type, options;
    std::vector<std::string> apps = scene->appearances();

    int apptypesize = 0;
    for (const auto & app : apps) {
        if ( app == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
        {
            data_type = app;
            foundDataType = true;
            options=" -D MOG_TYPE_8 ";
            apptypesize = boxm2_data_traits<BOXM2_MOG3_GREY>::datasize();
        }
        else if ( app == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
        {
            data_type = app;
            foundDataType = true;
            options=" -D MOG_TYPE_16 ";
            apptypesize = boxm2_data_traits<BOXM2_MOG3_GREY_16>::datasize();
        }
        else if ( app == boxm2_data_traits<BOXM2_FLOAT8>::prefix() )
        {
            data_type = app;
            foundDataType = true;
            options=" -D FLOAT8 ";
            apptypesize = boxm2_data_traits<BOXM2_FLOAT8>::datasize();
        }
    }
    if (!foundDataType) {
        std::cout<<"BOXM2_OCL_RENDER_SCENE_UNCERTAINTY_MAP ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<std::endl;
        return false;
    }
    if (ident.size() > 0) {
        data_type += "_" + ident;
    }

    // create a command queue.
    int status=0;
    cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
        CL_QUEUE_PROFILING_ENABLE,&status);
    if (status!=0) return false;
    std::vector<bocl_kernel*> kernels = boxm2_ocl_render_scene_uncertainty_map::get_kernels(device,options);
    unsigned cl_ni=RoundUp(ni,local_threads[0]);
    unsigned cl_nj=RoundUp(nj,local_threads[1]);

    // create expected image buff
    // create vis buff
    // create cum_alpha_integral_buff
    // create alpha_integral_buff

    // Expected image buff
    auto* buff = new float[cl_ni*cl_nj];
    std::fill(buff, buff + cl_ni*cl_nj, 0.0f);
    bocl_mem_sptr exp=opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), buff,"exp image buffer");
    exp->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // Vis Image
    auto* vis_buff = new float[cl_ni*cl_nj];
    std::fill(vis_buff, vis_buff + cl_ni*cl_nj, 1.0f);
    bocl_mem_sptr vis=opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), vis_buff,"vis image");
    vis->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // Alpha Integral Image
    auto* alpha_integral_buff = new float[cl_ni*cl_nj];
    std::fill(alpha_integral_buff, alpha_integral_buff + cl_ni*cl_nj, 0.0f);
    bocl_mem_sptr alpha_integral_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), alpha_integral_buff,"alpha integral buffer");
    alpha_integral_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    // Alpha Integral Image
    auto* cum_alpha_integral_buff = new float[cl_ni*cl_nj];
    std::fill(cum_alpha_integral_buff, cum_alpha_integral_buff + cl_ni*cl_nj, 0.0f);
    bocl_mem_sptr cum_alpha_integral_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), cum_alpha_integral_buff," cumulative alpha integral buffer");
    cum_alpha_integral_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    int img_dim_buff[4];
    img_dim_buff[0] = 0;   img_dim_buff[2] = ni;
    img_dim_buff[1] = 0;   img_dim_buff[3] = nj;
    bocl_mem_sptr exp_img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(int)*4, "image dims");
    exp_img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

    std::size_t global_threads[] = {cl_ni,cl_nj};

    float theta_min = 0.0 ;
    float theta_max = 3*vnl_math::pi/4;

    float phi_min = -vnl_math::pi ;
    float phi_max =  vnl_math::pi ;

    float phi_inc = 0.0;
    float theta_inc = 0.0;
    // create all buffers
    auto* ray_origins = new cl_float[4*cl_ni*cl_nj];
    auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
    vgl_box_3d<double> scene_bbox = scene->bounding_box();
    vgl_point_3d<double> center(scene_bbox.centroid_x(),scene_bbox.centroid_y(),-0.5);


    phi_inc =  (phi_max-phi_min)/(float)(cl_ni)  ;
    theta_inc = (theta_max-theta_min)/ (float) (cl_nj) ;

#if 0 // unused
    double radius = 10*std::sqrt(scene_bbox.width()*scene_bbox.width()+
                                scene_bbox.depth()*scene_bbox.depth()+
                                scene_bbox.height()*scene_bbox.height());
#endif
    for (unsigned j = 0 ; j < cl_nj; j++)
    {
        for (unsigned i = 0 ; i < cl_ni; i++)
        {
            float theta = theta_min + (float)j*theta_inc ;
            float phi   = phi_min + (float)i*phi_inc ;
            float dirx = std::sin(theta)*std::cos(phi);
            float diry = std::sin(theta)*std::sin(phi);
            float dirz = std::cos(theta);
            ray_origins[(j*cl_ni+i)*4+0] = (float) (center.x());
            ray_origins[(j*cl_ni+i)*4+1] = (float) (center.y());
            ray_origins[(j*cl_ni+i)*4+2] = (float) (center.z());
            ray_origins[(j*cl_ni+i)*4+3] = (float) (0.0);

            ray_directions[(j*cl_ni+i)*4+0] = (float) (dirx);
            ray_directions[(j*cl_ni+i)*4+1] = (float) (diry);
            ray_directions[(j*cl_ni+i)*4+2] = (float) (dirz);
            ray_directions[(j*cl_ni+i)*4+3] = (float) (0.0);
        }
    }

    bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins, "ray_origins buffer");
    ray_o_buff->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );
    bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions, "ray_directions buffer");
    ray_d_buff->create_buffer( CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR );

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
    std::vector<boxm2_block_id> vis_order = scene->get_vis_order_from_pt(center);
    std::vector<boxm2_block_id>::iterator id;

    bocl_kernel * kern = kernels[0];
    std::cout<<"PASS 1: Accumulating alpha integral"<<std::endl;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        boxm2_block_metadata mdata = scene->get_block_metadata(*id);
        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk       = opencl_cache->get_block(scene,*id);
        bocl_mem* blk_info  = opencl_cache->loaded_block_info();
        bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(scene,*id);
        // data type string may contain an identifier so determine the buffer size
        transfer_time += (float) transfer.all();
        //3. SET args
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );
        kern->set_arg( cum_alpha_integral_image.ptr() );
        kern->set_arg( exp_img_dim.ptr());
        kern->set_arg( cl_output.ptr() );
        kern->set_arg( lookup.ptr() );
        //local tree , cumsum buffer, imindex buffer
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );
        kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) );
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_int) );
        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        clFinish(queue);
        gpu_time += kern->exec_time();
        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
    }

    kern = kernels[1];
    std::cout<<"PASS 2: Rendering Uncertainty Map"<<std::endl;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        boxm2_block_metadata mdata = scene->get_block_metadata(*id);
        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk       = opencl_cache->get_block(scene,*id);
        bocl_mem* blk_info  = opencl_cache->loaded_block_info();
        bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(scene,*id);
        int alphaTypeSize   = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        // data type string may contain an identifier so determine the buffer size
        bocl_mem* mog       = opencl_cache->get_data(scene,*id,data_type,alpha->num_bytes()/alphaTypeSize*apptypesize,true);
        transfer_time += (float) transfer.all();
        //3. SET args
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( mog );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );
        kern->set_arg( exp.ptr() );
        kern->set_arg( alpha_integral_image.ptr() );
        kern->set_arg( cum_alpha_integral_image.ptr() );
        kern->set_arg( exp_img_dim.ptr());
        kern->set_arg( cl_output.ptr() );
        kern->set_arg( lookup.ptr() );
        //local tree , cumsum buffer, imindex buffer
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );
        kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) );
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_int) );
        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        clFinish(queue);
        gpu_time += kern->exec_time();
        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
    }

    exp->read_to_buffer(queue);
    clFinish(queue);
    for (unsigned c=0;c<nj;c++)
        for (unsigned r=0;r<ni;r++)
            (* exp_image)(r,c)=buff[c*cl_ni+r];

    std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
    clReleaseCommandQueue(queue);
    vnl_float_4 x =boxm2_ocl_render_scene_uncertainty_map::compute_cubic_trajectory( phi_min,phi_max,theta_min,theta_max,exp_image);
    radial_image->set_size(cl_ni,cl_ni);

#if 0 // unused
    float radial_phi_inc =  (phi_max-phi_min)/((float)cl_ni/std::sqrt(2.0))  ;
#endif
    float radial_theta_inc = (theta_max-theta_min)/ ((float)cl_ni/std::sqrt(2.0))  ;
    float img_center_x = (float) cl_ni/2;
    float img_center_y = (float) cl_ni/2;

    for (unsigned j = 0 ; j < cl_ni; j++)
        for (unsigned i = 0 ; i < cl_ni; i++)
        {
            float r = std::sqrt((j-img_center_y)*(j-img_center_y)+(i-img_center_x)*(i-img_center_x));
            float theta = theta_min + radial_theta_inc * r ;
            float phi   = std::atan2((j-img_center_y),(i-img_center_x)) ;

            int prev_i = (int)std::floor((phi-phi_min)/phi_inc );
            int prev_j = (int)std::floor((theta-theta_min)/theta_inc);

            prev_i = prev_i < 0 ? 0 : prev_i > (int)cl_ni ? cl_ni : prev_i;
            prev_j = prev_j < 0 ? 0 : prev_j > (int)cl_ni ? cl_ni : prev_j;
            (*radial_image)(i,j,0) = (unsigned char) std::floor(255.0f*buff[prev_j*cl_ni+prev_i]);
            (*radial_image)(i,j,1) = (unsigned char) std::floor(255.0f*buff[prev_j*cl_ni+prev_i]);
            (*radial_image)(i,j,2) = (unsigned char) std::floor(255.0f*buff[prev_j*cl_ni+prev_i]);
        }


    std::vector<std::string> cams_used = boxm2_util::camfiles_from_directory( std::move(cam_dir_1) );
    std::vector<std::string> cams_unused = boxm2_util::camfiles_from_directory( std::move(cam_dir_2) );

    std::map<float,std::string>  view_uncertainty_map;
#if 0
    for (unsigned i = 0 ; i<cams_unused.size();i++)
    {
        vpgl_camera_double_sptr  incam = boxm2_util::camera_from_file( cams_unused[i] );
        vpgl_perspective_camera<double>* pcam = dynamic_cast<vpgl_perspective_camera<double>* > (incam.ptr());
        vgl_point_3d<double> cc= pcam->get_camera_center();
        vgl_vector_3d<double> ray = cc - center;

        float theta = std::acos( ray.z()/ray.length());
        float phi = std::atan2(ray.y(),ray.x());

        float r = std::floor((theta_max-theta_min)/radial_theta_inc/2);

        int k = std::floor(r*std::cos(phi) +img_center_x );
        int j = std::floor(r*std::sin(phi) +img_center_y );

        //: find corresponding point on the trajectory
        float y = x[0]+x[1]*phi+x[2]*phi*phi+x[3]*phi*phi*phi;

        float yr = std::floor((y-theta_min)/radial_theta_inc);
        unsigned yi = std::floor(yr*std::cos(phi) +img_center_x );
        unsigned yj = std::floor(yr*std::sin(phi) +img_center_y );

        view_uncertainty_map[(* radial_image)(yi,yj,0)] = cams_unused[i];
        if (yi<0 || yi>=cl_ni || yj <0 || yj >=cl_ni)
            continue;
        std::cout<<(int)(* radial_image)(yi,yj,0)<<' '<< cams_unused[i]<<std::endl;
        for (unsigned ki = k-4;ki<=k+4;ki++)
            for (unsigned ji = j-4;ji<=j+4;ji++)
            {
                //(* radial_image)(ki,ji,0) = 0;
                //(* radial_image)(ki,ji,1) = 0;
                //(* radial_image)(ki,ji,2) = 255;
            }
    }
    std::map<float,std::string>::iterator iter ;
    for (iter = view_uncertainty_map.begin(); iter != view_uncertainty_map.end(); iter++)
        std::cout<<iter->second<<' '<<iter->first<<std::endl;
#endif // 0
    for (const auto & i : cams_used)
    {
        vpgl_camera_double_sptr  incam = boxm2_util::camera_from_file( i );
        auto* pcam = dynamic_cast<vpgl_perspective_camera<double>* > (incam.ptr());
        vgl_point_3d<double> cc= pcam->get_camera_center();
        vgl_vector_3d<double> ray = cc - center;

        // float theta = std::acos(ray.z()/ray.length());
        float phi = std::atan2(ray.y(),ray.x());

        float r = std::floor((theta_max-theta_min)/radial_theta_inc/2);
        int k = (int)std::floor(r*std::cos(phi) +img_center_x );
        int j = (int)std::floor(r*std::sin(phi) +img_center_y );

        for (int ki = k-7;ki<=k+7;++ki)
        {
            if (ki<0 || ki>=(int)cl_ni)
                continue;
            for (int ji = j-7;ji<=j+7;++ji)
            {
                if (ji<0 || ji>=(int)cl_ni)
                    continue;
                (* radial_image)(ki,ji,0) = 0;
                (* radial_image)(ki,ji,1) = 255;
                (* radial_image)(ki,ji,2) = 0;
            }
        }
    }
#if 0
    std::cout<<" DRAW TRAJECTORY"<<std::endl;
    for (float phi = phi_min; phi <=phi_max+phi_inc; phi+=phi_inc)
    {
        float y = x[0]+x[1]*phi+x[2]*phi*phi+x[3]*phi*phi*phi;
        float r = std::floor((y-theta_min)/radial_theta_inc);
        int i = std::floor(r*std::cos(phi) +img_center_x );
        int j = std::floor(r*std::sin(phi) +img_center_y );
        if (i<0 || i>=cl_ni || j <0 || j >=cl_ni)
            continue;
        for (int ii = i-3;ii<=i+3;ii++)
            for (int jj = j-3;jj<=j+3;jj++)
            {
                (* radial_image)(ii,jj,0) = 255;
                (* radial_image)(ii,jj,1) = 0;
                (* radial_image)(ii,jj,2) = 0;
            }
    }
#endif // 0
    std::cout<<"Cubic coefficients for the trajectory are "<<x<<std::endl;
    return true;
}

vnl_float_4 boxm2_ocl_render_scene_uncertainty_map::compute_cubic_trajectory(float phi_min,float phi_max,
                                                                             float theta_min, float theta_max,
                                                                             vil_image_view<float> * uimg)
{
    float theta_inc = (theta_max- theta_min)/((float)uimg->nj());
    float phi_inc = (phi_max- phi_min)/((float)uimg->ni());

    vnl_float_4x4 A(0.0);
    vnl_float_4 b;
    b[0] = 0.0;
    b[1] = 0.0;
    b[2] = 0.0;
    b[3] = 0.0;
    for (unsigned i = 0; i < uimg->ni(); i++)
    {
        float phi = phi_min + phi_inc * (float)(i) ;
        float sum_uj = 0.0;
        float sum_thetaj_uj = 0.0;
        for (unsigned j = 0; j < uimg->nj(); j++)
        {
            float theta = theta_min + theta_inc * (float)(j) ;
            float u = 1 - (*uimg)(i,j);
            sum_uj += u*u;
            sum_thetaj_uj += theta*u*u;
        }
        vnl_float_4x4 temp_A(1.0);
        vnl_float_4   temp_b;
        temp_b[0] = 1.0;

        for (unsigned r = 0; r < 4; r ++)
        {
            for (unsigned c = 0; c < 4; c ++)
            {
                if (r==0 && c==0)
                    temp_A(r,c) = 1.0;
                else if (c == 0)
                    temp_A(r,c) = temp_A(r-1,c) * phi;
                else
                    temp_A(r,c) = temp_A(r,c-1) * phi;
            }
            if ( r != 0)
                temp_b[r] = temp_b[r-1] * phi ;
        }

        A += sum_uj* temp_A;
        b += sum_thetaj_uj*temp_b;
    }

    vnl_float_4 x = vnl_inverse(A)*b;

    return x;
}

//Returns vector of color update kernels (and caches them per device
std::vector<bocl_kernel*>& boxm2_ocl_render_scene_uncertainty_map::get_kernels(const bocl_device_sptr& device, const std::string& opts)
{
    // compile kernels if not already compiled
    std::string identifier = device->device_identifier() + opts;
    if (kernels_.find(identifier) != kernels_.end())
        return kernels_[identifier];

    //otherwise compile the kernels
    std::cout<<"=== boxm2_ocl_update_color_process::compiling kernels on device "<<identifier<<"==="<<std::endl;

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
    src_paths.push_back(source_dir + "bit/render_uncertainty_map.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

    //set kernel options
    const std::string& options = opts ;

    std::vector<bocl_kernel*> vec_kernels;
    //have kernel construct itself using the context and device
    auto * alpha_integral_kernel=new bocl_kernel();
    std::string alpha_integral_options =options  + "-D RENDER_ALPHA_INTEGRAL -D STEP_CELL=step_cell_alpha_integral(aux_args.alpha,data_ptr,d*linfo->block_len,aux_args.alpha_int)";

    alpha_integral_kernel->create_kernel( &device->context(),
                                          device->device_id(),
                                          src_paths,
                                          "render_alpha_integral",   //kernel name
                                          alpha_integral_options,              //options
                                          "boxm2 opencl compute Alpha Integrals along the ray"); //kernel identifier (for error checking)
    vec_kernels.push_back(alpha_integral_kernel);

    auto * render_kernel=new bocl_kernel();
    std::string render_options =options  + "-D RENDER_USING_ALPHA_INTEGRAL -D STEP_CELL=step_cell_render_using_alpha_intergal(aux_args.mog,aux_args.alpha,data_ptr,d*linfo->block_len,aux_args.alpha_int,aux_args.alpha_int_cum,aux_args.expint)";
    render_kernel->create_kernel( &device->context(),
                                  device->device_id(),
                                  src_paths,
                                  "render_using_alpha_integral",   //kernel name
                                  render_options,              //options
                                  "boxm2 render using alpha integral"); //kernel identifier (for error checking)
    vec_kernels.push_back(render_kernel);
    //create normalize image kernel
    std::vector<std::string> norm_src_paths;
    norm_src_paths.push_back(source_dir + "pixel_conversion.cl");
    norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
    auto * normalize_render_kernel=new bocl_kernel();

    normalize_render_kernel->create_kernel( &device->context(),
                                            device->device_id(),
                                            norm_src_paths,
                                            "normalize_render_kernel",   //kernel name
                                            options + "-D RENDER",              //options
                                            "normalize render kernel"); //kernel identifier (for error checking)

    vec_kernels.push_back(normalize_render_kernel);

    //store and return
    kernels_[identifier] = vec_kernels;
    return kernels_[identifier];
}
