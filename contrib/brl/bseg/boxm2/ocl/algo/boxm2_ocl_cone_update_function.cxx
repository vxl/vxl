#include "boxm2_ocl_cone_update_function.h"
//
#include <vul/vul_timer.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <vsph/vsph_camera_bounds.h>
#include <vgl/vgl_ray_3d.h>
#include <boct/boct_bit_tree.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>


//render_cone_expected_image - pretty much the same as above but one of the local
//memory arguments is expanded
float boxm2_ocl_adaptive_cone_update( boxm2_scene_sptr & scene,
                                      bocl_device_sptr & device,
                                      boxm2_opencl_cache_sptr & opencl_cache,
                                      std::vector<bocl_kernel*> kernels,
                                      cl_command_queue& queue,
                                      const std::string& data_type,
                                      const std::string& num_obs_type,
                                      vpgl_camera_double_sptr cam ,
                                      const vil_image_view_base_sptr& in_img,
                                      unsigned int roi_ni0,
                                      unsigned int roi_nj0)
{
  enum {
    UPDATE_PASSONE = 0,
    UPDATE_BAYES= 1,
    UPDATE_CELL = 2,
  };
  float transfer_time=0.0f;
  float gpu_time=0.0f;
  std::size_t local_threads[2]={8,8};

  //camera check
  if (cam->type_name()!= "vpgl_perspective_camera" && cam->type_name() != "vpgl_generic_camera" ) {
    std::cout<<"Cannot render with camera of type "<<cam->type_name()<<std::endl;
    return 0.0f;
  }

  //grab input image, establish cl_ni, cl_nj (so global size is divisible by local size)
  vil_image_view_base_sptr float_img = boxm2_util::prepare_input_image(in_img);
  auto* img_view = static_cast<vil_image_view<float>* >(float_img.ptr());
  auto cl_ni=(unsigned)RoundUp(img_view->ni(),(int)local_threads[0]);
  auto cl_nj=(unsigned)RoundUp(img_view->nj(),(int)local_threads[1]);
  std::size_t global_threads[2] = {cl_ni, cl_nj};

  //set generic cam and get visible block order
  auto* ray_origins = new cl_float[4*cl_ni*cl_nj];
  auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
  bocl_mem_sptr ray_o_buff = new bocl_mem(device->context(), ray_origins, cl_ni*cl_nj * sizeof(cl_float4) , "ray_origins buffer");
  bocl_mem_sptr ray_d_buff = new bocl_mem(device->context(), ray_directions,  cl_ni*cl_nj * sizeof(cl_float4), "ray_directions buffer");
  boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);
  ray_d_buff->read_to_buffer(queue);

  ////////////////////////////////////////////////////////////////////////////////
  //gotta do this the old fashion way for debuggin....
  std::cout<<"  DEBUG: COMPUTING CONE HALF ANGLES ON CPU"<<std::endl;
  int cnt = 0;
  for (unsigned j=0;j<cl_nj;++j) {
    for (unsigned i=0;i<cl_ni;++i) {
      //calculate ray and ray angles at pixel ij
      vgl_ray_3d<double> ray_ij; //= cam->ray(i,j);
      double cone_half_angle, solid_angle;
      auto* pcam = (vpgl_perspective_camera<double>*) cam.ptr();
      vsph_camera_bounds::pixel_solid_angle(*pcam, i, j, ray_ij, cone_half_angle, solid_angle);
      ray_directions[4*cnt+3] = (cl_float) cone_half_angle;
      cnt++;
    }
  }
  ray_d_buff->write_to_buffer(queue);
  std::cout<<"opencl Half angle: "
          <<ray_directions[0]<<','
          <<ray_directions[1]<<','
          <<ray_directions[2]<<','
          <<ray_directions[3]<<'\n'
          <<"  DEBUG: FINISHED CONE HALF ANGLES ON CPU"<<std::endl;
  ////////////////////////////////////////////////////////////////////////////////

  //Visibility, Preinf, Norm, and input image buffers
  auto* vis_buff = new float[cl_ni*cl_nj];
  auto* pre_buff = new float[cl_ni*cl_nj];
  auto* norm_buff = new float[cl_ni*cl_nj];
  auto* input_buff=new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++)
  {
    vis_buff[i]=1.0f;
    pre_buff[i]=0.0f;
    norm_buff[i]=0.0f;
  }
  int count=0;
  for (unsigned int j=0;j<cl_nj;++j) {
    for (unsigned int i=0;i<cl_ni;++i) {
      input_buff[count] = 0.0f;
      if (i<img_view->ni() && j< img_view->nj())
        input_buff[count]=(*img_view)(i,j);
      ++count;
    }
  }
  bocl_mem_sptr in_image=new bocl_mem(device->context(),input_buff,cl_ni*cl_nj*sizeof(float),"input image buffer");
  in_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bocl_mem_sptr vis_image=new bocl_mem(device->context(),vis_buff,cl_ni*cl_nj*sizeof(float),"vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bocl_mem_sptr pre_image=new bocl_mem(device->context(),pre_buff,cl_ni*cl_nj*sizeof(float),"pre image buffer");
  pre_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bocl_mem_sptr norm_image=new bocl_mem(device->context(),norm_buff,cl_ni*cl_nj*sizeof(float),"pre image buffer");
  norm_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // Image Dimensions
  unsigned int img_dim_buff[] = { roi_ni0, roi_nj0, img_view->ni(), img_view->nj() };
  bocl_mem_sptr img_dim = new bocl_mem(device->context(), img_dim_buff, sizeof(unsigned int)*4, "image dims");
  img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // Output Array
  float output_arr[100] = {0};
  bocl_mem_sptr  cl_output=new bocl_mem(device->context(), output_arr, sizeof(float)*100, "output buffer");
  cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  // center buffer
  bocl_mem_sptr centerX = new bocl_mem(device->context(), boct_bit_tree::centerX, sizeof(cl_float)*585, "centersX lookup buffer");
  bocl_mem_sptr centerY = new bocl_mem(device->context(), boct_bit_tree::centerY, sizeof(cl_float)*585, "centersY lookup buffer");
  bocl_mem_sptr centerZ = new bocl_mem(device->context(), boct_bit_tree::centerZ, sizeof(cl_float)*585, "centersZ lookup buffer");
  centerX->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  centerY->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  centerZ->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  // set arguments
  std::vector<boxm2_block_id> vis_order = scene->get_vis_blocks(cam);
  std::vector<boxm2_block_id>::iterator id;
  for (unsigned int i=0; i<kernels.size(); ++i)
  {
    std::cout<<"Running kernel "<<i<<std::endl;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        //choose correct render kernel
        boxm2_block_metadata mdata = scene->get_block_metadata(*id);
        bocl_kernel* kern =  kernels[i];

        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk       = opencl_cache->get_block(scene, *id);
        bocl_mem* blk_info  = opencl_cache->loaded_block_info();
        bocl_mem* alpha     = opencl_cache->get_data<BOXM2_GAMMA>(scene, *id,0,false);
        auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
        int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_GAMMA>::prefix());
        // check for invalid parameters
        if( alphaTypeSize == 0 ) //This should never happen, it will result in division by zero later
        {
            std::cout << "ERROR: alphaTypeSize == 0 in " << __FILE__ << __LINE__ << std::endl;
            return false;
        }

        info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
        blk_info->write_to_buffer((queue));

        // data type string may contain an identifier so determine the buffer size
        int nobsTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NUM_OBS>::prefix());
        bocl_mem* mog       = opencl_cache->get_data(scene, *id,data_type);
        bocl_mem* num_obs   = opencl_cache->get_data(scene, *id,num_obs_type,alpha->num_bytes()/alphaTypeSize*nobsTypeSize,false);//,info_buffer->data_buffer_length*boxm2_data_info::datasize(num_obs_type));

        //grab an appropriately sized AUX data buffer
        int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
        bocl_mem *aux0   = opencl_cache->get_data<BOXM2_AUX0>(scene, *id, info_buffer->data_buffer_length*auxTypeSize);
        auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
        bocl_mem *aux1   = opencl_cache->get_data<BOXM2_AUX1>(scene, *id, info_buffer->data_buffer_length*auxTypeSize);

        transfer_time += (float) transfer.all();
        if (i==UPDATE_PASSONE)
        {
            std::cout<<"    "<<kern->id()<<": "<< (*id) <<std::endl;
            aux0->zero_gpu_buffer(queue);
            aux1->zero_gpu_buffer(queue);
            kern->set_arg( blk_info );
            kern->set_arg( blk );
            kern->set_arg( alpha );
            kern->set_arg( mog );
            kern->set_arg( aux0 );
            kern->set_arg( aux1 );
            kern->set_arg( lookup.ptr() );
            kern->set_arg( ray_o_buff.ptr() );
            kern->set_arg( ray_d_buff.ptr() );

            //image information
            kern->set_arg( img_dim.ptr() );
            kern->set_arg( in_image.ptr() );
            kern->set_arg( vis_image.ptr() );
            kern->set_arg( pre_image.ptr() );
            kern->set_arg( norm_image.ptr() );
            kern->set_arg( cl_output.ptr() );

            //set centers args
            kern->set_arg( centerX.ptr() );
            kern->set_arg( centerY.ptr() );
            kern->set_arg( centerZ.ptr() );

            kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
            kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer,
            kern->set_local_arg( local_threads[0]*local_threads[1]*73*sizeof(cl_uchar) ); //to visit lists

            //execute kernel
            kern->execute(queue, 2, local_threads, global_threads);
            int status = clFinish(queue);
            check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
            gpu_time += kern->exec_time();

            //clear render kernel args so it can reset em on next execution
            kern->clear_args();
            aux0->read_to_buffer(queue);
            aux1->read_to_buffer(queue);

            ///debugging save vis, pre, norm images
            #if 1
              vis_image->read_to_buffer(queue);
              pre_image->read_to_buffer(queue);
              norm_image->read_to_buffer(queue);
              int idx = 0;
              vil_image_view<float> vis_view(cl_ni,cl_nj);
              vil_image_view<float> norm_view(cl_ni,cl_nj);
              vil_image_view<float> pre_view(cl_ni,cl_nj);
              for (unsigned c=0;c<cl_nj;++c) {
                for (unsigned r=0;r<cl_ni;++r) {
                  vis_view(r,c) = vis_buff[idx];
                  norm_view(r,c) = norm_buff[idx];
                  pre_view(r,c) = pre_buff[idx];
                  idx++;
                }
              }
              vil_save( vis_view, "vis_debug.tiff");
              vil_save( norm_view, "norm_debug.tiff");
              vil_save( pre_view, "pre_debug.tiff");
            #endif
        }
        else if (i==UPDATE_BAYES)
        {
            std::cout<<"    "<<kern->id()<<": "<< (*id) <<std::endl;
            auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
            bocl_mem *aux2   = opencl_cache->get_data<BOXM2_AUX2>(scene, *id, info_buffer->data_buffer_length*auxTypeSize);
            aux2->zero_gpu_buffer(queue);
            auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
            bocl_mem *aux3   = opencl_cache->get_data<BOXM2_AUX3>(scene, *id, info_buffer->data_buffer_length*auxTypeSize);
            aux3->zero_gpu_buffer(queue);

            kern->set_arg( blk_info );
            kern->set_arg( blk );
            kern->set_arg( alpha );
            kern->set_arg( mog );
            kern->set_arg( aux0 );
            kern->set_arg( aux1 );
            kern->set_arg( aux2 );
            kern->set_arg( aux3 );
            kern->set_arg( lookup.ptr() );
            kern->set_arg( ray_o_buff.ptr() );
            kern->set_arg( ray_d_buff.ptr() );

            //image args
            kern->set_arg( img_dim.ptr() );
            kern->set_arg( in_image.ptr() );
            kern->set_arg( vis_image.ptr() );
            kern->set_arg( pre_image.ptr() );
            kern->set_arg( norm_image.ptr() );
            kern->set_arg( cl_output.ptr() );

            //set centers args
            kern->set_arg( centerX.ptr() );
            kern->set_arg( centerY.ptr() );
            kern->set_arg( centerZ.ptr() );

            kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
            kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer
            kern->set_local_arg( local_threads[0]*local_threads[1]*73*sizeof(cl_uchar) ); //to visit lists

            //execute kernel
            kern->execute(queue, 2, local_threads, global_threads);
            int status = clFinish(queue);
            check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
            gpu_time += kern->exec_time();

            //clear render kernel args so it can reset em on next execution
            kern->clear_args();

            aux0->read_to_buffer(queue);
            aux1->read_to_buffer(queue);
            aux2->read_to_buffer(queue);
            aux3->read_to_buffer(queue);
        }
        else if (i==UPDATE_CELL)
        {
            std::cout<<"    "<<kern->id()<<": "<< (*id) <<std::endl;
            auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
            bocl_mem *aux2   = opencl_cache->get_data<BOXM2_AUX2>(scene, *id, info_buffer->data_buffer_length*auxTypeSize);

            auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
            bocl_mem *aux3   = opencl_cache->get_data<BOXM2_AUX3>(scene, *id, info_buffer->data_buffer_length*auxTypeSize);

            local_threads[0] = 64;
            local_threads[1] = 1 ;
            global_threads[0]=RoundUp(info_buffer->data_buffer_length,local_threads[0]);
            global_threads[1]=1;

            kern->set_arg( blk_info );
            kern->set_arg( alpha );
            kern->set_arg( mog );
            kern->set_arg( num_obs );
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
            alpha->read_to_buffer(queue);
            mog->read_to_buffer(queue);
            num_obs->read_to_buffer(queue);
        }

        //read image out to buffer (from gpu)
        in_image->read_to_buffer(queue);
        vis_image->read_to_buffer(queue);
        pre_image->read_to_buffer(queue);
        norm_image->read_to_buffer(queue);
        cl_output->read_to_buffer(queue);
        clFinish(queue);
    }
  }

  delete [] vis_buff;
  delete [] pre_buff;
  delete [] norm_buff;
  delete [] input_buff;
  delete [] ray_origins;
  delete [] ray_directions;
  return gpu_time + transfer_time;
}


//render_cone_expected_image - pretty much the same as above but one of the local
//memory arguments is expanded
float boxm2_ocl_cone_update( boxm2_scene_sptr & scene,
                             bocl_device_sptr & device,
                             boxm2_opencl_cache_sptr & opencl_cache,
                             std::vector<bocl_kernel*> kernels,
                             cl_command_queue& queue,
                             const std::string&  /*data_type*/,
                             const std::string&  /*num_obs_type*/,
                             vpgl_camera_double_sptr cam ,
                             const vil_image_view_base_sptr& in_img,
                             unsigned int roi_ni0,
                             unsigned int roi_nj0)
{
  enum {
    UPDATE_PASSONE = 0,
    UPDATE_BAYES= 1,
    UPDATE_CELL = 2,
  };

  float transfer_time=0.0f;
  float gpu_time=0.0f;
  std::size_t local_threads[2]={8,8};

  //camera check
  if (cam->type_name()!= "vpgl_perspective_camera" && cam->type_name() != "vpgl_generic_camera" ) {
    std::cout<<"Cannot render with camera of type "<<cam->type_name()<<std::endl;
    return 0.0f;
  }

  //grab input image, establish cl_ni, cl_nj (so global size is divisible by local size)
  vil_image_view_base_sptr float_img = boxm2_util::prepare_input_image(in_img);
  auto* img_view = static_cast<vil_image_view<float>* >(float_img.ptr());
  auto cl_ni=(unsigned)RoundUp(img_view->ni(),(int)local_threads[0]);
  auto cl_nj=(unsigned)RoundUp(img_view->nj(),(int)local_threads[1]);
  std::size_t global_threads[2] = {cl_ni, cl_nj};

  //set generic cam and get visible block order
  auto* ray_origins = new cl_float[4*cl_ni*cl_nj];
  auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
  bocl_mem_sptr ray_o_buff = new bocl_mem(device->context(), ray_origins, cl_ni*cl_nj * sizeof(cl_float4) , "ray_origins buffer");
  bocl_mem_sptr ray_d_buff = new bocl_mem(device->context(), ray_directions,  cl_ni*cl_nj * sizeof(cl_float4), "ray_directions buffer");
  boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);
  ray_d_buff->read_to_buffer(queue);

  ////////////////////////////////////////////////////////////////////////////////
  //gotta do this the old fashion way for debuggin....
  int cnt = 0;
  for (unsigned j=0;j<cl_nj;++j) {
    for (unsigned i=0;i<cl_ni;++i) {
      //calculate ray and ray angles at pixel ij
      vgl_ray_3d<double> ray_ij; //= cam->ray(i,j);
      double cone_half_angle, solid_angle;
      auto* pcam = (vpgl_perspective_camera<double>*) cam.ptr();
      vsph_camera_bounds::pixel_solid_angle(*pcam, i, j, ray_ij, cone_half_angle, solid_angle);
      ray_directions[4*cnt+3] = (cl_float) cone_half_angle;
      cnt++;
    }
  }
  ray_d_buff->write_to_buffer(queue);
  std::cout<<"opencl Half angle: "
          <<ray_directions[0]<<','
          <<ray_directions[1]<<','
          <<ray_directions[2]<<','
          <<ray_directions[3]<<std::endl;
  ////////////////////////////////////////////////////////////////////////////////

  //Visibility, Preinf, Norm, and input image buffers
  auto* vis_buff = new float[cl_ni*cl_nj];
  auto* pre_buff = new float[cl_ni*cl_nj];
  auto* norm_buff = new float[cl_ni*cl_nj];
  auto* input_buff=new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++)
  {
    vis_buff[i]=1.0f;
    pre_buff[i]=0.0f;
    norm_buff[i]=0.0f;
  }
  int count=0;
  for (unsigned int j=0;j<cl_nj;++j) {
    for (unsigned int i=0;i<cl_ni;++i) {
      input_buff[count] = 0.0f;
      if (i<img_view->ni() && j< img_view->nj())
        input_buff[count]=(*img_view)(i,j);
      ++count;
    }
  }
  bocl_mem_sptr in_image=new bocl_mem(device->context(),input_buff,cl_ni*cl_nj*sizeof(float),"input image buffer");
  in_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bocl_mem_sptr vis_image=new bocl_mem(device->context(),vis_buff,cl_ni*cl_nj*sizeof(float),"vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bocl_mem_sptr pre_image=new bocl_mem(device->context(),pre_buff,cl_ni*cl_nj*sizeof(float),"pre image buffer");
  pre_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  bocl_mem_sptr norm_image=new bocl_mem(device->context(),norm_buff,cl_ni*cl_nj*sizeof(float),"pre image buffer");
  norm_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // Image Dimensions
  unsigned int img_dim_buff[] = { roi_ni0, roi_nj0, img_view->ni(), img_view->nj() };
  bocl_mem_sptr img_dim = new bocl_mem(device->context(), img_dim_buff, sizeof(unsigned int)*4, "image dims");
  img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // Output Array
  float output_arr[100] = {0};
  bocl_mem_sptr  cl_output=new bocl_mem(device->context(), output_arr, sizeof(float)*100, "output buffer");
  cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  // center buffer
  bocl_mem_sptr centerX = new bocl_mem(device->context(), boct_bit_tree::centerX, sizeof(cl_float)*585, "centersX lookup buffer");
  bocl_mem_sptr centerY = new bocl_mem(device->context(), boct_bit_tree::centerY, sizeof(cl_float)*585, "centersY lookup buffer");
  bocl_mem_sptr centerZ = new bocl_mem(device->context(), boct_bit_tree::centerZ, sizeof(cl_float)*585, "centersZ lookup buffer");
  centerX->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  centerY->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  centerZ->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  // set arguments
  std::vector<boxm2_block_id> vis_order = scene->get_vis_blocks(cam);
  std::vector<boxm2_block_id>::iterator id;
  for (unsigned int i=0; i<kernels.size(); ++i)
  {
    std::cout<<"Running kernel "<<i<<std::endl;
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
      //choose correct render kernel
      boxm2_block_metadata mdata = scene->get_block_metadata(*id);
      bocl_kernel* kern =  kernels[i];

      //write the image values to the buffer
      vul_timer transfer;
      bocl_mem* blk       = opencl_cache->get_block(scene, *id);
      bocl_mem* blk_info  = opencl_cache->loaded_block_info();
      bocl_mem* alpha     = opencl_cache->get_data<BOXM2_GAMMA>(scene, *id,0,false);
      auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
      int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_GAMMA>::prefix());
      info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
      blk_info->write_to_buffer((queue));
#if 0
      int nobsTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NUM_OBS>::prefix());

      // data type string may contain an identifier so determine the buffer size
      int appTypeSize = (int)boxm2_data_info::datasize(data_type);
      bocl_mem* mog     = opencl_cache->get_data(*id,data_type, /*alpha->num_bytes()/alphaTypeSize*appTypeSize*/ 0,false);    //info_buffer->data_buffer_length*boxm2_data_info::datasize(data_type));
      bocl_mem* num_obs = opencl_cache->get_data(*id,num_obs_type,/*alpha->num_bytes()/alphaTypeSize*nobsTypeSize*/ 0,false);//,info_buffer->data_buffer_length*boxm2_data_info::datasize(num_obs_type));
#endif
      bocl_mem* mog = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_MOG3_GREY>::prefix(), 0, false);
      bocl_mem* num_obs = opencl_cache->get_data(scene, *id, boxm2_data_traits<BOXM2_NUM_OBS>::prefix(), 0, false);

      //grab an appropriately sized AUX data buffer
      int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
      bocl_mem *aux0   = opencl_cache->get_data<BOXM2_AUX0>(scene, *id, info_buffer->data_buffer_length*auxTypeSize);
      auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
      bocl_mem *aux1   = opencl_cache->get_data<BOXM2_AUX1>(scene, *id, info_buffer->data_buffer_length*auxTypeSize);

      transfer_time += (float) transfer.all();
      if (i==UPDATE_PASSONE)
      {
        std::cout<<"    "<<kern->id()<<": "<< (*id) <<std::endl;
        aux0->zero_gpu_buffer(queue);
        aux1->zero_gpu_buffer(queue);
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( mog );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( lookup.ptr() );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );

        //image information
        kern->set_arg( img_dim.ptr() );
        kern->set_arg( in_image.ptr() );
        kern->set_arg( vis_image.ptr() );
        kern->set_arg( pre_image.ptr() );
        kern->set_arg( norm_image.ptr() );
        kern->set_arg( cl_output.ptr() );

        //set centers args
        kern->set_arg( centerX.ptr() );
        kern->set_arg( centerY.ptr() );
        kern->set_arg( centerZ.ptr() );

        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
        kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer,
        kern->set_local_arg( local_threads[0]*local_threads[1]*73*sizeof(cl_uchar) ); //to visit lists

        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
        aux0->read_to_buffer(queue);
        aux1->read_to_buffer(queue);
      }
      else if (i==UPDATE_BAYES)
      {
        std::cout<<"    "<<kern->id()<<": "<< (*id) <<std::endl;
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
        bocl_mem *aux2   = opencl_cache->get_data<BOXM2_AUX2>(scene, *id, info_buffer->data_buffer_length*auxTypeSize);
        aux2->zero_gpu_buffer(queue);
        auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
        bocl_mem *aux3   = opencl_cache->get_data<BOXM2_AUX3>(scene, *id, info_buffer->data_buffer_length*auxTypeSize);
        aux3->zero_gpu_buffer(queue);

        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( mog );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( aux2 );
        kern->set_arg( aux3 );
        kern->set_arg( lookup.ptr() );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );

        //image args
        kern->set_arg( img_dim.ptr() );
        kern->set_arg( in_image.ptr() );
        kern->set_arg( vis_image.ptr() );
        kern->set_arg( pre_image.ptr() );
        kern->set_arg( norm_image.ptr() );
        kern->set_arg( cl_output.ptr() );

        //set centers args
        kern->set_arg( centerX.ptr() );
        kern->set_arg( centerY.ptr() );
        kern->set_arg( centerZ.ptr() );

        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
        kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer
        kern->set_local_arg( local_threads[0]*local_threads[1]*73*sizeof(cl_uchar) ); //to visit lists

        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();

        aux0->read_to_buffer(queue);
        aux1->read_to_buffer(queue);
        aux2->read_to_buffer(queue);
        aux3->read_to_buffer(queue);
      }
      else if (i==UPDATE_CELL)
      {
        std::cout<<"    "<<kern->id()<<": "<< (*id) <<std::endl;
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
        bocl_mem *aux2   = opencl_cache->get_data<BOXM2_AUX2>(scene, *id, info_buffer->data_buffer_length*auxTypeSize);

        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
        bocl_mem *aux3   = opencl_cache->get_data<BOXM2_AUX3>(scene, *id, info_buffer->data_buffer_length*auxTypeSize);

        local_threads[0] = 64;
        local_threads[1] = 1 ;
        global_threads[0]=RoundUp(info_buffer->data_buffer_length,local_threads[0]);
        global_threads[1]=1;

        kern->set_arg( blk_info );
        kern->set_arg( alpha );
        kern->set_arg( mog );
        kern->set_arg( num_obs );
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
        alpha->read_to_buffer(queue);
        mog->read_to_buffer(queue);
        num_obs->read_to_buffer(queue);
      }

      //read image out to buffer (from gpu)
      in_image->read_to_buffer(queue);
      vis_image->read_to_buffer(queue);
      pre_image->read_to_buffer(queue);
      norm_image->read_to_buffer(queue);
      cl_output->read_to_buffer(queue);
      clFinish(queue);
    }
  }

  ///debugging save vis, pre, norm images
#if 0
  int idx = 0;
  vil_image_view<float> vis_view(cl_ni,cl_nj);
  vil_image_view<float> norm_view(cl_ni,cl_nj);
  vil_image_view<float> pre_view(cl_ni,cl_nj);
  for (unsigned c=0;c<cl_nj;++c) {
    for (unsigned r=0;r<cl_ni;++r) {
      vis_view(r,c) = vis_buff[idx];
      norm_view(r,c) = norm_buff[idx];
      pre_view(r,c) = pre_buff[idx];
      idx++;
    }
  }
  vil_save( vis_view, "vis_debug.tiff");
  vil_save( norm_view, "norm_debug.tiff");
  vil_save( pre_view, "pre_debug.tiff");
#endif

  delete [] vis_buff;
  delete [] pre_buff;
  delete [] norm_buff;
  delete [] input_buff;
  delete [] ray_origins;
  delete [] ray_directions;
  return gpu_time + transfer_time;
}
