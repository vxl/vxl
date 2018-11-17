// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_create_aux_process.cxx
//:
// \file
// \brief  A process for computing pre,vis and post per cell given an image. This information will be used for updating the scene in batch fashion.
//
// \author Ali Osman Ulusoy
// \date Sep 08, 2011

#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>

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

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

namespace boxm2_ocl_create_aux_process_globals
{
  constexpr unsigned n_inputs_ = 7;
  constexpr unsigned n_outputs_ = 0;
  enum {
    UPDATE_PRE_INF = 0,
    UPDATE_PREVISPOST = 1,
    UPDATE_CONVERT_AUX = 2
  };

  void compile_kernel(const bocl_device_sptr& device,std::vector<bocl_kernel*> & vec_kernels,const std::string& opts)
  {
    //gather all render sources... seems like a lot for rendering...
    std::vector<std::string> src_paths;
    std::string source_dir = std::string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "cell_utils.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    std::vector<std::string> second_pass_src = std::vector<std::string>(src_paths);

    src_paths.push_back(source_dir + "bit/update_kernels.cl");
    src_paths.push_back(source_dir + "update_functors.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");


    second_pass_src.push_back(source_dir + "bit/batch_update_kernels.cl");
    auto* convert_aux_int_float = new bocl_kernel();
    convert_aux_int_float->create_kernel(&device->context(),device->device_id(), second_pass_src, "convert_aux_and_normalize", opts+" -D CONVERT_AUX_NORMALIZE", "batch_update::convert_aux_and_normalize");

    second_pass_src.push_back(source_dir + "batch_update_functors.cl");
    second_pass_src.push_back(source_dir + "bit/cast_ray_bit.cl");

    //compilation options
    std::string options = opts + " -D INTENSITY  ";
    options += " -D DETERMINISTIC ";

    //create all passes
    auto* pre_inf = new bocl_kernel();
    std::string pre_opts = options + " -D PREINF -D STEP_CELL=step_cell_preinf(aux_args,data_ptr,llid,d) ";
    pre_inf->create_kernel(&device->context(),device->device_id(), src_paths, "pre_inf_main", pre_opts, "batch_update::pre_inf");
    vec_kernels.push_back(pre_inf);

    //push back cast_ray_bit
    auto* aux_pre_vis = new bocl_kernel();
    std::string bayes_opt = options + " -D AUX_PREVISPOST -D STEP_CELL=step_cell_aux_previspost(aux_args,data_ptr,llid,d) ";
    aux_pre_vis->create_kernel(&device->context(),device->device_id(), second_pass_src, "aux_previspost_main", bayes_opt, "batch_update::aux_previspost_main");
    vec_kernels.push_back(aux_pre_vis);

    ////may need DIFF LIST OF SOURCES FOR THSI GUY TOO
    vec_kernels.push_back(convert_aux_int_float);

    return ;
  }

  static std::map<std::string,std::vector<bocl_kernel*> > kernels;
}

bool boxm2_ocl_create_aux_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_create_aux_process_globals;

  //process takes 7 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "boxm2_cache_sptr";
  input_types_[4] = "vpgl_camera_double_sptr";
  input_types_[5] = "vil_image_view_base_sptr";
  input_types_[6] = "vcl_string";
  // in case the 7th input is not set
  brdb_value_sptr idx = new brdb_value_t<std::string>("");
  pro.set_input(6, idx);

  // process has no outputs
  std::vector<std::string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_)
      && pro.set_output_types(output_types_);
}

bool boxm2_ocl_create_aux_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_create_aux_process_globals;
  std::size_t local_threads[2]={8,8};
  std::size_t global_threads[2]={8,8};

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The number of inputs should be " << n_inputs_<< std::endl;
    return false;
  }
  float transfer_time=0.0f;
  float gpu_time=0.0f;
  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device = pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr opencl_cache = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  boxm2_cache_sptr          cache = pro.get_input<boxm2_cache_sptr>(i++);
  vpgl_camera_double_sptr cam = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr img = pro.get_input<vil_image_view_base_sptr>(i++);
  std::string ident = pro.get_input<std::string>(i++);

  long binCache = opencl_cache.ptr()->bytes_in_cache();
  std::cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<std::endl;

  bool foundDataType = false;
  std::string data_type,num_obs_type,options;
  std::vector<std::string> apps = scene->appearances();
  int appTypeSize;
  for (const auto & app : apps) {
    if ( app == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
    {
      data_type = app;
      foundDataType = true;
      options=" -D MOG_TYPE_8 ";
      appTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
    }
    else if ( app == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
    {
      data_type = app;
      foundDataType = true;
      options=" -D MOG_TYPE_16 ";
      appTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix());
    }
    else if ( app == boxm2_data_traits<BOXM2_NUM_OBS>::prefix() )
    {
      num_obs_type = boxm2_data_traits<BOXM2_NUM_OBS>::prefix();
    }
  }
  if (!foundDataType) {
    std::cout<<"BOXM2_OCL_UPDATE_AUX_PER_VIEW_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<std::endl;
    return false;
  }

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
    CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0) return false;

  std::string identifier=device->device_identifier()+options;
  // compile the kernel if not already compiled
  if (kernels.find(identifier)==kernels.end())
  {
    std::cout<<"===========Compiling kernels==========="<<std::endl;
    std::vector<bocl_kernel*> ks;
    compile_kernel(device,ks,options);
    kernels[identifier]=ks;
  }

  //grab input image, establish cl_ni, cl_nj (so global size is divisible by local size)
  vil_image_view_base_sptr float_img=boxm2_util::prepare_input_image(img);
  auto* img_view = static_cast<vil_image_view<float>* >(float_img.ptr());
  auto cl_ni=(unsigned)RoundUp(img_view->ni(),(int)local_threads[0]);
  auto cl_nj=(unsigned)RoundUp(img_view->nj(),(int)local_threads[1]);
  global_threads[0]=cl_ni;
  global_threads[1]=cl_nj;

  //set generic cam
  auto* ray_origins = new cl_float[4*cl_ni*cl_nj];
  auto* ray_directions = new cl_float[4*cl_ni*cl_nj];
  bocl_mem_sptr ray_o_buff = new bocl_mem(device->context(), ray_origins, cl_ni*cl_nj * sizeof(cl_float4) , "ray_origins buffer");
  bocl_mem_sptr ray_d_buff = new bocl_mem(device->context(), ray_directions,  cl_ni*cl_nj * sizeof(cl_float4), "ray_directions buffer");
  boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

  //Visibility, Preinf, Norm, and input image buffers
  auto* vis_buff = new float[cl_ni*cl_nj];
  auto* pre_buff = new float[cl_ni*cl_nj];
  auto* vis_inf_buff = new float[cl_ni*cl_nj];
  auto* pre_inf_buff = new float[cl_ni*cl_nj];
  auto* input_buff=new float[cl_ni*cl_nj];
  for (unsigned i=0;i<cl_ni*cl_nj;i++)
  {
    vis_buff[i]=1.0f;
    vis_inf_buff[i]=1.0f;
    pre_buff[i]=0.0f;
    pre_inf_buff[i]=0.0f;
  }
  int count=0;
  for (unsigned int j=0;j<cl_nj;++j)
    for (unsigned int i=0;i<cl_ni;++i)
    {
      input_buff[count] = 0.0f;
      if (i<img_view->ni() && j< img_view->nj()) input_buff[count]=(*img_view)(i,j);
      ++count;
    }

  bocl_mem_sptr in_image=new bocl_mem(device->context(),input_buff,cl_ni*cl_nj*sizeof(float),"input image buffer");
  in_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr vis_image=new bocl_mem(device->context(),vis_buff,cl_ni*cl_nj*sizeof(float),"vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr pre_image=new bocl_mem(device->context(),pre_buff,cl_ni*cl_nj*sizeof(float),"pre image buffer");
  pre_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr vis_inf_image=new bocl_mem(device->context(),vis_inf_buff,cl_ni*cl_nj*sizeof(float),"vis inf image buffer");
  vis_inf_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr pre_inf_image=new bocl_mem(device->context(),pre_inf_buff,cl_ni*cl_nj*sizeof(float),"pre inf image buffer");
  pre_inf_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // Image Dimensions
  int img_dim_buff[4];
  img_dim_buff[0] = 0;
  img_dim_buff[1] = 0;
  img_dim_buff[2] = img_view->ni();
  img_dim_buff[3] = img_view->nj();
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

  // app density used for proc_norm_image
  float app_buffer[4]={1.0,0.0,0.0,0.0};
  bocl_mem_sptr app_density = new bocl_mem(device->context(), app_buffer, sizeof(cl_float4), "app density buffer");
  app_density->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // set arguments
  std::vector<boxm2_block_id> vis_order = scene->get_vis_blocks(cam);
  std::vector<boxm2_block_id>::iterator id;
  for (unsigned int i=0; i<kernels[identifier].size(); ++i)
  {
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
      //choose correct render kernel
      boxm2_block_metadata mdata = scene->get_block_metadata(*id);
      bocl_kernel* kern =  kernels[identifier][i];

      //write the image values to the buffer
      vul_timer transfer;
      bocl_mem* blk = opencl_cache->get_block(scene,*id);
      bocl_mem* blk_info = opencl_cache->loaded_block_info();
      bocl_mem* alpha = opencl_cache->get_data<BOXM2_ALPHA>(scene,*id,0,false);
      auto* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
      int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
      info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
      blk_info->write_to_buffer((queue));

      //read aux0 and aux1 from cache. They should have been generated by boxm2OclCreateNormIntensities process
      int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
      bocl_mem *aux0 = opencl_cache->get_data(scene,*id, boxm2_data_traits<BOXM2_AUX0>::prefix(ident),0,false);
      bocl_mem *aux1 = opencl_cache->get_data(scene,*id, boxm2_data_traits<BOXM2_AUX1>::prefix(ident),0,false);

      //grab appearance model.
      bocl_mem* mog = opencl_cache->get_data(scene,*id,data_type,alpha->num_bytes()/alphaTypeSize*appTypeSize,false);

      transfer_time += (float) transfer.all();
      if (i==UPDATE_PRE_INF)
      {
        //will be deleted later...experimenting with BOXM2_NUM_OBS
        int nobsTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_FLOAT8>::prefix());
        bocl_mem* num_obs = opencl_cache->get_data(scene,*id,boxm2_data_traits<BOXM2_FLOAT8>::prefix(),alpha->num_bytes()/alphaTypeSize*nobsTypeSize,false);
        num_obs->zero_gpu_buffer(queue);

        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( mog );
        kern->set_arg( num_obs ); //this input is not used inside the kernel.
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( lookup.ptr() );
        // kern->set_arg( persp_cam.ptr() );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );

        kern->set_arg( img_dim.ptr() );
        kern->set_arg( vis_inf_image.ptr());
        kern->set_arg( pre_inf_image.ptr());
        kern->set_arg( cl_output.ptr() );
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
        kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer
        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();
#if 0
        aux0->read_to_buffer(queue);
        aux1->read_to_buffer(queue);
        num_obs->read_to_buffer(queue);
#endif
      }
      else if (i==UPDATE_PREVISPOST)
      {
        //initialize vis, pre and post bocl_mem objects

        //vis_array
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
        bocl_mem *aux2 = opencl_cache->get_data(scene,*id,boxm2_data_traits<BOXM2_AUX2>::prefix(ident), info_buffer->data_buffer_length*auxTypeSize,false);
        //pre_array
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
        bocl_mem *aux3 = opencl_cache->get_data(scene,*id,boxm2_data_traits<BOXM2_AUX3>::prefix(ident), info_buffer->data_buffer_length*auxTypeSize,false);
        //post_array
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX4>::prefix());
        bocl_mem *aux4 = opencl_cache->get_data(scene,*id,boxm2_data_traits<BOXM2_AUX4>::prefix(ident), info_buffer->data_buffer_length*auxTypeSize,false);

        aux3->zero_gpu_buffer(queue);
        aux2->zero_gpu_buffer(queue);
        aux4->zero_gpu_buffer(queue);

        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( mog );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( aux2 );
        kern->set_arg( aux3 );
        kern->set_arg( aux4 );
        kern->set_arg( lookup.ptr() );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );

        kern->set_arg( img_dim.ptr() );
        kern->set_arg( vis_image.ptr() );
        kern->set_arg( pre_image.ptr() );
        kern->set_arg( vis_inf_image.ptr());
        kern->set_arg( pre_inf_image.ptr());
        kern->set_arg( cl_output.ptr() );
        kern->set_local_arg( local_threads[0]*local_threads[1]   *sizeof(cl_uchar16) );//local tree,
        kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer
        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();

        //read results to CPU
        aux2->read_to_buffer(queue);
        aux3->read_to_buffer(queue);
        aux4->read_to_buffer(queue);
      }
      else if (i == UPDATE_CONVERT_AUX)
      {
        //vis_array
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
        bocl_mem *aux2 = opencl_cache->get_data(scene,*id,boxm2_data_traits<BOXM2_AUX2>::prefix(ident), info_buffer->data_buffer_length*auxTypeSize,false);
        //pre_array
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
        bocl_mem *aux3 = opencl_cache->get_data(scene,*id,boxm2_data_traits<BOXM2_AUX3>::prefix(ident), info_buffer->data_buffer_length*auxTypeSize,false);
        //post_array
        auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX4>::prefix());
        bocl_mem *aux4 = opencl_cache->get_data(scene,*id,boxm2_data_traits<BOXM2_AUX4>::prefix(ident), info_buffer->data_buffer_length*auxTypeSize,false);

        local_threads[0] = 64;
        local_threads[1] = 1 ;
        global_threads[0]=RoundUp(info_buffer->data_buffer_length,local_threads[0]);
        global_threads[1]=1;

        kern->set_arg( blk_info );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( aux2 );
        kern->set_arg( aux3 );
        kern->set_arg( aux4 );

        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();

        //read results to CPU
        aux0->read_to_buffer(queue);
        aux1->read_to_buffer(queue);
        aux2->read_to_buffer(queue);
        aux3->read_to_buffer(queue);
        aux4->read_to_buffer(queue);

#ifdef DEBUG
        float * aux0_f=static_cast<float*> (aux0->cpu_buffer());
        float * aux1_f=static_cast<float*> (aux1->cpu_buffer());
        float * aux2_f=static_cast<float*> (aux2->cpu_buffer());
        float * aux3_f=static_cast<float*> (aux3->cpu_buffer());
        float * aux4_f=static_cast<float*> (aux4->cpu_buffer());

        for (unsigned k=0;k<100;k++) {
          std::cout << aux0_f[k] <<' '<< aux1_f[k]<< ' ' << aux2_f[k] <<' '<< aux3_f[k] <<' '<< aux4_f[k] <<std::endl;
        }
#endif
      }

      clFinish(queue);
    }
  }

  delete [] vis_inf_buff;
  delete [] pre_inf_buff;
  delete [] vis_buff;
  delete [] pre_buff;
  delete [] input_buff;
  delete [] ray_origins;
  delete [] ray_directions;

  std::cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<std::endl;
  clReleaseCommandQueue(queue);
  return true;
}
