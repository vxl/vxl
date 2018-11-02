#ifndef bwm_tableau_img_h_
#define bwm_tableau_img_h_
//:
// \file

#include <iostream>
#include <string>
#include "bwm_tableau.h"
#include "bwm_observer_img.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgui/vgui_menu.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_command.h>

#include <bgui/bgui_picker_tableau.h>

class bwm_tableau_img : public bwm_tableau, public bgui_picker_tableau
{
 public:

  bwm_tableau_img(bwm_observer_img* obs)
    : bgui_picker_tableau(obs), my_observer_(obs) {}

  //: destructor
  // Tableaux are responsible for their observers
  virtual ~bwm_tableau_img() { delete my_observer_; }

  virtual std::string type_name() const { return "bwm_tableau_img"; }

  void get_popup(vgui_popup_params const &params, vgui_menu &menu);

  bool handle(const vgui_event& e);

  std::string img_path() const { return my_observer_->image_tableau()->file_name(); }

  virtual void create_box();
  virtual void create_polygon();
  virtual void create_polyline();
  virtual void create_point();
  virtual void create_pointset();
  virtual void copy();
  virtual void paste();
  virtual void create_vsol_spatial_object(vsol_spatial_object_2d_sptr obj);
  //************* IMAGE Processing Methods

  //: Draws the histogram on a graph (if the image is greyscale)
  void hist_plot() { my_observer_->hist_plot(); }

  //: Draws the histogram on a graph (if the image is greyscale)
  void hist_plot_in_poly() { my_observer_->hist_plot_in_poly(); }

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

  virtual void clear_all_frames();

  //: deselects all the selected objects on the tableau
  void deselect_all();

  void help_pop();
  void toggle_show_image_path();
  void zoom_to_fit();
  void scroll_to_point();
  void step_edges_vd();
  void lines_vd();
  void recover_edges();
  void recover_lines();
  void crop_image();
  //: mask operations
  void init_mask();
  void set_change_type() { my_observer_->set_change_type(); }
  void add_poly_to_mask();
  void remove_poly_from_mask();
  void save_mask();
  void save_changes_binary() { my_observer_->save_changes_binary(); }
  void load_changes_binary() { my_observer_->load_changes_binary(); }

  //: utilities
  void save_spatial_objects_2d();
  void load_spatial_objects_2d();
  void load_pointset_2d_ascii();
  void save_pointset_2d_ascii();
  void load_bounding_boxes_2d_ascii();
  void load_oriented_boxes_2d_ascii();
  //: internal detail
  void set_viewer(vgui_viewer2D_tableau_sptr viewer) { my_observer_->set_viewer(viewer); }

  void set_poly_mode() { my_observer_->set_draw_mode((bwm_observer_img::MODE_2D_POLY)); }
  void set_vertex_mode() { my_observer_->set_draw_mode((bwm_observer_img::MODE_2D_VERTEX)); }

 protected:
  void lock();
  void unlock();
  bwm_observer_img* my_observer_;
};

#endif
