#include <iostream>
#include <sstream>
#include "boxm2_ocl_update_tableau.h"
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
boxm2_ocl_update_tableau::boxm2_ocl_update_tableau()
{
  do_update_ = false;
  DECLARE_FUNC_CONS(boxm2_ocl_update_color_process);
  DECLARE_FUNC_CONS(boxm2_ocl_update_process);
  DECLARE_FUNC_CONS(boxm2_ocl_refine_process);
  DECLARE_FUNC_CONS(boxm2_ocl_merge_process);
  DECLARE_FUNC_CONS(boxm2_ocl_filter_process);
  DECLARE_FUNC_CONS(boxm2_write_cache_process);

  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_update_color_process, "boxm2OclUpdateColorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_update_process, "boxm2OclUpdateProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_refine_process, "boxm2OclRefineProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_merge_process, "boxm2OclMergeProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_filter_process, "boxm2OclFilterProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_write_cache_process, "boxm2WriteCacheProcess");

  REGISTER_DATATYPE(boxm2_cache_sptr);
  REGISTER_DATATYPE(boxm2_opencl_cache_sptr);
  REGISTER_DATATYPE(boxm2_scene_sptr);
  REGISTER_DATATYPE(bocl_mem_sptr);
  REGISTER_DATATYPE(vil_image_view_base_sptr);
  REGISTER_DATATYPE(float);
}

//: initialize tableau properties
bool boxm2_ocl_update_tableau::init_update (bocl_device_sptr device,
                                            boxm2_opencl_cache_sptr opencl_cache,
                                            boxm2_scene_sptr scene,
                                            unsigned ni,
                                            unsigned nj,
                                            vpgl_perspective_camera<double>* cam,
                                            std::vector<std::string>& update_imgs,
                                            std::vector<std::string>& update_cams)
{
  this->init(device, opencl_cache, scene, ni, nj, cam);
  cache_ = opencl_cache->get_cpu_cache();
  cams_ = update_cams;
  imgs_ = update_imgs;
  random_ = vnl_random(9667566);
  return true;
}


//: Handles tableau events (drawing and keys)
bool boxm2_ocl_update_tableau::handle(vgui_event const &e)
{
  //handle update command - keyboard press U
  if (e.type == vgui_KEY_PRESS && e.key == vgui_key('u')) {
    do_update_ = true;
    this->post_idle_request();
  }

  //handle refine command - keyboard press "d" (for divide)
  else if (e.type == vgui_KEY_PRESS && e.key == vgui_key('d')) {
    this->refine(.3f);
  }

  //handle refine command - keyboard press "d" (for divide)
  else if (e.type == vgui_KEY_PRESS && e.key == vgui_key('s')) {
    this->save();
  }

  else if (e.type == vgui_KEY_PRESS && e.key == vgui_key('f')) {
    this->filter();
  }
    else if (e.type == vgui_KEY_PRESS && e.key == vgui_key('m')) {
    this->merge(.2f);
  }

  //HANDLE idle events - do model updating
  else if (e.type == vgui_IDLE)
  {
    if (do_update_) {
      int frame = random_.lrand32(0, imgs_.size()-1);
      std::cout<<"updating with image: "<<imgs_[frame]<<" and "
              <<" cam: "<<cams_[frame]<<std::endl;
      //: Load an image resource object from a file.
      vil_image_view_base_sptr inim = vil_load(imgs_[frame].c_str());
      vpgl_camera_double_sptr incam = boxm2_util::camera_from_file(cams_[frame]);
      this->update_frame(inim, incam);
      this->post_redraw();
      return true;
    }
    else {
      std::cout<<"done idling"<<std::endl;
      return false;
    }
  }

  //if you click on the canvas, you wanna render, so turn off update
  else if (e.type == vgui_BUTTON_DOWN) {
    do_update_ = false;
  }


  if (boxm2_ocl_render_tableau::handle(e)) {
    return true;
  }
  return false;
}

//: updates scene given image and camera
float boxm2_ocl_update_tableau::update_frame(vil_image_view_base_sptr in_im, vpgl_camera_double_sptr in_cam)
{
  //set up brdb_value_sptr arguments...
  brdb_value_sptr brdb_device       = new brdb_value_t<bocl_device_sptr>(device_);
  brdb_value_sptr brdb_scene        = new brdb_value_t<boxm2_scene_sptr>(scene_);
  brdb_value_sptr brdb_opencl_cache = new brdb_value_t<boxm2_opencl_cache_sptr>(opencl_cache_);
  brdb_value_sptr brdb_cam          = new brdb_value_t<vpgl_camera_double_sptr>(in_cam);
  brdb_value_sptr brdb_img          = new brdb_value_t<vil_image_view_base_sptr>(in_im);
  brdb_value_sptr identifier        = new brdb_value_t<std::string>("");
  brdb_value_sptr brdb_mask_img     = new brdb_value_t<std::string>("");//f:/Tailwind/Towerorbit/mask3.png");

  //if scene has RGB data type, use color render process
  bool good =
       scene_->has_data_type(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix())
       ? bprb_batch_process_manager::instance()->init_process("boxm2OclUpdateColorProcess")
       : bprb_batch_process_manager::instance()->init_process("boxm2OclUpdateProcess");

  //set process args
  good = good
      && bprb_batch_process_manager::instance()->set_input(0, brdb_device) // device
      && bprb_batch_process_manager::instance()->set_input(1, brdb_scene)  //  scene
      && bprb_batch_process_manager::instance()->set_input(2, brdb_opencl_cache)
      && bprb_batch_process_manager::instance()->set_input(3, brdb_cam)    // camera
      && bprb_batch_process_manager::instance()->set_input(4, brdb_img)    // input image
      && bprb_batch_process_manager::instance()->set_input(5, identifier)    // input image
     // && bprb_batch_process_manager::instance()->set_input(6, brdb_mask_img)    // input image
      && bprb_batch_process_manager::instance()->run_process();

  if (good)
    return 0.0f;
  else
    return -1.0f;
}

//: refine scene
float boxm2_ocl_update_tableau::refine(float thresh)
{
  //set up brdb_value_sptr arguments...
  brdb_value_sptr brdb_device       = new brdb_value_t<bocl_device_sptr>(device_);
  brdb_value_sptr brdb_scene        = new brdb_value_t<boxm2_scene_sptr>(scene_);
  brdb_value_sptr brdb_opencl_cache = new brdb_value_t<boxm2_opencl_cache_sptr>(opencl_cache_);
  brdb_value_sptr brdb_thresh       = new brdb_value_t<float>(thresh);

  //if scene has RGB data type, use color render process
  bool good = bprb_batch_process_manager::instance()->init_process("boxm2OclRefineProcess");

  //set process args
  good = good
      && bprb_batch_process_manager::instance()->set_input(0, brdb_device) // device
      && bprb_batch_process_manager::instance()->set_input(1, brdb_scene)  //  scene
      && bprb_batch_process_manager::instance()->set_input(2, brdb_opencl_cache)
      && bprb_batch_process_manager::instance()->set_input(3, brdb_thresh)    // camera
      && bprb_batch_process_manager::instance()->run_process();

  if (good)
    return 0.0f;
  else
    return -1.0f;
}
//: refine scene
float boxm2_ocl_update_tableau::merge(float thresh)
{
  //set up brdb_value_sptr arguments...
  brdb_value_sptr brdb_device       = new brdb_value_t<bocl_device_sptr>(device_);
  brdb_value_sptr brdb_scene        = new brdb_value_t<boxm2_scene_sptr>(scene_);
  brdb_value_sptr brdb_opencl_cache = new brdb_value_t<boxm2_opencl_cache_sptr>(opencl_cache_);
  brdb_value_sptr brdb_thresh       = new brdb_value_t<float>(thresh);

  //if scene has RGB data type, use color render process
  bool good = bprb_batch_process_manager::instance()->init_process("boxm2OclMergeProcess");

  //set process args
  good = good
      && bprb_batch_process_manager::instance()->set_input(0, brdb_device) // device
      && bprb_batch_process_manager::instance()->set_input(1, brdb_scene)  //  scene
      && bprb_batch_process_manager::instance()->set_input(2, brdb_opencl_cache)
      && bprb_batch_process_manager::instance()->set_input(3, brdb_thresh)    // camera
      && bprb_batch_process_manager::instance()->run_process();

  if (good)
    return 0.0f;
  else
    return -1.0f;
}
//: filters scene
float boxm2_ocl_update_tableau::filter()
{
  //set up brdb_value_sptr arguments...
  brdb_value_sptr brdb_device       = new brdb_value_t<bocl_device_sptr>(device_);
  brdb_value_sptr brdb_scene        = new brdb_value_t<boxm2_scene_sptr>(scene_);
  brdb_value_sptr brdb_opencl_cache = new brdb_value_t<boxm2_opencl_cache_sptr>(opencl_cache_);

  //if scene has RGB data type, use color render process
  bool good = bprb_batch_process_manager::instance()->init_process("boxm2OclFilterProcess");

  //set process args
  good = good
      && bprb_batch_process_manager::instance()->set_input(0, brdb_device) // device
      && bprb_batch_process_manager::instance()->set_input(1, brdb_scene)  //  scene
      && bprb_batch_process_manager::instance()->set_input(2, brdb_opencl_cache)
      && bprb_batch_process_manager::instance()->run_process();
  if (good)
    return 0.0f;
  else
    return -1.0f;
}

//:save scene
float boxm2_ocl_update_tableau::save()
{
  //set up brdb_value_sptr arguments...
  brdb_value_sptr brdb_scene = new brdb_value_t<boxm2_scene_sptr>(scene_);
  brdb_value_sptr brdb_cache = new brdb_value_t<boxm2_cache_sptr>(cache_);

  //if scene has RGB data type, use color render process
  bool good = bprb_batch_process_manager::instance()->init_process("boxm2WriteCacheProcess");

  //set process args
  good = good
      && bprb_batch_process_manager::instance()->set_input(0, brdb_cache)  //  scene
      && bprb_batch_process_manager::instance()->run_process();

  if (good)
    return 0.0f;
  else
    return -1.0f;
}
