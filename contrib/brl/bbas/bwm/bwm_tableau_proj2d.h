#ifndef bwm_tableau_proj2d_h_
#define bwm_tableau_proj2d_h_

#include "bwm_observable.h"
#include "bwm_observer_proj2d.h"

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_list.h>

#include <vgui/vgui_menu.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_command.h>

#include <bgui/bgui_picker_tableau.h>

#include <vpgl/vpgl_camera.h>

#include <vsol/vsol_polygon_3d_sptr.h>

class bwm_tableau_proj2d : public vgui_wrapper_tableau {

public:
  bwm_tableau_proj2d(bwm_observer_proj2d* obs) : my_observer_(obs) {}

  bwm_tableau_proj2d(vcl_string& image_path, vpgl_camera<double>& camera) {}

  virtual ~bwm_tableau_proj2d(){}

  virtual vcl_string type_name() { return "bwm_tableau_proj2d"; }

 // bool handle(const vgui_event &e) { return bwm_tableau::handle(e); }

  void set_observer(bwm_observer_proj2d* obs) { my_observer_ = obs; }

  void get_popup(vgui_popup_params const &params, vgui_menu &menu){}; 

protected:

  bwm_observer_proj2d* my_observer_;
  
};

#endif
