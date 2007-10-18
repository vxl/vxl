#include "bwm_tableau_lidar.h"
#include <vgui/vgui_projection_inspector.h>
#include <vgui/vgui_dialog.h>

bool bwm_tableau_lidar::handle(const vgui_event &e)
{
  vgui_projection_inspector pi;
    
  if (e.type == vgui_BUTTON_DOWN && e.button == vgui_LEFT && e.modifier == 0) {
    // take the position of the first point
    float x, y; 
    double lat, lon, elev;
    pi.window_to_image_coordinates(e.wx, e.wy, x, y);
    my_observer_->get_img_to_wgs(x, y, lat, lon, elev);
    vcl_cout << "[" << x << "," << y << "] -> (" << 
      lat << "," << lon << "," << elev << ")" << vcl_endl;
    return true;
  }
  return bwm_tableau_cam::handle(e);
}

class bwm_label_lidar_command : public vgui_command
{
 public:
  bwm_label_lidar_command(bwm_tableau_lidar* t) : tab(t) {}
  void execute() { tab->label_lidar(); }

  bwm_tableau_lidar *tab;
};

void bwm_tableau_lidar::get_popup(vgui_popup_params const &params, vgui_menu &menu)
{
  vgui_menu  submenu;
  
  submenu.add("Label (Color)...", new bwm_label_lidar_command(this)); 
  menu.add("Lidar", submenu);
}

void bwm_tableau_lidar::label_lidar() 
{
  
  double gnd_thresh = 1.0;         // maximum ground height
  double bld_diff_thresh = 0.5;    // maximum first/last return diff for building
  double min_bld_height = 2.5;     // minimum building height
  double min_bld_area = 35;        // minimum building area (in pixels)
  double max_veg_height = 10.0;    // anything above will automatically be labeled building
  double veg_diff_thresh = 0.75;   // minimum first/last return difference for vegitation

  vgui_dialog params("Labeling Parameters");
  params.field("Max Ground Height..", gnd_thresh);
  params.field("Max first/last return diff..", bld_diff_thresh);
  params.field("Min Building Height..", min_bld_height);
  params.field("Min Building Area..", min_bld_area);
  params.field("Max Vegitation Height..", max_veg_height);
  params.field("first/last return diff for vegitation..", veg_diff_thresh);
  if (!params.ask())
    return;

  lidar_labeling_params lidar_params(gnd_thresh, bld_diff_thresh, min_bld_height, min_bld_area, max_veg_height, veg_diff_thresh);
  my_observer_->label_lidar(lidar_params);
}
