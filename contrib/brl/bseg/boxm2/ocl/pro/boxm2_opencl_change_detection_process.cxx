#include "boxm2_opencl_change_detection_process.h"

//boxm2 data structures
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <vil/vil_save.h>
#include <boxm2/ocl/boxm2_ocl_util.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vul/vul_timer.h>
#include <vcl_where_root_dir.h>

//TODO IN THIS INIT METHOD: Need to pass in a ref to the OPENCL_CACHE so this
//class can easily access BOCL_MEMs
bool boxm2_opencl_change_detection_process::init_kernel(cl_context* context,
                                                        cl_device_id* device,
                                                        vcl_string opts)
{
  context_ = context;

  //gather all render sources... seems like a lot for rendering...
  vcl_vector<vcl_string> src_paths;
  vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/ocl/cl/";
  src_paths.push_back(source_dir + "scene_info.cl");
  src_paths.push_back(source_dir + "cell_utils.cl");
  src_paths.push_back(source_dir + "bit/bit_tree_library_functions.cl");
  src_paths.push_back(source_dir + "backproject.cl");
  src_paths.push_back(source_dir + "statistics_library_functions.cl");
  src_paths.push_back(source_dir + "expected_functor.cl");
  src_paths.push_back(source_dir + "ray_bundle_library_opt.cl");
  src_paths.push_back(source_dir + "bit/render_bit_scene.cl");

  //set kernel options
  vcl_string options = "-D INTENSITY ";
  options += "-D NVIDIA ";
  options += "-D CHANGE ";
  options += opts;

  //have kernel construct itself using the context and device
  bool created =  change_kernel_.create_kernel( context_,
                                                device,
                                                src_paths,
                                                "change_detection_bit_scene",   //kernel name
                                                options,              //options
                                                "boxm2 opencl change"); //kernel identifier (for error checking)
  vcl_vector<vcl_string> norm_src_paths;
  norm_src_paths.push_back(source_dir + "cell_utils.cl");
  norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");

  //have kernel construct itself using the context and device
  created = created &&  normalize_change_kernel_.create_kernel( context_,
                                                                device,
                                                                norm_src_paths,
                                                                "normalize_change_kernel",   //kernel name
                                                                options,              //options
                                                                "boxm2 opencl norm change"); //kernel identifier (for error checking)

  return created;
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
//  6) ocl_mem_sptr in_img     //produced here
//  7) ocl_mem_sptr exp_img     //produced here
//  8) ocl_mem_sptr exp_img_dim //produced here
//  9) ocl_mem_sptr cl_output;  //produced here
//  10) ocl_mem_sptr bit_lookup  //produced here
//  11) ocl_mem_sptr ray_vis    //produced here
bool boxm2_opencl_change_detection_process::execute(vcl_vector<brdb_value_sptr>& input, vcl_vector<brdb_value_sptr>& output)
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
  boxm2_ocl_util::set_persp_camera(cam, cam_buffer);
  bocl_mem persp_cam((*context_), cam_buffer, 3*sizeof(cl_float16), "persp cam buffer");
  persp_cam.create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  //in image buffer
  brdb_value_t<vil_image_view_base_sptr>* brdb_inimg = static_cast<brdb_value_t<vil_image_view_base_sptr>* >( input[i++].ptr() );
  vil_image_view_base_sptr inimg = brdb_inimg->value();
  vil_image_view<float>* in_img_view = static_cast<vil_image_view<float>* >(inimg.ptr());
  this->write_input_image(in_img_view);
  //input exp image buffer
  brdb_value_t<vil_image_view_base_sptr>* brdb_expimg = static_cast<brdb_value_t<vil_image_view_base_sptr>* >( input[i++].ptr() );
  vil_image_view_base_sptr expimg = brdb_expimg->value();
  vil_image_view<float>* exp_img_view = static_cast<vil_image_view<float>* >(expimg.ptr());
  this->write_exp_image(exp_img_view);

  //cd image buffer
  brdb_value_t<vil_image_view_base_sptr>* brdb_cdimg = static_cast<brdb_value_t<vil_image_view_base_sptr>* >( input[i++].ptr() );
  vil_image_view_base_sptr cdimg = brdb_cdimg->value();
  vil_image_view<unsigned int>* cd_img_view = static_cast<vil_image_view<unsigned int>* >(cdimg.ptr());
  cd_img_view->fill(0);
  if (!image_) {
    unsigned int* cd_buff = cd_img_view->begin();
    image_ = new bocl_mem((*context_), cd_buff, cd_img_view->size() * sizeof(unsigned int), "cd image buffer");
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
  vil_image_view<float>* prob_exp_img_view = new vil_image_view<float>(vis_img_view->ni(),vis_img_view->nj());
  prob_exp_img_view->fill(0.0);
  if (!prob_exp_img_) {
    float* prob_exp_img_buff = prob_exp_img_view->begin();
    prob_exp_img_ = new bocl_mem((*context_), prob_exp_img_buff, prob_exp_img_view->size() * sizeof(float), "visibility image buffer");
    prob_exp_img_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  }
  else {
    prob_exp_img_->set_cpu_buffer(prob_exp_img_view->begin());
    prob_exp_img_->write_to_buffer(*command_queue_);
  }

  //exp image dimensions
  int* img_dim_buff = new int[4];
  img_dim_buff[0] = 0;
  img_dim_buff[1] = 0;
  img_dim_buff[2] = in_img_view->ni();
  img_dim_buff[3] = in_img_view->nj();
  bocl_mem exp_img_dim((*context_), img_dim_buff, sizeof(int)*4, "image dims");
  exp_img_dim.create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //output buffer
  float* output_arr = new float[100];
  for (int i=0; i<100; ++i) output_arr[i] = 0.0f;
  bocl_mem cl_output((*context_), output_arr, sizeof(float)*100, "output buffer");
  cl_output.create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //bit lookup buffer
  cl_uchar* lookup_arr = new cl_uchar[256];
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem lookup((*context_), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup.create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //2. set workgroup size
  vcl_size_t lThreads[] = {8, 8};
  vcl_size_t gThreads[] = {exp_img_view->ni(), exp_img_view->nj()};

  //For each ID in the visibility order, grab that block
  vcl_vector<boxm2_block_id> vis_order = scene->get_vis_blocks( (vpgl_perspective_camera<double>*) cam.ptr());

  vcl_vector<boxm2_block_id>::iterator id;
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
    //write the image values to the buffer
    vul_timer transfer;
    bocl_mem* blk       = cache_->get_block(*id);
    bocl_mem* alpha     = cache_->get_data<BOXM2_ALPHA>(*id);
    bocl_mem* mog       = cache_->get_data<BOXM2_MOG3_GREY>(*id);
    bocl_mem* blk_info  = cache_->loaded_block_info();
    transfer_time_ += (float) transfer.all();

    ////3. SET args
    change_kernel_.set_arg( blk_info );
    change_kernel_.set_arg( blk );
    change_kernel_.set_arg( alpha );
    change_kernel_.set_arg( mog );
    change_kernel_.set_arg( &persp_cam );
    change_kernel_.set_arg( in_image_ );
    change_kernel_.set_arg( exp_image_ );
    change_kernel_.set_arg( prob_exp_img_ );
    change_kernel_.set_arg( image_ );
    change_kernel_.set_arg( &exp_img_dim);
    change_kernel_.set_arg( &cl_output );
    change_kernel_.set_arg( &lookup );
    change_kernel_.set_arg( vis_img_ );

    //local tree , cumsum buffer, imindex buffer
    change_kernel_.set_local_arg( lThreads[0]*lThreads[1]*sizeof(cl_uchar16) );
    change_kernel_.set_local_arg( lThreads[0]*lThreads[1]*10*sizeof(cl_uchar) );
    change_kernel_.set_local_arg( lThreads[0]*lThreads[1]*sizeof(cl_int) );

    //execute kernel
    change_kernel_.execute( (*command_queue_), 2, lThreads, gThreads);
    clFinish(*command_queue_);
    gpu_time_ += change_kernel_.exec_time();

    //clear render kernel args so it can reset em on next execution
    change_kernel_.clear_args();
  }
  // normalize
  {
      normalize_change_kernel_.set_arg( image_ );
      normalize_change_kernel_.set_arg( prob_exp_img_ );
      normalize_change_kernel_.set_arg( vis_img_ );
      normalize_change_kernel_.set_arg( &exp_img_dim);
      normalize_change_kernel_.execute( (*command_queue_), 2, lThreads, gThreads);
      clFinish(*command_queue_);
      gpu_time_ += normalize_change_kernel_.exec_time();

      //clear render kernel args so it can reset em on next execution
      normalize_change_kernel_.clear_args();
  }

  //read image out to buffer (from gpu)
  image_->read_to_buffer(*command_queue_);
  vis_img_->read_to_buffer(*command_queue_);

  //clean up camera, lookup_arr, img_dim_buff
  delete[] output_arr;
  delete[] img_dim_buff;
  delete[] lookup_arr;
  delete[] cam_buffer;

  //record total time
  total_time_ = (float) total.all();

  return true;
}


bool boxm2_opencl_change_detection_process::clean()
{
  if (in_image_) delete in_image_;
  if (image_) delete image_;
  if (vis_img_) delete vis_img_;
  image_ = 0;
  vis_img_ = 0;
  return true;
}

bool boxm2_opencl_change_detection_process::write_input_image(vil_image_view<float>* input_image)
{
  vil_image_view<float>::iterator iter;

  //write to buffer (or create it)
  float* buff = (in_image_) ? (float*) in_image_->cpu_buffer() : new float[ input_image->size()];
  int i=0;
  for (iter = input_image->begin(); iter != input_image->end(); ++iter, ++i) {
    buff[i] = (*iter);
  }

  //now write to bocl_mem
  if (!in_image_) {
    //create mem
    in_image_ = new bocl_mem((*context_), buff, input_image->size() * sizeof(cl_float), "input image buffer");
    in_image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  }
  else {
    in_image_->write_to_buffer(*command_queue_);
  }
  return true;
}

bool boxm2_opencl_change_detection_process::write_exp_image(vil_image_view<float>* exp_image)
{
  vil_image_view<float>::iterator iter;

  //write to buffer (or create it)
  float* buff = (exp_image_) ? (float*) exp_image_->cpu_buffer() : new float[ exp_image->size()];
  int i=0;
  for (iter = exp_image->begin(); iter != exp_image->end(); ++iter, ++i) {
    buff[i] = (*iter);
  }

  //now write to bocl_mem
  if (!exp_image_) {
    //create mem
    exp_image_ = new bocl_mem((*context_), buff, exp_image->size() * sizeof(cl_float), "input image buffer");
    exp_image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  }
  else {
    exp_image_->write_to_buffer(*command_queue_);
  }
  return true;
}
