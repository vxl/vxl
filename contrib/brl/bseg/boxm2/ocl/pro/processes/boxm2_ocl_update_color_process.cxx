// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_update_color_process.cxx
//:
// \file
// \brief  A process for updating a color model
//
// \author Vishal Jain
// \date Mar 25, 2011

#include <bprb/bprb_func_process.h>

#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <boxm2/ocl/boxm2_opencl_cache.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/ocl/algo/boxm2_ocl_camera_converter.h>
#include <vil/vil_save.h>
#include <vil/vil_image_view.h>
//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace boxm2_ocl_update_color_process_globals
{
  const unsigned n_inputs_  = 5;
  const unsigned n_outputs_ = 0;
  vcl_size_t local_threads[2]={8,8};
  vcl_size_t global_threads[2]={8,8};
  enum {
      UPDATE_SEGLEN = 0,
      UPDATE_COMPRESS = 1,
      UPDATE_PREINF = 2,
      UPDATE_PROC   = 3,
      UPDATE_BAYES  = 4,
      UPDATE_CELL   = 5
  };

  void compile_kernel(bocl_device_sptr device,vcl_vector<bocl_kernel*> & vec_kernels,vcl_string opts)
  {
    vcl_vector<vcl_string> src_paths;
    vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "pixel_conversion.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "bit/update_rgb_kernels.cl");
    vcl_vector<vcl_string> non_ray_src = vcl_vector<vcl_string>(src_paths);
    src_paths.push_back(source_dir + "update_rgb_functors.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");
    
    //compilation options
    vcl_string options = " -D INTENSITY ";
    options += " -D DETERMINISTIC -D MOG_TYPE_8 ";
    options += opts;

    //seg len pass
    bocl_kernel* seg_len = new bocl_kernel();
    vcl_string seg_opts = options + " -D SEGLEN -D STEP_CELL=step_cell_seglen(aux_args,data_ptr,llid,d) ";
    seg_len->create_kernel(&device->context(), device->device_id(), src_paths, "seg_len_main", seg_opts, "update_color::seg_len");
    vec_kernels.push_back(seg_len);

    //create  compress rgb pass
    bocl_kernel* comp = new bocl_kernel();
    vcl_string comp_opts = options + " -D COMPRESS_RGB ";
    comp->create_kernel(&device->context(), device->device_id(), non_ray_src, "compress_rgb", comp_opts, "update_color::compress_rgb");
    vec_kernels.push_back(comp);

    bocl_kernel* pre_inf = new bocl_kernel();
    vcl_string pre_opts = options + " -D PREINF -D STEP_CELL=step_cell_preinf(aux_args,data_ptr,llid,d) ";
    pre_inf->create_kernel(&device->context(), device->device_id(), src_paths, "pre_inf_main", pre_opts, "update_color::pre_inf");
    vec_kernels.push_back(pre_inf);

    //may need DIFF LIST OF SOURCES FOR THIS GUY
    bocl_kernel* proc_img = new bocl_kernel();
    proc_img->create_kernel(&device->context(), device->device_id(), non_ray_src, "proc_norm_image", options, "update_color::proc_norm_image");
    vec_kernels.push_back(proc_img);

    //push back cast_ray_bit
    bocl_kernel* bayes_main = new bocl_kernel();
    vcl_string bayes_opt = options + " -D BAYES -D STEP_CELL=step_cell_bayes(aux_args,data_ptr,llid,d) ";
    bayes_main->create_kernel(&device->context(), device->device_id(), src_paths, "bayes_main", bayes_opt, "update_color::bayes_main");
    vec_kernels.push_back(bayes_main);

    //may need DIFF LIST OF SOURCES FOR THSI GUY TOO
    bocl_kernel* update = new bocl_kernel();
    update->create_kernel(&device->context(), device->device_id(), non_ray_src, "update_bit_scene_main", options, "update_color::update_main");
    vec_kernels.push_back(update);

    return ;
  }

  //: Map of kernels should persist between process executions
  static vcl_map<vcl_string,vcl_vector<bocl_kernel*> > kernels;
}

bool boxm2_ocl_update_color_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_color_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "vil_image_view_base_sptr";

  // process has 1 output:
  // output[0]: scene sptr
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_ocl_update_color_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_update_color_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  float transfer_time=0.0f;
  float gpu_time=0.0f;
  
  //get the inputs: device, scene, opencl_cache, cam, img
  unsigned argIdx = 0;
  bocl_device_sptr          device = pro.get_input<bocl_device_sptr>(argIdx++);
  boxm2_scene_sptr          scene = pro.get_input<boxm2_scene_sptr>(argIdx++);
  boxm2_opencl_cache_sptr   opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(argIdx++);
  vpgl_camera_double_sptr   cam = pro.get_input<vpgl_camera_double_sptr>(argIdx++);
  vil_image_view_base_sptr  img = pro.get_input<vil_image_view_base_sptr>(argIdx++);

  //make sure the scene corresponds to this datatype 
  bool foundDataType = false, foundNumObsType = false;
  vcl_string data_type, num_obs_type, options;
  if( scene->has_data_type(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix()) ) {
    data_type = boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix();
    foundDataType = true;
    options=" -D MOG_TYPE_8 ";
  }
  if( scene->has_data_type(boxm2_data_traits<BOXM2_NUM_OBS_SINGLE>::prefix()) ) {
    num_obs_type = boxm2_data_traits<BOXM2_NUM_OBS_SINGLE>::prefix();
    foundNumObsType = true;
  }
  if (!foundDataType) {
    vcl_cout<<"boxm2_ocl_update_color_process ERROR: scene doesn't have BOXM2_GAUSS_RGB data type"<<vcl_endl;
    return false;
  }
  if (!foundNumObsType) {
    vcl_cout<<"boxm2_ocl_update_color_process ERROR: scene doesn't have BOXM2_NUM_OBS_SINGLE type"<<vcl_endl;
    return false;
  }

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
                                                CL_QUEUE_PROFILING_ENABLE,&status);
  if(!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)) )
    return false; 

  //: compile kernels if not already compiled
  vcl_string identifier = device->device_identifier() + options;
  if (kernels.find(identifier)==kernels.end())
  {
    vcl_cout<<"=== boxm2_ocl_update_color_process::compiling kernels on device "<<identifier<<"==="<<vcl_endl;
    vcl_vector<bocl_kernel*> ks;
    compile_kernel(device,ks,options);
    kernels[identifier]=ks;
  }
  
  //prepare input image
  vil_image_view_base_sptr float_img = boxm2_ocl_util::prepare_input_image(img);
  if( float_img->pixel_format() != VIL_PIXEL_FORMAT_RGBA_BYTE ) {
    vcl_cout<<"boxm2_ocl_update_color_process::using a non RGBA image!!"<<vcl_endl;
    return false; 
  }
  vil_image_view<vil_rgba<vxl_byte> >* img_view = static_cast<vil_image_view<vil_rgba<vxl_byte> >* >(float_img.ptr());
  
  //grab input image dimensions - round up to the nearest 8 for OPENCL
  unsigned cl_ni=RoundUp(img_view->ni(),local_threads[0]);
  unsigned cl_nj=RoundUp(img_view->nj(),local_threads[1]);
  global_threads[0]=cl_ni;
  global_threads[1]=cl_nj;
  
  //: create all buffers
  //cl_float cam_buffer[48];
  //boxm2_ocl_util::set_persp_camera(cam, cam_buffer);
  //bocl_mem_sptr persp_cam = new bocl_mem(device->context(), cam_buffer, 3*sizeof(cl_float16), "persp cam buffer");
  //persp_cam->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
  cl_float* ray_origins = new cl_float[4*cl_ni*cl_nj]; 
  cl_float* ray_directions = new cl_float[4*cl_ni*cl_nj]; 
  bocl_mem_sptr ray_o_buff = new bocl_mem(device->context(), ray_origins, cl_ni*cl_nj * sizeof(cl_float4) , "ray_origins buffer");
  bocl_mem_sptr ray_d_buff = new bocl_mem(device->context(), ray_directions,  cl_ni*cl_nj * sizeof(cl_float4), "ray_directions buffer");
  boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff); 

  //create vis, pre, norm and input image buffers
  float* vis_buff = new float[cl_ni*cl_nj];
  float* pre_buff = new float[cl_ni*cl_nj];
  float* norm_buff = new float[cl_ni*cl_nj];
  vcl_fill(vis_buff, vis_buff+cl_ni*cl_nj, 1.0f); 
  vcl_fill(pre_buff, pre_buff+cl_ni*cl_nj, 0.0f); 
  vcl_fill(norm_buff, norm_buff+cl_ni*cl_nj, 0.0f); 
  
  //initialize input image buffer
  int numFloats = 4; 
  float* input_buff = new float[numFloats*cl_ni*cl_nj];  //need to store RGB (or YUV values)
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
      }
      ++count;
    }
  }

  bocl_mem_sptr in_image=new bocl_mem(device->context(),input_buff,cl_ni*cl_nj*sizeof(cl_float4),"input image buffer");
  in_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr vis_image=new bocl_mem(device->context(),vis_buff,cl_ni*cl_nj*sizeof(float),"vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr pre_image=new bocl_mem(device->context(),pre_buff,cl_ni*cl_nj*sizeof(float),"pre image buffer");
  pre_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr norm_image=new bocl_mem(device->context(),norm_buff,cl_ni*cl_nj*sizeof(float),"pre image buffer");
  norm_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //: Image Dimensions
  int img_dim_buff[4];
  img_dim_buff[0] = 0;
  img_dim_buff[1] = 0;
  img_dim_buff[2] = img_view->ni();
  img_dim_buff[3] = img_view->nj();
  bocl_mem_sptr img_dim=new bocl_mem(device->context(), img_dim_buff, sizeof(int)*4, "image dims");
  img_dim->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //: Output Array
  float output_arr[100];
  for (int i=0; i<100; ++i) output_arr[i] = 0.0f;
  bocl_mem_sptr  cl_output=new bocl_mem(device->context(), output_arr, sizeof(float)*100, "output buffer");
  cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //: bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup=new bocl_mem(device->context(), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  // app density used for proc_norm_image
  float app_buffer[4]={1.0,0.0,0.0,0.0};
  bocl_mem_sptr app_density = new bocl_mem(device->context(), app_buffer, sizeof(cl_float4), "app density buffer");
  app_density->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //: set arguments
  vcl_vector<boxm2_block_id> vis_order = scene->get_vis_blocks( (vpgl_perspective_camera<double>*)  cam.ptr());
  vcl_vector<boxm2_block_id>::iterator id;
  for (unsigned int i=0; i<kernels[identifier].size(); ++i)
  {
    if ( i == UPDATE_PROC ) {      
      local_threads[0] = 8; local_threads[1] = 8;    
      global_threads[0]=cl_ni; global_threads[1]=cl_nj;
      bocl_kernel * proc_kern=kernels[identifier][i];
        
      proc_kern->set_arg( norm_image.ptr() );
      proc_kern->set_arg( vis_image.ptr() );
      proc_kern->set_arg( pre_image.ptr());
      proc_kern->set_arg( img_dim.ptr() );

      //execute kernel
      proc_kern->execute( queue, 2, local_threads, global_threads);
      int status = clFinish(queue);
      if(!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)) )
        return false; 
      proc_kern->clear_args();
      norm_image->read_to_buffer(queue);
      continue;
    }
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
        //choose correct render kernel
        boxm2_block_metadata mdata = scene->get_block_metadata(*id);
        bocl_kernel* kern =  kernels[identifier][i];

        //write the image values to the buffer
        vul_timer transfer;
        bocl_mem* blk       = opencl_cache->get_block(*id);
        bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(*id);
        bocl_mem* mog       = opencl_cache->get_data(*id,data_type);
        bocl_mem* num_obs   = opencl_cache->get_data(*id, num_obs_type);
        bocl_mem * blk_info  = opencl_cache->loaded_block_info();

        //make sure the scene info data size reflects the real data size
        boxm2_scene_info* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
        int alphaTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
        blk_info->write_to_buffer((queue));

        //grab an appropriately sized AUX data buffer
        int auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
        bocl_mem *aux0   = opencl_cache->get_data<BOXM2_AUX0>(*id, info_buffer->data_buffer_length*auxTypeSize);
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
        bocl_mem *aux1   = opencl_cache->get_data<BOXM2_AUX1>(*id, info_buffer->data_buffer_length*auxTypeSize);
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
        bocl_mem *aux2   = opencl_cache->get_data<BOXM2_AUX2>(*id, info_buffer->data_buffer_length*auxTypeSize);
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
        bocl_mem *aux3   = opencl_cache->get_data<BOXM2_AUX3>(*id, info_buffer->data_buffer_length*auxTypeSize);
     
        transfer_time += (float) transfer.all();
        if(i==UPDATE_SEGLEN)
        {
            local_threads[0] = 8; local_threads[1] = 8;    
            global_threads[0]=cl_ni; global_threads[1]=cl_nj;
            kern->set_arg( blk_info );
            kern->set_arg( blk );
            kern->set_arg( alpha );
            kern->set_arg( aux0 );
            kern->set_arg( aux1 );
            kern->set_arg( aux2 );
            kern->set_arg( aux3 );
            kern->set_arg( lookup.ptr() );
            // kern->set_arg( persp_cam.ptr() );
            kern->set_arg( ray_o_buff.ptr() );
            kern->set_arg( ray_d_buff.ptr() );
            
            kern->set_arg( img_dim.ptr() );
            kern->set_arg( in_image.ptr() );
            kern->set_arg( cl_output.ptr() );
            kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
            kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar4) ); //ray bundle,
            kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_int) );    //cell pointers,
            kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_float4) ); //cached aux,
            kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer
        }
        else if(i==UPDATE_COMPRESS)
        {
            local_threads[0] = 64;
            local_threads[1] = 1 ;        
            global_threads[0]=RoundUp(info_buffer->data_buffer_length,local_threads[0]);
            global_threads[1]=1;
            kern->set_arg( blk_info );
            kern->set_arg( aux0 );
            kern->set_arg( aux1 );
            kern->set_arg( aux2 );
            kern->set_arg( aux3 );
        }
        else if(i==UPDATE_PREINF)
        {
            local_threads[0] = 8; local_threads[1] = 8;    
            global_threads[0]=cl_ni; global_threads[1]=cl_nj;
            kern->set_arg( blk_info );
            kern->set_arg( blk );
            kern->set_arg( alpha );
            kern->set_arg( mog );
            kern->set_arg( num_obs );
            kern->set_arg( aux0 );
            kern->set_arg( aux1 );
            kern->set_arg( lookup.ptr() );    
            
            // kern->set_arg( persp_cam.ptr() );
            kern->set_arg( ray_o_buff.ptr() );
            kern->set_arg( ray_d_buff.ptr() );
            
            kern->set_arg( img_dim.ptr() );
            kern->set_arg( vis_image.ptr() );
            kern->set_arg( pre_image.ptr() );
            kern->set_arg( cl_output.ptr() );
            kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
            kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer
        }
        else if(i==UPDATE_BAYES)
        {
            local_threads[0] = 8; local_threads[1] = 8;    
            global_threads[0]=cl_ni; global_threads[1]=cl_nj;
            kern->set_arg( blk_info );
            kern->set_arg( blk );
            kern->set_arg( alpha );
            kern->set_arg( mog );
            kern->set_arg( num_obs );
            kern->set_arg( aux0 );
            kern->set_arg( aux1 );
            kern->set_arg( aux2 );
            kern->set_arg( aux3 );
            kern->set_arg( lookup.ptr() );
            
            // kern->set_arg( persp_cam.ptr() );
            kern->set_arg( ray_o_buff.ptr() );
            kern->set_arg( ray_d_buff.ptr() );
            
            kern->set_arg( img_dim.ptr() );
            kern->set_arg( vis_image.ptr() );
            kern->set_arg( pre_image.ptr() );
            kern->set_arg( norm_image.ptr() );
            kern->set_arg( cl_output.ptr() );
            kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
            kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_short2) ); //ray bundle,
            kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_int) );    //cell pointers,
            kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_float) ); //cached aux,
            kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer
        }
        else if(i==UPDATE_CELL)
        {
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
            kern->set_arg( cl_output.ptr() );
        }
        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        if(!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)) )
          return false; 
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();

        //read info back to host memory
        blk->read_to_buffer(queue);
        alpha->read_to_buffer(queue);
        mog->read_to_buffer(queue);
        num_obs->read_to_buffer(queue);
        aux0->read_to_buffer(queue);
        aux1->read_to_buffer(queue);
        aux2->read_to_buffer(queue);
        aux3->read_to_buffer(queue);

        //read image out to buffer (from gpu)
        in_image->read_to_buffer(queue);
        vis_image->read_to_buffer(queue);
        pre_image->read_to_buffer(queue);
        cl_output->read_to_buffer(queue);
        clFinish(queue);
    }
  }


   delete [] vis_buff;  
   delete [] pre_buff;  
   delete [] norm_buff; 
   delete [] input_buff;
   
   //reset local threads to 8/8 (default); 
   local_threads[0] = 8; 
   local_threads[1] = 8;    
   global_threads[0]=cl_ni; 
   global_threads[1]=cl_nj;

  vcl_cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<vcl_endl;
  clReleaseCommandQueue(queue);
  return true;
}
