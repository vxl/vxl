#include "boxm2_opencl_render_process.h"

//boxm2 data structures
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_data_base.h>
#include <vil/vil_save.h>
#include <boxm2/boxm2_util.h>

//brdb stuff
#include <brdb/brdb_value.h>

//directory utility
#include <vcl_where_root_dir.h>

bool boxm2_opencl_render_process::init_kernel(cl_context& context,
                                              cl_device_id& device,
                                              vcl_string opts)
{
  context_ = &context;

  //gather all render sources... seems like a lot for rendering...
  vcl_vector<vcl_string> src_paths;
  vcl_string source_dir = vcl_string(VCL_SOURCE_ROOT_DIR) + "/contrib/brl/bseg/boxm2/cl/";
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
  options += "-D RENDER ";
  options += opts;

  //have kernel construct itself using the context and device
  bool created =  render_kernel_.create_kernel( &context,
                                                &device,
                                                src_paths,
                                                "render_bit_scene",   //kernel name
                                                options,              //options
                                                "boxm2 opencl render"); //kernel identifier (for error checking)

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
  vcl_cout<<"GPu RENDER argcounts:"<<input.size()<<vcl_endl;

  //1. get the arguments blocks/camera/img etc from the input vector
  int i = 0;
  brdb_value_t<bocl_mem_sptr>* info  = static_cast<brdb_value_t<bocl_mem_sptr>* >( input[i++].ptr() );
  bocl_mem_sptr info_sptr            = info->value();
  brdb_value_t<bocl_mem_sptr>* blk   = static_cast<brdb_value_t<bocl_mem_sptr>* >( input[i++].ptr() );
  bocl_mem_sptr blk_sptr             = blk->value();
  brdb_value_t<bocl_mem_sptr>* alpha = static_cast<brdb_value_t<bocl_mem_sptr>* >( input[i++].ptr() );
  bocl_mem_sptr alpha_sptr           = alpha->value();
  brdb_value_t<bocl_mem_sptr>* mogs  = static_cast<brdb_value_t<bocl_mem_sptr>* >( input[i++].ptr() );
  bocl_mem_sptr mogs_sptr            = mogs->value();

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

  //vis image buffer
  brdb_value_t<vil_image_view_base_sptr>* brdb_vis = static_cast<brdb_value_t<vil_image_view_base_sptr>* >( input[i++].ptr() );
  vil_image_view_base_sptr visimg = brdb_vis->value();
  vil_image_view<float>* vis_img_view = static_cast<vil_image_view<float>* >(visimg.ptr());
  if (!vis_img_) {
    float* vis_buff = vis_img_view->begin();
    vis_img_ = new bocl_mem((*context_), vis_buff, vis_img_view->size() * sizeof(float), "visibility image buffer");
    vis_img_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
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

  //3. SET args
  render_kernel_.set_arg( info_sptr.ptr() );
  render_kernel_.set_arg( blk_sptr.ptr() );
  render_kernel_.set_arg( alpha_sptr.ptr() );
  render_kernel_.set_arg( mogs_sptr.ptr() );
  render_kernel_.set_arg( &persp_cam );
  render_kernel_.set_arg( image_ );
  render_kernel_.set_arg( &exp_img_dim);
  render_kernel_.set_arg( &cl_output );
  render_kernel_.set_arg( &lookup );
  render_kernel_.set_arg( vis_img_ );

  //local tree , cumsum buffer, imindex buffer
  render_kernel_.set_local_arg( lThreads[0]*lThreads[1]*sizeof(cl_uchar16) );
  render_kernel_.set_local_arg( lThreads[0]*lThreads[1]*10*sizeof(cl_uchar) );
  render_kernel_.set_local_arg( lThreads[0]*lThreads[1]*sizeof(cl_int) );

  //execute kernel
  render_kernel_.execute( (*command_queue_), lThreads, gThreads);
  vcl_cout<<"Before "<<(reinterpret_cast<float*>(vis_img_->cpu_buffer()))[0]<<vcl_endl;

  //read output, do something, blah blah
  cl_output.read_to_buffer(*command_queue_);
  image_->read_to_buffer(*command_queue_);
  vis_img_->read_to_buffer(*command_queue_);

  vcl_cout<<"Execution time: "<<render_kernel_.exec_time()<<" ms"<<vcl_endl;

  vcl_cout<<"After "<<(reinterpret_cast<float*>(vis_img_->cpu_buffer()))[0]<<vcl_endl;
  render_kernel_.clear_args();

  //clean up camera, lookup_arr, img_dim_buff
  delete[] output_arr;
  delete[] img_dim_buff;
  delete[] lookup_arr;
  delete[] cam_buffer;
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

