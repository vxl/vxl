#ifndef bwm_tableau_coin3d_h_
#define bwm_tableau_coin3d_h_

//#include "bwm_tableau.h"
#include "bwm_observable.h"
#include "bwm_observer_coin3d.h"

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_list.h>

#include <vgui/vgui_menu.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_wrapper_tableau.h>
#include <bgui/bgui_picker_tableau.h>

#include <vpgl/vpgl_camera.h>

#include <vsol/vsol_polygon_3d_sptr.h>

class bwm_tableau_coin3d : public vgui_wrapper_tableau {

public:
  bwm_tableau_coin3d() {}

  bwm_tableau_coin3d(vcl_string& image_path, vpgl_camera<double>& camera) {}

  virtual ~bwm_tableau_coin3d() { delete my_observer_; }

   virtual vcl_string type_name() const { return "bwm_tableau_coin3d"; }

  //bool handle(const vgui_event &e) { return vgui_wrapper_tableau::handle(e); }

  void set_observer(bwm_observer_coin3d* obs) { my_observer_ = obs; }

  void get_popup(vgui_popup_params const &params, vgui_menu &menu); 

  //: moves the object, by moving the selected vertex to the specified 
  // point on the tableau
  void move();

  //: extrudes the selected face, 
  void extrude_face();  

  //: draws a line onto a given face, dividing into two faces
  void divide_face();

  void create_inner_face();

  //: Select a polygon before you call this method. If it is a multiface 
  // object, it deletes the object where the selected polygon belongs to
  void clear_object();

  //: Deletes all the objects created so far
  void clear_all(); 

  void help_pop();

  SoNode* root() { return my_observer_->root(); }
protected:

  bwm_observer_coin3d* my_observer_;
  
};

#endif
