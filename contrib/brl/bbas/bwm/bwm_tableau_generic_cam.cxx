#include <iostream>
#include <sstream>
#include "bwm_tableau_generic_cam.h"
#include <vpgl/vpgl_generic_camera.h>
#include "bwm_observer_mgr.h"
#include "algo/bwm_utils.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgui/vgui_dialog.h>
#include <vil/vil_save.h>

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
std::string bwm_tableau_generic_cam::save_camera()
{
  std::string cam_path = my_observer_->camera_path();
  return cam_path;
}

void bwm_tableau_generic_cam::ray_image()
{
  static int component = 0;
  static int level=0;
  vgui_dialog ray_dlg("Save ray image");
  static std::string ext, file_path;
  ray_dlg.file("Ray Filename", ext, file_path);
  ray_dlg.field("Ray Level ",  level);
  ray_dlg.choice("Ray component", "origin", "direction", component);
  if (!ray_dlg.ask())
    return;
  vil_image_resource_sptr res = my_observer_->ray_image(component,level);
  if (!res) {
    std::cout << "In bwm_observer_generic_cam:: null ray image returned\n";
    return;
  }
  if (!vil_save_image_resource(res, file_path.c_str()))
    std::cout << "In bwm_tableau_generic_cam:: ray image could not be saved\n";
}
