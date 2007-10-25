#ifndef bwm_tableau_img_h_
#define bwm_tableau_img_h_

#include "bwm_tableau.h"
#include "bwm_observer_img.h"

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_list.h>

#include <vgui/vgui_menu.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_command.h>

#include <bgui/bgui_picker_tableau.h>

#include <vsol/vsol_polygon_2d_sptr.h>

class bwm_tableau_img : public bwm_tableau, public bgui_picker_tableau {

public:

  bwm_tableau_img(bwm_observer_img* obs) 
    : bgui_picker_tableau(obs), my_observer_(obs) {}

  //: destructor
  // tableaus are responsible from their observers
  virtual ~bwm_tableau_img() { delete my_observer_; }

  virtual vcl_string type_name() const { return "bwm_tableau_img"; }

  void get_popup(vgui_popup_params const &params, vgui_menu &menu);

  bool handle(const vgui_event& e);

  virtual void create_polygon(); 
  virtual void create_polyline();

  //************* IMAGE Processing Methods

  //: Draws the hostogram on a graph (if the image is greyscale)
  void hist_plot() { my_observer_->hist_plot(); }

  //: Draws the intensity profile on a selected line(if the image is greyscale)
  void intensity_profile();

  //: Provides a menu for adjusting the image display range map
  void range_map();

  //: Select a polygon before you call this method. If it is a multiface 
  // object, it deletes the object where the selected polygon belongs to
  void clear_poly();

  //: Deletes all the objects created so far
  void clear_all(); 

  //: deselects all the selected objects on the tableau
  void deselect_all();

  //void move_corr();

  //void world_pt_corr();

  void save();

  void help_pop();

protected:

  bwm_observer_img* my_observer_;
  
};

#endif

