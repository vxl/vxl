#include "bwm_tableau_rat_cam.h"
#include "bwm_popup_menu.h"
#include <vsol/vsol_point_2d.h>

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
