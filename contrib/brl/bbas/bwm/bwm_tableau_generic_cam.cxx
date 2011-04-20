#include "bwm_tableau_generic_cam.h"
#include <vpgl/vpgl_generic_camera.h>
#include "bwm_observer_mgr.h"
#include "algo/bwm_utils.h"
#include <vul/vul_file.h>
#include <vcl_sstream.h>


bool bwm_tableau_generic_cam::handle(const vgui_event &e)
{
  return bwm_tableau_cam::handle(e);
}

void bwm_tableau_generic_cam::get_popup(vgui_popup_params const &params, vgui_menu &menu)
{
  vgui_menu parent_menu;
  bwm_tableau_cam::get_popup(params, menu);
}
// basically does nothing for now
vcl_string bwm_tableau_generic_cam::save_camera()
{
  vcl_string cam_path = my_observer_->camera_path();
  return cam_path;
}
