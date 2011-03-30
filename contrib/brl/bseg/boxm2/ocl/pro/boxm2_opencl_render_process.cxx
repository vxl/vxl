#include "boxm2_opencl_render_process.h"

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
#include <bpro/core/bbas_pro/bbas_1d_array_float.h>

//TODO IN THIS INIT METHOD: Need to pass in a ref to the OPENCL_CACHE so this
//class can easily access BOCL_MEMs
bool boxm2_opencl_render_process::init_kernel(cl_context* context,
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
  src_paths.push_back(source_dir + "bit/cast_ray_bit.cl");

  //set kernel options
  //#define STEP_CELL step_cell_render(mixture_array, alpha_array, data_ptr, d, &vis, &expected_int);
  vcl_string options = " -D INTENSITY ";
  options += " -D RENDER ";
  options += " -D STEP_CELL=step_cell_render(aux_args.mog,aux_args.alpha,data_ptr,d,vis,aux_args.expint)";
  options += opts;

  //have kernel construct itself using the context and device
  bool created =  render_kernel_.create_kernel( context_,
                                                device,
                                                src_paths,
                                                "render_bit_scene",   //kernel name
                                                options + " -D DETERMINISTIC ",              //options
                                                "boxm2 opencl render"); //kernel identifier (for error checking)

  created = created&&rand_kernel_.create_kernel(context_,
                                                device,
                                                src_paths,
                                                "render_bit_scene",   //kernel name
                                                options,              //options
                                                "boxm2 opencl render random blocks"); //kernel identifier (for error checking)
  //create normalize image kernel
  vcl_vector<vcl_string> norm_src_paths;
  norm_src_paths.push_back(source_dir + "cell_utils.cl");
  norm_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
  created = created && normalize_render_kernel_.create_kernel( context_,
                                                               device,
                                                               norm_src_paths,
                                                               "normalize_render_kernel",   //kernel name
                                                               options,              //options
                                                               "normalize render kernel"); //kernel identifier (for error checking)

  vcl_string render_gl_options = " -D INTENSITY ";
  render_gl_options += " -D RENDER_GL ";

  vcl_vector<vcl_string> render_gl_src_paths;
  render_gl_src_paths.push_back(source_dir + "cell_utils.cl");
  render_gl_src_paths.push_back(source_dir + "bit/normalize_kernels.cl");
  created = created && render_gl_kernel_.create_kernel( context_,
                                                        device,
                                                        render_gl_src_paths,
                                                        "render_kernel_gl",   //kernel name
                                                        render_gl_options,              //options
                                                        " render gl kernel"); //kernel identifier (for error checking)

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
//  6) ocl_mem_sptr exp_img     //produced here
//  7) ocl_mem_sptr exp_img_dim //produced here
//  8) ocl_mem_sptr cl_output;  //produced here
//  9) ocl_mem_sptr bit_lookup  //produced here
//  10) ocl_mem_sptr ray_vis    //produced here
bool boxm2_opencl_render_process::execute(vcl_vector<brdb_value_sptr>& input, vcl_vector<brdb_value_sptr>& output)
{
  transfer_time_ = 0.0f; gpu_time_ = 0.0f; total_time_ = 0.0f;
  vul_timer total;
  int inIdx = 0;

  //scene argument
  brdb_value_t<boxm2_scene_sptr>* scene_brdb = static_cast<brdb_value_t<boxm2_scene_sptr>* >( input[inIdx++].ptr() );
  boxm2_scene_sptr scene = scene_brdb->value();
  bool foundDataType = false;
  vcl_vector<vcl_string> apps = scene->appearances();
  for (unsigned int i=0; i<apps.size(); ++i) {
    if ( apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY>::prefix() || apps[i] == boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix() )
    {
      data_type_ = apps[i];
      foundDataType = true;
    }
  }
  if (!foundDataType) {
    vcl_cout<<"BOXM2_OPENCL_RENDER_PROCESS ERROR: scene doesn't have BOXM2_MOG3_GREY or BOXM2_MOG3_GREY_16 data type"<<vcl_endl;
    return false;
  }

  //camera
  brdb_value_t<vpgl_camera_double_sptr>* brdb_cam = static_cast<brdb_value_t<vpgl_camera_double_sptr>* >( input[inIdx++].ptr() );
  vpgl_camera_double_sptr cam = brdb_cam->value();
  cl_float* cam_buffer = new cl_float[16*3];
  boxm2_ocl_util::set_persp_camera(cam, cam_buffer);
  bocl_mem persp_cam((*context_), cam_buffer, 3*sizeof(cl_float16), "persp cam buffer");
  persp_cam.create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);

  //exp image buffer
  brdb_value_t<vil_image_view_base_sptr>* brdb_expimg = static_cast<brdb_value_t<vil_image_view_base_sptr>* >( input[inIdx++].ptr() );
  vil_image_view_base_sptr expimg = brdb_expimg->value();
  vil_image_view<float>* exp_img_view = static_cast<vil_image_view<float>* >(expimg.ptr());
  exp_img_view->fill(0.0f);
  if (!image_) {
    float* exp_buff = exp_img_view->begin();
    image_ = new bocl_mem((*context_), exp_buff, exp_img_view->size() * sizeof(float), "exp image buffer");
    image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  }
  else {
    image_->set_cpu_buffer(exp_img_view->begin());
    image_->write_to_buffer(*command_queue_);
  }
  if (!gl_image_) {
    gl_image_ = new bocl_mem((*context_), NULL, exp_img_view->size() * sizeof(float), "exp image buffer");
    gl_image_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  }
  else {
    //image_->set_cpu_buffer(exp_img_view->begin());
    gl_image_->write_to_buffer(*command_queue_);
  }

  //vis image buffer
  brdb_value_t<vil_image_view_base_sptr>* brdb_vis = static_cast<brdb_value_t<vil_image_view_base_sptr>* >( input[inIdx++].ptr() );
  vil_image_view_base_sptr visimg = brdb_vis->value();
  vil_image_view<float>* vis_img_view = static_cast<vil_image_view<float>* >(visimg.ptr());
  vis_img_view->fill(1.0f);
  if (!vis_img_) {
    float* vis_buff = vis_img_view->begin();
    vis_img_ = new bocl_mem((*context_), vis_buff, vis_img_view->size() * sizeof(float), "visibility image buffer");
    vis_img_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  }
  else {
    vis_img_->set_cpu_buffer(vis_img_view->begin());
    vis_img_->write_to_buffer(*command_queue_);
  }
#if 0
  brdb_value_t<vcl_string>* brdb_data_type = static_cast<brdb_value_t<vcl_string>* >( input[inIdx++].ptr() );
  data_type_=brdb_data_type->value();
#endif

  //exp image dimensions
  int* img_dim_buff = new int[4];
  img_dim_buff[0] = 0;
  img_dim_buff[1] = 0;
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
  boxm2_ocl_util::set_bit_lookup(lookup_arr);
  bocl_mem lookup((*context_), lookup_arr, sizeof(cl_uchar)*256, "bit lookup buffer");
  lookup.create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

  //2. set workgroup size
  vcl_size_t lThreads[] = {8, 8};
  vcl_size_t gThreads[] = {RoundUp(exp_img_view->ni(),lThreads[0]), RoundUp(exp_img_view->nj(),lThreads[1])};

  //For each ID in the visibility order, grab that block
  vcl_vector<boxm2_block_id> vis_order = scene->get_vis_blocks( (vpgl_perspective_camera<double>*) cam.ptr());
  vcl_vector<boxm2_block_id>::iterator id;
  for (id = vis_order.begin(); id != vis_order.end(); ++id)
  {
    //choose correct render kernel
    boxm2_block_metadata mdata = scene->get_block_metadata(*id);
    bocl_kernel& kern = (mdata.random_) ? rand_kernel_ : render_kernel_;

    //write the image values to the buffer
    vul_timer transfer;
    bocl_mem* blk       = cache_->get_block(*id);
    bocl_mem* alpha     = cache_->get_data<BOXM2_ALPHA>(*id);
    bocl_mem* mog       = cache_->get_data(*id, data_type_);
    bocl_mem* blk_info  = cache_->loaded_block_info();
    transfer_time_ += (float) transfer.all();

    ////3. SET args
    kern.set_arg( blk_info );
    kern.set_arg( blk );
    kern.set_arg( alpha );
    kern.set_arg( mog );
    kern.set_arg( &persp_cam );
    kern.set_arg( image_ );
    kern.set_arg( &exp_img_dim);
    kern.set_arg( &cl_output );
    kern.set_arg( &lookup );
    kern.set_arg( vis_img_ );

    //local tree , cumsum buffer, imindex buffer
    kern.set_local_arg( lThreads[0]*lThreads[1]*sizeof(cl_uchar16) );
    kern.set_local_arg( lThreads[0]*lThreads[1]*10*sizeof(cl_uchar) );
    kern.set_local_arg( lThreads[0]*lThreads[1]*sizeof(cl_int) );

    //execute kernel
    kern.execute( (*command_queue_), 2, lThreads, gThreads);
    clFinish(*command_queue_);
    gpu_time_ += kern.exec_time();

    //clear render kernel args so it can reset em on next execution
    kern.clear_args();
  }
  if (!gl_image_)
  // normalize
  {
    vcl_cout<<"NOT DOING GL IMAGE"<<vcl_endl;
    normalize_render_kernel_.set_arg( image_ );
    normalize_render_kernel_.set_arg( vis_img_ );
    normalize_render_kernel_.set_arg( &exp_img_dim);
    normalize_render_kernel_.execute( (*command_queue_), 2, lThreads, gThreads);
    clFinish(*command_queue_);
    gpu_time_ += normalize_render_kernel_.exec_time();

    //clear render kernel args so it can reset em on next execution
    normalize_render_kernel_.clear_args();
    image_->read_to_buffer(*command_queue_);
    vis_img_->read_to_buffer(*command_queue_);
  }
  //read image out to buffer (from gpu)

  else
  {
    float* mini_buf = new float[1];
    float* maxi_buf = new float[1];

    brdb_value_t<float>* brdb_mini = static_cast<brdb_value_t<float>* >( input[inIdx++].ptr() );
    mini_buf[0]=brdb_mini->value();

    brdb_value_t<float>* brdb_maxi = static_cast<brdb_value_t<float>* >( input[inIdx++].ptr() );
    maxi_buf[0]=brdb_maxi->value();


    brdb_value_t<bbas_1d_array_float_sptr>* brdb_tf = static_cast<brdb_value_t<bbas_1d_array_float_sptr>* >( input[inIdx++].ptr() );
    bbas_1d_array_float_sptr tf=brdb_tf->value();

    //mini_buf[0]=0.0f;
    //maxi_buf[0]=1.0f;

    float* tf_buf= new float[256];
    for (int i=0; i<256; ++i) tf_buf[i] =tf->data_array[i] ;

    mini_=new bocl_mem((*context_),mini_buf,sizeof(float),"Mini buffer");
    mini_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    maxi_=new bocl_mem((*context_),maxi_buf,sizeof(float),"Maxi buffer");
    maxi_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);
    tf_=new bocl_mem((*context_),tf_buf,256*sizeof(float),"TF buffer");
    tf_->create_buffer(CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR);

    render_gl_kernel_.set_arg( mini_ );
    render_gl_kernel_.set_arg( maxi_ );
    render_gl_kernel_.set_arg( tf_ );
    render_gl_kernel_.set_arg( vis_img_ );
    render_gl_kernel_.set_arg( image_ );
    render_gl_kernel_.set_arg( gl_image_ );
    render_gl_kernel_.set_arg( &exp_img_dim);
    render_gl_kernel_.execute( (*command_queue_), 2, lThreads, gThreads);
    clFinish(*command_queue_);
    gpu_time_ += render_gl_kernel_.exec_time();

    //clear render kernel args so it can reset em on next execution
    render_gl_kernel_.clear_args();
  }

  //clean up camera, lookup_arr, img_dim_buff
  delete[] output_arr;
  delete[] img_dim_buff;
  delete[] lookup_arr;
  delete[] cam_buffer;

  //record total time
  total_time_ = (float) total.all();

  return true;
}


bool boxm2_opencl_render_process::clean()
{
  if (image_) delete image_;
  if (vis_img_) delete vis_img_;
  image_ = 0;
  vis_img_ = 0;
  return true;
}

