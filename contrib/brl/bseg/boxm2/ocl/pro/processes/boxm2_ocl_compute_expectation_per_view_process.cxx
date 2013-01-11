// This is brl/bseg/boxm2/ocl/pro/processes/boxm2_ocl_compute_expectation_per_view_process.cxx
//:
// \file
// \brief   This process computes the expectation (probability that voxel produced the intensity along the ray) of each voxel along image rays.
//  Its main purpose is to save the expectation in BOXM2_EXPECTATION as well as the intensity observed in BOXM2_PIXEL.
//  The process accounts for the variable number of rays that pass through a voxel by keeping this information in BOXM2_NUM_OBS_SINGLE_INT.
//  Once the total number of rays in all voxels is known, it allocates appropriately sized buffers to hold BOXM2_EXPECTATION and BOXM2_PIXEL.
//  The data start indices of each voxel is saved in BOXM2_DATA_INDEX, which together with BOXM2_NUM_OBS_SINGLE_INT, describe the data layout.
//  Furthermore, the process also saves segment lengths of each ray (BOXM2_AUX3) and the sum of expectations prior to the voxel
//  (BOXM2_AUX2), via the same method. These quantities will be used to update the scene in boxm2_ocl_batch_update_scene_process.
//
// \author Ali Osman Ulusoy
// \date Jun 12, 2012


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

namespace boxm2_ocl_compute_expectation_per_view_process_globals
{
  const unsigned n_inputs_  = 8;
  const unsigned n_outputs_ = 2;
  enum {
    COMPUTE_SEGLEN = 0,
    COMPUTE_EXPSUM = 1,
    REINIT_VIS = 2,
    COMPUTE_EXPECTATION = 3,
    CONVERT_EXP = 4
  };

  void compile_kernel(bocl_device_sptr device,vcl_vector<bocl_kernel*> & vec_kernels,vcl_string opts)
  {
    //gather all render sources... seems like a lot for rendering...
    vcl_vector<vcl_string> src_paths;
    vcl_string source_dir = boxm2_ocl_util::ocl_src_root();
    src_paths.push_back(source_dir + "scene_info.cl");
    src_paths.push_back(source_dir + "pixel_conversion.cl");
    src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
    src_paths.push_back(source_dir + "backproject.cl");
    src_paths.push_back(source_dir + "statistics_library_functions.cl");
    src_paths.push_back(source_dir + "batch/em_kernels.cl");
    vcl_vector<vcl_string> non_ray_tracing_paths = vcl_vector<vcl_string>(src_paths);

    src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
    src_paths.push_back(source_dir + "batch/em_functors.cl");
    src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");
    vcl_string options = " -D INTENSITY -D DETERMINISTIC" + opts;


    //seg len pass
    bocl_kernel* seg_len = new bocl_kernel();
    vcl_string seg_opts = options + " -D SEGLENNOBS -D STEP_CELL=step_cell_seglen(aux_args,data_ptr,llid,d)";
    seg_len->create_kernel(&device->context(), device->device_id(), src_paths, "seg_len_nobs_main", seg_opts, "batch_update::seg_len_nobs");
    vec_kernels.push_back(seg_len);

    //exp sum
    bocl_kernel* exp_sum = new bocl_kernel();
    vcl_string pre_opts = options + " -D EXPSUM -D STEP_CELL=step_cell_expsum(aux_args,data_ptr,llid,d)";
    exp_sum->create_kernel(&device->context(), device->device_id(), src_paths, "exp_sum_main", pre_opts, "batch_update::exp_sum");
    vec_kernels.push_back(exp_sum);

    bocl_kernel* proc_img = new bocl_kernel();
    vcl_string proc_opts = options + " -D REINIT_VIS ";
    proc_img->create_kernel(&device->context(), device->device_id(), non_ray_tracing_paths, "reinit_vis_image", proc_opts, "update::reinit_vis_image");
    vec_kernels.push_back(proc_img);

    //compute expectation
    bocl_kernel* expectation = new bocl_kernel();
    vcl_string expectation_opts = options + " -D EXPECTATION -D STEP_CELL=step_cell_expectation(aux_args,data_ptr,llid,d)";
    expectation->create_kernel(&device->context(), device->device_id(), src_paths, "expectation_main", expectation_opts, "batch_update::expectation");
    vec_kernels.push_back(expectation);

    //normalize aux and convert to float.
    bocl_kernel* convert_exp_to_float = new bocl_kernel();
    vcl_string convert_opts2 = options + " -D CONVERT_EXP ";
    convert_exp_to_float->create_kernel(&device->context(),device->device_id(), non_ray_tracing_paths, "convert_exp_to_float", convert_opts2, "batch_update::convert_exp_to_float");
    vec_kernels.push_back(convert_exp_to_float);


    return ;
  }


  static vcl_map<vcl_string,vcl_vector<bocl_kernel*> > kernels;
}

bool boxm2_ocl_compute_expectation_per_view_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_ocl_compute_expectation_per_view_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bocl_device_sptr";
  input_types_[1] = "boxm2_scene_sptr";
  input_types_[2] = "boxm2_cache_sptr";
  input_types_[3] = "boxm2_opencl_cache_sptr";
  input_types_[4] = "vpgl_camera_double_sptr";
  input_types_[5] = "vil_image_view_base_sptr";
  input_types_[6] = "vcl_string";
  input_types_[7] = "vil_image_view_base_sptr";

  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "float";

  bool good = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
  // in case the 6th input is not set
  brdb_value_sptr idx6 = new brdb_value_t<vcl_string>("");
  pro.set_input(6, idx6);

  // in case the 7th input is not set
  brdb_value_sptr empty_mask = new brdb_value_t<vil_image_view_base_sptr>(new vil_image_view<unsigned char>(1,1));
  pro.set_input(7, empty_mask);

  return good;
}

bool boxm2_ocl_compute_expectation_per_view_process(bprb_func_process& pro)
{
  using namespace boxm2_ocl_compute_expectation_per_view_process_globals;
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
  vil_image_view_base_sptr img          = pro.get_input<vil_image_view_base_sptr>(i++);
  vcl_string suffix                     = pro.get_input<vcl_string>(i++);
  vil_image_view_base_sptr mask_sptr    = pro.get_input<vil_image_view_base_sptr>(i++);

  long binCache = opencl_cache.ptr()->bytes_in_cache();
  vcl_cout<<"Update MBs in cache: "<<binCache/(1024.0*1024.0)<<vcl_endl;

  //catch a "null" mask (not really null because that throws an error)
  bool use_mask = false;
  if ( mask_sptr->ni() == img->ni() && mask_sptr->nj() == img->nj() ) {
    vcl_cout<<"Update using mask."<<vcl_endl;
    use_mask = true;
  }

  vil_image_view<unsigned char >* mask_map = 0;
  if (use_mask) {
    mask_map = dynamic_cast<vil_image_view<unsigned char> *>(mask_sptr.ptr());
    if (!mask_map) {
      vcl_cout<<"boxm2_ocl_compute_expectation_per_view_process:: mask map is not an unsigned char map"<<vcl_endl;
      return false;
    }
  }

  bool foundDataType = false;
  vcl_string data_type,num_obs_type,options;
  vcl_vector<vcl_string> apps = scene->appearances();
  int appTypeSize;
  for (unsigned int i=0; i<apps.size(); ++i) {
    if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
    {
      data_type = apps[i];
      foundDataType = true;
      options=" -D MOG_TYPE_8 ";
      appTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
    }
  }
  if (!foundDataType) {
    vcl_cout<<"BOXM2_OCL_COMPUTE_EXPECTATION_PER_VIEW_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY data type"<<vcl_endl;
    return false;
  }

  // create a command queue.
  int status=0;
  cl_command_queue queue = clCreateCommandQueue(device->context(),*(device->device_id()),CL_QUEUE_PROFILING_ENABLE,&status);
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
  vil_image_view_base_sptr float_img=boxm2_util::prepare_input_image(img);
  vil_image_view<float>* img_view = static_cast<vil_image_view<float>* >(float_img.ptr());
  unsigned cl_ni=(unsigned)RoundUp(img_view->ni(),(int)local_threads[0]);
  unsigned cl_nj=(unsigned)RoundUp(img_view->nj(),(int)local_threads[1]);
  global_threads[0]=cl_ni;
  global_threads[1]=cl_nj;

  //set generic cam
  cl_float* ray_origins = new cl_float[4*cl_ni*cl_nj];
  cl_float* ray_directions = new cl_float[4*cl_ni*cl_nj];
  //vcl_cout << "allocating ray_o_buff: ni = " << cl_ni << ", nj = " << cl_nj << "  size = " << cl_ni*cl_nj*sizeof(cl_float4) << vcl_endl;
  bocl_mem_sptr ray_o_buff = opencl_cache->alloc_mem( cl_ni*cl_nj * sizeof(cl_float4) , ray_origins,"ray_origins buffer");
  //vcl_cout << "allocating ray_d_buff: ni = " << cl_ni << ", nj = " << cl_nj << vcl_endl;
  bocl_mem_sptr ray_d_buff = opencl_cache->alloc_mem(cl_ni*cl_nj * sizeof(cl_float4), ray_directions,"ray_directions buffer");
  boxm2_ocl_camera_converter::compute_ray_image( device, queue, cam, cl_ni, cl_nj, ray_o_buff, ray_d_buff);

  //Visibility, Preinf, Norm, and input image buffers
  float* vis_buff = new float[cl_ni*cl_nj];
  float* exp_denom_buff = new float[cl_ni*cl_nj];
  float* pre_exp_num_buff = new float[cl_ni*cl_nj];
  float* pi_inf_buff = new float[cl_ni*cl_nj];


  for (unsigned i=0;i<cl_ni*cl_nj;i++)
  {
    vis_buff[i]=1.0f;
    exp_denom_buff[i]=0.0f;
    pre_exp_num_buff[i]=0.0f;
    pi_inf_buff[i]=0.0f;
  }


  float* input_buff=new float[cl_ni*cl_nj];
  //copy input vals into image
  int count=0;
  for (unsigned int j=0;j<cl_nj;++j) {
    for (unsigned int i=0;i<cl_ni;++i) {
      input_buff[count] = 0.0f;
      if ( i<img_view->ni() && j< img_view->nj() )
      {
        if (use_mask)
        {
          if ((*mask_map)(i,j) > 0 )
            input_buff[count] = (*img_view)(i,j);
          else
            input_buff[count] = -1.0;
        }
        else
          input_buff[count] = (*img_view)(i,j);
       }
      ++count;
    }
  }

  //vcl_cout << "allocating input_buff: ni = " << cl_ni << ", nj = " << cl_nj << vcl_endl;
  bocl_mem_sptr in_image=opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float),input_buff,"input image buffer");
  in_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //vcl_cout << "allocating vis_buff: ni = " << cl_ni << ", nj = " << cl_nj << vcl_endl;
  bocl_mem_sptr vis_image=opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float),vis_buff,"vis image buffer");
  vis_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr exp_denom_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), exp_denom_buff, "expectation denom image buffer");
  exp_denom_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr pre_exp_num_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), pre_exp_num_buff, "pre-expectation numerator image buffer");
  pre_exp_num_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  bocl_mem_sptr pi_inf_image = opencl_cache->alloc_mem(cl_ni*cl_nj*sizeof(float), pi_inf_buff, "pi infinity image buffer");
  pi_inf_image->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

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
  bocl_mem_sptr  cl_output = opencl_cache->alloc_mem(sizeof(float)*100, output_arr,"output buffer");
  cl_output->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // bit lookup buffer
  cl_uchar lookup_arr[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem_sptr lookup =  opencl_cache->alloc_mem(sizeof(cl_uchar)*256, lookup_arr,"bit lookup buffer");
  lookup->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);


  //BACKGROUND MODEL
  float app_buffer[4]={0.0,0.00,0.00,0.0}; //var = 0.03 for middlebury
  bocl_mem_sptr app_density = new bocl_mem(device->context(), app_buffer, sizeof(cl_float4), "app density buffer");
  app_density->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  // set arguments
  vcl_vector<boxm2_block_id> vis_order = scene->get_vis_blocks(cam);
  vcl_vector<boxm2_block_id>::iterator id;
  for (unsigned int i=0; i<kernels[identifier].size(); i++)
  {
    vcl_cout << "Running kernel no " << i << '\n'
             << "==================================" << vcl_endl;

    if ( i == REINIT_VIS ) {
      bocl_kernel * proc_kern=kernels[identifier][i];

      proc_kern->set_arg( img_dim.ptr() );
      proc_kern->set_arg( in_image.ptr() );
      proc_kern->set_arg( vis_image.ptr() );
      proc_kern->set_arg( exp_denom_image.ptr() );
      proc_kern->set_arg( pi_inf_image.ptr() );
      proc_kern->set_arg( app_density.ptr());

      //execute kernel
      proc_kern->execute( queue, 2, local_threads, global_threads);
      int status = clFinish(queue);
      if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
        return false;
      proc_kern->clear_args();

      //Compute the log likelihood for this image
      exp_denom_image->read_to_buffer(queue);
      pi_inf_image->read_to_buffer(queue);

      vil_image_view<float>* exp_img_out=new vil_image_view<float>(cl_ni,cl_nj);
      for (unsigned c=0;c<cl_nj;c++)
        for (unsigned r=0;r<cl_ni;r++)
          (*exp_img_out)(r,c)=vis_buff[c*cl_ni+r];
      pro.set_output_val<vil_image_view_base_sptr>(0, exp_img_out);

      float expectation_sum = 0;
      for (unsigned int i=0;i<cl_ni*cl_nj;++i) {
          if (exp_denom_buff[i] > 0)
            expectation_sum += vcl_log(exp_denom_buff[i]);
      }
      pro.set_output_val<float>(1, expectation_sum);


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
      bocl_mem* blk_info  = opencl_cache->loaded_block_info();
      bocl_mem* alpha     = opencl_cache->get_data<BOXM2_ALPHA>(*id,0,false);
      boxm2_scene_info* info_buffer = (boxm2_scene_info*) blk_info->cpu_buffer();
      int alphaTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
      info_buffer->data_buffer_length = (int) (alpha->num_bytes()/alphaTypeSize);
      blk_info->write_to_buffer((queue));

      //grab an appropriately sized AUX data buffer
      int auxTypeSize  = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX0>::prefix());
      bocl_mem *aux0   = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_AUX0>::prefix(suffix),info_buffer->data_buffer_length*auxTypeSize,false);
      auxTypeSize      = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());

      //also grab mog
      bocl_mem* mog       = opencl_cache->get_data(*id,data_type,alpha->num_bytes()/alphaTypeSize*appTypeSize,false);

      transfer_time += (float) transfer.all();
      if (i==COMPUTE_SEGLEN)
      {
        int nobsTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_NUM_OBS_SINGLE_INT>::prefix());
        bocl_mem* num_obs_single   = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_NUM_OBS_SINGLE_INT>::prefix(suffix),info_buffer->data_buffer_length*nobsTypeSize,false);

        auxTypeSize = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_DATA_INDEX>::prefix());
        bocl_mem* currIdx   = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_DATA_INDEX>::prefix(suffix),info_buffer->data_buffer_length*auxTypeSize,false);

        int int_zero = 0;
        aux0->fill(queue,int_zero,"int",true);
        num_obs_single->zero_gpu_buffer(queue);
        currIdx->zero_gpu_buffer(queue);

        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( aux0 ); //cum seglen
        kern->set_arg( num_obs_single ); //num obs in voxel
        kern->set_arg( lookup.ptr() );

        // kern->set_arg( persp_cam.ptr() );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );

        kern->set_arg( img_dim.ptr() );
        kern->set_arg( in_image.ptr() );
        kern->set_arg( cl_output.ptr() );
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
        kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer

        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
          return false;
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();

        //read back num obs per cell
        num_obs_single->read_to_buffer(queue);
        unsigned int * nobs =static_cast<unsigned int*> (num_obs_single->cpu_buffer());
        unsigned int * currIdx_buf =static_cast<unsigned int*> (currIdx->cpu_buffer());
        unsigned int total_num_rays = 0;
        for (int i = 0; i < info_buffer->data_buffer_length; ++i)
        {
          currIdx_buf[i] = total_num_rays;
          total_num_rays += nobs[i];
        }
        currIdx->write_to_buffer(queue);

        vcl_cout << "Allocating " << total_num_rays << " num rays..." << vcl_endl;
        //alloc pixel obs
        int pixelTypeSize      = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_PIXEL>::prefix());
        bocl_mem* all_obs = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_PIXEL>::prefix(suffix),total_num_rays*pixelTypeSize,false);
        //alloc expectations
        int expTypeSize      = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_EXPECTATION>::prefix());
        bocl_mem* all_exp = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_EXPECTATION>::prefix(suffix),total_num_rays*expTypeSize,false);
        //alloc pre expectations
        int preexpTypeSize      = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX2>::prefix());
        bocl_mem* all_pre_exp = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_AUX2>::prefix(suffix),total_num_rays*preexpTypeSize,false);
        //alloc seglens
        int seglenTypeSize      = (int)boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX3>::prefix());
        bocl_mem* all_seglen = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_AUX3>::prefix(suffix),total_num_rays*seglenTypeSize,false);
      }
      else if (i == COMPUTE_EXPSUM)
      {
        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( mog );
        kern->set_arg( aux0 ); //cum seglen
        kern->set_arg( lookup.ptr() );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );

        kern->set_arg( img_dim.ptr() );
        kern->set_arg( in_image.ptr() );
        kern->set_arg( vis_image.ptr() );
        kern->set_arg( exp_denom_image.ptr() );
        kern->set_arg( cl_output.ptr() );
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
        kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer
        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
          return false;
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();

        exp_denom_image->read_to_buffer(queue);
        vis_image->read_to_buffer(queue);
      }
      else if (i == COMPUTE_EXPECTATION)
      {
        bocl_mem* all_obs     = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_PIXEL>::prefix(suffix));
        bocl_mem* all_exp     = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_EXPECTATION>::prefix(suffix));
        bocl_mem* all_pre_exp = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_AUX2>::prefix(suffix));
        bocl_mem* all_seglen = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_AUX3>::prefix(suffix));
        bocl_mem* num_obs_single = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_NUM_OBS_SINGLE_INT>::prefix(suffix));
        bocl_mem* currIdx   = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_DATA_INDEX>::prefix(suffix));

        //init buffers
        unsigned char zero_char = 0;
        all_obs->fill(queue,zero_char,"char", true);

        float zero_float = 0;
        all_pre_exp->fill(queue,zero_float,"float", true);
        all_seglen->fill(queue,zero_float,"float", true);

        const int minus_one = -1;
        //fill aux4 with -1s.
        all_exp->fill(queue,minus_one,"int", true);

        kern->set_arg( blk_info );
        kern->set_arg( blk );
        kern->set_arg( alpha );
        kern->set_arg( mog );
        kern->set_arg( aux0 ); //seglen
        kern->set_arg( all_pre_exp );
        kern->set_arg( all_seglen );
        kern->set_arg( all_obs ); //all obs
        kern->set_arg( all_exp ); //all exp
        kern->set_arg( currIdx ); //indices indicating where to begin writing obs/exp for each cell
        kern->set_arg( num_obs_single );    //number of obs per cell

        kern->set_arg( lookup.ptr() );
        kern->set_arg( ray_o_buff.ptr() );
        kern->set_arg( ray_d_buff.ptr() );

        kern->set_arg( img_dim.ptr() );
        kern->set_arg( in_image.ptr() );
        kern->set_arg( vis_image.ptr() );
        kern->set_arg( exp_denom_image.ptr() );
        kern->set_arg( pre_exp_num_image.ptr() );
        kern->set_arg( pi_inf_image.ptr() );
        kern->set_arg( cl_output.ptr() );
        kern->set_local_arg( local_threads[0]*local_threads[1]*sizeof(cl_uchar16) );//local tree,
        kern->set_local_arg( local_threads[0]*local_threads[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer
        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        if (!check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status)))
          return false;
        gpu_time += kern->exec_time();

        //clear render kernel args so it can reset em on next execution
        kern->clear_args();

        pre_exp_num_image->read_to_buffer(queue);
        all_exp->read_to_buffer(queue);
        all_obs->read_to_buffer(queue);
        all_pre_exp->read_to_buffer(queue);
        all_seglen->read_to_buffer(queue);
      }
      else if (i == CONVERT_EXP)
      {
        bocl_mem* all_exp     = opencl_cache->get_data(*id, boxm2_data_traits<BOXM2_EXPECTATION>::prefix(suffix));
        unsigned int total_num_rays = all_exp->num_bytes() /  sizeof(boxm2_data_traits<BOXM2_EXPECTATION>::datatype);
        vcl_cout << "Total num of rays: " << total_num_rays << vcl_endl;

        local_threads[0] = 64;
        local_threads[1] = 1;
        global_threads[0]=RoundUp(total_num_rays ,local_threads[0]);
        global_threads[1]=1;

        bocl_mem_sptr total_num_rays_gpu = new bocl_mem(device->context(), &total_num_rays, sizeof(cl_uint), "total number of rays buffer");
        total_num_rays_gpu->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

        kern->set_arg( total_num_rays_gpu.ptr() );
        kern->set_arg( all_exp );

        //execute kernel
        kern->execute(queue, 2, local_threads, global_threads);
        int status = clFinish(queue);
        check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
        gpu_time += kern->exec_time();
        //clear render kernel args so it can reset em on next execution
        kern->clear_args();

        all_exp->read_to_buffer(queue);


        opencl_cache->deep_remove_data(*id,boxm2_data_traits<BOXM2_AUX0>::prefix(suffix), true);
        opencl_cache->deep_remove_data(*id,boxm2_data_traits<BOXM2_AUX2>::prefix(suffix), true);
        opencl_cache->deep_remove_data(*id,boxm2_data_traits<BOXM2_AUX3>::prefix(suffix), true);
        opencl_cache->deep_remove_data(*id,boxm2_data_traits<BOXM2_PIXEL>::prefix(suffix ), true);
        opencl_cache->deep_remove_data(*id,boxm2_data_traits<BOXM2_NUM_OBS_SINGLE_INT>::prefix(suffix ), true);
        opencl_cache->deep_remove_data(*id,boxm2_data_traits<BOXM2_DATA_INDEX>::prefix( suffix ), true);
        opencl_cache->deep_remove_data(*id,boxm2_data_traits<BOXM2_EXPECTATION>::prefix( suffix ), true);
      }

      clFinish(queue);
    }
  }

  vcl_cout << "Done." << vcl_endl;

  opencl_cache->unref_mem(vis_image.ptr());
  opencl_cache->unref_mem(in_image.ptr());
  opencl_cache->unref_mem(exp_denom_image.ptr());
  opencl_cache->unref_mem(pre_exp_num_image.ptr());
  opencl_cache->unref_mem(pi_inf_image.ptr());
  opencl_cache->unref_mem(cl_output.ptr());
  opencl_cache->unref_mem(lookup.ptr());
  opencl_cache->unref_mem(ray_d_buff.ptr());
  opencl_cache->unref_mem(ray_o_buff.ptr());

  delete [] vis_buff;
  delete [] input_buff;
  delete [] exp_denom_buff;
  delete [] pre_exp_num_buff;
  delete [] pi_inf_buff;
  delete [] ray_origins;
  delete [] ray_directions;

  vcl_cout<<"Gpu time "<<gpu_time<<" transfer time "<<transfer_time<<vcl_endl;
  clReleaseCommandQueue(queue);
  return true;
}
