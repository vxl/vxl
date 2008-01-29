#include "bwm_tableau_rat_cam.h"
#include "bwm_popup_menu.h"
#include "bwm_observer_mgr.h"

#include "algo/bwm_utils.h"
#include <vsol/vsol_point_2d.h>

/*bwm_tableau_rat_cam::bwm_tableau_rat_cam(vcl_string name, 
    //vcl_string& image_path, 
    //vcl_string& cam_path, 
    //bool display_image_path) 
    : bwm_tableau_cam(my_observer_), my_observer_(0)
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

  // check if the camera path is not empty, if it is NITF, the camera
  // info is in the image, not a seperate file
  if (cam_path.size() == 0)
  {
    bwm_utils::show_error("Camera tableaus need a valid camera path!");
    return;
  }
  
  vpgl_rational_camera<double> *camera_ = read_rational_camera<double>(cam_path);
  if (!camera_) {
    bwm_utils::show_error("[" + cam_path + "] is not a valid rational camera path");
    return;
  }
  //
  //my_observer_ = new bwm_observer_rat_cam(img, camera_, cam_path);
  //bwm_tableau_cam::set_observer(my_observer_);
  //t = new bwm_tableau_rat_cam(rat_observer);
     
  // add the observer to the observer pool
  //bwm_observer_mgr::instance()->add(my_observer_);
  //my_observer_->set_tab_name(name);
  //vgui_viewer2D_tableau_sptr viewer = vgui_viewer2D_tableau_new(this);

  //my_observer_->set_viewer(viewer);
}*/

bool bwm_tableau_rat_cam::handle(const vgui_event &e)
{
  return bwm_tableau_cam::handle(e);
}

void bwm_tableau_rat_cam::get_popup(vgui_popup_params const &params, vgui_menu &menu)
{
  menu.clear();

  bwm_popup_menu pop(this);
  vgui_menu submenu;
  pop.get_menu(menu);
}

void bwm_tableau_rat_cam::define_lvcs()
{
  float x1, y1;
  pick_point(&x1, &y1);
  my_observer_->define_lvcs(x1, y1);
}

void bwm_tableau_rat_cam::adjust_camera_offset()
{
  float x1, y1;
  pick_point(&x1, &y1);
  vsol_point_2d_sptr img_point = new vsol_point_2d(x1, y1);
  my_observer_->adjust_camera_offset(img_point);
}

void bwm_tableau_rat_cam::adjust_camera_to_world_pt()
{
  my_observer_->adjust_camera_to_world_pt();
}

void bwm_tableau_rat_cam::center_pos()
{
  my_observer_->center_pos();
}

void bwm_tableau_rat_cam::save()
{
  my_observer_->save();
}

void bwm_tableau_rat_cam::project_edges_from_master()
{
  my_observer_->project_edges_from_master();
}

void bwm_tableau_rat_cam::register_search_to_master()
{
  my_observer_->register_search_to_master();
}
