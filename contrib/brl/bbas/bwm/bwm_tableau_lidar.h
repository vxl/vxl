#ifndef bwm_tableau_lidar_h_
#define bwm_tableau_lidar_h_

#include "bwm_tableau_cam.h"
#include "bwm_observable.h"
#include "bwm_observer_lidar.h"

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_list.h>

#include <vgui/vgui_menu.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_command.h>

#include <bgui/bgui_picker_tableau.h>

#include <vpgl/vpgl_camera.h>

#include <vsol/vsol_polygon_3d_sptr.h>

class bwm_tableau_lidar : public bwm_tableau_cam {

public:

  bwm_tableau_lidar(bwm_observer_lidar* observer) 
    : bwm_tableau_cam(observer), my_observer_(observer) {}

  virtual ~bwm_tableau_lidar(){}

  virtual vcl_string type_name() const { return "bwm_tableau_lidar"; }

  bool handle(const vgui_event &e);

  void set_observer(bwm_observer_lidar* obs) { my_observer_ = obs; }

  void get_popup(vgui_popup_params const &params, vgui_menu &menu); 

  void label_lidar();

protected:

  bwm_observer_lidar* my_observer_;
  
};

#endif
