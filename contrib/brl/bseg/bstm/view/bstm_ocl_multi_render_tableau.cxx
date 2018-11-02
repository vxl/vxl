#include <iostream>
#include <sstream>
#include "bstm_ocl_multi_render_tableau.h"
//:
// \file
#include <vpgl/vpgl_perspective_camera.h>
#include <vgui/vgui_modifier.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>

#include <boxm2/view/boxm2_view_utils.h>

#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>

//: Constructor
bstm_ocl_multi_render_tableau::bstm_ocl_multi_render_tableau()
{
  num_gpus_ = 0;
  ni_=1280;
  nj_=720;
  DECLARE_FUNC_CONS(bstm_ocl_render_gl_expected_image_process);
  DECLARE_FUNC_CONS(bstm_ocl_render_gl_expected_color_process);
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_ocl_render_gl_expected_image_process, "bstmOclRenderGlExpectedImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, bstm_ocl_render_gl_expected_color_process, "bstmOclRenderGlExpectedColorProcess");

  REGISTER_DATATYPE(bstm_opencl_cache_sptr);
  REGISTER_DATATYPE(bstm_scene_sptr);
  REGISTER_DATATYPE(bocl_mem_sptr);
  REGISTER_DATATYPE(float);
}

//: initialize tableau properties
bool bstm_ocl_multi_render_tableau::init(std::vector<bocl_device_sptr> devices,
                                    std::vector<bstm_opencl_cache_sptr> opencl_caches,
                                    bstm_scene_sptr scene,
                                    unsigned ni,
                                    unsigned nj,
                                    vpgl_perspective_camera<double> * cam,vgui_slider_tableau_sptr slider)
{
    //set image dimensions, camera and scene
    ni_ = ni;
    nj_ = nj;


    cam_   = (*cam);
    default_cam_ = (*cam);

    default_stare_point_.set(scene->bounding_box().centroid().x(),
                             scene->bounding_box().centroid().y(),
                             scene->bounding_box().min_z());
    stare_point_=default_stare_point_;
    scale_ =scene->bounding_box().height();
    //create the scene
    scene_ = scene;
    opencl_caches_=opencl_caches;
    devices_=devices;
    num_gpus_ = opencl_caches.size();
    queues_.resize(num_gpus_);
    pbuffer_.resize(num_gpus_);
    clgl_buffer_.resize(num_gpus_);
    exp_img_.resize(num_gpus_);
    exp_img_dim_.resize(num_gpus_);


    do_init_ocl=true;
    render_trajectory_ = true;
    render_label_ = false;
    trajectory_ = new boxm2_trajectory(80.0, 80.0, -1.0, scene_->bounding_box(), ni, nj);
    cam_iter_ = trajectory_->begin();

    //set bb for time
    scene->bounding_box_t(scene_min_t_,scene_max_t_);

    slider_ = slider;


    compute_gpu_assignments();

    return true;
}


void bstm_ocl_multi_render_tableau::compute_gpu_assignments()
{
  //compute total gpu mem
  total_gpu_mem_ = 0;
  for(int i = 0; i < num_gpus_; i++)
    total_gpu_mem_ += opencl_caches_[i]->max_memory_in_cache();

  unsigned long curr_mem_size = 0;
  unsigned curr_gpu_id = 0;

  unsigned min_block_id,max_block_id;
  scene_->blocks_ids_bounding_box_t(min_block_id,max_block_id);
  std::cout << "Scene is comprised of " << max_block_id - min_block_id << " time blocks..." << std::endl;
  gpu_ids_.resize(max_block_id - min_block_id);
  for(int scene_blk_id = 0; scene_blk_id< gpu_ids_.size(); scene_blk_id++)
  {
    std::cout << "Is " << (float)scene_blk_id / gpu_ids_.size() << " smaller than " << (float)(curr_mem_size  + opencl_caches_[curr_gpu_id]->max_memory_in_cache()) / total_gpu_mem_ << std::endl;
    if((float)scene_blk_id / gpu_ids_.size() < (float)(curr_mem_size  + opencl_caches_[curr_gpu_id]->max_memory_in_cache()) / total_gpu_mem_)
      gpu_ids_[scene_blk_id] = curr_gpu_id;
    else {
      curr_mem_size  += opencl_caches_[curr_gpu_id]->max_memory_in_cache();
      gpu_ids_[scene_blk_id] = ++curr_gpu_id;
    }
    std::cout << gpu_ids_[scene_blk_id]  << std::endl;
  }

}

//: Handles tableau events (drawing and keys)
bool bstm_ocl_multi_render_tableau::handle(vgui_event const &e)
{

  //toggle color - this is a hack to get color models to show as grey
  if (e.type == vgui_KEY_PRESS) {
    if (e.key == vgui_key('p')) {
      //play...
    }
    if (e.key == vgui_key('l')) {
      render_label_ = !render_label_;
    }
  }
  else if (e.type == vgui_DRAW) //draw handler - called on post_draw()
  {

    if (do_init_ocl) {
      this->init_clgl();
      do_init_ocl = false;
    }

    float gpu_time = this->render_frame();
    this->setup_gl_matrices();
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glRasterPos2i(0, 1);
    glPixelZoom(1,-1);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, pbuffer_[gpu_id(time_)]);
    glDrawPixels(ni_, nj_, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

    //calculate and write fps to status
    std::stringstream str;
    str<<".  rendering at ~ "<< (1000.0f / gpu_time) <<" fps ";
    if (status_) {
      status_->write(str.str().c_str());
    }
    return true;
  }

  if (bstm_cam_tableau::handle(e))
    return true;


  return false;
}

//TODO: the gpu assignment could be made intelligent
//Currently it divides evenly the time among gpus.
unsigned bstm_ocl_multi_render_tableau::gpu_id(double time)
{
  return gpu_ids_[std::floor( time * gpu_ids_.size() )];
}

//: calls on ray manager to render frame into the pbuffer_
float bstm_ocl_multi_render_tableau::render_frame()
{
  int gpu_idx = gpu_id(time_);

  cl_int status = clEnqueueAcquireGLObjects( queues_[gpu_idx], 1,&exp_img_[gpu_idx]->buffer(), 0, 0, 0);
  exp_img_[gpu_idx]->zero_gpu_buffer( queues_[gpu_idx] );
  if (!check_val(status,CL_SUCCESS,"clEnqueueAcquireGLObjects failed. (gl_image)"+error_to_string(status)))
      return -1.0f;

  double scaled_time = scene_min_t_ + time_ * (scene_max_t_ - scene_min_t_);

  //set up brdb_value_sptr arguments...
  brdb_value_sptr brdb_device = new brdb_value_t<bocl_device_sptr>(devices_[gpu_idx]);
  brdb_value_sptr brdb_scene = new brdb_value_t<bstm_scene_sptr>(scene_);
  brdb_value_sptr brdb_opencl_cache = new brdb_value_t<bstm_opencl_cache_sptr>(opencl_caches_[gpu_idx]);
  vpgl_camera_double_sptr cam = new vpgl_perspective_camera<double>(cam_);
  brdb_value_sptr brdb_cam = new brdb_value_t<vpgl_camera_double_sptr>(cam);
  brdb_value_sptr brdb_ni = new brdb_value_t<unsigned>(ni_);
  brdb_value_sptr brdb_nj = new brdb_value_t<unsigned>(nj_);
  brdb_value_sptr exp_img = new brdb_value_t<bocl_mem_sptr>(exp_img_[gpu_idx]);
  brdb_value_sptr exp_img_dim = new brdb_value_t<bocl_mem_sptr>(exp_img_dim_[gpu_idx]);
  brdb_value_sptr brdb_time = new brdb_value_t<float>(scaled_time);
  brdb_value_sptr brdb_render_label = new brdb_value_t<bool>(render_label_);



  //if scene has RGB data type, use color render process
  bool good = true;
  if(scene_->has_data_type(bstm_data_traits<BSTM_GAUSS_RGB>::prefix()) )
    good = bprb_batch_process_manager::instance()->init_process("bstmOclRenderGlExpectedColorProcess");
  else
    good = bprb_batch_process_manager::instance()->init_process("bstmOclRenderGlExpectedImageProcess");

  //set process args
  good = good && bprb_batch_process_manager::instance()->set_input(0, brdb_device); // device
  good = good && bprb_batch_process_manager::instance()->set_input(1, brdb_scene); //  scene
  good = good && bprb_batch_process_manager::instance()->set_input(2, brdb_opencl_cache);
  good = good && bprb_batch_process_manager::instance()->set_input(3, brdb_cam);// camera
  good = good && bprb_batch_process_manager::instance()->set_input(4, brdb_ni);  // ni for rendered image
  good = good && bprb_batch_process_manager::instance()->set_input(5, brdb_nj);   // nj for rendered image
  good = good && bprb_batch_process_manager::instance()->set_input(6, exp_img);   // exp image ( gl buffer)
  good = good && bprb_batch_process_manager::instance()->set_input(7, exp_img_dim);   // exp image dimensions
  good = good && bprb_batch_process_manager::instance()->set_input(8, brdb_time);   // time
  good = good && bprb_batch_process_manager::instance()->set_input(9, brdb_render_label);   // render_label?

  good = good && bprb_batch_process_manager::instance()->run_process();

  //grab float output from render gl process
  unsigned int time_id = 0;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, time_id);
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, time_id);
  brdb_selection_sptr S = DATABASE->select("float_data", Q);
  if (S->size()!=1){
      std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
          << " no selections\n";
  }
  brdb_value_sptr value;
  if (!S->get_value(std::string("value"), value)) {
      std::cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
          << " didn't get value\n";
  }
  float time = value->val<float>();

  //release gl buffer
  status = clEnqueueReleaseGLObjects(queues_[gpu_idx], 1, &exp_img_[gpu_idx]->buffer(), 0, 0, 0);
  clFinish( queues_[gpu_idx] );

  return time;

}

//: private helper method to init_clgl stuff (gpu processor)
bool bstm_ocl_multi_render_tableau::init_clgl()
{

  for(int gpu_no = 0; gpu_no < num_gpus_; gpu_no++)
  {
    //get relevant blocks
    std::cout<<"Data Path: "<<scene_->data_path()<<std::endl;
    devices_[gpu_no]->context() = boxm2_view_utils::create_clgl_context(*(devices_[gpu_no]->device_id()));
    opencl_caches_[gpu_no]->set_context(devices_[gpu_no]->context());

    std::cout << "HANDLE" << std::endl;

    int status_queue=0;
    queues_[gpu_no] =  clCreateCommandQueue(devices_[gpu_no]->context(),*(devices_[gpu_no]->device_id()),CL_QUEUE_PROFILING_ENABLE,&status_queue);



    // delete old buffer
    if (pbuffer_[gpu_no]) {
        clReleaseMemObject(clgl_buffer_[gpu_no]);
        glDeleteBuffers(1, &pbuffer_[gpu_no]);
    }

    ////generate glBuffer, and bind to ray_mgr->image_gl_buf_
    glGenBuffers(1, &pbuffer_[gpu_no]);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbuffer_[gpu_no]);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, ni_*nj_*sizeof(GLubyte)*4, 0, GL_STREAM_DRAW);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    //create OpenCL buffer from GL PBO, and set kernel and arguments
    int status = 0;
    clgl_buffer_[gpu_no] = clCreateFromGLBuffer(devices_[gpu_no]->context(),
                                        CL_MEM_WRITE_ONLY,
                                        pbuffer_[gpu_no],
                                        &status);
    exp_img_[gpu_no] = new bocl_mem(devices_[gpu_no]->context(),  NULL, RoundUp(ni_,8)*RoundUp(nj_,8)*sizeof(GLubyte)*4, "exp image (gl) buffer");
    exp_img_[gpu_no]->set_gl_buffer(clgl_buffer_[gpu_no]);

    int img_dim_buff[4];
    img_dim_buff[0] = 0;   img_dim_buff[2] = ni_;
    img_dim_buff[1] = 0;   img_dim_buff[3] = nj_;
    exp_img_dim_[gpu_no]=new bocl_mem(devices_[gpu_no]->context(), img_dim_buff, sizeof(int)*4, "image dims");
    exp_img_dim_[gpu_no]->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);


  }
  return true;
}
