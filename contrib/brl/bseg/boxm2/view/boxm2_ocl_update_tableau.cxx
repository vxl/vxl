#include "boxm2_ocl_update_tableau.h"
//:
// \file
#include <vil/vil_load.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vgui/vgui_modifier.h>
#include <vcl_sstream.h>
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
  DECLARE_FUNC_CONS(boxm2_ocl_update_color_process);
  DECLARE_FUNC_CONS(boxm2_ocl_update_process);
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_update_color_process, "boxm2OclUpdateColorProcess");
  REG_PROCESS_FUNC_CONS(bprb_func_process, bprb_batch_process_manager, boxm2_ocl_update_process, "boxm2OclUpdateProcess");

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
                                            vcl_vector<vcl_string>& update_imgs,
                                            vcl_vector<vcl_string>& update_cams)
{
  this->init(device, opencl_cache, scene, ni, nj, cam); 
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
    int frame = random_.lrand32(0, imgs_.size()-1);
    
    vcl_cout<<"updating with image: "<<imgs_[frame]<<" and "
            <<" cam: "<<cams_[frame]<<vcl_endl;
    
    //: Load an image resource object from a file.
    vil_image_view_base_sptr inim = vil_load(imgs_[frame].c_str()); 
    vpgl_camera_double_sptr incam = boxm2_util::camera_from_file(cams_[frame]); 
    this->update_frame(inim, incam); 
  }
  
  if (boxm2_ocl_render_tableau::handle(e)) {
    return true;
  }
  return false;
}

//: calls on ray manager to render frame into the pbuffer_
float boxm2_ocl_update_tableau::update_frame(vil_image_view_base_sptr in_im, vpgl_camera_double_sptr in_cam)
{
    //set up brdb_value_sptr arguments...
    brdb_value_sptr brdb_device       = new brdb_value_t<bocl_device_sptr>(device_);
    brdb_value_sptr brdb_scene        = new brdb_value_t<boxm2_scene_sptr>(scene_);
    brdb_value_sptr brdb_opencl_cache = new brdb_value_t<boxm2_opencl_cache_sptr>(opencl_cache_);
    brdb_value_sptr brdb_cam    = new brdb_value_t<vpgl_camera_double_sptr>(in_cam);
    brdb_value_sptr brdb_img    = new brdb_value_t<vil_image_view_base_sptr>(in_im);
    
    //if scene has RGB data type, use color render process
    bool good = true; 
    if(scene_->has_data_type(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix()) )
      good = bprb_batch_process_manager::instance()->init_process("boxm2OclUpdateColorProcess");
    else
      good = bprb_batch_process_manager::instance()->init_process("boxm2OclUpdateProcess");
      
    //set process args
    good = good && bprb_batch_process_manager::instance()->set_input(0, brdb_device); // device
    good = good && bprb_batch_process_manager::instance()->set_input(1, brdb_scene); //  scene 
    good = good && bprb_batch_process_manager::instance()->set_input(2, brdb_opencl_cache); 
    good = good && bprb_batch_process_manager::instance()->set_input(3, brdb_cam);// camera
    good = good && bprb_batch_process_manager::instance()->set_input(4, brdb_img);   // input image
    good = good && bprb_batch_process_manager::instance()->run_process();
    
    //grab float output from render gl process
    //unsigned int time_id = 0;
    //good = good && bprb_batch_process_manager::instance()->commit_output(0, time_id);
    //brdb_query_aptr Q = brdb_query_comp_new("id", brdb_query::EQ, time_id);
    //brdb_selection_sptr S = DATABASE->select("float_data", Q);
    //if (S->size()!=1){
        //vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
            //<< " no selections\n";
    //}
    //brdb_value_sptr value;
    //if (!S->get_value(vcl_string("value"), value)) {
        //vcl_cout << "in bprb_batch_process_manager::set_input_from_db(.) -"
            //<< " didn't get value\n";
    //}
    //float time = value->val<float>();
    return 0.0f;
}
