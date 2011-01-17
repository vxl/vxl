#include "boxm2_opencl_update_process.h"

//boxm2 data structures
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <vil/vil_save.h>
#include <vil/vil_math.h>
#include <vil/vil_load.h>
#include <boxm2/boxm2_util.h>

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
  src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");
  src_paths.push_back(source_dir + "bit/update_kernels.cl");

  //compilation options
  vcl_string options = "-D INTENSITY ";
  options += "-D NVIDIA ";
  options += opts;

  //create all passes
  bocl_kernel* seg_len = new bocl_kernel();
  seg_len->create_kernel(context_, device, src_paths, "seg_len_main", options+" -D SEGLEN -D ATOMIC_OPT", "update::seg_len");
  update_kernels_.push_back(seg_len);

  bocl_kernel* pre_inf = new bocl_kernel();
  pre_inf->create_kernel(context_, device, src_paths, "pre_inf_main", options+" -D PREINF", "update::pre_inf");
  update_kernels_.push_back(pre_inf);

  //may need DIFF LIST OF SOURCES FOR THIS GUY
  bocl_kernel* proc_img = new bocl_kernel();
  proc_img->create_kernel(context_, device, src_paths, "proc_norm_image", options, "update::proc_norm_image");
  update_kernels_.push_back(proc_img);

  bocl_kernel* bayes_main = new bocl_kernel();
  bayes_main->create_kernel(context_, device, src_paths, "bayes_main", options+" -D BAYES -D ATOMIC_OPT", "update::bayes_main");
  update_kernels_.push_back(bayes_main);

  //may need DIFF LIST OF SOURCES FOR THSI GUY TOO
  bocl_kernel* update = new bocl_kernel();
  update->create_kernel(context_, device, src_paths, "update_bit_scene_main", options, "update::update_main");
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
  int i = 0;

  //scene argument
  brdb_value_t<boxm2_scene_sptr>* scene_brdb = static_cast<brdb_value_t<boxm2_scene_sptr>* >( input[i++].ptr() );
  boxm2_scene_sptr scene = scene_brdb->value();

  //camera
  brdb_value_t<vpgl_camera_double_sptr>* brdb_cam = static_cast<brdb_value_t<vpgl_camera_double_sptr>* >( input[i++].ptr() );
  vpgl_camera_double_sptr cam = brdb_cam->value();
  cl_float* cam_buffer = new cl_float[16*3];
  boxm2_util::set_persp_camera(cam, cam_buffer);
  persp_cam_ = new bocl_mem((*context_), cam_buffer, 3*sizeof(cl_float16), "persp cam buffer");
  persp_cam_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //input image buffer
  brdb_value_t<vil_image_view_base_sptr>* brdb_img = static_cast<brdb_value_t<vil_image_view_base_sptr>* >( input[i++].ptr() );
  vil_image_view_base_sptr img = brdb_img->value();
  vil_image_view<float>* img_view = static_cast<vil_image_view<float>* >(img.ptr());
  this->write_input_image(img_view);

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
  boxm2_util::set_bit_lookup(lookup_arr);
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
#ifdef DEBUG
    vcl_cout<<"UPDATE KERNEL : "<<i<<vcl_endl;
#endif
    if ( i == UPDATE_PROC ) {
      this->set_workspace(i);
      this->set_args(i);

      //execute kernel
      update_kernels_[i]->execute( (*command_queue_), lThreads_, gThreads_);
      int status = clFinish(*command_queue_);
      check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
      update_kernels_[i]->clear_args();
      image_->read_to_buffer(*command_queue_);
      continue;
    }

    //zip through visible blocks, and execute this pass's kernel
    for (id = vis_order.begin(); id != vis_order.end(); ++id)
    {
      //write the image values to the buffer
      vul_timer transfer;
      blk_       = cache_->get_block(*id);
      alpha_     = cache_->get_data<BOXM2_ALPHA>(*id);
      mog_       = cache_->get_data<BOXM2_MOG3_GREY>(*id);
      num_obs_   = cache_->get_data<BOXM2_NUM_OBS>(*id);
      blk_info_  = cache_->loaded_block_info();

      //get aux data
      aux_       = cache_->get_data<BOXM2_AUX>(*id);
      transfer_time_ += (float) transfer.all();

      //set workspace and args for this pass
      this->set_workspace(i);
      this->set_args(i);

      //execute kernel
      update_kernels_[i]->execute( (*command_queue_), lThreads_, gThreads_);
      int status = clFinish(*command_queue_);
      check_val(status, MEM_FAILURE, "UPDATE EXECUTE FAILED: " + error_to_string(status));
      //gpu_time_ += update_kernels_[i]->exec_time();

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

bool boxm2_opencl_update_process::set_workspace(unsigned pass)
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
      gThreads_[0] = RoundUp(numbuf*datlen,64);
      gThreads_[1] = 1;
      lThreads_[0]  = 64;
      lThreads_[1]  = 1;
      break;
    }
  }
  return true;
}


bool boxm2_opencl_update_process::set_args(unsigned pass)
{
  switch (pass)
  {
    case UPDATE_SEGLEN :
      update_kernels_[pass]->set_arg( blk_info_ );
      update_kernels_[pass]->set_arg( blk_ );
      update_kernels_[pass]->set_arg( alpha_ );
      update_kernels_[pass]->set_arg( num_obs_ );
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
      update_kernels_[pass]->set_arg( image_ );
      update_kernels_[pass]->set_arg( cl_output_ );
      update_kernels_[pass]->set_local_arg( lThreads_[0]*lThreads_[1]*sizeof(cl_uchar16) );//local tree,
      update_kernels_[pass]->set_local_arg( lThreads_[0]*lThreads_[1]*10*sizeof(cl_uchar) ); //cumsum buffer, imindex buffer
      break;
    case UPDATE_PROC :
      update_kernels_[pass]->set_arg( image_ );
      update_kernels_[pass]->set_arg( app_density_ );
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
      update_kernels_[pass]->set_arg( image_ );
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
  vil_image_view<float>::iterator iter;

  //write to buffer (or create it)
  float* buff = (image_) ? (float*) image_->cpu_buffer() : new float[4 * input_image->size()];
  int i=0;
  for (iter = input_image->begin(); iter != input_image->end(); ++iter, ++i) {
    buff[4*i] = (*iter);
    buff[4*i + 1] = 0.0f;
    buff[4*i + 2] = 1.0f;
    buff[4*i + 3] = 0.0f;
  }

  //now write to bocl_mem
  if (!image_) {
    //create mem
    image_ = new bocl_mem((*context_), buff, input_image->size() * sizeof(cl_float4), "input image buffer");
    image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  }
  else {
    image_->write_to_buffer(*command_queue_);
  }
  return true;
}
