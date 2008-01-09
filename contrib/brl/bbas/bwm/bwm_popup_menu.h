#ifndef bwm_popup_menu_h_
#define bwm_popup_menu_h_

#include <vgui/vgui_popup_params.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_tableau.h>

class bwm_popup_menu {
public:

  bwm_popup_menu(vgui_tableau_sptr tab) : tab_(tab) {}
  virtual ~bwm_popup_menu() {}
  void get_menu(vgui_menu& menu);

  // Image Processing Related Menu Items
 /* virtual void create_box();
  virtual void create_polygon();
  virtual void create_polyline();
  virtual void create_point();
  virtual void create_pointset();*/

  //************* IMAGE Processing Methods

  //: Draws the hostogram on a graph (if the image is greyscale)
  void hist_plot();// { (tab_->hist_plot(); }

  //: Draws the intensity profile on a selected line(if the image is greyscale)
  void intensity_profile();

  //: Provides a menu for adjusting the image display range map
  void range_map();

  //: Select a polygon before you call this method.
  // If it is a multiface object, it deletes the object where the selected
  // polygon belongs to.
  void clear_poly();

  //: clears the edges inside a box
  void clear_box();

  //: Deletes all the objects created so far
  void clear_all();

  //: deselects all the selected objects on the tableau
  void deselect_all();

  void save();
  void help_pop();
  void toggle_show_image_path();
  void zoom_to_fit();
  void scroll_to_point();
  void step_edges_vd();
  void lines_vd();
  void recover_edges();
  void recover_lines();

private:
  vgui_tableau_sptr tab_;

};

#endif