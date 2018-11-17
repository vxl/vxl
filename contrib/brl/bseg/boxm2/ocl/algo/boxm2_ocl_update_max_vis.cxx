// This is brl/bseg/boxm2/ocl/algo/boxm2_ocl_update_max_vis.cxx
#include <fstream>
#include <iostream>
#include <algorithm>
#include "boxm2_ocl_update_max_vis.h"
//:
// \file
// \brief  A process for updating visibility and replacing by the maximum
//
// \author Vishal Jain
// \date Nov 11, 2013

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
#include <vpgl/vpgl_lvcs_sptr.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>
#include <vnl/vnl_numeric_traits.h>
#include <vil/vil_save.h>

//: Map of kernels should persist between process executions
std::map<std::string,std::vector<bocl_kernel*> > boxm2_ocl_update_max_vis::kernels_;

//Main public method, updates color model
bool boxm2_ocl_update_max_vis::update_max_vis(const boxm2_scene_sptr& scene,
                                      bocl_device_sptr         device,
                                      const boxm2_opencl_cache_sptr&  opencl_cache,
                                      vpgl_camera_double_sptr  cam,
                                      unsigned int ni,
                                      unsigned int nj,
                                      const vil_image_view_base_sptr& mask_sptr,
                                      float resnearfactor,
                                      float resfarfactor)
{
  enum {
    UPDATE_SEGLEN_VIS = 0,
    UPDATE_CELL_VIS   = 1
  };
  float transfer_time=0.0f;
  float gpu_time=0.0f;
  std::size_t local_threads[2]={8,8};
  std::size_t global_threads[2]={8,8};

  //cache size sanity check
  std::size_t binCache = opencl_cache.ptr()->bytes_in_cache();
  std::cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue( device->context(),
                                                 *(device->device_id()),
                                                 CL_QUEUE_PROFILING_ENABLE,
                                                 &status);
  bool use_mask = false;
  vil_image_view<unsigned char >* mask_map = nullptr;

  if ( mask_sptr->ni() == ni && mask_sptr->nj() == nj ) {
      std::cout<<"Update using mask."<<std::endl;
      use_mask = true;
      mask_map = dynamic_cast<vil_image_view<unsigned char> *>(mask_sptr.ptr());
      if (!mask_map) {
          std::cout<<"boxm2_update_process:: mask map is not an unsigned char map"<<std::endl;
          return false;
      }
  }

  if (status!=0)
    return false;
  std::string options = "";
  // compile the kernel if not already compiled
  std::vector<bocl_kernel*>& kernels = get_kernels(device, options);

  auto cl_ni=(unsigned)RoundUp(ni,(int)local_threads[0]);
  auto cl_nj=(unsigned)RoundUp(nj,(int)local_threads[1]);
  global_threads[0]=cl_ni;
  global_threads[1]=cl_nj;
  //set generic cam
  auto* ray_origins    = new cl_float[4*cl_ni*cl_nj];
  auto* ray_directions = new cl_float[4*cl_ni*cl_nj];

  bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins, "ray_origins buffer");
  bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions, "ray_directions buffer");

  if(cam->type_name() == "vpgl_geo_camera" )
  {
      vpgl_lvcs_sptr lvcs = new vpgl_lvcs(scene->lvcs());
      auto* geocam = static_cast<vpgl_geo_camera*>(cam.ptr());

      // crop relevant image data into a view
      vgl_box_3d<double> scene_bbox = scene->bounding_box();
      vgl_box_2d<double> proj_bbox;
      double u,v;

      geocam->project(scene_bbox.min_x(), scene_bbox.min_y(), scene_bbox.min_z(), u, v);
      proj_bbox.add(vgl_point_2d<double>(u,v));
      geocam->project(scene_bbox.max_x(), scene_bbox.max_y(), scene_bbox.max_z(), u, v);
      proj_bbox.add(vgl_point_2d<double>(u,v));

      std::cout<<"Scene BBox "<<scene_bbox<<" Proj Box "<<proj_bbox<<std::endl;
      int min_i = int(std::max(0.0, std::floor(proj_bbox.min_x())));
      int min_j = int(std::max(0.0, std::floor(proj_bbox.min_y())));
      int max_i = int(std::min(ni-1.0, std::ceil(proj_bbox.max_x())));
      int max_j = int(std::min(nj-1.0, std::ceil(proj_bbox.max_y())));

      if ((min_i > max_i) || (min_j > max_j)) {
          std::cerr << "Error: boxm2_ocl_ingest_buckeye_dem_process: No overlap between scene and DEM image.\n";
          return false;
      }
      // initialize ray origin buffer, first and last return buffers
      int count=0;
      for (unsigned int j=0;j<cl_nj;++j) {
          for (unsigned int i=0;i<cl_ni;++i) {
              if ( i < ni && j < nj ) {
                  int count4 = count*4;
                  double full_i = min_i + i + 0.25;
                  double full_j = min_j + j + 0.25;
                  double lat,lon, x, y, z_first, z_last;
                  double el_first = 0;
                  geocam->img_to_global(full_i, full_j,  lon, lat);
                  lvcs->global_to_local(lon,lat,el_first, vpgl_lvcs::wgs84, x, y, z_first);
                  // start rays slightly above maximum height of model
                  float z_origin = float(scene_bbox.max_z()) + 1.0f;
                  ray_origins[count4+0] = float(x);
                  ray_origins[count4+1] = float(y);
                  // ray will begin just above "top" of scene, with direction pointing in negative z direction
                  ray_origins[count4+2] = z_origin;
                  ray_origins[count4+3] = 0.0;
                  ray_directions[count4+0] = 0.0 ;
                  ray_directions[count4+1] = 0.0 ;
                  ray_directions[count4+2] = -1.0 ;
                  ray_directions[count4+3] = 0.0 ;
              }
              ++count;
          }
      }
      ray_o_buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
      ray_d_buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  }
  else
  {
      boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);
  }
  float tnearfar[2] = { 1e-8, 1e8} ;
  if(cam->type_name() == "vpgl_perspective_camera")
  {
      float f  = ((vpgl_perspective_camera<double> *)cam.ptr())->get_calibration().focal_length()*((vpgl_perspective_camera<double> *)cam.ptr())->get_calibration().x_scale();
      tnearfar[0] = f* scene->finest_resolution()/resnearfactor ;
      tnearfar[1] = f* scene->finest_resolution()/resfarfactor ;
      std::cout<<"Near and Far Clipping planes "<<tnearfar[0]<<" "<<tnearfar[1]<<std::endl;
  }
  bocl_mem_sptr tnearfar_mem_ptr = opencl_cache->alloc_mem(2*sizeof(float), tnearfar, "tnearfar  buffer");
  tnearfar_mem_ptr->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //Visibility, Preinf, Norm, and input image buffers
  auto* vis_buff = new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++)
    vis_buff[i]=1.0f;
  bocl_mem_sptr vis_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), vis_buff, "vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
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
  if(cam->type_name() == "vpgl_geo_camera" )
  {
      vis_order= scene->get_block_ids();
  }
  else if(cam->type_name() == "vpgl_perspective_camera")
      vis_order= scene->get_vis_blocks_opt((vpgl_perspective_camera<double>*)cam.ptr(),ni,nj);
  else
      vis_order= scene->get_vis_blocks(cam);

  int* mask_buff = new int[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++)
    mask_buff[i]=1;

  if (use_mask)
  {
      int count = 0;
      for (unsigned int j=0;j<cl_nj;++j) {
          for (unsigned int i=0;i<cl_ni;++i) {
              if ( i<mask_map->ni() && j<mask_map->nj() ) {
                  if ( (*mask_map)(i,j)==0 )
                      mask_buff[count] = 0;
                  else
                      mask_buff[count] = 1;
              }
              ++count;
          }
      }
  }
  //bocl_mem_sptr vis_image=new bocl_mem(device->context(),vis_buff,cl_ni*cl_nj*sizeof(float),"vis image buffer");
  bocl_mem_sptr mask_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), mask_buff, "vis image buffer");
  mask_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);



  for (unsigned int i=0; i<kernels.size(); ++i)
  {
    for (auto & id : vis_order)
    {
      //choose correct render kernel
      std::cout<<"Block id "<<id<<std::endl;
        boxm2_block_metadata mdata = scene->get_block_metadata(id);
      bocl_kernel* kern = kernels[i];
      //write the image values to the buffer
      vul_timer transfer;
      bocl_mem* blk       = opencl_cache->get_block(scene,id);
      bocl_mem* blk_info  = opencl_cache->loaded_block_info();
      bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(scene,id,0,true);
      auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
      int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
      info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
      blk_info->write_to_buffer((queue));
      //grab an appropriately sized AUX data buffer
      int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
      bocl_mem *aux0   = opencl_cache->get_data<BOXM2_AUX0>(scene,id, info_buffer->data_buffer_length*auxTypeSize,false);
      auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
      bocl_mem *aux2   = opencl_cache->get_data<BOXM2_AUX2>(scene,id, info_buffer->data_buffer_length*auxTypeSize,false);
      transfer_time += (float) transfer.all();
      if (i==UPDATE_SEGLEN_VIS)
      {
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( aux0 );
        kern->set_arg( aux2 );
        kern->set_arg( lookup.ptr() );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );
        kern->set_arg( tnearfar_mem_ptr.ptr() );
        kern->set_arg( img_dim.ptr() );
        kern->set_arg( vis_image.ptr() );
        kern->set_arg( mask_image.ptr() );
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
        aux0->read_to_buffer(queue);
        aux2->read_to_buffer(queue);
        vis_image->read_to_buffer(queue);
      }
      else if (i==UPDATE_CELL_VIS)
      {
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
        bocl_mem *aux0   = opencl_cache->get_data<BOXM2_AUX0>(scene,id, info_buffer->data_buffer_length*auxTypeSize,false);
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
        bocl_mem *aux2   = opencl_cache->get_data<BOXM2_AUX2>(scene,id, info_buffer->data_buffer_length*auxTypeSize,false);
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_VIS_SCORE>::prefix());
        bocl_mem *aux_vis_score   = opencl_cache->get_data<BOXM2_VIS_SCORE>(scene,id, info_buffer->data_buffer_length*auxTypeSize,false);
        bocl_mem* blk       = opencl_cache->get_block(scene,id);
        bocl_mem* blk_info  = opencl_cache->loaded_block_info();
        bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(scene,id,0,false);
        auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
        int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
        blk_info->write_to_buffer((queue));

        local_threads[0] = 64;
        local_threads[1] = 1 ;
        global_threads[0]=RoundUp(info_buffer->data_buffer_length,local_threads[0]);
        global_threads[1]=1;
        kern->set_arg( blk_info );
        kern->set_arg( aux_vis_score );
        kern->set_arg( aux0 );
        kern->set_arg( aux2 );
        kern->set_arg( cl_output.ptr() );
        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
          return false;
        gpu_time += kern->exec_time();
        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
        aux_vis_score->read_to_buffer(queue);
      }

      vis_image->read_to_buffer(queue);
      cl_output->read_to_buffer(queue);
      clFinish(queue);
    }
  }

  delete [] vis_buff;
  delete [] mask_buff;
  delete [] ray_origins;
  delete [] ray_directions;
  opencl_cache->unref_mem(vis_image.ptr());
  opencl_cache->unref_mem(mask_image.ptr());
  opencl_cache->unref_mem(ray_o_buff.ptr());
  opencl_cache->unref_mem(ray_d_buff.ptr());
  opencl_cache->unref_mem(tnearfar_mem_ptr.ptr());
  std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
  clReleaseCommandQueue(queue);
  return true;
}


//Returns vector of color update kernels (and caches them per device
std::vector<bocl_kernel*>& boxm2_ocl_update_max_vis::get_kernels(const bocl_device_sptr& device, const std::string& opts)
{
  // compile kernels if not already compiled
  std::string identifier = device->device_identifier() + opts;
  if (kernels_.find(identifier) != kernels_.end())
    return kernels_[identifier];

  //otherwise compile the kernels
  std::cout<<"=== boxm2_ocl_update_max_vis_process::compiling kernels on device "<<identifier<<"==="<<std::endl;

  std::vector<std::string> src_paths;
  std::string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "pixel_conversion.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "backproject.cl");
  src_paths.push_back(source_dir + "atomics_util.cl");
  src_paths.push_back(source_dir + "statistics_library_functions.cl");
  src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
  src_paths.push_back(source_dir + "fusion/update_kernels.cl");
  std::vector<std::string> non_ray_src = std::vector<std::string>(src_paths);
  src_paths.push_back(source_dir + "update_functors.cl");
  src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

  //compilation options
  const std::string& options = /*"-D ATOMIC_FLOAT " +*/ opts;

  //populate vector of kernels
  std::vector<bocl_kernel*> vec_kernels;

  //seg len pass
  auto* seg_len = new bocl_kernel();
  std::string seg_opts = options + " -D VIS_SEGLEN  -D STEP_CELL=step_cell_vis_seglen(aux_args,data_ptr,llid,d)";
  seg_len->create_kernel(&device->context(), device->device_id(), src_paths, "vis_seg_len_main", seg_opts, "fusion::vis_seg_len_main");
  vec_kernels.push_back(seg_len);

  //may need DIFF LIST OF SOURCES FOR
  auto* update = new bocl_kernel();
  std::string update_opts = options + " -D UPDATE_MAX_VIS_SCORE";
  update->create_kernel(&device->context(), device->device_id(), non_ray_src, "update_max_vis_score", update_opts, "fusion::update_max_vis_score");
  vec_kernels.push_back(update);

  //store and return
  kernels_[identifier] = vec_kernels;
  return kernels_[identifier];
}



//: Map of kernels should persist between process executions
std::map<std::string,std::vector<bocl_kernel*> > boxm2_ocl_update_cosine_angle::kernels_;

//Main public method, updates color model
bool boxm2_ocl_update_cosine_angle::update_cosine_angle(const boxm2_scene_sptr& scene,
                                                        bocl_device_sptr         device,
                                                        const boxm2_opencl_cache_sptr&  opencl_cache,
                                                        vpgl_camera_double_sptr  cam,
                                                        unsigned int ni,
                                                        unsigned int nj,
                                                        const vil_image_view_base_sptr& mask_sptr,
                                                        float resnearfactor,
                                                        float resfarfactor)
{
  enum {
    ACCUMLATE_PER_RAY = 0,
    ACCUMLATE_PER_VIEW   = 1
  };
  float transfer_time=0.0f;
  float gpu_time=0.0f;
  std::size_t local_threads[2]={8,8};
  std::size_t global_threads[2]={8,8};

  //cache size sanity check
  std::size_t binCache = opencl_cache.ptr()->bytes_in_cache();
  std::cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue( device->context(),
                                                 *(device->device_id()),
                                                 CL_QUEUE_PROFILING_ENABLE,
                                                 &status);
  bool use_mask = false;
  vil_image_view<unsigned char >* mask_map = nullptr;

  if ( mask_sptr->ni() == ni && mask_sptr->nj() == nj ) {
      std::cout<<"Update using mask."<<std::endl;
      use_mask = true;
      mask_map = dynamic_cast<vil_image_view<unsigned char> *>(mask_sptr.ptr());
      if (!mask_map) {
          std::cout<<"boxm2_update_process:: mask map is not an unsigned char map"<<std::endl;
          return false;
      }
  }

  if (status!=0)
    return false;
  std::string options = "";
  // compile the kernel if not already compiled
  std::vector<bocl_kernel*>& kernels = get_kernels(device, options);

  auto cl_ni=(unsigned)RoundUp(ni,(int)local_threads[0]);
  auto cl_nj=(unsigned)RoundUp(nj,(int)local_threads[1]);
  global_threads[0]=cl_ni;
  global_threads[1]=cl_nj;
  //set generic cam
  auto* ray_origins    = new cl_float[4*cl_ni*cl_nj];
  auto* ray_directions = new cl_float[4*cl_ni*cl_nj];

  bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins, "ray_origins buffer");
  bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions, "ray_directions buffer");

  if(cam->type_name() == "vpgl_geo_camera" )
  {
      vpgl_lvcs_sptr lvcs = new vpgl_lvcs(scene->lvcs());
      auto* geocam = static_cast<vpgl_geo_camera*>(cam.ptr());

      // crop relevant image data into a view
      vgl_box_3d<double> scene_bbox = scene->bounding_box();
      vgl_box_2d<double> proj_bbox;
      double u,v;

      geocam->project(scene_bbox.min_x(), scene_bbox.min_y(), scene_bbox.min_z(), u, v);
      proj_bbox.add(vgl_point_2d<double>(u,v));
      geocam->project(scene_bbox.max_x(), scene_bbox.max_y(), scene_bbox.max_z(), u, v);
      proj_bbox.add(vgl_point_2d<double>(u,v));

      std::cout<<"Scene BBox "<<scene_bbox<<" Proj Box "<<proj_bbox<<std::endl;
      int min_i = int(std::max(0.0, std::floor(proj_bbox.min_x())));
      int min_j = int(std::max(0.0, std::floor(proj_bbox.min_y())));
      int max_i = int(std::min(ni-1.0, std::ceil(proj_bbox.max_x())));
      int max_j = int(std::min(nj-1.0, std::ceil(proj_bbox.max_y())));

      if ((min_i > max_i) || (min_j > max_j)) {
          std::cerr << "Error: boxm2_ocl_ingest_buckeye_dem_process: No overlap between scene and DEM image.\n";
          return false;
      }

      // initialize ray origin buffer, first and last return buffers
      int count=0;

      for (unsigned int j=0;j<cl_nj;++j) {
          for (unsigned int i=0;i<cl_ni;++i) {
              if ( i < ni && j < nj ) {
                  int count4 = count*4;
                  double full_i = min_i + i + 0.25;
                  double full_j = min_j + j + 0.25;
                  double lat,lon, x, y, z_first, z_last;
                  double el_first = 0;
                  geocam->img_to_global(full_i, full_j,  lon, lat);
                  lvcs->global_to_local(lon,lat,el_first, vpgl_lvcs::wgs84, x, y, z_first);

                  // start rays slightly above maximum height of model
                  float z_origin = float(scene_bbox.max_z()) + 1.0f;
                  ray_origins[count4+0] = float(x);
                  ray_origins[count4+1] = float(y);
                  // ray will begin just above "top" of scene, with direction pointing in negative z direction
                  ray_origins[count4+2] = z_origin;
                  ray_origins[count4+3] = 0.0;
                  ray_directions[count4+0] = 0.0 ;
                  ray_directions[count4+1] = 0.0 ;
                  ray_directions[count4+2] = -1.0 ;
                  ray_directions[count4+3] = 0.0 ;
              }
              ++count;
          }
      }

      ray_o_buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
      ray_d_buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  }
  else
  {
      boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);
  }
  float tnearfar[2] = { 1e-8, 1e8} ;
  if(cam->type_name() == "vpgl_perspective_camera")
  {
      float f  = ((vpgl_perspective_camera<double> *)cam.ptr())->get_calibration().focal_length()*((vpgl_perspective_camera<double> *)cam.ptr())->get_calibration().x_scale();
      tnearfar[0] = f* scene->finest_resolution()/resnearfactor ;
      tnearfar[1] = f* scene->finest_resolution()/resfarfactor ;
      std::cout<<"Near and Far Clipping planes "<<tnearfar[0]<<" "<<tnearfar[1]<<std::endl;
  }
  bocl_mem_sptr tnearfar_mem_ptr = opencl_cache->alloc_mem(2*sizeof(float), tnearfar, "tnearfar  buffer");
  tnearfar_mem_ptr->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //Visibility, Preinf, Norm, and input image buffers
  auto* vis_buff = new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++)
    vis_buff[i]=1.0f;
  bocl_mem_sptr vis_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), vis_buff, "vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
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
  if(cam->type_name() == "vpgl_geo_camera" )
      vis_order= scene->get_block_ids();
  else if(cam->type_name() == "vpgl_perspective_camera")
      vis_order= scene->get_vis_blocks_opt((vpgl_perspective_camera<double>*)cam.ptr(),ni,nj);
  else
      vis_order= scene->get_vis_blocks(cam);

  int* mask_buff = new int[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++)
    mask_buff[i]=1;

  if (use_mask)
  {
      int count = 0;
      for (unsigned int j=0;j<cl_nj;++j) {
          for (unsigned int i=0;i<cl_ni;++i) {
              if ( i<mask_map->ni() && j<mask_map->nj() ) {
                  if ( (*mask_map)(i,j)==0 )
                      mask_buff[count] = 0;
                  else
                      mask_buff[count] = 1;
              }
              ++count;
          }
      }
  }
  //bocl_mem_sptr vis_image=new bocl_mem(device->context(),vis_buff,cl_ni*cl_nj*sizeof(float),"vis image buffer");
  bocl_mem_sptr mask_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), mask_buff, "vis image buffer");
  mask_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  for (unsigned int i=0; i<kernels.size(); ++i)
  {
    for (auto & id : vis_order)
    {
      //choose correct render kernel
      std::cout<<"Block id "<<id<<std::endl;
      boxm2_block_metadata mdata = scene->get_block_metadata(id);
      bocl_kernel* kern = kernels[i];
      //write the image values to the buffer
      vul_timer transfer;
      bocl_mem* blk       = opencl_cache->get_block(scene,id);
      bocl_mem* blk_info  = opencl_cache->loaded_block_info();
      bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(scene,id,0,true);
      auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
      int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
      info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
      blk_info->write_to_buffer((queue));
      //grab an appropriately sized AUX data buffer
      int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
      bocl_mem *aux0   = opencl_cache->get_data<BOXM2_AUX0>(scene,id, info_buffer->data_buffer_length*auxTypeSize,false);
      auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
      bocl_mem *aux1   = opencl_cache->get_data<BOXM2_AUX1>(scene,id, info_buffer->data_buffer_length*auxTypeSize,false);
      auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
      bocl_mem *aux2   = opencl_cache->get_data<BOXM2_AUX2>(scene,id, info_buffer->data_buffer_length*auxTypeSize,false);

      int normalTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NORMAL>::prefix());
      bocl_mem *normal_array   = opencl_cache->get_data<BOXM2_NORMAL>(scene,id, info_buffer->data_buffer_length*normalTypeSize,true);
      transfer_time += (float) transfer.all();
      if (i==ACCUMLATE_PER_RAY)
      {
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( normal_array );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( aux2 );
        kern->set_arg( lookup.ptr() );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );
        kern->set_arg( tnearfar_mem_ptr.ptr() );
        kern->set_arg( img_dim.ptr() );
        kern->set_arg( vis_image.ptr() );
        kern->set_arg( mask_image.ptr() );
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
        aux0->read_to_buffer(queue);
        aux2->read_to_buffer(queue);
        vis_image->read_to_buffer(queue);
        opencl_cache->deep_remove_data(scene,id,boxm2_data_traits<BOXM2_AUX2>::prefix());
      }
      else if (i==ACCUMLATE_PER_VIEW)
      {
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
        bocl_mem *aux0   = opencl_cache->get_data<BOXM2_AUX0>(scene,id, info_buffer->data_buffer_length*auxTypeSize,false);
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
        bocl_mem *aux1   = opencl_cache->get_data<BOXM2_AUX1>(scene,id, info_buffer->data_buffer_length*auxTypeSize,false);
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
        bocl_mem *aux2   = opencl_cache->get_data<BOXM2_AUX2>(scene,id, info_buffer->data_buffer_length*auxTypeSize,false);

        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_EXPECTATION>::prefix());
        bocl_mem *expectation_array   = opencl_cache->get_data<BOXM2_EXPECTATION>(scene,id, info_buffer->data_buffer_length*auxTypeSize,false,"normaldot");

        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
        bocl_mem *vis_array   = opencl_cache->get_data<BOXM2_AUX3>(scene,id, info_buffer->data_buffer_length*auxTypeSize,false,"normaldot");

        bocl_mem* blk       = opencl_cache->get_block(scene,id);
        bocl_mem* blk_info  = opencl_cache->loaded_block_info();
        bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(scene,id,0,false);
        auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
        int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
        blk_info->write_to_buffer((queue));

        local_threads[0] = 64;
        local_threads[1] = 1 ;
        global_threads[0]=RoundUp(info_buffer->data_buffer_length,local_threads[0]);
        global_threads[1]=1;
        kern->set_arg( blk_info );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( aux2 );
        kern->set_arg( expectation_array );
        kern->set_arg( vis_array );
        kern->set_arg( cl_output.ptr() );
        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
          return false;
        gpu_time += kern->exec_time();
        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
        expectation_array->read_to_buffer(queue);
        vis_array->read_to_buffer(queue);
      }

      vis_image->read_to_buffer(queue);
      cl_output->read_to_buffer(queue);
      clFinish(queue);
    }
  }

  delete [] vis_buff;
  delete [] mask_buff;
  delete [] ray_origins;
  delete [] ray_directions;
  opencl_cache->unref_mem(vis_image.ptr());
  opencl_cache->unref_mem(mask_image.ptr());
  opencl_cache->unref_mem(ray_o_buff.ptr());
  opencl_cache->unref_mem(ray_d_buff.ptr());
  opencl_cache->unref_mem(tnearfar_mem_ptr.ptr());
  std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
  clReleaseCommandQueue(queue);
  return true;
}


//Returns vector of color update kernels (and caches them per device
std::vector<bocl_kernel*>& boxm2_ocl_update_cosine_angle::get_kernels(const bocl_device_sptr& device, const std::string& opts)
{
  // compile kernels if not already compiled
  std::string identifier = device->device_identifier() + opts;
  if (kernels_.find(identifier) != kernels_.end())
    return kernels_[identifier];

  //otherwise compile the kernels
  std::cout<<"=== boxm2_ocl_update_cosine_angle::compiling kernels on device "<<identifier<<"==="<<std::endl;

  std::vector<std::string> src_paths;
  std::string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "pixel_conversion.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "backproject.cl");
  src_paths.push_back(source_dir + "atomics_util.cl");
  src_paths.push_back(source_dir + "statistics_library_functions.cl");
  src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
  src_paths.push_back(source_dir + "fusion/update_kernels.cl");
  std::vector<std::string> non_ray_src = std::vector<std::string>(src_paths);
  src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

  //compilation options
  const std::string& options = /*"-D ATOMIC_FLOAT " +*/ opts;

  //populate vector of kernels
  std::vector<bocl_kernel*> vec_kernels;

  //seg len pass
  auto* seg_len = new bocl_kernel();
  std::string seg_opts = options + " -D VIEW_NORMAL_DOT  -D STEP_CELL=step_cell_view_normal_dot(aux_args,data_ptr,llid,d)";
  seg_len->create_kernel(&device->context(), device->device_id(), src_paths, "view_normal_dot_main", seg_opts, "fusion::vis_seg_len_main");
  vec_kernels.push_back(seg_len);

  //may need DIFF LIST OF SOURCES FOR
  auto* update = new bocl_kernel();
  std::string update_opts = options + " -D UPDATE_AVG_VIEW_NORMAL_DOT";
  update->create_kernel(&device->context(), device->device_id(), non_ray_src, "update_avg_view_normal_dot", update_opts, "fusion::update_avg_view_normal_dot");
  vec_kernels.push_back(update);

  //store and return
  kernels_[identifier] = vec_kernels;
  return kernels_[identifier];
}

//: Map of kernels should persist between process executions
std::map<std::string,std::vector<bocl_kernel*> > boxm2_ocl_update_surface_density::kernels_;

//Main public method, updates color model
bool boxm2_ocl_update_surface_density::update_surface_density(const boxm2_scene_sptr& scene,
                                                        bocl_device_sptr         device,
                                                        const boxm2_opencl_cache_sptr&  opencl_cache,
                                                        vpgl_camera_double_sptr  cam,
                                                        unsigned int ni,
                                                        unsigned int nj,
                                                        vil_image_view<float> & exp_depth_img,
                                                        vil_image_view<float> & std_depth_img,
                                                        float resnearfactor,
                                                        float resfarfactor)
{
  enum {
    ACCUMLATE_PER_RAY = 0,
    ACCUMLATE_PER_VIEW   = 1
  };
  float transfer_time=0.0f;
  float gpu_time=0.0f;
  std::size_t local_threads[2]={8,8};
  std::size_t global_threads[2]={8,8};

  //cache size sanity check
  std::size_t binCache = opencl_cache.ptr()->bytes_in_cache();
  std::cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue( device->context(),
                                                 *(device->device_id()),
                                                 CL_QUEUE_PROFILING_ENABLE,
                                                 &status);

  if (status!=0)
    return false;
  std::string options = "";
  // compile the kernel if not already compiled
  std::vector<bocl_kernel*>& kernels = get_kernels(device, options);

  auto cl_ni=(unsigned)RoundUp(ni,(int)local_threads[0]);
  auto cl_nj=(unsigned)RoundUp(nj,(int)local_threads[1]);
  global_threads[0]=cl_ni;
  global_threads[1]=cl_nj;
  //set generic cam
  auto* ray_origins    = new cl_float[4*cl_ni*cl_nj];
  auto* ray_directions = new cl_float[4*cl_ni*cl_nj];

  bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_origins, "ray_origins buffer");
  bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(cl_float4), ray_directions, "ray_directions buffer");

  if(cam->type_name() == "vpgl_geo_camera" )
  {
      vpgl_lvcs_sptr lvcs = new vpgl_lvcs(scene->lvcs());
      auto* geocam = static_cast<vpgl_geo_camera*>(cam.ptr());
      // crop relevant image data into a view
      vgl_box_3d<double> scene_bbox = scene->bounding_box();
      vgl_box_2d<double> proj_bbox;
      double u,v;
      geocam->project(scene_bbox.min_x(), scene_bbox.min_y(), scene_bbox.min_z(), u, v);
      proj_bbox.add(vgl_point_2d<double>(u,v));
      geocam->project(scene_bbox.max_x(), scene_bbox.max_y(), scene_bbox.max_z(), u, v);
      proj_bbox.add(vgl_point_2d<double>(u,v));
      std::cout<<"Scene BBox "<<scene_bbox<<" Proj Box "<<proj_bbox<<std::endl;
      int min_i = int(std::max(0.0, std::floor(proj_bbox.min_x())));
      int min_j = int(std::max(0.0, std::floor(proj_bbox.min_y())));
      int max_i = int(std::min(ni-1.0, std::ceil(proj_bbox.max_x())));
      int max_j = int(std::min(nj-1.0, std::ceil(proj_bbox.max_y())));
      if ((min_i > max_i) || (min_j > max_j)) {
          std::cerr << "Error: boxm2_ocl_ingest_buckeye_dem_process: No overlap between scene and DEM image.\n";
          return false;
      }
      // initialize ray origin buffer, first and last return buffers
      int count=0;
      for (unsigned int j=0;j<cl_nj;++j) {
          for (unsigned int i=0;i<cl_ni;++i) {
              if ( i < ni && j < nj ) {
                  int count4 = count*4;
                  double full_i = min_i + i + 0.25;
                  double full_j = min_j + j + 0.25;
                  double lat,lon, x, y, z_first, z_last;
                  double el_first = 0;
                  geocam->img_to_global(full_i, full_j,  lon, lat);
                  lvcs->global_to_local(lon,lat,el_first, vpgl_lvcs::wgs84, x, y, z_first);
                  // start rays slightly above maximum height of model
                  float z_origin = float(scene_bbox.max_z()) + 1.0f;
                  ray_origins[count4+0] = float(x);
                  ray_origins[count4+1] = float(y);
                  // ray will begin just above "top" of scene, with direction pointing in negative z direction
                  ray_origins[count4+2] = z_origin;
                  ray_origins[count4+3] = 0.0;
                  ray_directions[count4+0] = 0.0 ;
                  ray_directions[count4+1] = 0.0 ;
                  ray_directions[count4+2] = -1.0 ;
                  ray_directions[count4+3] = 0.0 ;
              }
              ++count;
          }
      }
      ray_o_buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
      ray_d_buff->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  }
  else
  {
      boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);
  }
  float tnearfar[2] = { 1e-8, 1e8} ;
  if(cam->type_name() == "vpgl_perspective_camera")
  {
      float f  = ((vpgl_perspective_camera<double> *)cam.ptr())->get_calibration().focal_length()*((vpgl_perspective_camera<double> *)cam.ptr())->get_calibration().x_scale();
      tnearfar[0] = f* scene->finest_resolution()/resnearfactor ;
      tnearfar[1] = f* scene->finest_resolution()/resfarfactor ;
      std::cout<<"Near and Far Clipping planes "<<tnearfar[0]<<" "<<tnearfar[1]<<std::endl;
  }
  bocl_mem_sptr tnearfar_mem_ptr = opencl_cache->alloc_mem(2*sizeof(float), tnearfar, "tnearfar  buffer");
  tnearfar_mem_ptr->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //Visibility, Preinf, Norm, and input image buffers
  auto* vis_buff = new float[cl_ni*cl_nj];
  auto* exp_depth_buf = new float[cl_ni*cl_nj];
  auto* std_depth_buf = new float[cl_ni*cl_nj];
  int count  = 0;
  for (unsigned int j=0;j<cl_nj;++j)
      for (unsigned int i=0;i<cl_ni;++i)
          if ( i<exp_depth_img.ni() && j<exp_depth_img.nj() )
          {
              exp_depth_buf[count] = exp_depth_img(i,j);
              std_depth_buf[count++] = std_depth_img(i,j);
          }

  for (unsigned i=0;i<cl_ni*cl_nj;i++)
    vis_buff[i]=1.0f;
  bocl_mem_sptr vis_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), vis_buff, "vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bocl_mem_sptr cl_exp_depth_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), exp_depth_buf, "exp depth buffer");
  cl_exp_depth_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bocl_mem_sptr cl_std_depth_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), std_depth_buf, "std depth buffer");
  cl_std_depth_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
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
  if(cam->type_name() == "vpgl_geo_camera" )
  {
      vis_order= scene->get_block_ids(); // order does not matter for a top down orthographic camera  and axis aligned blocks
  }
  else if(cam->type_name() == "vpgl_perspective_camera")
      vis_order= scene->get_vis_blocks_opt((vpgl_perspective_camera<double>*)cam.ptr(),ni,nj);
  else
      vis_order= scene->get_vis_blocks(cam);

  for (unsigned int i=0; i<kernels.size(); ++i)
  {
    for (auto & id : vis_order)
    {
      //choose correct render kernel
      std::cout<<"Block id "<<id<<std::endl;
      boxm2_block_metadata mdata = scene->get_block_metadata(id);
      bocl_kernel* kern = kernels[i];
      //write the image values to the buffer
      vul_timer transfer;
      bocl_mem* blk       = opencl_cache->get_block(scene,id);
      bocl_mem* blk_info  = opencl_cache->loaded_block_info();
      bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(scene,id,0,true);
      auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
      int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
      info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
      blk_info->write_to_buffer((queue));
      //grab an appropriately sized AUX data buffer
      int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
      bocl_mem *aux0   = opencl_cache->get_data<BOXM2_AUX0>(scene,id, info_buffer->data_buffer_length*auxTypeSize,false);
      auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
      bocl_mem *aux1   = opencl_cache->get_data<BOXM2_AUX1>(scene,id, info_buffer->data_buffer_length*auxTypeSize,false);
      auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
      bocl_mem *aux2   = opencl_cache->get_data<BOXM2_AUX2>(scene,id, info_buffer->data_buffer_length*auxTypeSize,false);
      int normalTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NORMAL>::prefix());
      bocl_mem *normal_array   = opencl_cache->get_data<BOXM2_NORMAL>(scene,id, info_buffer->data_buffer_length*normalTypeSize,true);
      transfer_time += (float) transfer.all();
      if (i==ACCUMLATE_PER_RAY)
      {
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( aux2 );
        kern->set_arg( lookup.ptr() );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );
        kern->set_arg( cl_exp_depth_image.ptr() );
        kern->set_arg( cl_std_depth_image.ptr() );
        kern->set_arg( tnearfar_mem_ptr.ptr() );
        kern->set_arg( img_dim.ptr() );
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
        aux0->read_to_buffer(queue);
        aux1->read_to_buffer(queue);
        aux2->read_to_buffer(queue);
        vis_image->read_to_buffer(queue);
      }
      else if (i==ACCUMLATE_PER_VIEW)
      {
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
        bocl_mem *aux0   = opencl_cache->get_data<BOXM2_AUX0>(scene,id, info_buffer->data_buffer_length*auxTypeSize,false);
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
        bocl_mem *aux1   = opencl_cache->get_data<BOXM2_AUX1>(scene,id, info_buffer->data_buffer_length*auxTypeSize,false);
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
        bocl_mem *aux2   = opencl_cache->get_data<BOXM2_AUX2>(scene,id, info_buffer->data_buffer_length*auxTypeSize,false);
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_EXPECTATION>::prefix());
        bocl_mem *density_array   = opencl_cache->get_data<BOXM2_EXPECTATION>(scene,id, info_buffer->data_buffer_length*auxTypeSize,false,"surfacedensity");
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
        bocl_mem *vis_array   = opencl_cache->get_data<BOXM2_AUX3>(scene,id, info_buffer->data_buffer_length*auxTypeSize,false,"surfacedensity");
        bocl_mem* blk       = opencl_cache->get_block(scene,id);
        bocl_mem* blk_info  = opencl_cache->loaded_block_info();
        bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(scene,id,0,false);
        auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
        int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
        blk_info->write_to_buffer((queue));

        local_threads[0] = 64;
        local_threads[1] = 1 ;
        global_threads[0]=RoundUp(info_buffer->data_buffer_length,local_threads[0]);
        global_threads[1]=1;
        kern->set_arg( blk_info );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( aux2 );
        kern->set_arg( density_array );
        kern->set_arg( vis_array );
        kern->set_arg( cl_output.ptr() );
        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
          return false;
        gpu_time += kern->exec_time();
        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
        density_array->read_to_buffer(queue);
        vis_array->read_to_buffer(queue);
        opencl_cache->deep_remove_data(scene,id,boxm2_data_traits<BOXM2_AUX0>::prefix());
        opencl_cache->deep_remove_data(scene,id,boxm2_data_traits<BOXM2_AUX1>::prefix());
        opencl_cache->deep_remove_data(scene,id,boxm2_data_traits<BOXM2_AUX2>::prefix());
      }

      vis_image->read_to_buffer(queue);
      cl_output->read_to_buffer(queue);
      clFinish(queue);
    }
  }
  delete [] exp_depth_buf;
  delete [] std_depth_buf;
  delete [] vis_buff;
  delete [] ray_origins;
  delete [] ray_directions;
  opencl_cache->unref_mem(vis_image.ptr());
  opencl_cache->unref_mem(cl_exp_depth_image.ptr());
  opencl_cache->unref_mem(cl_std_depth_image.ptr());
  opencl_cache->unref_mem(ray_o_buff.ptr());
  opencl_cache->unref_mem(ray_d_buff.ptr());
  opencl_cache->unref_mem(tnearfar_mem_ptr.ptr());
  std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
  clReleaseCommandQueue(queue);
  return true;
}


//Returns vector of color update kernels (and caches them per device
std::vector<bocl_kernel*>& boxm2_ocl_update_surface_density::get_kernels(const bocl_device_sptr& device, const std::string& opts)
{
  // compile kernels if not already compiled
  std::string identifier = device->device_identifier() + opts;
  if (kernels_.find(identifier) != kernels_.end())
    return kernels_[identifier];

  //otherwise compile the kernels
  std::cout<<"=== boxm2_ocl_update_cosine_angle::compiling kernels on device "<<identifier<<"==="<<std::endl;

  std::vector<std::string> src_paths;
  std::string source_dir = boxm2_ocl_util::ocl_src_root();
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "pixel_conversion.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "backproject.cl");
  src_paths.push_back(source_dir + "atomics_util.cl");
  src_paths.push_back(source_dir + "statistics_library_functions.cl");
  src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
  src_paths.push_back(source_dir + "fusion/update_kernels.cl");
  std::vector<std::string> non_ray_src = std::vector<std::string>(src_paths);
  src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

  //compilation options
  const std::string& options = /*"-D ATOMIC_FLOAT " +*/ opts;

  //populate vector of kernels
  std::vector<bocl_kernel*> vec_kernels;

  //seg len pass
  auto* seg_len = new bocl_kernel();
  std::string seg_opts = options + " -D ACCUMULATE_SURFACE_DENSITY  -D STEP_CELL=step_cell_surface_density(tblock,aux_args,data_ptr,llid,d)";
  seg_len->create_kernel(&device->context(), device->device_id(), src_paths, "accumulate_surface_density_main", seg_opts, "fusion::accumulate_surface_density_main");
  vec_kernels.push_back(seg_len);

  //may need DIFF LIST OF SOURCES FOR
  auto* update = new bocl_kernel();
  std::string update_opts = options + " -D UPDATE_AVG_SURFACE_DENSITY";
  update->create_kernel(&device->context(), device->device_id(), non_ray_src, "update_avg_surface_density", update_opts, "fusion::update_avg_surface_density");
  vec_kernels.push_back(update);

  //store and return
  kernels_[identifier] = vec_kernels;
  return kernels_[identifier];
}
