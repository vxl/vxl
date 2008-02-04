#ifndef bwm_tableau_rat_cam_h_
#define bwm_tableau_rat_cam_h_

#include "bwm_tableau_cam.h"
#include "bwm_observer_rat_cam.h"

#include <vgui/vgui_wrapper_tableau.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_command.h>

#include <bgui/bgui_picker_tableau.h>

#include <vpgl/vpgl_rational_camera.h>

class bwm_tableau_rat_cam : public bwm_tableau_cam
{
 protected:
  bwm_observer_rat_cam* my_observer_;

 public:
#if 0 // commented out, both here and in the .cxx file
  bwm_tableau_rat_cam(vcl_string name,
                      vcl_string& image_path,
                      vcl_string& cam_path,
                      bool display_image_path);
#endif

  bwm_tableau_rat_cam(bwm_observer_rat_cam* observer)
    : bwm_tableau_cam(observer), my_observer_(observer) {}

  void set_observer(bwm_observer_rat_cam* observer) { my_observer_ = observer; }

  virtual ~bwm_tableau_rat_cam(){}

  virtual vcl_string type_name() const { return "bwm_tableau_rat_cam"; }

  bool handle(const vgui_event &);

  void get_popup(vgui_popup_params const &params, vgui_menu &menu);

  void adjust_camera_offset();
  void adjust_camera_to_world_pt();
  void center_pos();

  // ******************** LVCS Menu

  void load_lvcs();
  void save_lvcs();
  void define_lvcs();
  void convert_file_to_lvcs();

  // ********************* Save Menu
  void save();

  // ****************** Misc
  void project_edges_from_master();
  void register_search_to_master();
};

#endif
