// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_create_norm_intensities_process.cxx
//:
// \file
// \brief  A process for computing cumulative segment length, normalized intensity and number of observations per cell given an image. This information will be used for updating the scene in batch fashion.
//
// \author Ali Osman Ulusoy
// \date Sep 08, 2011

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

namespace boxm2_ocl_create_norm_intensities_process_globals
{
  const unsigned n_inputs_  = 6;
  const unsigned n_outputs_ = 0;
  enum {
    UPDATE_CREATE_NORM        = 0,
    CONVERT_NOBS_INT_SHORT    = 1
  };

  void compile_kernel(bocl_device_sptr device,vcl_vector<bocl_kernel*> & vec_kernels,vcl_string opts)
  {
    //gather all render sources... seems like a lot for rendering...
    vcl_vector<vcl_string> src_paths;
    vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "cell_utils.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    //src_paths.push_back(source_dir + "statistics_library_functions.cl");
    //src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    vcl_vector<vcl_string> second_pass_src = vcl_vector<vcl_string>(src_paths);

    //src_paths.push_back(source_dir + "bit/update_kernels.cl");
    //src_paths.push_back(source_dir + "update_functors.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");


    second_pass_src.push_back(source_dir + "bit/batch_update_kernels.cl");
    bocl_kernel* convert_nobs_int_short = new bocl_kernel();
    convert_nobs_int_short->create_kernel(&device->context(),device->device_id(), second_pass_src, "convert_nobs_int_short", opts+" -D CONVERT_NOBS_INT_SHORT", "batch_update::convert_nobs_int_to_short");

    second_pass_src.push_back(source_dir + "batch_update_functors.cl");
    second_pass_src.push_back(source_dir + "bit/cast_ray_bit.cl");

    //compilation options
    vcl_string options = opts + " -D INTENSITY  ";
    options += " -D DETERMINISTIC ";

    //push back cast_ray_bit
    bocl_kernel* seg_len = new bocl_kernel();
    vcl_string bayes_opt = options + " -D SEGLENNOBS -D STEP_CELL=step_cell_seglen_nobs(aux_args,data_ptr,llid,d) ";
    seg_len->create_kernel(&device->context(),device->device_id(), second_pass_src, "seg_len_nobs_main", bayes_opt, "batch_update::seg_len_nobs_main");
    vec_kernels.push_back(seg_len);

    ////may need DIFF LIST OF SOURCES FOR THSI GUY TOO
#if 0
    vec_kernels.push_back(convert_nobs_int_short);
#endif
    return ;
  }

  static vcl_map<vcl_string,vcl_vector<bocl_kernel*> > kernels;
}

bool boxm2_ocl_create_norm_intensities_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_create_norm_intensities_process_globals;

  //process takes 6 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_opencl_cache_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "vil_image_view_base_sptr";
  input_types_[5] = "vcl_string";
  // in case the 6th input is not set
  brdb_value_sptr idx = new brdb_value_t<vcl_string>("");
  pro.set_input(5, idx);

  // process has no outputs
  vcl_vector<vcl_string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_)
      && pro.set_output_types(output_types_);
}

bool boxm2_ocl_create_norm_intensities_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_create_norm_intensities_process_globals;
  vcl_size_t local_threads[2]={8,8};
  vcl_size_t global_threads[2]={8,8};

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_<< vcl_endl;
    return false;
  }
  float transfer_time=0.0f;
  float gpu_time=0.0f;
  //get the inputs
  unsigned i = 0;
  bocl_device_sptr device               = pro.get_input<bocl_device_sptr>(i++);
  boxm2_scene_sptr scene                = pro.get_input<boxm2_scene_sptr>(i++);
  boxm2_opencl_cache_sptr opencl_cache  = pro.get_input<boxm2_opencl_cache_sptr>(i++);
  vpgl_camera_double_sptr cam           = pro.get_input<vpgl_camera_double_sptr>(i++);
  vil_image_view_base_sptr img          = pro.get_input<vil_image_view_base_sptr>(i++);
  vcl_string ident                      = pro.get_input<vcl_string>(i++);

  long binCache = opencl_cache.ptr()->bytes_in_cache();
  vcl_cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<vcl_endl;

  bool foundDataType = false, foundNumObsType = false;
  vcl_string data_type, num_obs_type, num_obs_type_short, options;
  vcl_vector<vcl_string> apps = scene->appearances();
  // int appTypeSize;
  for (unsigned int i=0; i<apps.size(); ++i) {
    if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
    {
      data_type = apps[i];
      foundDataType = true;
      options=" -D MOG_TYPE_8 ";
      // appTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
    }
    else if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
    {
      data_type = apps[i];
      foundDataType = true;
      options=" -D MOG_TYPE_16 ";
      // appTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix());
    }
    else if ( apps[i] == boxm2_data_traits<BOXM2_NUM_OBS>::prefix() )
    {
      num_obs_type = boxm2_data_traits<BOXM2_NUM_OBS_SINGLE_INT>::prefix();
#if 0
      num_obs_type_short = boxm2_data_traits<BOXM2_NUM_OBS_SINGLE>::prefix();
#endif
      foundNumObsType = true;
    }
  }
  if (!foundDataType) {
    vcl_cout<<"BOXM2_OCL_UPDATE_AUX_PER_VIEW_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<vcl_endl;
    return false;
  }
  if (!foundNumObsType) {
    vcl_cout<<"BOXM2_OPENCL_UPDATE_PROCESS ERROR: scene doesn't have BOXM2_NUM_OBS type"<<vcl_endl;
    return false;
  }
  if (ident.size() > 0) {
    num_obs_type += "_" + ident;
#if 0
    num_obs_type_short += "_" + ident;
#endif
  }

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),
    CL_QUEUE_PROFILING_ENABLE,&status);
  if (status!=0) return false;

  vcl_string identifier=device->device_identifier()+options;
  // compile the kernel if not already compiled
  if (kernels.find(identifier)==kernels.end())
  {
    vcl_cout<<"===========Compiling kernels==========="<<vcl_endl;
    vcl_vector<bocl_kernel*> ks;
    compile_kernel(device,ks,options);
    kernels[identifier]=ks;
  }

  //grab input image, establish cl_ni, cl_nj (so global size is divisible by local size)
  vil_image_view_base_sptr float_img=boxm2_util::prepare_input_image(img,true);
  vil_image_view<float>* img_view = static_cast<vil_image_view<float>* >(float_img.ptr());
  unsigned cl_ni=(unsigned)RoundUp(img_view->ni(),(int)local_threads[0]);
  unsigned cl_nj=(unsigned)RoundUp(img_view->nj(),(int)local_threads[1]);
  global_threads[0]=cl_ni;
  global_threads[1]=cl_nj;

  //set generic cam
  cl_float* ray_origins = new cl_float[4*cl_ni*cl_nj];
  cl_float* ray_directions = new cl_float[4*cl_ni*cl_nj];
  bocl_mem_sptr ray_o_buff = new bocl_mem(device->context(), ray_origins, cl_ni*cl_nj * sizeof(cl_float4) , "ray_origins buffer");
  bocl_mem_sptr ray_d_buff = new bocl_mem(device->context(), ray_directions,  cl_ni*cl_nj * sizeof(cl_float4), "ray_directions buffer");
  boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

  //Visibility, Preinf, Norm, and input image buffers
  float* input_buff=new float[cl_ni*cl_nj];
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
  for (unsigned int i=0; i<kernels[identifier].size(); ++i)
  {
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
      //choose correct render kernel
      boxm2_block_metadata mdata = scene->get_block_metadata(*id);
      bocl_kernel* kern =  kernels[identifier][i];

      //write the image values to the buffer
      vul_timer transfer;
      bocl_mem* blk       = opencl_cache->get_block(*id);
      bocl_mem* blk_info  = opencl_cache->loaded_block_info();
      bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(*id,0,false);
      boxm2_scene_info* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
      int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
      info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
      blk_info->write_to_buffer((queue));
      // data type string may contain an identifier so determine the buffer size
      //bocl_mem* mog       = opencl_cache->get_data(*id,data_type,alpha->num_bytes()/alphaTypeSize*appTypeSize,false);

      //vcl_cout << "Printing contents of lru cache: " << *(static_cast<boxm2_lru_cache*>( opencl_cache->get_cpu_cache().ptr() )) << vcl_endl;

      transfer_time += (float) transfer.all();
      if (i==UPDATE_CREATE_NORM)
      {
        //allocate appropriate size bocl_mem objects for number of observations, segment lengths and normalized intensities.
        int nobsTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NUM_OBS_SINGLE_INT>::prefix());
        bocl_mem* num_obs   = opencl_cache->get_data(*id,num_obs_type, alpha->num_bytes()/alphaTypeSize*nobsTypeSize,false);

        int auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
        bocl_mem *aux0  = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_AUX0>::prefix(ident),info_buffer->data_buffer_length*auxTypeSize,false);

        auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX1>::prefix());
        bocl_mem *aux1  = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_AUX1>::prefix(ident),info_buffer->data_buffer_length*auxTypeSize,false);

        aux0->zero_gpu_buffer(queue);
        aux1->zero_gpu_buffer(queue);
        num_obs->zero_gpu_buffer(queue);

        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( aux0 );
        kern->set_arg( aux1 );
        kern->set_arg( num_obs );
        kern->set_arg( lookup.ptr() );

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

        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        check_val(status, MEM_FAILURE, "SEGLEN EXECUTE FAILED: " + error_to_string(status));
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();

        //read results to cpu memory
        aux0->read_to_buffer(queue);
        aux1->read_to_buffer(queue);
        num_obs->read_to_buffer(queue);

#ifdef DEBUG
        int * aux0_f=static_cast<int*> (aux0->cpu_buffer());
        int * aux2_f=static_cast<int*> (aux1->cpu_buffer());
        unsigned int * num_obs_f=static_cast<unsigned int*> (num_obs->cpu_buffer());
        for (unsigned k=0;k<1000;k++)
          vcl_cout<< aux0_f[k] / 100000.0f << ' ' <<  aux2_f[k] / 100000.0f << ' ' <<  num_obs_f[k] << ',' << vcl_endl;

        vcl_cout << "Printing contents of lru cache: "
                 << static_cast<boxm2_lru_cache*>( opencl_cache->get_cpu_cache().ptr() ) << vcl_endl;
#endif
      }
#if 0
      else if (i==CONVERT_NOBS_INT_SHORT)
      {
        int nobsTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NUM_OBS_SINGLE_INT>::prefix());
        bocl_mem* num_obs   = opencl_cache->get_data(*id,num_obs_type,0,true);

        //////////////////////////////

        int nobsTypeSizeShort = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NUM_OBS_SINGLE>::prefix());
        bocl_mem* num_obsShort   = opencl_cache->get_data(*id, num_obs_type_short, info_buffer->data_buffer_length*nobsTypeSizeShort);
        //num_obsShort->zero_gpu_buffer(queue);

        local_threads[0] = 64;
        local_threads[1] = 1;
        global_threads[0]=RoundUp(info_buffer->data_buffer_length,local_threads[0]);
        global_threads[1]=1;

        kern->set_arg( blk_info );
        kern->set_arg( num_obs );
        kern->set_arg( num_obsShort );


        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        check_val(status, MEM_FAILURE, "CONVERT NOBS TO SHORT EXECUTE FAILED: " + error_to_string(status));
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();

        //write info to disk
        num_obs->read_to_buffer(queue);

        /////////////////////
        //clFinish(queue);
#ifdef DEBUG
        unsigned int* num_obs_f=static_cast<unsigned int*> (num_obs->cpu_buffer());
        for (unsigned k=0;k<100;k++)
          vcl_cout<< num_obs_f[k] << ',' << vcl_endl;
#endif
      }
#endif
      clFinish(queue);
    }
  }

  delete [] input_buff;
  delete [] ray_origins;
  delete [] ray_directions;

  vcl_cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<vcl_endl;
  clReleaseCommandQueue(queue);
  return true;
}
