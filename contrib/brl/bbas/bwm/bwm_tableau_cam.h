#ifndef bwm_tableau_cam_h_
#define bwm_tableau_cam_h_
//:
// \file

#include "bwm_observable.h"
#include "bwm_observer_cam.h"
#include "bwm_tableau_img.h"

#include <vcl_string.h>

#include <vgui/vgui_menu.h>
#include <vgui/vgui_event.h>
#include <vgui/vgui_command.h>

#include <bgui/bgui_picker_tableau.h>

#include <vpgl/vpgl_camera.h>

class bwm_tableau_cam : public bwm_tableau_img
{
 public:

  bwm_tableau_cam(bwm_observer_cam* obs) : bwm_tableau_img(obs), my_observer_(obs) {}

  //: destructor
  // Tableaux are responsible for deleting their observers
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

  //: set the observer as per the image type
  void set_eo();
  void set_other_mode();

  //: moves the object, by moving the selected vertex to the specified point on the tableau
  void move_obj_by_vertex();

  //: extrudes the selected face,
  void extrude_face();

  //: draws a line onto a given face, dividing into two faces
  void divide_face();

  //: Draws the hostogram on a graph (if the image is greyscale)
  void hist_plot();

  void move_corr();

  void set_corr_to_vertex();

  void world_pt_corr();

  void load_mesh();

  void load_mesh_multiple();

  void scan_regions();

  //------------ save methods

  //: saves the observables with their save method specific to the object
  void save();
  void save_all();
  void save(vcl_string path);
  void save_all(vcl_string path);

  //: deletes the selected object
  void delete_object();

  //: deletes all 3D objects
  void delete_all();

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
  bool show_vertices_;
  //void bwm_vertex_toggle(const void* boolref);
};

#endif
