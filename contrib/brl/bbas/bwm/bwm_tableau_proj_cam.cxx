#include "bwm_tableau_proj_cam.h"
#include "bwm_observer_mgr.h"
#include "algo/bwm_utils.h"

/*bwm_tableau_proj_cam::bwm_tableau_proj_cam(vcl_string name, 
    vcl_string& image_path, 
    vcl_string& cam_path,                
    bool display_image_path)
: bwm_tableau_cam(my_observer_)
{
  bgui_image_tableau_sptr img = bgui_image_tableau_new();
  img->show_image_path(display_image_path);

  // LOAD IMAGE
  vgui_range_map_params_sptr params;
  vil_image_resource_sptr img_res = bwm_utils::load_image(image_path, params);

  if (!img_res) {
     bwm_utils::show_error("Image [" + image_path + "] is NOT found");
     return;
  }

  img->set_image_resource(img_res, params);
  img->set_file_name(image_path);

  // LOAD CAMERA
  //vpgl_proj_camera<double> *camera_proj=0;
  //vpgl_rational_camera<double> *camera_rat=0;
  //my_observer_ = (bwm_observer_cam*)0;
  bwm_tableau_cam* t = 0;

  // check if the camera path is not empty, if it is NITF, the camera
  // info is in the image, not a seperate file
  if (cam_path.size() == 0)
  {
    bwm_utils::show_error("Camera tableaus need a valid camera path!");
    return;
  }
  
   
  camera_ = read_projective_camera(cam_path);
  my_observer_ = new bwm_observer_proj_cam(img, &camera_, cam_path);
  //t = new bwm_tableau_proj_cam(proj_observer);
   
  // add the observer to the observer pool
  bwm_observer_mgr::instance()->add(my_observer_);
  my_observer_->set_tab_name(name);
  vgui_viewer2D_tableau_sptr viewer = vgui_viewer2D_tableau_new(this);

  my_observer_->set_viewer(viewer);
}*/

bool bwm_tableau_proj_cam::handle(const vgui_event &e) 
{ 
  return bwm_tableau_cam::handle(e); 
}

void bwm_tableau_proj_cam::get_popup(vgui_popup_params const &params, vgui_menu &menu) 
{
  vgui_menu parent_menu;
  bwm_tableau_cam::get_popup(params, menu);
}

