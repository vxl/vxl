#include "boxm2_opencl_update_process.h"

//boxm2 data structures
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <vil/vil_save.h>
#include <vil/vil_math.h>
#include <vil/vil_load.h>
#include <boxm2/ocl/boxm2_ocl_util.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>

//TODO IN THIS INIT METHOD: Need to pass in a ref to the OPENCL_CACHE so this
//class can easily access BOCL_MEMs
bool boxm2_opencl_update_process::init_kernel(cl_context* context,
                                              cl_device_id* device,
                                              vcl_string opts)
{
  context_ = context;

  vcl_vector<vcl_string> src_paths;
  vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "cell_utils.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "backproject.cl");
  src_paths.push_back(source_dir + "statistics_library_functions.cl");
  src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
  src_paths.push_back(source_dir + "bit/update_kernels.cl");
  vcl_vector<vcl_string> non_ray_src = vcl_vector<vcl_string>(src_paths);
  src_paths.push_back(source_dir + "update_functors.cl");
  src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

  //compilation options
  vcl_string options = " -D INTENSITY ";
  options += " -D DETERMINISTIC ";
  options += opts;

  //create all passes
  bocl_kernel* seg_len = new bocl_kernel();
  vcl_string seg_opts = options + " -D SEGLEN -D STEP_CELL=step_cell_seglen(aux_args,data_ptr,llid,d) ";
  seg_len->create_kernel(context_, device, src_paths, "seg_len_main", seg_opts, "update::seg_len");
  update_kernels_.push_back(seg_len);

  bocl_kernel* pre_inf = new bocl_kernel();
  vcl_string pre_opts = options + " -D PREINF -D STEP_CELL=step_cell_preinf(aux_args,data_ptr,llid,d) ";
  pre_inf->create_kernel(context_, device, src_paths, "pre_inf_main", pre_opts, "update::pre_inf");
  update_kernels_.push_back(pre_inf);

  //may need DIFF LIST OF SOURCES FOR THIS GUY
  bocl_kernel* proc_img = new bocl_kernel();
  proc_img->create_kernel(context_, device, non_ray_src, "proc_norm_image", options, "update::proc_norm_image");
  update_kernels_.push_back(proc_img);

  //push back cast_ray_bit
  bocl_kernel* bayes_main = new bocl_kernel();
  vcl_string bayes_opt = options + " -D BAYES -D STEP_CELL=step_cell_bayes(aux_args,data_ptr,llid,d) ";
  bayes_main->create_kernel(context_, device, src_paths, "bayes_main", bayes_opt, "update::bayes_main");
  update_kernels_.push_back(bayes_main);

  //may need DIFF LIST OF SOURCES FOR THSI GUY TOO
  bocl_kernel* update = new bocl_kernel();
  update->create_kernel(context_, device, non_ray_src, "update_bit_scene_main", options, "update::update_main");
  update_kernels_.push_back(update);

  return true;
}

// Opencl Update Process
// arguments will be (should be)
// * scene pointer
// * camera (for input image)
// * input image
// * visibility image...
bool boxm2_opencl_update_process::execute(vcl_vector<brdb_value_sptr>& input, vcl_vector<brdb_value_sptr>& output)
{
  transfer_time_ = 0.0f; gpu_time_ = 0.0f; total_time_ = 0.0f;
  vul_timer total;
  int inIdx = 0;

  //scene argument and APP_TYPE and OBS_TYPE (if needed)
  brdb_value_t<boxm2_scene_sptr>* scene_brdb = static_cast<brdb_value_t<boxm2_scene_sptr>* >( input[inIdx++].ptr() );
  boxm2_scene_sptr scene = scene_brdb->value();
  bool foundDataType = false, foundNumObsType = false;
  vcl_vector<vcl_string> apps = scene->appearances();
  for (unsigned int i=0; i<apps.size(); ++i) {
    if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() || apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() )
    {
      data_type_ = apps[i];
      foundDataType = true;
    }
    else if ( apps[i] == boxm2_data_traits<BOXM2_NUM_OBS>::prefix() )
    {
      num_obs_type_ = apps[i];
      foundNumObsType = true;
    }
  }
  if (!foundDataType) {
    vcl_cout<<"BOXM2_OPENCL_UPDATE_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<vcl_endl;
    return false;
  }
  if (!foundNumObsType) {
    vcl_cout<<"BOXM2_OPENCL_UPDATE_PROCESS ERROR: scene doesn't have BOXM2_NUM_OBS type"<<vcl_endl;
    return false;
  }


  //camera
  brdb_value_t<vpgl_camera_double_sptr>* brdb_cam = static_cast<brdb_value_t<vpgl_camera_double_sptr>* >( input[inIdx++].ptr() );
  vpgl_camera_double_sptr cam = brdb_cam->value();
  cl_float* cam_buffer = new cl_float[16*3];
  boxm2_ocl_util::set_persp_camera(cam, cam_buffer);
  persp_cam_ = new bocl_mem((*context_), cam_buffer, 3*sizeof(cl_float16), "persp cam buffer");
  persp_cam_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //input image buffer
  brdb_value_t<vil_image_view_base_sptr>* brdb_img = static_cast<brdb_value_t<vil_image_view_base_sptr>* >( input[inIdx++].ptr() );
  vil_image_view_base_sptr img = brdb_img->value();
  vil_image_view<float>* img_view = static_cast<vil_image_view<float>* >(img.ptr());
  this->write_input_image(img_view);

  /////////////////////// NEW IMAGE BUFFERS ////////////////////////////////////
  //vis image buffer
  float* vis_buffer = new float[img_view->size()];
  for (unsigned int i=0; i<img_view->size(); ++i) vis_buffer[i] = 1.0f;
  if (vis_image_) delete vis_image_;
  vis_image_ = new bocl_mem((*context_), vis_buffer, img_view->size()*sizeof(cl_float), "vis_image_ buffer");
  vis_image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //pre buffer
  float* pre_buffer = new float[img_view->size()];
  for (unsigned int i=0; i<img_view->size(); ++i) pre_buffer[i] = 0.0f;
  if (pre_image_) delete pre_image_;
  pre_image_ = new bocl_mem((*context_), pre_buffer, img_view->size()*sizeof(cl_float), "pre_image_ buffer");
  pre_image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //norm image buffer
  float* norm_buffer = new float[img_view->size()];
  for (unsigned int i=0; i<img_view->size(); ++i) norm_buffer[i] = 0.0f;
  if (norm_image_) delete norm_image_;
  norm_image_ = new bocl_mem((*context_), norm_buffer, img_view->size()*sizeof(cl_float), "norm_image_ buffer");
  norm_image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

#if 0 //////////////////////////////////////////////////////////////////////////////
  //store data type
  brdb_value_t<float>* brdb_init_sigma = static_cast<brdb_value_t<float>* >( input[inIdx++].ptr() );
  float init_sigma=brdb_data_type->value();
  //store data type
  brdb_value_t<vcl_string>* brdb_data_type = static_cast<brdb_value_t<vcl_string>* >( input[inIdx++].ptr() );
  data_type_=brdb_data_type->value();
#endif //////////////////////////////////////////////////////////////////////////////

  //exp image dimensions
  img_size_[0] = img_view->ni();
  img_size_[1] = img_view->nj();
  int* img_dim_buff = new int[4];
  img_dim_buff[0] = 0;
  img_dim_buff[1] = 0;
  img_dim_buff[2] = img_view->ni();
  img_dim_buff[3] = img_view->nj();
  img_dim_ = new bocl_mem((*context_), img_dim_buff, sizeof(cl_int4), "image dims");
  img_dim_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //output buffer
  float* output_arr = new float[500];
  for (int i=0; i<500; ++i) output_arr[i] = 0.0f;
  cl_output_ = new bocl_mem((*context_), output_arr, sizeof(float)*500, "output buffer");
  cl_output_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //bit lookup buffer
  cl_uchar* lookup_arr = new cl_uchar[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  lookup_ = new bocl_mem((*context_), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  // app density used for proc_norm_image
  float* app_buffer = new float[4];
  app_buffer[0] = 1.0f;
  app_buffer[1] = 0.0f;
  app_buffer[2] = 0.0f;
  app_buffer[3] = 0.0f;
  app_density_ = new bocl_mem((*context_), app_buffer, sizeof(cl_float4), "app density buffer");
  app_density_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //For each ID in the visibility order, grab that block
  vcl_vector<boxm2_block_id> vis_order = scene->get_vis_blocks( (vpgl_perspective_camera<double>*) cam.ptr());
  vcl_vector<boxm2_block_id>::iterator id;

  //Go through each kernel, execute on each block
  for (unsigned int i=0; i<update_kernels_.size(); ++i)
  {
#if 1
    vcl_cout<<"UPDATE KERNEL : "<<i<<vcl_endl;
#endif
    if ( i == UPDATE_PROC ) {
      this->set_workspace(i);
      this->set_args(i);

      //execute kernel
      update_kernels_[i]->execute( (*command_queue_), 2, lThreads_, gThreads_);
      int status = clFinish(*command_queue_);
      check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
      update_kernels_[i]->clear_args();
      norm_image_->read_to_buffer(*command_queue_);
      continue;
    }

    //zip through visible blocks, and execute this pass's kernel
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
      //write the image values to the buffer
      vul_timer transfer;
      blk_       = cache_->get_block(*id);
      alpha_     = cache_->get_data<BOXM2_ALPHA>(*id);
      mog_       = cache_->get_data(*id, data_type_);
      num_obs_   = cache_->get_data(*id, num_obs_type_);
      blk_info_  = cache_->loaded_block_info();

      //make sure the data_len field in the info_buffer reflects the true data length
      boxm2_block_metadata mdata = scene->get_block_metadata(*id);
      if (!mdata.random_) {
        boxm2_scene_info* info_buffer = (boxm2_scene_info*) blk_info_->cpu_buffer();
        int alphaTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_ALPHA>::prefix());
        info_buffer->data_buffer_length = (int) (alpha_->num_bytes()/alphaTypeSize);
        blk_info_->write_to_buffer((*command_queue_));

        //grab an appropriately sized AUX data buffer
        int auxTypeSize = boxm2_data_info::datasize(boxm2_data_traits<BOXM2_AUX>::prefix());
        aux_       = cache_->get_data<BOXM2_AUX>(*id, info_buffer->data_buffer_length*auxTypeSize);
        vcl_cout<<"Alpha buffer length: "<<info_buffer->data_buffer_length<<vcl_endl;
      }
      else  {
        //get aux data
        aux_       = cache_->get_data<BOXM2_AUX>(*id);
      }
      transfer_time_ += (float) transfer.all();

      //set workspace and args for this pass
      this->set_workspace(i);
      this->set_args(i);

      //execute kernel
      update_kernels_[i]->execute( (*command_queue_), 2, lThreads_, gThreads_);
      int status = clFinish(*command_queue_);
      check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
      gpu_time_ += update_kernels_[i]->exec_time();

      //clear render kernel args so it can reset em on next execution
      update_kernels_[i]->clear_args();

      //write info to disk
      blk_->read_to_buffer(*command_queue_);
      alpha_->read_to_buffer(*command_queue_);
      mog_->read_to_buffer(*command_queue_);
      num_obs_->read_to_buffer(*command_queue_);
      aux_->read_to_buffer(*command_queue_);

      //read image out to buffer (from gpu)
      image_->read_to_buffer(*command_queue_);
      vis_image_->read_to_buffer(*command_queue_);
      pre_image_->read_to_buffer(*command_queue_);
      cl_output_->read_to_buffer(*command_queue_);
      clFinish(*command_queue_);
    }
  }

  //clean up camera, lookup_arr, img_dim_buff
  delete[] output_arr;
  delete[] img_dim_buff;
  delete[] lookup_arr;
  delete[] cam_buffer;
  delete[] app_buffer;

  delete cl_output_;
  delete persp_cam_;
  delete img_dim_;
  delete lookup_;
  delete app_density_;

  //record total time
  total_time_ = (float) total.all();
  return true;
}

bool boxm2_opencl_update_process::clean()
{
  return true;
}

bool boxm2_opencl_update_process::set_workspace(unsigned int pass)
{
  switch (pass) {
    case UPDATE_SEGLEN:
    case UPDATE_PREINF:
    case UPDATE_PROC:
    case UPDATE_BAYES:
      lThreads_[0]  = 8;
      lThreads_[1]  = 8;
      gThreads_[0] = RoundUp(img_size_[0],lThreads_[0]);
      gThreads_[1] = RoundUp(img_size_[1],lThreads_[1]);
      break;
    case UPDATE_CELL:
    {
      boxm2_scene_info* info_buffer = (boxm2_scene_info*) blk_info_->cpu_buffer();
      int numbuf = info_buffer->num_buffer;
      int datlen = info_buffer->data_buffer_length;
      vcl_cout<<" update shape: num_buff="<<numbuf<<",bufflength:"<<datlen<<vcl_endl;
      gThreads_[0] = RoundUp(numbuf*datlen,64);
      gThreads_[1] = 1;
      lThreads_[0]  = 64;
      lThreads_[1]  = 1;
      break;
    }
  }
  return true;
}


bool boxm2_opencl_update_process::set_args(unsigned int pass)
{
  switch (pass)
  {
    case UPDATE_SEGLEN :
      update_kernels_[pass]->set_arg( blk_info_ );
      update_kernels_[pass]->set_arg( blk_ );
      update_kernels_[pass]->set_arg( alpha_ );
      update_kernels_[pass]->set_arg( aux_ );
      update_kernels_[pass]->set_arg( lookup_ );
      update_kernels_[pass]->set_arg( persp_cam_ );
      update_kernels_[pass]->set_arg( img_dim_ );
      update_kernels_[pass]->set_arg( image_ );
      update_kernels_[pass]->set_arg( cl_output_ );
      update_kernels_[pass]->set_local_arg( lThreads_[0]*lThreads_[1]*sizeof(cl_uchar16) );//local tree,
      update_kernels_[pass]->set_local_arg( lThreads_[0]*lThreads_[1]*sizeof(cl_uchar4) ); //ray bundle,
      update_kernels_[pass]->set_local_arg( lThreads_[0]*lThreads_[1]*sizeof(cl_int) );    //cell pointers,
      update_kernels_[pass]->set_local_arg( lThreads_[0]*lThreads_[1]*sizeof(cl_float4) ); //cached aux,
      update_kernels_[pass]->set_local_arg( lThreads_[0]*lThreads_[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer
      break;
    case UPDATE_PREINF :
      update_kernels_[pass]->set_arg( blk_info_ );
      update_kernels_[pass]->set_arg( blk_ );
      update_kernels_[pass]->set_arg( alpha_ );
      update_kernels_[pass]->set_arg( mog_ );
      update_kernels_[pass]->set_arg( num_obs_ );
      update_kernels_[pass]->set_arg( aux_ );
      update_kernels_[pass]->set_arg( lookup_ );
      update_kernels_[pass]->set_arg( persp_cam_ );
      update_kernels_[pass]->set_arg( img_dim_ );
      update_kernels_[pass]->set_arg( vis_image_ );
      update_kernels_[pass]->set_arg( pre_image_ );
      update_kernels_[pass]->set_arg( cl_output_ );
      update_kernels_[pass]->set_local_arg( lThreads_[0]*lThreads_[1]*sizeof(cl_uchar16) );//local tree,
      update_kernels_[pass]->set_local_arg( lThreads_[0]*lThreads_[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer
      break;
    case UPDATE_PROC :
      update_kernels_[pass]->set_arg( norm_image_ );
      update_kernels_[pass]->set_arg( vis_image_ );
      update_kernels_[pass]->set_arg( pre_image_ );
      update_kernels_[pass]->set_arg( img_dim_ );
      break;
    case UPDATE_BAYES :
      update_kernels_[pass]->set_arg( blk_info_ );
      update_kernels_[pass]->set_arg( blk_ );
      update_kernels_[pass]->set_arg( alpha_ );
      update_kernels_[pass]->set_arg( mog_ );
      update_kernels_[pass]->set_arg( num_obs_ );
      update_kernels_[pass]->set_arg( aux_ );
      update_kernels_[pass]->set_arg( lookup_ );
      update_kernels_[pass]->set_arg( persp_cam_ );
      update_kernels_[pass]->set_arg( img_dim_ );
      update_kernels_[pass]->set_arg( vis_image_ );
      update_kernels_[pass]->set_arg( pre_image_ );
      update_kernels_[pass]->set_arg( norm_image_ );
      update_kernels_[pass]->set_arg( cl_output_ );
      update_kernels_[pass]->set_local_arg( lThreads_[0]*lThreads_[1]*sizeof(cl_uchar16) );//local tree,
      update_kernels_[pass]->set_local_arg( lThreads_[0]*lThreads_[1]*sizeof(cl_short2) ); //ray bundle,
      update_kernels_[pass]->set_local_arg( lThreads_[0]*lThreads_[1]*sizeof(cl_int) );    //cell pointers,
      update_kernels_[pass]->set_local_arg( lThreads_[0]*lThreads_[1]*sizeof(cl_float) ); //cached aux,
      update_kernels_[pass]->set_local_arg( lThreads_[0]*lThreads_[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer
      break;
    case UPDATE_CELL :
      update_kernels_[pass]->set_arg( blk_info_ );
      update_kernels_[pass]->set_arg( alpha_ );
      update_kernels_[pass]->set_arg( mog_ );
      update_kernels_[pass]->set_arg( num_obs_ );
      update_kernels_[pass]->set_arg( aux_ );
      update_kernels_[pass]->set_arg( cl_output_ );
      break;
  }
  return true;
}


bool boxm2_opencl_update_process::write_input_image(vil_image_view<float>* input_image)
{
  //write to buffer (or create it)
  unsigned int ni=RoundUp(input_image->ni(),8);
  unsigned int nj=RoundUp(input_image->nj(),8);
  float* buff = (image_) ? (float*) image_->cpu_buffer() : new float[ni*nj];
  int count=0;
  for (unsigned int j=0;j<nj;++j)
  {
    for (unsigned int i=0;i<ni;++i)
    {
      buff[count] = 0.0f;
      if (i<input_image->ni() && j< input_image->nj())
        buff[count]=(*input_image)(i,j);
      ++count;
    }
  }


  //now write to bocl_mem
  if (!image_) {
    //create mem
    image_ = new bocl_mem((*context_), buff, ni*nj * sizeof(cl_float), "input image buffer (float image)");
    image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  }
  else {
    image_->write_to_buffer(*command_queue_);
  }
  return true;
}
