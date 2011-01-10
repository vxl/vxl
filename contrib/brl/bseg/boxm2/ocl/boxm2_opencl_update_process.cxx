#include "boxm2_opencl_update_process.h"

//boxm2 data structures
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <vil/vil_save.h>
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
  vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/cl/";  
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
  bocl_kernel seg_len; 
  seg_len.create_kernel(context_, device, src_paths, "seg_len_main", options+" -D SEGLEN", "update::seg_len"); 
  update_kernels_.push_back(seg_len); 
  
  bocl_kernel pre_inf; 
  pre_inf.create_kernel(context_, device, src_paths, "pre_inf_main", options+" -D PREINF", "update::pre_inf");
  update_kernels_.push_back(pre_inf); 

  //may need DIFF LIST OF SOURCES FOR THIS GUY
  bocl_kernel proc_img; 
  proc_img.create_kernel(context_, device, src_paths, "proc_norm_image", options, "update::proc_norm_image");
  update_kernels_.push_back(proc_img); 

  bocl_kernel bayes_main;
  bayes_main.create_kernel(context_, device, src_paths, "bayes_main", options+" -D BAYES", "update::bayes_main");
  update_kernels_.push_back(bayes_main); 

  //may need DIFF LIST OF SOURCES FOR THSI GUY TOO
  bocl_kernel update;
  update.create_kernel(context_, device, src_paths, "update_bit_scene_main", options, "update::update_main"); 
  update_kernels_.push_back(update); 

  return true;
}


//  NEED TO FIGURE OUT HOW TO differentiate between SCENE/BLOCK/DATA arguments and Camera/Image arguments
// the scene level stuff needs to live on the processor, other
//OPENCL RENDER PROCESS:
// arguments will be (should be)
//  1) ocl_mem_sptr scene_info
//  2) ocl_mem_sptr block
//  3) ocl_mem_sptr alpha
//  4) ocl_mem_sptr mog
//  5) ocl_mem_sptr persp_cam   //produced here
//  6) ocl_mem_sptr exp_img     //produced here
//  7) ocl_mem_sptr exp_img_dim //produced here
//  8) ocl_mem_sptr cl_output;  //produced here
//  9) ocl_mem_sptr bit_lookup  //produced here
//  10) ocl_mem_sptr ray_vis    //produced here
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
  bocl_mem persp_cam((*context_), cam_buffer, 3*sizeof(cl_float16), "persp cam buffer");
  persp_cam.create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  
  //exp image buffer
  brdb_value_t<vil_image_view_base_sptr>* brdb_expimg = static_cast<brdb_value_t<vil_image_view_base_sptr>* >( input[i++].ptr() );
  vil_image_view_base_sptr expimg = brdb_expimg->value();
  vil_image_view<float>* exp_img_view = static_cast<vil_image_view<float>* >(expimg.ptr());
  if (!image_) {
    float* exp_buff = exp_img_view->begin();
    image_ = new bocl_mem((*context_), exp_buff, exp_img_view->size() * sizeof(float), "exp image buffer");
    image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  }
  else {
    image_->write_to_buffer(*command_queue_);
  }

  //vis image buffer
  brdb_value_t<vil_image_view_base_sptr>* brdb_vis = static_cast<brdb_value_t<vil_image_view_base_sptr>* >( input[i++].ptr() );
  vil_image_view_base_sptr visimg = brdb_vis->value();
  vil_image_view<float>* vis_img_view = static_cast<vil_image_view<float>* >(visimg.ptr());
  if (!vis_img_) {
    float* vis_buff = vis_img_view->begin();
    vis_img_ = new bocl_mem((*context_), vis_buff, vis_img_view->size() * sizeof(float), "visibility image buffer");
    vis_img_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  } 
  else {
    vis_img_->set_cpu_buffer(vis_img_view->begin()); 
    vis_img_->write_to_buffer(*command_queue_);
  }
      
  //exp image dimensions
  int* img_dim_buff = new int[4];
  img_dim_buff[0] = exp_img_view->ni();
  img_dim_buff[1] = exp_img_view->nj();
  img_dim_buff[2] = exp_img_view->ni();
  img_dim_buff[3] = exp_img_view->nj();
  bocl_mem exp_img_dim((*context_), img_dim_buff, sizeof(int)*4, "image dims");
  exp_img_dim.create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  
  //output buffer
  float* output_arr = new float[100];
  for (int i=0; i<100; ++i) output_arr[i] = 0.0f;
  bocl_mem cl_output((*context_), output_arr, sizeof(float)*100, "output buffer");
  cl_output.create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //bit lookup buffer
  cl_uchar* lookup_arr = new cl_uchar[256];
  boxm2_util::set_bit_lookup(lookup_arr);
  bocl_mem lookup((*context_), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup.create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //2. set workgroup size
  vcl_size_t lThreads[] = {8, 8};
  vcl_size_t gThreads[] = {exp_img_view->ni(), exp_img_view->nj()};
  
  //For each ID in the visibility order, grab that block
  vcl_vector<boxm2_block_id> vis_order = scene->get_vis_blocks( (vpgl_perspective_camera<double>*) cam.ptr()); 
  
  vcl_vector<boxm2_block_id>::iterator id; 
  for(id = vis_order.begin(); id != vis_order.end(); ++id) 
  {
    
    //write the image values to the buffer
    vul_timer transfer; 
    bocl_mem* blk       = cache_->get_block(*id);
    bocl_mem* alpha     = cache_->get_data<BOXM2_ALPHA>(*id);
    bocl_mem* mog       = cache_->get_data<BOXM2_MOG3_GREY>(*id);
    bocl_mem* blk_info  = cache_->loaded_block_info(); 
    transfer_time_ += (float) transfer.all(); 

    ////3. SET args
    for(int i=0; i<update_kernels_.size(); i++)
    {
      update_kernels_[i].set_arg( blk_info );
      update_kernels_[i].set_arg( blk );
      update_kernels_[i].set_arg( alpha );
      update_kernels_[i].set_arg( mog );
      update_kernels_[i].set_arg( &persp_cam );
      update_kernels_[i].set_arg( image_ );
      update_kernels_[i].set_arg( &exp_img_dim);
      update_kernels_[i].set_arg( &cl_output );
      update_kernels_[i].set_arg( &lookup );
      update_kernels_[i].set_arg( vis_img_ );
        
      //local tree , cumsum buffer, imindex buffer
      update_kernels_[i].set_local_arg( lThreads[0]*lThreads[1]*sizeof(cl_uchar16) );
      update_kernels_[i].set_local_arg( lThreads[0]*lThreads[1]*10*sizeof(cl_uchar) );
      update_kernels_[i].set_local_arg( lThreads[0]*lThreads[1]*sizeof(cl_int) );
        
      //execute kernel
      update_kernels_[i].execute( (*command_queue_), lThreads, gThreads);
      clFinish(*command_queue_); 
      gpu_time_ += update_kernels_[i].exec_time(); 
    
      //clear render kernel args so it can reset em on next execution
      update_kernels_[i].clear_args();
    }


  }
  
  
  //read image out to buffer (from gpu)
  image_->read_to_buffer(*command_queue_);
  vis_img_->read_to_buffer(*command_queue_);
  //cl_output.read_to_buffer(*command_queue_);

  //clean up camera, lookup_arr, img_dim_buff
  delete[] output_arr;
  delete[] img_dim_buff;
  delete[] lookup_arr;
  delete[] cam_buffer;
  
  //record total time
  total_time_ = (float) total.all(); 
  
  return true;
}


bool boxm2_opencl_update_process::clean()
{
  if (image_) delete image_;
  if (vis_img_) delete vis_img_;
  image_ = 0;
  vis_img_ = 0;
  return true;
}


#if 0

bool boxm_update_bit_scene_manager::set_update_args(unsigned pass)
{
  cl_int status = CL_SUCCESS;
  int i=0;

  //set raytrace update args -------------------------------------------------
  if (pass==0)
  {
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&scene_info_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (render scene info)"))
      return false;
    //block pointers
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&block_ptrs_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (block_ptrs_buf_)"))
      return false;
    // the tree buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cells_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cells_buf_)"))
      return false;
    // alpha buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_alpha_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_data_buf_)"))
      return false;
    //num_obs
     status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&cell_num_obs_buf_);
    if (!this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (cell_num_obs_buf_)"))
      return 0;
    //aux data arrays
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_seg_len_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_seg lens_)"))
      return false;
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_mean_obs_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mean_obs_)"))
      return false;
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_vis_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_vis_)"))
      return false;
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_beta_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_Beta)"))
      return false;
#if 0
    ////locking array
       //status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&cell_lock_buf_);
    //if (!this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (cell_lock_buf)"))
      //return 0;
    //status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_cum_beta_buf_);
    //if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mean_vis_)"))
      //return false;
    //status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_mean_vis_buf_);
    //if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mean_vis_)"))
      //return false;
#endif
    //bit lookup buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&bit_lookup_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output)"))
      return false;
    //local copy of the tree (one for each thread/ray)
    status = clSetKernelArg(update_kernels_[pass],i++,this->bni_*this->bnj_*sizeof(cl_uchar16),0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local tree)"))
      return false;
    // camera buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&persp_cam_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (persp_cam_buf_)"))
      return false;
    // cam+matrix buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&persp_mat_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (pers_mat_buf_)"))
      return false;
    // roi dimensions
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&img_dims_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (Img dimensions)"))
      return false;
    // input image
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&image_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (input_image)"))
      return false;
    // offset factor
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&factor_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (factor_buf_)"))
      return false;
    // offset x and y
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&offset_x_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (offset_x_buf_)"))
      return false;
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&offset_y_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (offset_y_buf_)"))
      return false;
    // ray bundle array
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_uchar4)*this->bni_*this->bnj_,0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local cache ptr bundle)"))
      return false;
    // cell pointers (cached)
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_int)*this->bni_*this->bnj_,0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local cell pointers)"))
      return false;
    // cached aux data
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_float4)*this->bni_*this->bnj_,0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local cached aux data)"))
      return false;
    //cum sum lookup buffer
    status = clSetKernelArg(update_kernels_[pass],i++,this->bni_*this->bnj_*10*sizeof(cl_uchar), 0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cumsum buff)"))
      return false;
    //output float buffer (one float for each buffer)
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&output_debug_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output debugger)"))
      return false;
  }
  if (pass==1)
  {
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&scene_info_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (render scene info)"))
      return false;
    //block pointers
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&block_ptrs_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (block_ptrs_buf_)"))
      return false;
    // the tree buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cells_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cells_buf_)"))
      return false;
    // alpha buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_alpha_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_data_buf_)"))
      return false;
    //mixture buffer
    status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&cell_mixture_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mixture_buf)"))
      return false;
    ////last weight buffer
    //status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&cell_weight_buf_);
    //if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_last_weight_buf_)"))
      //return false;
    //cell num obs buffer
    status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&cell_num_obs_buf_);
    if (!this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (cell_num_obs_buf_)"))
      return false;
    //aux data arrays
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_seg_len_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_seg lens_)"))
      return false;
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_mean_obs_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mean_obs_)"))
      return false;
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_vis_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_vis_)"))
      return false;
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_beta_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_Beta)"))
      return false;

#if 0
    //cum beta and mean vis buffers
    //status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_cum_beta_buf_);
    //if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mean_vis_)"))
      //return false;
    //status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_mean_vis_buf_);
    //if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mean_vis_)"))
      //return false;
    ////cell aux datas
    //status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_aux_data_buf_);
    //if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mean_vis_)"))
      //return false;
#endif
    //bit lookup buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&bit_lookup_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output)"))
      return false;
    //local copy of the tree (one for each thread/ray)
    status = clSetKernelArg(update_kernels_[pass],i++,this->bni_*this->bnj_*sizeof(cl_uchar16),0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local tree)"))
      return false;
    // camera buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&persp_cam_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (data)"))
      return false;
    // cam+matrix buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&persp_mat_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (pers_mat_buf_)"))
      return false;
    // roi dimensions
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&img_dims_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (Img dimensions)"))
      return false;
    // input image
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&image_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (input_image)"))
      return false;
    // offset factor
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&factor_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (factor_buf_)"))
      return false;
    // offset x and y
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&offset_x_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (offset_x_buf_)"))
      return false;
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&offset_y_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (offset_y_buf_)"))
      return false;
    //cum sum lookup buffer
    status = clSetKernelArg(update_kernels_[pass],i++,this->bni_*this->bnj_*10*sizeof(cl_uchar), 0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cumsum buff)"))
      return false;
    //output float buffer (one float for each buffer)
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&output_debug_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output debugger)"))
      return false;
  }
  if (pass==2) //divide alpha which is not done
  {
    //scene info
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&scene_info_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (render scene info)"))
      return false;
    // alpha buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_alpha_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_data_buf_)"))
      return false;
    //aux data
    //aux data arrays
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_seg_len_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_seg lens_)"))
      return false;
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_mean_obs_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mean_obs_)"))
      return false;
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_vis_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_vis_)"))
      return false;
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_beta_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_Beta)"))
      return false;
    ////cum beta and mean vis buffers
    //status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_cum_beta_buf_);
    //if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mean_vis_)"))
      //return false;
    //status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_mean_vis_buf_);
    //if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mean_vis_)"))
      //return false;
    //cell aux data buffer
    //status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_aux_data_buf_);
    //if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_data_buf_)"))
      //return false;
    //output float buffer (one float for each buffer)
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&output_debug_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output debugger)"))
      return false;
  }
  if (pass==4)
  {
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&scene_info_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (render scene info)"))
      return false;
    //block pointers
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&block_ptrs_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (block_ptrs_buf_)"))
      return false;
    // the tree buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cells_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cells_buf_)"))
      return false;
    // alpha buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_alpha_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_data_buf_)"))
      return false;
    //mixture buffer
    status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&cell_mixture_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mixture_buf)"))
      return false;
    //last weight buffer
    //status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&cell_weight_buf_);
    //if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_last_weight_buf_)"))
      //return false;
    //cell num obs buffer
    status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&cell_num_obs_buf_);
    if (!this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (cell_num_obs_buf_)"))
      return false;
    //aux data arrays
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_seg_len_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_seg lens_)"))
      return false;
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_mean_obs_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mean_obs_)"))
      return false;
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_vis_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_vis_)"))
      return false;
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_beta_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_Beta)"))
      return false;

#if 0
    ////locking array
    //status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&cell_lock_buf_);
    //if (!this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (cell_lock_buf)"))
      //return 0;
    //aux data
    //cum beta and mean vis buffers
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_cum_beta_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mean_vis_)"))
      return false;
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_mean_vis_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mean_vis_)"))
      return false;
    ////cell aux datas
    //status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_aux_data_buf_);
    //if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mean_vis_)"))
      //return false;
#endif
    //bit lookup buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&bit_lookup_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output)"))
      return false;
    //local copy of the tree (one for each thread/ray)
    status = clSetKernelArg(update_kernels_[pass],i++,this->bni_*this->bnj_*sizeof(cl_uchar16),0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local tree)"))
      return false;
    // camera buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&persp_cam_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (data)"))
      return false;
    // cam+matrix buffer
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&persp_mat_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (pers_mat_buf_)"))
      return false;
    // roi dimensions
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&img_dims_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (Img dimensions)"))
      return false;
    // input image
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&image_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (input_image)"))
      return false;
    // offset factor
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&factor_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (factor_buf_)"))
      return false;
    // offset x and y
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&offset_x_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (offset_x_buf_)"))
      return false;
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&offset_y_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (offset_y_buf_)"))
      return false;
    // ray bundle array
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_uchar4)*this->bni_*this->bnj_,0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local cache ptr bundle)"))
      return false;
    // cell pointers (cached)
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_int)*this->bni_*this->bnj_,0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local cell pointers)"))
      return false;
    // cached_vis
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_float)*this->bni_*this->bnj_,0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (local image_vect)"))
      return false;
    //cum sum lookup buffer
    status = clSetKernelArg(update_kernels_[pass],i++,this->bni_*this->bnj_*10*sizeof(cl_uchar), 0);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cumsum buff)"))
      return false;
    //output float buffer (one float for each buffer)
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&output_debug_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output debugger)"))
      return false;
  }
  //END ray trace update pass args -------------------------------------------------

  //set pass 4 args ------------------------------------------------------------
  if (pass == 5)
  {
    i=0;
    //scene info
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem), (void *)&scene_info_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (render scene info)"))
      return false;
    //alpha
    status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&cell_alpha_buf_);
    if (!this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (cell_alpha_buf_)"))
      return false;
    //mixture
    status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&cell_mixture_buf_);
    if (!this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (cell_mixture_buf_)"))
      return false;
    //last weight buffer
    //status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&cell_weight_buf_);
    //if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_last_weight_buf_)"))
      //return false;
    //num obs
    status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&cell_num_obs_buf_);
    if (!this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (cell_num_obs_buf_)"))
      return false;
    //aux data arrays
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_seg_len_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_seg lens_)"))
      return false;
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_mean_obs_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mean_obs_)"))
      return false;
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_vis_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_vis_)"))
      return false;
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_beta_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_Beta)"))
      return false;
    //cum beta and mean vis buffers
    //status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_cum_beta_buf_);
    //if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mean_vis_)"))
      //return false;
    //status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&cell_mean_vis_buf_);
    //if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (cell_mean_vis_)"))
      //return false;
    // aux data
    //status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&cell_aux_data_buf_);
    //if (!this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (cell_aux_data_buf_)"))
      //return false;
    //output float buffer (one float for each buffer)
    status = clSetKernelArg(update_kernels_[pass],i++,sizeof(cl_mem),(void *)&output_debug_buf_);
    if (!this->check_val(status,CL_SUCCESS,"clSetKernelArg failed. (output debugger)"))
      return false;
    return true;
  }
  //end pass 4 args ------------------------------------------------------------

  //set pass 2 args norm image process------------------------------------------
  if (pass == 3)
  {
    status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&image_buf_);
    if (!this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (image array)"))
      return false;
    status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&app_density_buf_);
    if (!this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (remote surface appearance)"))
      return false;
    status = clSetKernelArg(update_kernels_[pass], i++, sizeof(cl_mem), (void *)&img_dims_buf_);
    if (!this->check_val(status, CL_SUCCESS, "clSetKernelArg failed. (image dimensions)"))
      return false;
  }
  //END pass 2 args ------------------------------------------------------------

  return true;
}
#endif
