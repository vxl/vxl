#include <iostream>
#include <sstream>
#include "boxm2_ocl_change_tableau.h"
//:
// \file
#include <vil/vil_load.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vgui/vgui_modifier.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/boxm2_util.h>
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
boxm2_ocl_change_tableau::boxm2_ocl_change_tableau()
{
  DECLARE_FUNC_CONS(boxm2_ocl_change_detection_process);
  DECLARE_FUNC_CONS(boxm2_ocl_render_expected_image_process);
  DECLARE_FUNC_CONS(boxm2_ocl_render_expected_color_process);
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_expected_image_process, "boxm2OclRenderExpectedImageProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_render_expected_color_process, "boxm2OclRenderExpectedColorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_change_detection_process, "boxm2OclChangeDetectionProcess");

  REGISTER_DATATYPE(boxm2_cache_sptr);
  REGISTER_DATATYPE(boxm2_opencl_cache_sptr);
  REGISTER_DATATYPE(boxm2_scene_sptr);
  REGISTER_DATATYPE(bocl_mem_sptr);
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  REGISTER_DATATYPE(float);
}

//: initialize tableau properties
bool boxm2_ocl_change_tableau::init_change (bocl_device_sptr device,
                                            boxm2_opencl_cache_sptr opencl_cache,
                                            boxm2_scene_sptr scene,
                                            unsigned ni,
                                            unsigned nj,
                                            vpgl_perspective_camera<double>* cam,
                                            std::vector<std::string>& change_imgs,
                                            std::vector<std::string>& change_cams)
{
  this->init(device, opencl_cache, scene, ni, nj, cam);
  cams_ = change_cams;
  imgs_ = change_imgs;
  frame_ = 0;
  do_render_change_ = false;
  n_ = 1;
  return true;
}


//: Handles tableau events (drawing and keys)
bool boxm2_ocl_change_tableau::handle(vgui_event const &e)
{
  //if we're in change mode, grab draw
  if (do_render_change_ && e.type == vgui_DRAW) {
    float gpu_time = 0.0f;
    this->setup_gl_matrices();
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glRasterPos2i(0, 1);
    glPixelZoom(1,-1);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, pbuffer_);
    glDrawPixels(ni_, nj_, GL_RGBA, GL_UNSIGNED_BYTE, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
    return true;
  }

  //handle update command - keyboard press U
  if (e.type == vgui_KEY_PRESS && e.key == vgui_key('n')) {
    frame_ = (frame_+1 >= cams_.size()) ? 0 : frame_+1;
    do_render_change_ = true;
    this->change_detect(frame_);
    this->post_redraw();
    return true;
  }
  else if ( e.type == vgui_KEY_PRESS && e.key == vgui_key('p') ) {
    frame_ = (frame_-1 < 0) ? cams_.size()-1 : frame_-1;
    do_render_change_ = true;
    this->change_detect(frame_);
    this->post_redraw();
    return true;
  }
  else if ( e.type == vgui_KEY_PRESS && e.key == vgui_key('+') ){
    n_ = (n_+2 > 9) ?  9 : n_+2;
    std::cout<<"Setting N to "<< n_ <<std::endl;
    this->change_detect(frame_);
    this->post_redraw();
  }
  else if ( e.type == vgui_KEY_PRESS && e.key == vgui_key('-') ){
    n_ = (n_ - 2 < 1) ? 1 : n_-2;
    std::cout<<"Setting N to "<<n_<<std::endl;
    this->change_detect(frame_);
    this->post_redraw();
  }
  else if ( e.type == vgui_KEY_PRESS && e.key == vgui_key('b') ) {
    do_render_change_ = false;
  }

  //otherwise let the ocl_render_tableau handle stuff
  if (boxm2_ocl_render_tableau::handle(e)) {
    return true;
  }
  return false;
}

//: updates scene given image and camera
float boxm2_ocl_change_tableau::change_detect(int frame)
{
  std::cout<<"Running change detection on frame: "<<frame<<std::endl;

  //image and camera from file
  vil_image_view_base_sptr change_img = vil_load(imgs_[frame].c_str());
  vpgl_camera_double_sptr  change_cam = boxm2_util::camera_from_file(cams_[frame]);

  //----------------------------------------------------------------------------
  //render image from cam viewpoint
  //--------------------------------------------------------------------  --------
  bool good = true;
  if (scene_->has_data_type(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix()) )
    good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderExpectedColorProcess");
  else
    good = bprb_batch_process_manager::instance()->init_process("boxm2OclRenderExpectedImageProcess");

  brdb_value_sptr brdb_device       = new brdb_value_t<bocl_device_sptr>(device_);
  brdb_value_sptr brdb_scene        = new brdb_value_t<boxm2_scene_sptr>(scene_);
  brdb_value_sptr brdb_opencl_cache = new brdb_value_t<boxm2_opencl_cache_sptr>(opencl_cache_);
  brdb_value_sptr brdb_cam          = new brdb_value_t<vpgl_camera_double_sptr>(change_cam);
  brdb_value_sptr brdb_ni           = new brdb_value_t<unsigned>(ni_);
  brdb_value_sptr brdb_nj           = new brdb_value_t<unsigned>(nj_);

  //set process args
  good = good && bprb_batch_process_manager::instance()->set_input(0, brdb_device); // device
  good = good && bprb_batch_process_manager::instance()->set_input(1, brdb_scene); //  scene
  good = good && bprb_batch_process_manager::instance()->set_input(2, brdb_opencl_cache);
  good = good && bprb_batch_process_manager::instance()->set_input(3, brdb_cam);// camera
  good = good && bprb_batch_process_manager::instance()->set_input(4, brdb_ni);  // ni for rendered image
  good = good && bprb_batch_process_manager::instance()->set_input(5, brdb_nj);   // nj for rendered image
  good = good && bprb_batch_process_manager::instance()->run_process();

  //vil image output from render process
  unsigned int time_id = 0;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, time_id);
  brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, time_id);
  brdb_selection_sptr S = DATABASE->select("vil_image_view_base_sptr_data", Q);
  if (S->size()!=1){
      std::cout << "in bprb_batch_process_manager (from ocl_change_tableau)::set_input_from_db(.) -"
               << " no selections\n";
  }
  brdb_value_sptr value;
  if (!S->get_value(std::string("value"), value)) {
      std::cout << "in bprb_batch_process_manager (from ocl_change_tableau)::set_input_from_db(.) -"
               << " didn't get value\n";
  }
  vil_image_view_base_sptr exp_img = value->val<vil_image_view_base_sptr>();

  //----------------------------------------------------------------------------
  // run change detection using rendered image....
  //----------------------------------------------------------------------------
  //set up brdb_value_sptr arguments...
  brdb_value_sptr brdb_change_img   = new brdb_value_t<vil_image_view_base_sptr>(change_img);
  brdb_value_sptr brdb_exp_img      = new brdb_value_t<vil_image_view_base_sptr>(exp_img);
  brdb_value_sptr brdb_n            = new brdb_value_t<int>(n_);

  //if scene has RGB data type, use color render process
  good = bprb_batch_process_manager::instance()->init_process("boxm2OclChangeDetectionProcess");

  //set process args
  good = good
      && bprb_batch_process_manager::instance()->set_input(0, brdb_device) // device
      && bprb_batch_process_manager::instance()->set_input(1, brdb_scene)  //  scene
      && bprb_batch_process_manager::instance()->set_input(2, brdb_opencl_cache)
      && bprb_batch_process_manager::instance()->set_input(3, brdb_cam)    // camera
      && bprb_batch_process_manager::instance()->set_input(4, brdb_change_img)    // input image
      && bprb_batch_process_manager::instance()->set_input(5, brdb_exp_img)    // input image
      && bprb_batch_process_manager::instance()->set_input(6, brdb_n)         //nxn change
      && bprb_batch_process_manager::instance()->run_process();

  //vil image output from render process
  unsigned int change_id = 0;
  good = good && bprb_batch_process_manager::instance()->commit_output(0, change_id);
  Q = brdb_query_comp_new("id", brdb_query::EQ, change_id);
  S = DATABASE->select("vil_image_view_base_sptr_data", Q);
  if (S->size()!=1){
      std::cout << "in bprb_batch_process_manager (from ocl_change_tableau)::set_input_from_db(.) -"
               << " no selections\n";
  }
  if (!S->get_value(std::string("value"), value)) {
      std::cout << "in bprb_batch_process_manager (from ocl_change_tableau)::set_input_from_db(.) -"
               << " didn't get value\n";
  }
  vil_image_view_base_sptr change_out = value->val<vil_image_view_base_sptr>();

  //----------------------------------------------------------------------------
  // Write change image to p buffer
  //----------------------------------------------------------------------------

  //get usable img pointers
  vil_image_view<vxl_byte>* in  = (vil_image_view<vxl_byte>*) change_img.ptr();
  vil_image_view<float>*    out = (vil_image_view<float>*) change_out.ptr();

  //test CP buffer
  cl_uint* cpbuffer = new cl_uint[ni_*nj_];
  int c = 0;
  for (int j=0; j<out->nj(); ++j) {
    for (int i=0; i<out->ni(); ++i) {
      vxl_byte pix_in = (*in)(i,j)/4;
      float pchange   = (*out)(i,j);
      cpbuffer[c]     =   ((unsigned int) (255) << 24)
                        | ((unsigned int) (pix_in) << 16)
                        | ((unsigned int) (pix_in) << 8 )
                        | ((unsigned int) (pchange*255.0f));
      ++c;
    }
  }

  //write to gpu buffer
  cl_int status = clEnqueueAcquireGLObjects( queue_, 1,&exp_img_->buffer(), 0, 0, 0);
  exp_img_->zero_gpu_buffer( queue_ );
  std::cout<<"Writing to buffer on gpu ..." <<std::endl;
  cl_event ceEvent_ = 0;
  status = clEnqueueWriteBuffer( queue_,
                                 exp_img_->buffer(),
                                 CL_TRUE,          //True=BLocking, False=NonBlocking
                                 0,
                                 ni_*nj_*sizeof(cl_uint),
                                 cpbuffer,
                                 0,                //cl_uint num_events_in_wait_list
                                 0,
                                 &ceEvent_);
  if (!check_val(status,MEM_FAILURE,"clEnqueueWriteBuffer from aux failed: " + error_to_string(status)))
    return MEM_FAILURE;

  //release gl buffer
  status = clEnqueueReleaseGLObjects(queue_, 1, &exp_img_->buffer(), 0, 0, 0);
  clFinish( queue_ );

  if (good)
    return 0.0f;
  else
    return -1.0f;
}
