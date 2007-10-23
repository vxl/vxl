#ifndef bwm_tableau_cam_h_
#define bwm_tableau_cam_h_

#include "bwm_observable.h"
#include "bwm_observer_cam.h"

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_list.h>

#include <vgui/vgui_menu.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_command.h>

#include <bgui/bgui_picker_tableau.h>

#include <vpgl/vpgl_camera.h>

#include <vsol/vsol_polygon_3d_sptr.h>

class bwm_tableau_cam : public bgui_picker_tableau {//public bwm_tableau {

public:

  bwm_tableau_cam(bwm_observer_cam* obs) : bgui_picker_tableau(obs), my_observer_(obs) {}

  //: destructor
  // tableaus are responsible from deleting their observers
  virtual ~bwm_tableau_cam(){ delete my_observer_; }

  virtual vcl_string type_name() const { return "bwm_tableau_cam"; }

  bool bwm_tableau_cam::handle(const vgui_event& e);

  void set_observer(bwm_observer_cam* obs) { my_observer_ = obs; }

  bwm_observer_cam* observer() const { return this->my_observer_; }

  void get_popup(vgui_popup_params const &params, vgui_menu &menu);

  virtual void create_polygon_mesh(); 

  virtual void triangulate_mesh();

  // sets this tableau as the master for camera direction 
  void set_master();

  //: moves the object, by moving the selected vertex to the specified 
  // point on the tableau
  void move_obj_by_vertex();

  //: extrudes the selected face, 
  void extrude_face();  

  //: draws a line onto a given face, dividing into two faces
  void divide_face();

  //: Draws the hostogram on a graph (if the image is greyscale)
  void hist_plot();

  //: Draws the intensity profile on a selected line(if the image is greyscale)
  void intensity_profile();

  //: Provides a menu for adjusting the image display range map
  void range_map();

  //: Select a polygon before you call this method. If it is a multiface 
  // object, it deletes the object where the selected polygon belongs to
  void clear_object();

  //: Deletes all the objects created so far
  void clear_all(); 

  //: deselects all the selected objects on the tableau
  void deselect_all();

  void move_corr();

  void world_pt_corr();

  void save();

  //------------ Projection Plane related methods
  
  //: selects an existing polygon as the projection plane
  void select_proj_plane();

  //: defines a vertical projection plane passing through a specified line
  void define_proj_plane();

  //: specify a x-y plane as the projection plane with a given z
  void define_xy_proj_plane();

  //: specify a y-z plane as the projection plane with a given x
  void define_yz_proj_plane();

  //: specify a x-z plane as the projection plane with a given x
  void define_xz_proj_plane();

  //: add inner face to an existing face
  void create_inner_face();

  void create_interior();

  void label_roof();

  void label_wall();

  void help_pop();

protected:

  bwm_observer_cam* my_observer_;
  
};

#endif

