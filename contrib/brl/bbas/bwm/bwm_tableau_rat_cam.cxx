#include "bwm_tableau_rat_cam.h"
#include "bwm_popup_menu.h"
#include "bwm_observer_mgr.h"

#include "algo/bwm_utils.h"
#include <vsol/vsol_point_2d.h>
#include <vul/vul_file.h>

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

vcl_string bwm_tableau_rat_cam::save_camera()
{
  vcl_string img_path = this->img_path();
  vcl_string cam_path = my_observer_->camera_path();

  // see if the camera is adjusted
  if (my_observer_->camera_adjusted()) {
    //need to save the new camera
    vcl_string new_cam_path = vul_file::strip_extension(cam_path);
    int pos = new_cam_path.find("_v", 0);
    if (pos != vcl_string::npos) {
      new_cam_path.erase(pos, new_cam_path.length()-1);
    }
    vcl_stringstream strm;
    strm << vcl_fixed << timer_.real();
    vcl_string str(strm.str());
    new_cam_path += "_v" + str + vul_file::extension(cam_path);
    my_observer_->camera().save(new_cam_path);
    return new_cam_path;
  }
  return "";
}
