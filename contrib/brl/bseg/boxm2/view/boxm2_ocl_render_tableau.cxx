#include "boxm2_ocl_render_tableau.h"
//:
// \file
#include <vpgl/vpgl_perspective_camera.h>
#include <vgui/vgui_modifier.h>
#include <vcl_sstream.h>
#include <boxm2/ocl/boxm2_ocl_util.h>
#include <boxm2/view/boxm2_view_utils.h>
#include <bocl/bocl_device.h>
#include <bocl/bocl_kernel.h>


#include <brdb/brdb_value.h>
#include <brdb/brdb_selection.h>

#include <bprb/bprb_batch_process_manager.h>
#include <bprb/bprb_parameters.h>
#include <bprb/bprb_macros.h>
#include <bprb/bprb_func_process.h>

//: Constructor
boxm2_ocl_render_tableau::boxm2_ocl_render_tableau()
{
  curr_frame_ = 0;
  pbuffer_=0;
  ni_=640;
  nj_=480;
  DECLARE_FUNC_CONS(boxm2_ocl_render_gl_expected_image_process);
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_gl_expected_image_process, "boxm2OclRenderGlExpectedImageProcess");

  REGISTER_DATATYPE(boxm2_opencl_cache_sptr);
  REGISTER_DATATYPE(boxm2_scene_sptr);
  REGISTER_DATATYPE(bocl_mem_sptr);
}

//: initialize tableau properties
bool boxm2_ocl_render_tableau::init(vcl_string scene_file,
                                    unsigned ni,
                                    unsigned nj,
                                    vpgl_perspective_camera<double> * cam)
{
    //set image dimensions, camera and scene
    ni_ = ni;
    nj_ = nj;
    cam_   = (*cam);
    default_cam_ = (*cam);
    //create the scene
    scene_ = new boxm2_scene(scene_file);
    do_init_ocl=true;

  return true;
}


//: Handles tableau events (drawing and keys)
bool boxm2_ocl_render_tableau::handle(vgui_event const &e)
{
  //draw handler - called on post_draw()
  if (e.type == vgui_DRAW)
  {
    if (do_init_ocl) {
      this->init_clgl();
      do_init_ocl = false;
      vcl_cout<<" ::::::"<<do_init_ocl<<";;;;;;;";
    }
    float gpu_time = this->render_frame();
    this->setup_gl_matrices();
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glRasterPos2i(0, 1);
    glPixelZoom(1,-1);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, pbuffer_);
    glDrawPixels(ni_, nj_, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);

    //calculate and write fps to status
    vcl_stringstream str;
    str<<".  rendering at ~ "<< (1000.0f / gpu_time) <<" fps ";
    status_->write(str.str().c_str());

    return true;
  }



  if (boxm2_cam_tableau::handle(e))
    return true;

  return false;
}

//: calls on ray manager to render frame into the pbuffer_
float boxm2_ocl_render_tableau::render_frame()
{
    cl_int status = clEnqueueAcquireGLObjects( queue_, 1,&exp_img_->buffer(), 0, 0, 0);
    exp_img_->zero_gpu_buffer( queue_ );
    if (!check_val(status,CL_SUCCESS,"clEnqueueAcquireGLObjects failed. (gl_image)"+error_to_string(status)))
        return -1.0f;
    brdb_value_sptr brdb_device = new brdb_value_t<bocl_device_sptr>(device_);
    brdb_value_sptr brdb_scene = new brdb_value_t<boxm2_scene_sptr>(scene_);
    brdb_value_sptr brdb_opencl_cache = new brdb_value_t<boxm2_opencl_cache_sptr>(opencl_cache_);
    vpgl_camera_double_sptr cam = new vpgl_perspective_camera<double>(cam_);
    brdb_value_sptr brdb_cam = new brdb_value_t<vpgl_camera_double_sptr>(cam);
    brdb_value_sptr brdb_ni = new brdb_value_t<unsigned>(ni_);
    brdb_value_sptr brdb_nj = new brdb_value_t<unsigned>(nj_);
    brdb_value_sptr exp_img = new brdb_value_t<bocl_mem_sptr>(exp_img_);
    brdb_value_sptr exp_img_dim = new brdb_value_t<bocl_mem_sptr>(exp_img_dim_);
    //: the process goes here
    bool good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderGlExpectedImageProcess");
    good = good && bprb_batch_process_manager::instance()->set_input(0, brdb_device); // device
    good = good && bprb_batch_process_manager::instance()->set_input(1, brdb_scene); //  scene 
    good = good && bprb_batch_process_manager::instance()->set_input(2, brdb_opencl_cache); 
    good = good && bprb_batch_process_manager::instance()->set_input(3, brdb_cam);// camera
    good = good && bprb_batch_process_manager::instance()->set_input(4, brdb_ni);  // ni for rendered image
    good = good && bprb_batch_process_manager::instance()->set_input(5, brdb_nj);   // nj for rendered image
    good = good && bprb_batch_process_manager::instance()->set_input(6, exp_img);   // exp image ( gl buffer)
    good = good && bprb_batch_process_manager::instance()->set_input(7, exp_img_dim);   // exp image dimensions
    good = good && bprb_batch_process_manager::instance()->run_process();

    status = clEnqueueReleaseGLObjects(queue_, 1, &exp_img_->buffer(), 0, 0, 0);
    clFinish( queue_ );
    return 0.0f;
}

//: private helper method to init_clgl stuff (gpu processor)
bool boxm2_ocl_render_tableau::init_clgl()
{
  //get relevant blocks
  vcl_cout<<"Data Path: "<<scene_->data_path()<<vcl_endl;
  mgr_ =bocl_manager_child::instance();
  device_ = mgr_->gpus_[0];
  device_->context() = boxm2_view_utils::create_clgl_context(*(device_->device_id()));

  //create cache, grab singleton instance
  boxm2_lru_cache::create(scene_);
  opencl_cache_=new boxm2_opencl_cache(scene_, device_);


  int status_queue=0;
  queue_ =  clCreateCommandQueue(device_->context(),*(device_->device_id()),CL_QUEUE_PROFILING_ENABLE,&status_queue);

  // delete old buffer
  if (pbuffer_) {
      clReleaseMemObject(clgl_buffer_);
      glDeleteBuffers(1, &pbuffer_);
  }

  ////generate glBuffer, and bind to ray_mgr->image_gl_buf_
  glGenBuffers(1, &pbuffer_);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pbuffer_);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, ni_*nj_*sizeof(GLubyte)*4, 0, GL_STREAM_DRAW);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

  //create OpenCL buffer from GL PBO, and set kernel and arguments
  int status = 0;
  clgl_buffer_ = clCreateFromGLBuffer(device_->context(),
                                      CL_MEM_WRITE_ONLY,
                                      pbuffer_,
                                      &status);
  exp_img_ = new bocl_mem(device_->context(),  NULL, RoundUp(ni_,8)*RoundUp(nj_,8)*sizeof(GLubyte)*4, "exp image (gl) buffer");
  exp_img_->set_gl_buffer(clgl_buffer_);

  int img_dim_buff[4];
  img_dim_buff[0] = 0;   img_dim_buff[2] = ni_;
  img_dim_buff[1] = 0;   img_dim_buff[3] = nj_;
  exp_img_dim_=new bocl_mem(device_->context(), img_dim_buff, sizeof(int)*4, "image dims");
  exp_img_dim_->create_buffer(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR);
  return true;
}
