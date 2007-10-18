#include "bwm_tableau_proj_cam.h"

bool bwm_tableau_proj_cam::handle(const vgui_event &e) 
{ 
  return bwm_tableau_cam::handle(e); 
}

void bwm_tableau_proj_cam::get_popup(vgui_popup_params const &params, vgui_menu &menu) 
{
  vgui_menu parent_menu;
  bwm_tableau_cam::get_popup(params, parent_menu);
 
  menu.add("3D Modeling", parent_menu);
}

