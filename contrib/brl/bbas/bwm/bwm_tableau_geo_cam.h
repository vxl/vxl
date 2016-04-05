#ifndef bwm_tableau_geo_cam_h_
#define bwm_tableau_geo_cam_h_
//:
// \file
#include "bwm_tableau_cam.h"
#include "bwm_observer_geo_cam.h"

#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_command.h>

#include <bgui/bgui_picker_tableau.h>
#include <vul/vul_timer.h>

class bwm_tableau_geo_cam : public bwm_tableau_cam
{
 public:

  bwm_tableau_geo_cam(bwm_observer_geo_cam* obs)
    : bwm_tableau_cam(obs), my_observer_(obs) {}

  virtual ~bwm_tableau_geo_cam(){}

  virtual std::string type_name() const { return "bwm_tableau_geo_cam"; }

  bool handle(const vgui_event &);

  void get_popup(vgui_popup_params const &params, vgui_menu &menu);

  //: saves the camera with a new version number (if adjusted) and returns the path
  std::string save_camera();

 protected:
  bwm_observer_geo_cam* my_observer_;
  vul_timer timer_;
};

#endif
